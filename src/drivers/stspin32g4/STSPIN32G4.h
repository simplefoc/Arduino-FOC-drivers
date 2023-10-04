
#pragma once

#include "Arduino.h"

#ifdef ARDUINO_GENERIC_G431VBTX

#include "Wire.h"
#include "drivers/BLDCDriver6PWM.h"


#define STSPIN32G4_PIN_INUL  PE8
#define STSPIN32G4_PIN_INUH  PE9
#define STSPIN32G4_PIN_INVL  PE10
#define STSPIN32G4_PIN_INVH  PE11
#define STSPIN32G4_PIN_INWL  PE12
#define STSPIN32G4_PIN_INWH  PE13

#define STSPIN32G4_PIN_WAKE  PE7
#define STSPIN32G4_PIN_READY PE14
#define STSPIN32G4_PIN_FAULT PE15

#define STSPIN32G4_PIN_SDA   PC9
#define STSPIN32G4_PIN_SCL   PC8

#define STSPIN32G4_I2C_ADDR  0b1000111

#define STSPIN32G4_REG_POWMNG 0x01
#define STSPIN32G4_REG_LOGIC  0x02
#define STSPIN32G4_REG_READY  0x07
#define STSPIN32G4_REG_NFAULT 0x08
#define STSPIN32G4_REG_CLEAR  0x09
#define STSPIN32G4_REG_STBY   0x0A
#define STSPIN32G4_REG_LOCK   0x0B
#define STSPIN32G4_REG_RESET  0x0C
#define STSPIN32G4_REG_STATUS 0x80





union STSPIN32G4Status {
    struct {
        uint8_t vcc_uvlo:1;
        uint8_t thsd:1;
        uint8_t vds_p:1;
        uint8_t reset:1;
        uint8_t unused:3;
        uint8_t lock:1;
    };
    uint8_t reg;
};



union STSPIN32G4NFault {
    struct {
        uint8_t vcc_uvlo_flt:1;
        uint8_t thsd_flt:1;
        uint8_t vds_p_flt:1;
        uint8_t unused:5;
    };
    uint8_t reg;
};


union STSPIN32G4Ready {
    struct {
        uint8_t vcc_uvlo_rdy:1;
        uint8_t thsd_rdy:1;
        uint8_t unused:1;
        uint8_t stby_rdy:1;
        uint8_t unused2:4;
    };
    uint8_t reg;
};



union STSPIN32G4Logic {
    struct {
        uint8_t ilock:1;
        uint8_t dtmin:1;
        uint8_t vds_p_deg:2;
        uint8_t unused:4;
    };
    uint8_t reg;
};



union STSPIN32G4PowMng {
    struct {
        uint8_t vcc_val:2;
        uint8_t unused:2;
        uint8_t stby_reg_en:1;
        uint8_t vcc_dis:1;
        uint8_t reg3v3_dis:1;
        uint8_t unused2:1;
    };
    uint8_t reg;
};











class STSPIN32G4 : public BLDCDriver6PWM {
public:
    STSPIN32G4();
    ~STSPIN32G4();

    int init() override;

    void wake();
    void sleep();
    bool isReady();
    bool isFault();

    STSPIN32G4Status status();
    void lock();
    void unlock();
    STSPIN32G4NFault getNFaultRegister();
    STSPIN32G4Ready getReadyRegister();
    STSPIN32G4Logic getLogicRegister();
    STSPIN32G4PowMng getPowMngRegister();
    void setNFaultRegister(STSPIN32G4NFault value);
    void setReadyRegister(STSPIN32G4Ready value);
    void setLogicRegister(STSPIN32G4Logic value);
    void setPowMngRegister(STSPIN32G4PowMng value);    
    void resetRegisters();
    void clearFaults();

protected:
    uint8_t readRegister(uint8_t reg);
    void writeRegister(uint8_t reg, uint8_t val);

    TwoWire _wire;
};


#endif