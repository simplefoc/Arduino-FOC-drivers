

#include "./AS5600.h"


AS5600::AS5600(uint8_t address) : _address(address) {};
AS5600::~AS5600() {};

void AS5600::init(TwoWire* wire){
    _wire = wire;
    _wire->begin();
    if (!closeTransactions) {
        setAngleRegister();
    }
};


void AS5600::setAngleRegister() {
    _wire->beginTransmission(_address);
    if (useHysteresis)
        _wire->write(AS5600_REG_ANGLE);
    else
        _wire->write(AS5600_REG_ANGLE_RAW);
    _wire->endTransmission(false);
}


uint16_t AS5600::angle() {
    uint16_t result = 0;
    if (closeTransactions) {
        setAngleRegister();
    }
    _wire->requestFrom(_address, (uint8_t)2, (uint8_t)closeTransactions);
    result = (_wire->read()&0x0F)<<8;
    result |= _wire->read();
    return result;
};


uint16_t AS5600::readRawAngle() {
    return readRegister(AS5600_REG_ANGLE_RAW, 2) & 0x0FFF;
};


uint16_t AS5600::readAngle() {
    return readRegister(AS5600_REG_ANGLE, 2) & 0x0FFF;
};



uint16_t AS5600::readMagnitude() {
    return readRegister(AS5600_REG_MAGNITUDE, 2) & 0x0FFF;
};


AS5600Status AS5600::readStatus() {
    AS5600Status result;
    result.reg = (uint8_t)readRegister(AS5600_REG_STATUS, 1); // TODO: shift bits around
    return result;
};


uint8_t AS5600::readAGC() {
    return (uint8_t)readRegister(AS5600_REG_AGC, 1);
};



AS5600Conf AS5600::readConf() {
    AS5600Conf result;
    result.reg = readRegister(AS5600_REG_CONF, 2);
    return result;
};


uint16_t AS5600::readMang() {
    return readRegister(AS5600_REG_MANG, 2) & 0x0FFF;
};


uint16_t AS5600::readMPos() {
    return readRegister(AS5600_REG_MPOS, 2) & 0x0FFF;
};


uint16_t AS5600::readZPos() {
    return readRegister(AS5600_REG_ZPOS, 2) & 0x0FFF;
};


uint8_t AS5600::readZMCO() {
    return (readRegister(AS5600_REG_ZMCO, 1)&0x03);
};

uint8_t AS5600::readI2CAddr() {
    return (readRegister(AS5600_REG_I2CADDR, 1)>>1);
};


// set registers
void AS5600::setConf(AS5600Conf value) {
    // TODO: read before write
    writeRegister(AS5600_REG_CONF, value.reg);
};


void AS5600::setMang(uint16_t value) {
    // TODO: read before write
    writeRegister(AS5600_REG_MANG, value);
};


void AS5600::setMPos(uint16_t value) {
    // TODO: read before write
    writeRegister(AS5600_REG_MPOS, value);
};


void AS5600::setZPos(uint16_t value) {
    // TODO: read before write
    writeRegister(AS5600_REG_ZPOS, value);
};

void AS5600::setI2CAddr(uint8_t value) {
    uint8_t val = (uint8_t)readRegister(AS5600_REG_I2CADDR, 1);
    val = (value<<1) | (val&0x01);
    writeRegister(AS5600_REG_I2CADDR, val);
};

void AS5600::setI2CUpdt(uint8_t value) {
    uint8_t val = (uint8_t)readRegister(AS5600_REG_I2CUPDT, 1);
    val = (value<<1) | (val&0x01);
    writeRegister(AS5600_REG_I2CUPDT, val);
};


void AS5600::burnSettings(){
    writeRegister(AS5600_REG_BURN, 0x40, 1);
}



uint16_t AS5600::readRegister(uint8_t reg, uint8_t len){
    uint16_t result = 0;
    _wire->beginTransmission(_address);
    _wire->write(reg);
    _wire->endTransmission(false);
    _wire->requestFrom(_address, len, (uint8_t)closeTransactions);
    result = _wire->read();
    if (len == 2) {
        result <<= 8;
        result |= _wire->read();
    }
    if (!closeTransactions) {
        setAngleRegister();
    }
    return result;
};



void AS5600::writeRegister(uint8_t reg, uint16_t val, uint8_t len){
    _wire->beginTransmission(_address);
    _wire->write(reg);
    if (len == 2) {
        _wire->write(val>>8);
    }
    _wire->write(val&0xFF);
    _wire->endTransmission(closeTransactions);
    if (!closeTransactions) {
        setAngleRegister();
    }
};


