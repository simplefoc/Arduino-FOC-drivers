# CAN Communication Protocol

This document describes the CAN-based communication protocol for SimpleFOC motor control using the CANCommander class.

## Overview

The protocol uses **29-bit Extended CAN IDs** with a register-based architecture for efficient motor control and monitoring. The CAN ID structure is optimized for hardware filtering by placing the most significant bits (address) at the top.

## CAN ID Structure

The 29-bit CAN Extended ID is divided into the following fields:

```
Bit [28]     - Not used
Bits [27:20] - Node Address     (8 bits) - MSBs for efficient hardware filtering
Bits [19:16] - Packet Type      (4 bits) - Request/Response type
Bits [15:8]  - Register Number  (8 bits) - Target register (see register table)
Bits [7:0]   - Motor Index      (8 bits) - Motor selection (0-255)
```

### Bit Shifts (Defined in CANCommander.h)

```cpp
#define CAN_ADDRESS_SHIFT 20        // Address in MSBs for efficient filtering
#define CAN_PACKET_TYPE_SHIFT 16    // Packet type after address
#define CAN_REGISTER_SHIFT 8        // Register number
#define CAN_MOTOR_INDEX_SHIFT 0     // Motor selection (0-256)
```

## Packet Types

The protocol defines the following packet types:

| Value | Name               | Description                                      |
|-------|-------------------|--------------------------------------------------|
| 0x1   | CAN_READ_REQUEST  | Request to read a register value                 |
| 0x2   | CAN_WRITE_REQUEST | Request to write a value to a register           |
| 0x3   | CAN_READ_RESPONSE | Response containing register data                |
| 0xF   | CAN_SYNC          | Synchronization message                          |

## Protocol Operation

### Reading a Register

1. **Master** sends a `CAN_READ_REQUEST` with the target register number
2. **Node** responds with a `CAN_READ_RESPONSE` containing the register data

### Writing to a Register

1. **Master** sends a `CAN_WRITE_REQUEST` with register number and data payload
2. **Node** processes the write request
3. If echo is enabled, **Node** sends a `CAN_READ_RESPONSE` with the updated value

### Addressing

- **Unicast**: Direct message to a specific node address (0x00 - 0xFE)
- **Broadcast**: Address 0xFF sends to all nodes on the bus
- Hardware filtering is configured to accept only messages for the node's address or broadcast

## Register Map

The registers are defined in `SimpleFOCRegisters.h`. 

## Frame Diagram (Extended ID + Data)

```
CAN Extended ID (29 bits)
┌────────┬──────────────┬──────────────┬──────────────┬─────────────────┐
│ Bit 28 │ 27 ........20│ 19 ....... 16│ 15 ........ 8│   7 ........ 0  |
│  (Rsv) │  Address (8) │   Type (4)   │ Register (8) │ Motor index (8) |
└────────┴──────────────┴──────────────┴──────────────┴─────────────────┘
Data (0..8 bytes)
┌─────────────────────────────────────────────────────┐
│ Payload (register-specific encoding: byte/float/u32)│
└─────────────────────────────────────────────────────┘
```

Small payloads (status, single byte flags) use only first bytes. Multi-byte values (float, uint32_t) are little-endian raw copies.

## Implemented Registers (Summary)

Selected register groups (see header for full list):

| Group | Examples |
|-------|----------|
| Status & Control | `REG_STATUS`, `REG_ENABLE`, `REG_CONTROL_MODE`, `REG_TORQUE_MODE` |
| Motion Targets   | `REG_TARGET` (position/velocity/torque depending on mode) |
| Sensor Feedback  | `REG_ANGLE`, `REG_VELOCITY`, `REG_SENSOR_ANGLE`, `REG_SENSOR_VELOCITY` |
| Phase / Driver   | `REG_PHASE_VOLTAGE`, `REG_PHASE_STATE`, `REG_DRIVER_ENABLE` |
| FOC Internal     | `REG_VOLTAGE_Q/D`, `REG_CURRENT_Q/D`, `REG_CURRENT_ABC`, `REG_CURRENT_DC` |
| PID Velocity     | `REG_VEL_PID_P/I/D/LIM/RAMP`, `REG_VEL_LPF_T` |
| PID Position     | `REG_ANG_PID_P/I/D/LIM/RAMP`, `REG_ANG_LPF_T` |
| PID Current Q/D  | `REG_CURQ_PID_*`, `REG_CURD_PID_*`, `REG_CURQ_LPF_T`, `REG_CURD_LPF_T` |
| Limits           | `REG_VOLTAGE_LIMIT`, `REG_CURRENT_LIMIT`, `REG_VELOCITY_LIMIT`, `REG_PWM_FREQUENCY` |
| Motor Params     | `REG_PHASE_RESISTANCE`, `REG_KV`, `REG_INDUCTANCE`, current gains & offsets |
| System Info      | `REG_NUM_MOTORS`, `REG_SYS_TIME` |
| Custom Range     | `REG_CUSTOM_START (0xE0)` .. +32 via registration |

Registers are read-only (RO), write-only (WO) or read/write (R/W) as indicated in the header.

**📋 Complete register reference**: See [REGISTERS.md](../REGISTERS.md) for the full table with access types, data types, and sizes.

## Adding a Custom Register

1. Pick an ID >= `REG_CUSTOM_START` (0xE0)
2. Define handlers:
```cpp
bool myRead(RegisterIO& io, FOCMotor* m) { io << (float)m->shaft_velocity; return true; }
bool myWrite(RegisterIO& io, FOCMotor* m) { float v; io >> v; m->target = v; return true; }
```
3. Register with the commander (after `init()`):
```cpp
commander.addCustomRegister(0xE0, 4, myRead, myWrite); // size in bytes
```
4. Access via normal CAN read/write frames using that register number.

## CANCommander Core Features

| Feature | Description |
|---------|-------------|
| Multi-motor addressing | Up to 16 motors per node (`motor_idx` field) |
| Register abstraction | Uniform access to control, telemetry, tuning parameters |
| Broadcast & Unicast | Address 0xFF = broadcast; per-node filtering in hardware |
| Echo-on-write | Optional read-back response for writes (`echo` flag) |
| Sync packets | Lightweight synchronization (`CAN_SYNC`) |
| Compact frames | Up to 2 floats per frame (8 data bytes) |
| Custom extension | User-defined registers without modifying core tables |
| Error tracking | Status register includes last error & register accessed |

## Typical Exchange

| Action | ID Fields | Data |
|--------|-----------|------|
| Read register | addr, READ_REQUEST, reg, motor | (empty) |
| Response | addr, READ_RESPONSE, reg, motor | encoded value |
| Write register | addr, WRITE_REQUEST, reg, motor | new value bytes |
| Optional echo | addr, READ_RESPONSE, reg, motor | echoed value |
| Sync | addr/broadcast, SYNC | optional ack byte |

## Filtering
Nodes may enable strict filtering (default) or promiscuous (`no_filter=true`). Filtering masks only the address bits; broadcast (0xFF) frames pass via software check.

## Notes
- All numeric types are raw binary (IEEE754 float, uint32_t little-endian).
- Multi-frame transfers are not used; split larger conceptual sets into separate reads.
- Keep custom register sizes ≤8 bytes for single-frame responses.
- Version tag: `SIMPLEFOC_REGISTERS_VERSION` changes only on incompatible register layout updates.

## Minimal Usage Snippet
```cpp
CANio can(rxPin, txPin);
CANCommander commander(can, 0x12); // address 0x12
commander.init();
commander.addMotor(&motor);
// in loop:
commander.run();
```

Add custom:
```cpp
commander.addCustomRegister(0xE1, 4, myRead, myWrite);
```

Refer to `SimpleFOCRegisters.h` for the authoritative register list.
