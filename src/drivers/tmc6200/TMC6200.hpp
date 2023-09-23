#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <drivers/BLDCDriver3PWM.h>
#include <drivers/BLDCDriver6PWM.h>
#include "TMC6200_Registers.hpp"

#define TMC6200_VERSION 0x10

enum TMC6200_PWMMode {
    Individual = 0, // 6PWM
    SingleLine = 1, // 3PWM
};

enum TMC6200_AmplificationGain {
    _5 = 0,
    _10 = 1,
    _Also_10 = 2, // maybe just remove this
    _20 = 3,
};

enum TMC6200_ShortDelay {
    _750nS = 0,
    _1500nS = 1,
};

enum TMC6200_ShortFilter {
    _100nS = 0,
    _1uS = 1,
    _2uS = 2,
    _3uS = 3,
};

enum TMC6200_ParallelProtect {
    Detected = 0,
    All = 1,
};

enum TMC6200_FaultDirect {
    AtLeastOneBridgeDown = 0,
    EachProtectiveAction = 1,
};

enum TMC6200_OTSelect {
    _150 = 0b00,
    _143 = 0b01,
    _136 = 0b10,
    _120 = 0b11,
};

enum TMC6200_DRVStrength {
    Weak = 0b00,
    WeakTC = 0b01, // (medium above OTPW level)
    Medium = 0b10,
    Strong = 0b11,
};

class TMC6200GStatus {
public:
    TMC6200GStatus(TMC6200_GSTAT status) : status(status) {};

    bool isReset() const { return status.RESET == 0b1; };

    bool isOverTemperaturePreWarning() const { return status.DRV_OTPW == 0b1; };

    bool isOverTemperature() const { return status.DRV_OT == 0b1; };

    bool isChargePumpUnderVoltage() const { return status.UV_CP == 0b1; };

    bool hasUShorted() const { return status.SHORT_DET_U == 0b1; };

    bool hasVShorted() const { return status.SHORT_DET_V == 0b1; };

    bool hasWShorted() const { return status.SHORT_DET_W == 0b1; };

    bool isUShortedToGround() const { return status.S2GU == 0b1; };

    bool isUShortedToSupply() const { return status.S2VSU == 0b1; };

    bool isVShortedToGround() const { return status.S2GV == 0b1; };

    bool isVShortedToSupply() const { return status.S2VSV == 0b1; };

    bool isWShortedToGround() const { return status.S2GW == 0b1; };

    bool isWShortedToSupply() const { return status.S2VSW == 0b1; };

    TMC6200_GSTAT status;
};

class TMC6200Driver {

public:
    TMC6200Driver(int csPin) : csPin(csPin), spi(&SPI), settings(1000000, MSBFIRST, SPI_MODE3) {};

    virtual ~TMC6200Driver() {};

    virtual void init(SPIClass *_spi);

    void setDriverState(bool state);

    void setPWMMode(TMC6200_PWMMode pwmMode);

    void setFaultDirect(TMC6200_FaultDirect faultDirect);

    void setCurrentSenseGain(TMC6200_AmplificationGain amplificationGain);

    void setOverTemperatureThreshold(TMC6200_OTSelect otLevel);

    void setDriverStrength(TMC6200_DRVStrength strength);

    void setCurrentSenseAmplifierState(bool state);

    void setShortDelay(TMC6200_ShortDelay shortDelay);

    void shortFilter(TMC6200_ShortFilter shortFilter);

    void setShortToSupplySensitivityLevel(uint8_t level);

    void setShortToGroundSensitivityLevel(uint8_t level);

    void setShortRetries(uint8_t retries);

    void setParallelProtect(TMC6200_ParallelProtect parallelProtect);

    void setShortToGroundDetectionState(bool state);

    void setShortToSupplyDetectionState(bool state);

    void setBBMCycles(uint8_t clockCycles);

    void setStatus(TMC6200_GSTAT status);

    TMC6200GStatus getStatus();

    TMC6200_IOIN getInputs();

private:
    uint32_t readRegister(uint8_t addr);

    void writeRegister(uint8_t addr, uint32_t data);

    int csPin;
    SPIClass *spi;
    SPISettings settings;
};


class TMC6200Driver3PWM : public TMC6200Driver, public BLDCDriver3PWM {

public:
    TMC6200Driver3PWM(int phA, int phB, int phC, int cs, int en = NOT_SET) :
            TMC6200Driver(cs), BLDCDriver3PWM(phA, phB, phC, en) {};

    ~TMC6200Driver3PWM() override = default;

    virtual void init(SPIClass *_spi = &SPI) override;
};


class TMC6200Driver6PWM : public TMC6200Driver, public BLDCDriver6PWM {

public:
    TMC6200Driver6PWM(int phA_h, int phA_l, int phB_h, int phB_l, int phC_h, int phC_l, int cs, int en = NOT_SET) :
            TMC6200Driver(cs), BLDCDriver6PWM(phA_h, phA_l, phB_h, phB_l, phC_h, phC_l, en) {};

    ~TMC6200Driver6PWM() override = default;

    virtual void init(SPIClass *_spi = &SPI) override;
};