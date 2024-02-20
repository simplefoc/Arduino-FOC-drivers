
#pragma once

#include <inttypes.h>
#include "common/base_classes/FOCMotor.h"
#include "./RegisterIO.h"


// this constant is changed each time the registers definition are changed *in an incompatible way*. This means that just adding new registers 
// does not change the version, but removing or changing the meaning of existing registers does, or changing the number of an existing register.
#define SIMPLEFOC_REGISTERS_VERSION 0x01



typedef enum : uint8_t  {
    REG_STATUS = 0x00,          // RO - 1 byte (motor status)
    REG_TARGET = 0x01,          // R/W - float
    REG_ENABLE_ALL = 0x03,      // WO  - 1 byte
    REG_ENABLE = 0x04,          // R/W - 1 byte
    REG_CONTROL_MODE = 0x05,    // R/W - 1 byte
    REG_TORQUE_MODE = 0x06,     // R/W - 1 byte
    REG_MODULATION_MODE = 0x07, // R/W - 1 byte

    REG_ANGLE = 0x09,           // RO - float
    REG_POSITION = 0x10,        // RO - int32_t full rotations + float position (0-2PI, in radians) (4 bytes + 4 bytes)
    REG_VELOCITY = 0x11,        // RO - float
    REG_SENSOR_ANGLE = 0x12,    // RO - float
    REG_SENSOR_MECHANICAL_ANGLE = 0x13,    // RO - float
    REG_SENSOR_VELOCITY = 0x14, // RO - float
    REG_SENSOR_TIMESTAMP = 0x15,// RO - uint32_t

    REG_PHASE_VOLTAGE = 0x16,  // R/W - 3xfloat = 12 bytes
    REG_PHASE_STATE = 0x17,     // R/W - 3 bytes (1 byte per phase)
    REG_DRIVER_ENABLE = 0x18,   // R/W - 1 byte

    REG_TELEMETRY_REG = 0x1A,           // R/W - 1 + n*2 bytes, number of registers (n) = 1 byte + n * (motor number + register number, 1 byte each)
    REG_TELEMETRY_CTRL = 0x1B,          // R/W - 1 byte 
    REG_TELEMETRY_DOWNSAMPLE = 0x1C,    // R/W - uint32_t
    REG_ITERATIONS_SEC = 0x1D,          // RO - uint32_t

    REG_VOLTAGE_Q = 0x20,       // RO - float
    REG_VOLTAGE_D = 0x21,       // RO - float
    REG_CURRENT_Q = 0x22,       // RO - float
    REG_CURRENT_D = 0x23,       // RO - float
    REG_CURRENT_A = 0x24,       // RO - float
    REG_CURRENT_B = 0x25,       // RO - float
    REG_CURRENT_C = 0x26,       // RO - float
    REG_CURRENT_ABC = 0x27,     // RO - 3xfloat = 12 bytes
    REG_CURRENT_DC = 0x28,      // RO - float

    REG_VEL_PID_P = 0x30,         // R/W - float
    REG_VEL_PID_I = 0x31,         // R/W - float
    REG_VEL_PID_D = 0x32,         // R/W - float
    REG_VEL_PID_LIM = 0x33,       // R/W - float
    REG_VEL_PID_RAMP = 0x34,      // R/W - float
    REG_VEL_LPF_T = 0x35,         // R/W - float
    REG_ANG_PID_P = 0x36,         // R/W - float
    REG_ANG_PID_I = 0x37,         // R/W - float
    REG_ANG_PID_D = 0x38,         // R/W - float
    REG_ANG_PID_LIM = 0x39,       // R/W - float
    REG_ANG_PID_RAMP = 0x3A,      // R/W - float
    REG_ANG_LPF_T = 0x3B,         // R/W - float

    REG_CURQ_PID_P = 0x40,         // R/W - float
    REG_CURQ_PID_I = 0x41,         // R/W - float
    REG_CURQ_PID_D = 0x42,         // R/W - float
    REG_CURQ_PID_LIM = 0x43,       // R/W - float
    REG_CURQ_PID_RAMP = 0x44,      // R/W - float
    REG_CURQ_LPF_T = 0x45,         // R/W - float
    REG_CURD_PID_P = 0x46,         // R/W - float
    REG_CURD_PID_I = 0x47,         // R/W - float
    REG_CURD_PID_D = 0x48,         // R/W - float
    REG_CURD_PID_LIM = 0x49,         // R/W - float
    REG_CURD_PID_RAMP = 0x4A,         // R/W - float
    REG_CURD_LPF_T = 0x4B,         // R/W - float

    REG_VOLTAGE_LIMIT = 0x50,       // R/W - float
    REG_CURRENT_LIMIT = 0x51,       // R/W - float
    REG_VELOCITY_LIMIT = 0x52,      // R/W - float
    REG_DRIVER_VOLTAGE_LIMIT = 0x53,// R/W - float
    REG_PWM_FREQUENCY = 0x54,       // R/W - uint32_t
    REG_DRIVER_VOLTAGE_PSU = 0x55,  // R/W - float
    REG_VOLTAGE_SENSOR_ALIGN = 0x56,// R/W - float
    REG_MOTION_DOWNSAMPLE = 0x5F,   // R/W - uint32_t

    REG_ZERO_ELECTRIC_ANGLE = 0x60, // RO - float
    REG_SENSOR_DIRECTION = 0x61,    // RO - 1 byte
    REG_ZERO_OFFSET = 0x62,         // R/W - float
    REG_POLE_PAIRS = 0x63,          // RO - uint32_t
    REG_PHASE_RESISTANCE = 0x64,    // R/W - float
    REG_KV = 0x65,                  // R/W - float
    REG_INDUCTANCE = 0x66,          // R/W - float
    REG_CURA_GAIN = 0x67,           // R/W - float
    REG_CURB_GAIN = 0x68,           // R/W - float
    REG_CURC_GAIN = 0x69,           // R/W - float
    REG_CURA_OFFSET = 0x6A,         // R/W - float
    REG_CURB_OFFSET = 0x6B,         // R/W - float
    REG_CURC_OFFSET = 0x6C,         // R/W - float

    REG_NUM_MOTORS = 0x70,          // RO - 1 byte
    REG_SYS_TIME = 0x71,            // RO - uint32_t
    REG_MOTOR_ADDRESS = 0x7F,       // R/W - float

} SimpleFOCRegister;


class SimpleFOCRegisters {
public:
    SimpleFOCRegisters();
    virtual ~SimpleFOCRegisters();
    virtual uint8_t sizeOfRegister(uint8_t reg);
    virtual bool registerToComms(RegisterIO& comms, uint8_t reg, FOCMotor* motor);
    virtual bool commsToRegister(RegisterIO& comms, uint8_t reg, FOCMotor* motor);

    static SimpleFOCRegisters* regs;
};
