#ifndef __I2CCOMMANDERREGISTERS_H__
#define __I2CCOMMANDERREGISTERS_H__


typedef enum : uint8_t  {
    REG_STATUS = 0x00,          // RO - 3 bytes (currently set motor, last command register, last command error status) + 1 byte / motor (motor stati)
    REG_MOTOR_ADDRESS = 0x01,   // R/W - 1 byte
    REG_REPORT = 0x02,          // R/W - Write: variable, Read: variable, up to 32 bytes
    REG_ENABLE_ALL = 0x03,      // WO  - 1 byte
    REG_ENABLE = 0x04,          // R/W - 1 byte
    REG_CONTROL_MODE = 0x05,    // R/W - 1 byte
    REG_TORQUE_MODE = 0x06,     // R/W - 1 byte
    REG_MODULATION_MODE = 0x07, // R/W - 1 byte

    REG_TARGET = 0x08,          // R/W - float
    REG_ANGLE = 0x09,           // RO - float
    REG_POSITION = 0x10,        // RO - double
    REG_VELOCITY = 0x11,        // RO - float
    REG_SENSOR_ANGLE = 0x12,    // RO - float

    REG_VOLTAGE_Q = 0x20,       // RO - float
    REG_VOLTAGE_D = 0x21,       // RO - float
    REG_CURRENT_Q = 0x22,       // RO - float
    REG_CURRENT_D = 0x23,       // RO - float
    REG_CURRENT_A = 0x24,       // RO - float
    REG_CURRENT_B = 0x25,       // RO - float
    REG_CURRENT_C = 0x26,       // RO - float
    REG_CURRENT_ABC = 0x27,     // RO - 3xfloat = 12 bytes

    REG_VEL_PID_P = 0x30,         // R/W - float
    REG_VEL_PID_I = 0x31,         // R/W - float
    REG_VEL_PID_D = 0x32,         // R/W - float
    REG_VEL_LPF_T = 0x33,         // R/W - float
    REG_ANG_PID_P = 0x34,         // R/W - float
    REG_VEL_LIMIT = 0x35,         // R/W - float
    REG_VEL_MAX_RAMP = 0x36,        // R/W - float

    REG_CURQ_PID_P = 0x40,         // R/W - float
    REG_CURQ_PID_I = 0x41,         // R/W - float
    REG_CURQ_PID_D = 0x42,         // R/W - float
    REG_CURQ_LPF_T = 0x43,         // R/W - float
    REG_CURD_PID_P = 0x44,         // R/W - float
    REG_CURD_PID_I = 0x45,         // R/W - float
    REG_CURD_PID_D = 0x46,         // R/W - float
    REG_CURD_LPF_T = 0x47,         // R/W - float

    REG_MAX_VOLTAGE = 0x50,         // R/W - float
    REG_MAX_CURRENT = 0x51,         // R/W - float
    REG_MOTION_DOWNSAMPLE = 0x52,   // R/W - uint32_t

    REG_ZERO_ELECTRIC_ANGLE = 0x60, // RO - float
    REG_SENSOR_DIRECTION = 0x61,    // RO - 1 byte
    REG_ZERO_OFFSET = 0x62,         // R/W - float
    REG_POLE_PAIRS = 0x63,          // RO - uint32_t
    REG_PHASE_RESISTANCE = 0x64,    // RO - float

    REG_NUM_MOTORS = 0x70,      // RO - 1 byte
    REG_SYS_TIME = 0x71,        // RO - uint32_t

} I2CCommander_Register;





#endif