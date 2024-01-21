#include "MA330.h"

MA330::MA330(SPISettings settings, int nCS) : settings(settings), nCS(nCS) {

};
MA330::~MA330() {

};

void MA330::init(SPIClass* _spi) {
    spi = _spi;
    if (nCS >= 0) {
        pinMode(nCS, OUTPUT);
        digitalWrite(nCS, HIGH);
    }
};

float MA330::getCurrentAngle() {
    return (readRawAngle() * _2PI)/MA330_CPR;
}; // angle in radians, return current value

uint16_t MA330::readRawAngle() {
    uint16_t angle = transfer16(0x0000);
    return angle;
}; // 9-14bit angle value

uint16_t MA330::getZero() {
    uint16_t result = readRegister(MA330_REG_ZERO_POSITION_MSB)<<8;
    result |= readRegister(MA330_REG_ZERO_POSITION_LSB);
    return result;
};
uint8_t MA330::getBiasCurrentTrimming() {
    return readRegister(MA330_REG_BCT);
};
bool MA330::isBiasCurrrentTrimmingX() {
    return (readRegister(MA330_REG_ET) & 0x01)==0x01;
};
bool MA330::isBiasCurrrentTrimmingY() {
    return (readRegister(MA330_REG_ET) & 0x02)==0x02;
};
uint16_t MA330::getPulsesPerTurn() {
    uint16_t result = readRegister(MA330_REG_ILIP_PPT_LSB)>>6;
    result |= ((uint16_t)readRegister(MA330_REG_PPT_MSB))<<2;
    return result+1;
};
uint8_t MA330::getIndexLength() {
    return (readRegister(MA330_REG_ILIP_PPT_LSB)>>2)&0x0F;
};
uint8_t MA330::getNumberPolePairs() {
    return (readRegister(MA330_REG_NPP)>>5)&0x07;;
};
uint8_t MA330::getRotationDirection() {
    return (readRegister(MA330_REG_RD)>>7);
};
uint8_t MA330::getFieldStrengthHighThreshold() {
    return (readRegister(MA330_REG_MGLT_MGHT)>>2)&0x07;
};
uint8_t MA330::getFieldStrengthLowThreshold() {
    return (readRegister(MA330_REG_MGLT_MGHT)>>5)&0x07;
};
uint8_t MA330::getFilterWidth() {
    return readRegister(MA330_REG_FW);
};
uint8_t MA330::getHysteresis() {
    return readRegister(MA330_REG_HYS);
};
FieldStrength MA330::getFieldStrength() {
    return (FieldStrength)(readRegister(MA330_REG_MGH_MGL)>>6);
};



void MA330::setZero(uint16_t value) {
    writeRegister(MA330_REG_ZERO_POSITION_MSB, value>>8);
    writeRegister(MA330_REG_ZERO_POSITION_LSB, value&0x00FF);
};
void MA330::setBiasCurrentTrimming(uint8_t value) {
    writeRegister(MA330_REG_BCT, value);
};
void MA330::setBiasCurrrentTrimmingEnabled(bool Xenabled, bool Yenabled) {
    uint8_t val = Xenabled ? 0x01 : 0x00;
    val |= (Yenabled ? 0x02 : 0x00);
    writeRegister(MA330_REG_ET, val);
};
void MA330::setPulsesPerTurn(uint16_t value) {
    uint16_t pptVal = value - 1;
    writeRegister(MA330_REG_PPT_MSB, pptVal>>2);
    uint8_t val = readRegister(MA330_REG_ILIP_PPT_LSB);
    val &= 0x3F;
    val |= (pptVal&0x03)<<6;
    writeRegister(MA330_REG_ILIP_PPT_LSB, val);
};
void MA330::setIndexLength(uint8_t value) {
    uint8_t val = readRegister(MA330_REG_ILIP_PPT_LSB);
    val &= 0xC0;
    val |= ((value<<2)&0x3F);
    writeRegister(MA330_REG_ILIP_PPT_LSB, val);
};
void MA330::setNumberPolePairs(uint8_t value) {
    uint8_t val = readRegister(MA330_REG_NPP);
    val &= 0x1F;
    val |= (value<<5);
    writeRegister(MA330_REG_NPP, val);
};
void MA330::setRotationDirection(uint8_t value) {
    if (value==0)
        writeRegister(MA330_REG_RD, 0x00);
    else
        writeRegister(MA330_REG_RD, 0x80);
};
void MA330::setFilterWidth(uint8_t value) {
    writeRegister(MA330_REG_FW, value);
};
void MA330::setHysteresis(uint8_t value) {
    writeRegister(MA330_REG_HYS, value);
};
void MA330::setFieldStrengthThresholds(uint8_t high, uint8_t low) {
    uint8_t val = (low<<5) | (high<<2);
    writeRegister(MA330_REG_MGLT_MGHT, val);
};


uint16_t MA330::transfer16(uint16_t outValue) {
    spi->beginTransaction(settings);
    if (nCS >= 0)
        digitalWrite(nCS, LOW);
    uint16_t value = spi->transfer16(outValue);
    if (nCS >= 0)
        digitalWrite(nCS, HIGH);
    spi->endTransaction();
    return value;
};
uint8_t MA330::readRegister(uint8_t reg) {
    uint16_t cmd = 0x4000 | ((reg&0x001F)<<8);
    uint16_t value = transfer16(cmd);
    delayMicroseconds(1);
    value = transfer16(0x0000);
    return value>>8;
};
uint8_t MA330::writeRegister(uint8_t reg, uint8_t value) {
    uint16_t cmd = 0x8000 | ((reg&0x1F)<<8) | value;
    uint16_t result = transfer16(cmd);
    delay(20); // 20ms delay required
    result = transfer16(0x0000);
    return result>>8;
};