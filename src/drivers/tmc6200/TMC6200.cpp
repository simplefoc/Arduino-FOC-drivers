#include "TMC6200.hpp"

void TMC6200Driver::init(SPIClass *_spi) {
    spi = _spi;
    settings = SPISettings(500000, MSBFIRST, SPI_MODE3);

    pinMode(csPin, OUTPUT);
    digitalWrite(csPin, HIGH);
}

// TMC6200_GCONF ------

void TMC6200Driver::setDriverState(bool state) {
    TMC6200_GCONF gConf;
    gConf.reg = readRegister(TMC6200_GCONF_REG);

    gConf.DISABLE = state ? 0 : 1;
    writeRegister(TMC6200_GCONF_REG, gConf.reg);
}

void TMC6200Driver::setPWMMode(TMC6200_PWMMode pwmMode) {
    TMC6200_GCONF gConf;
    gConf.reg = readRegister(TMC6200_GCONF_REG);

    gConf.SINGLE_LINE = pwmMode;
    writeRegister(TMC6200_GCONF_REG, gConf.reg);
}

void TMC6200Driver::setFaultDirect(TMC6200_FaultDirect faultDirect) {
    TMC6200_GCONF gConf;
    gConf.reg = readRegister(TMC6200_GCONF_REG);

    gConf.FAULT_DIRECT = faultDirect;
    writeRegister(TMC6200_GCONF_REG, gConf.reg);
}

void TMC6200Driver::setCurrentSenseAmplifierState(bool state) {
    TMC6200_GCONF gConf;
    gConf.reg = readRegister(TMC6200_GCONF_REG);

    gConf.AMPLIFIER_OFF = state ? 0 : 1;
    writeRegister(TMC6200_GCONF_REG, gConf.reg);
}

void TMC6200Driver::setCurrentSenseGain(TMC6200_AmplificationGain amplificationGain) {
    TMC6200_GCONF gConf;
    gConf.reg = readRegister(TMC6200_GCONF_REG);

    gConf.AMPLIFICATION = amplificationGain;
    writeRegister(TMC6200_GCONF_REG, gConf.reg);

    gConf.reg = readRegister(TMC6200_GCONF_REG);
}

// TMC6200_DRV_CONF ------

void TMC6200Driver::setOverTemperatureThreshold(TMC6200_OTSelect otLevel) {
    TMC6200_DRV_CONF drvConf;
    drvConf.reg = readRegister(TMC6200_DRV_CONF_REG);

    drvConf.OT_SELECT = otLevel;
    writeRegister(TMC6200_DRV_CONF_REG, drvConf.reg);
}

void TMC6200Driver::setDriverStrength(TMC6200_DRVStrength strength) {
    TMC6200_DRV_CONF drvConf;
    drvConf.reg = readRegister(TMC6200_DRV_CONF_REG);

    drvConf.DRV_STRENGTH = strength;
    writeRegister(TMC6200_DRV_CONF_REG, drvConf.reg);
}

void TMC6200Driver::setBBMCycles(uint8_t clockCycles) {
    TMC6200_DRV_CONF drvConf;
    drvConf.reg = readRegister(TMC6200_DRV_CONF_REG);

    drvConf.BBMCLKS = clockCycles;
    writeRegister(TMC6200_DRV_CONF_REG, drvConf.reg);
}

// TMC6200_SHORT_CONF ------

void TMC6200Driver::setShortDelay(TMC6200_ShortDelay shortDelay) {
    TMC6200_SHORT_CONF shortConf;
    shortConf.reg = readRegister(TMC6200_SHORT_CONF_REG);

    shortConf.SHORT_DELAY = shortDelay;
    writeRegister(TMC6200_SHORT_CONF_REG, shortConf.reg);
}

void TMC6200Driver::shortFilter(TMC6200_ShortFilter shortFilter) {
    TMC6200_SHORT_CONF shortConf;
    shortConf.reg = readRegister(TMC6200_SHORT_CONF_REG);

    shortConf.SHORT_FILTER = shortFilter;
    writeRegister(TMC6200_SHORT_CONF_REG, shortConf.reg);
}

void TMC6200Driver::setShortToSupplySensitivityLevel(uint8_t level) {
    TMC6200_SHORT_CONF shortConf;
    shortConf.reg = readRegister(TMC6200_SHORT_CONF_REG);

    shortConf.S2VS_LEVEL = level;
    writeRegister(TMC6200_SHORT_CONF_REG, shortConf.reg);
}

void TMC6200Driver::setShortToGroundSensitivityLevel(uint8_t level) {
    TMC6200_SHORT_CONF shortConf;
    shortConf.reg = readRegister(TMC6200_SHORT_CONF_REG);

    shortConf.S2G_LEVEL = level;
    writeRegister(TMC6200_SHORT_CONF_REG, shortConf.reg);
}

void TMC6200Driver::setShortRetries(uint8_t retries) {
    TMC6200_SHORT_CONF shortConf;
    shortConf.reg = readRegister(TMC6200_SHORT_CONF_REG);

    shortConf.RETRY = retries;
    writeRegister(TMC6200_SHORT_CONF_REG, shortConf.reg);
}

void TMC6200Driver::setParallelProtect(TMC6200_ParallelProtect parallelProtect) {
    TMC6200_SHORT_CONF shortConf;
    shortConf.reg = readRegister(TMC6200_SHORT_CONF_REG);

    shortConf.PROTECT_PARALLEL = parallelProtect;
    writeRegister(TMC6200_SHORT_CONF_REG, shortConf.reg);
}

void TMC6200Driver::setShortToGroundDetectionState(bool state) {
    TMC6200_SHORT_CONF shortConf;
    shortConf.reg = readRegister(TMC6200_SHORT_CONF_REG);

    shortConf.DISABLE_S2G = state ? 0 : 1;
    writeRegister(TMC6200_SHORT_CONF_REG, shortConf.reg);
}

void TMC6200Driver::setShortToSupplyDetectionState(bool state) {
    TMC6200_SHORT_CONF shortConf;
    shortConf.reg = readRegister(TMC6200_SHORT_CONF_REG);

    shortConf.DISABLE_S2VS = state ? 0 : 1;
    writeRegister(TMC6200_SHORT_CONF_REG, shortConf.reg);
}

// TMC6200_IOIN ------

TMC6200_IOIN TMC6200Driver::getInputs() {
    TMC6200_IOIN ioin;
    ioin.reg = readRegister(TMC6200_IOIN_REG);

    return ioin;
}

// TMC6200_GSTAT ------

TMC6200GStatus TMC6200Driver::getStatus() {
    TMC6200_GSTAT gstat;
    gstat.reg = readRegister(TMC6200_GSTAT_REG);

    return {gstat};
}

void TMC6200Driver::setStatus(TMC6200_GSTAT status) {
    writeRegister(TMC6200_GSTAT_REG, status.reg);
}

uint32_t TMC6200Driver::readRegister(uint8_t addr) {
    digitalWrite(csPin, LOW);
    spi->beginTransaction(settings);

    // Address
    spi->transfer(TMC_ADDRESS(addr));

    // 32bit Data
    uint32_t value = 0;
    value |= (spi->transfer(0x00) << 24);
    value |= (spi->transfer(0x00) << 16);
    value |= (spi->transfer(0x00) << 8);
    value |= (spi->transfer(0x00) << 0);

    spi->end();
    digitalWrite(csPin, HIGH);

    return value;
}

void TMC6200Driver::writeRegister(uint8_t addr, uint32_t data) {
    digitalWrite(csPin, LOW);
    spi->beginTransaction(settings);

    // Address
    spi->transfer(addr | TMC_WRITE_BIT);

    // 32bit Data
    spi->transfer(0xFF & (data >> 24));
    spi->transfer(0xFF & (data >> 16));
    spi->transfer(0xFF & (data >> 8));
    spi->transfer(0xFF & (data >> 0));

    spi->end();
    digitalWrite(csPin, HIGH);
}

// --------------------

void TMC6200Driver3PWM::init(SPIClass *_spi) {
    TMC6200Driver::init(_spi);
    delayMicroseconds(1);
    TMC6200Driver::setPWMMode(TMC6200_PWMMode::SingleLine);
    BLDCDriver3PWM::init();
};


void TMC6200Driver6PWM::init(SPIClass *_spi) {
    TMC6200Driver::init(_spi);
    delayMicroseconds(1);
    TMC6200Driver::setPWMMode(TMC6200_PWMMode::Individual);
    BLDCDriver6PWM::init();
};
