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
