# SimpleFOC CAN Registers Reference

This document summarizes the registers defined in `SimpleFOCRegisters.h`, their access type (RO/RW/WO), data type, and size in bytes. Sizes reflect payload bytes in a single CAN frame.

Note: Multi-value registers fit in one frame (≤ 8 bytes) only where indicated; larger compound values are still sent as single frames using compact encoding as implemented by `SimpleFOCRegisters`.

## Legend
- RO: Read-only
- RW: Read/Write
- WO: Write-only
- u8: uint8_t (1 byte)
- u32: uint32_t (4 bytes)
- f32: float (4 bytes)
- 3x f32: three floats (12 bytes, may be sent across multiple frames by higher-level tooling)

## Registers

| ID | Name | Access | Type | Size |
|----|------|--------|------|------|
| 0x00 | REG_STATUS | RO | u8×4 (motor, last_reg, last_err, status) | 4 |
| 0x01 | REG_TARGET | RW | f32 | 4 |
| 0x03 | REG_ENABLE_ALL | WO | u8 | 1 |
| 0x04 | REG_ENABLE | RW | u8 | 1 |
| 0x05 | REG_CONTROL_MODE | RW | u8 | 1 |
| 0x06 | REG_TORQUE_MODE | RW | u8 | 1 |
| 0x07 | REG_MODULATION_MODE | RW | u8 | 1 |
| 0x09 | REG_ANGLE | RO | f32 | 4 |
| 0x10 | REG_POSITION | RO | u32 + f32 (turns + rad) | 8 |
| 0x11 | REG_VELOCITY | RO | f32 | 4 |
| 0x12 | REG_SENSOR_ANGLE | RO | f32 | 4 |
| 0x13 | REG_SENSOR_MECHANICAL_ANGLE | RO | f32 | 4 |
| 0x14 | REG_SENSOR_VELOCITY | RO | f32 | 4 |
| 0x15 | REG_SENSOR_TIMESTAMP | RO | u32 | 4 |
| 0x16 | REG_PHASE_VOLTAGE | RW | 3x f32 | 12 |
| 0x17 | REG_PHASE_STATE | RW | u8×3 | 3 |
| 0x18 | REG_DRIVER_ENABLE | RW | u8 | 1 |
| 0x1A | REG_TELEMETRY_REG | RW | u8 + n×(u8+u8) | variable |
| 0x1B | REG_TELEMETRY_CTRL | RW | u8 | 1 |
| 0x1C | REG_TELEMETRY_DOWNSAMPLE | RW | u32 | 4 |
| 0x1D | REG_ITERATIONS_SEC | RO | u32 | 4 |
| 0x20 | REG_VOLTAGE_Q | RO | f32 | 4 |
| 0x21 | REG_VOLTAGE_D | RO | f32 | 4 |
| 0x22 | REG_CURRENT_Q | RO | f32 | 4 |
| 0x23 | REG_CURRENT_D | RO | f32 | 4 |
| 0x24 | REG_CURRENT_A | RO | f32 | 4 |
| 0x25 | REG_CURRENT_B | RO | f32 | 4 |
| 0x26 | REG_CURRENT_C | RO | f32 | 4 |
| 0x27 | REG_CURRENT_ABC | RO | 3x f32 | 12 |
| 0x28 | REG_CURRENT_DC | RO | f32 | 4 |
| 0x30 | REG_VEL_PID_P | RW | f32 | 4 |
| 0x31 | REG_VEL_PID_I | RW | f32 | 4 |
| 0x32 | REG_VEL_PID_D | RW | f32 | 4 |
| 0x33 | REG_VEL_PID_LIM | RW | f32 | 4 |
| 0x34 | REG_VEL_PID_RAMP | RW | f32 | 4 |
| 0x35 | REG_VEL_LPF_T | RW | f32 | 4 |
| 0x36 | REG_ANG_PID_P | RW | f32 | 4 |
| 0x37 | REG_ANG_PID_I | RW | f32 | 4 |
| 0x38 | REG_ANG_PID_D | RW | f32 | 4 |
| 0x39 | REG_ANG_PID_LIM | RW | f32 | 4 |
| 0x3A | REG_ANG_PID_RAMP | RW | f32 | 4 |
| 0x3B | REG_ANG_LPF_T | RW | f32 | 4 |
| 0x40 | REG_CURQ_PID_P | RW | f32 | 4 |
| 0x41 | REG_CURQ_PID_I | RW | f32 | 4 |
| 0x42 | REG_CURQ_PID_D | RW | f32 | 4 |
| 0x43 | REG_CURQ_PID_LIM | RW | f32 | 4 |
| 0x44 | REG_CURQ_PID_RAMP | RW | f32 | 4 |
| 0x45 | REG_CURQ_LPF_T | RW | f32 | 4 |
| 0x46 | REG_CURD_PID_P | RW | f32 | 4 |
| 0x47 | REG_CURD_PID_I | RW | f32 | 4 |
| 0x48 | REG_CURD_PID_D | RW | f32 | 4 |
| 0x49 | REG_CURD_PID_LIM | RW | f32 | 4 |
| 0x4A | REG_CURD_PID_RAMP | RW | f32 | 4 |
| 0x4B | REG_CURD_LPF_T | RW | f32 | 4 |
| 0x50 | REG_VOLTAGE_LIMIT | RW | f32 | 4 |
| 0x51 | REG_CURRENT_LIMIT | RW | f32 | 4 |
| 0x52 | REG_VELOCITY_LIMIT | RW | f32 | 4 |
| 0x53 | REG_DRIVER_VOLTAGE_LIMIT | RW | f32 | 4 |
| 0x54 | REG_PWM_FREQUENCY | RW | u32 | 4 |
| 0x55 | REG_DRIVER_VOLTAGE_PSU | RW | f32 | 4 |
| 0x56 | REG_VOLTAGE_SENSOR_ALIGN | RW | f32 | 4 |
| 0x5F | REG_MOTION_DOWNSAMPLE | RW | u32 | 4 |
| 0x60 | REG_ZERO_ELECTRIC_ANGLE | RO | f32 | 4 |
| 0x61 | REG_SENSOR_DIRECTION | RO | u8 | 1 |
| 0x62 | REG_ZERO_OFFSET | RW | f32 | 4 |
| 0x63 | REG_POLE_PAIRS | RO | u32 | 4 |
| 0x64 | REG_PHASE_RESISTANCE | RW | f32 | 4 |
| 0x65 | REG_KV | RW | f32 | 4 |
| 0x66 | REG_INDUCTANCE | RW | f32 | 4 |
| 0x67 | REG_CURA_GAIN | RW | f32 | 4 |
| 0x68 | REG_CURB_GAIN | RW | f32 | 4 |
| 0x69 | REG_CURC_GAIN | RW | f32 | 4 |
| 0x6A | REG_CURA_OFFSET | RW | f32 | 4 |
| 0x6B | REG_CURB_OFFSET | RW | f32 | 4 |
| 0x6C | REG_CURC_OFFSET | RW | f32 | 4 |
| 0x70 | REG_NUM_MOTORS | RO | u8 | 1 |
| 0x71 | REG_SYS_TIME | RO | u32 | 4 |
| 0x7F | REG_MOTOR_ADDRESS | RW | f32 | 4 |

## Custom Registers
- Range starts at `REG_CUSTOM_START (0xE0)` and supports up to `MAX_CUSTOM_REGISTERS` (32).
- Register handlers provide size and read/write callbacks:

```cpp
bool addCustomRegister(uint8_t reg, uint8_t size,
    RegisterReadHandler readHandler,
    RegisterWriteHandler writeHandler);
```

Example:
```cpp
bool myRead(RegisterIO& io, FOCMotor* m) { io << (float)m->shaft_velocity; return true; }
bool myWrite(RegisterIO& io, FOCMotor* m) { float v; io >> v; m->target = v; return true; }
commander.addCustomRegister(0xE0, 4, myRead, myWrite);
```

## Notes
- Floats and integers are transmitted as raw little-endian binary.
- Multi-component values (e.g., 3x floats) may exceed 8 bytes; higher layers should split or compress if needed.
- The version constant `SIMPLEFOC_REGISTERS_VERSION` changes only on incompatible schema updates.
