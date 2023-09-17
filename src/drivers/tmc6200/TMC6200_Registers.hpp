#pragma once

// ------

#define TMC_WRITE_BIT 0x80
#define TMC_ADDRESS_MASK 0x7F
#define TMC_ADDRESS(x) ((x) & (TMC_ADDRESS_MASK))

// ------

#define TMC6200_GCONF_REG           0x00 // RW
#define TMC6200_GSTAT_REG           0x01 // RW + WC
#define TMC6200_IOIN_REG            0x04 // R

#define TMC6200_OTP_PROG_REG        0x06 // not used
#define TMC6200_OTP_READ_REG        0x07 // not used
#define TMC6200_FACTORY_CONF_REG    0x08 // not used

#define TMC6200_SHORT_CONF_REG      0x09 // RW
#define TMC6200_DRV_CONF_REG        0x0A // RW

typedef union {
    struct {
        uint32_t DISABLE: 1;
        uint32_t SINGLE_LINE: 1;
        uint32_t FAULT_DIRECT: 1;
        uint32_t : 1;
        uint32_t AMPLIFICATION: 2;
        uint32_t AMPLIFIER_OFF: 1;
        uint32_t TMC6200_TEST_MODE: 1;
        uint32_t : 24;
    };
    uint32_t reg;
} TMC6200_GCONF;

typedef union {
    struct {
        uint32_t RESET: 1;
        uint32_t DRV_OTPW: 1;
        uint32_t DRV_OT: 1;
        uint32_t UV_CP: 1;

        uint32_t SHORT_DET_U: 1;
        uint32_t S2GU: 1;
        uint32_t S2VSU: 1;
        uint32_t : 1;

        uint32_t SHORT_DET_V: 1;
        uint32_t S2GV: 1;
        uint32_t S2VSV: 1;
        uint32_t : 1;

        uint32_t SHORT_DET_W: 1;
        uint32_t S2GW: 1;
        uint32_t S2VSW: 1;
        uint32_t : 1;
    };
    uint32_t reg;
} TMC6200_GSTAT;

typedef union {
    struct {
        unsigned UL: 1;
        uint32_t UH: 1;
        uint32_t VL: 1;
        uint32_t VH: 1;
        uint32_t WL: 1;
        uint32_t WH: 1;
        uint32_t DRV_EN: 1;
        uint32_t _reserved1: 1;
        uint32_t OTPW: 1;
        uint32_t OT136C: 1;
        uint32_t OT143C: 1;
        uint32_t OT150C: 1;
        uint32_t _reserved2: 12;
        uint32_t VERSION: 8;
    };

    uint32_t reg;
} TMC6200_IOIN;

typedef union {
    struct {
        uint32_t S2VS_LEVEL: 4;
        uint32_t : 4;
        uint32_t S2G_LEVEL: 4;
        uint32_t : 4;
        uint32_t SHORT_FILTER: 2;
        uint32_t : 2;
        uint32_t SHORT_DELAY: 1;
        uint32_t : 4;
        uint32_t RETRY: 2;
        uint32_t : 2;
        uint32_t PROTECT_PARALLEL: 1;
        uint32_t DISABLE_S2G: 1;
        uint32_t DISABLE_S2VS: 1;
        uint32_t : 2;
    };
    uint32_t reg;
} TMC6200_SHORT_CONF;

typedef union {
    struct {
        uint32_t BBMCLKS: 5;
        uint32_t    : 11;
        uint32_t OT_SELECT: 2;
        uint32_t DRV_STRENGTH: 2;
        uint32_t    : 12;
    };
    uint32_t reg;
} TMC6200_DRV_CONF;