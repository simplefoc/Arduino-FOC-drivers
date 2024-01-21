#include "MA730.h"

MA730::MA730(SPISettings settings, int nCS) : settings(settings), nCS(nCS) {

};
MA730::~MA730() {

};

void MA730::init(SPIClass* _spi) {
    spi = _spi;
    if (nCS >= 0) {
        pinMode(nCS, OUTPUT);
        digitalWrite(nCS, HIGH);
    }
};

float MA730::getCurrentAngle() {
    return (readRawAngle() * _2PI)/MA730_CPR;
}; // angle in radians, return current value

uint16_t MA730::readRawAngle() {
    uint16_t angle = transfer16(0x0000);
    return angle;
}; // 14bit angle value

uint16_t MA730::getZero() {
    uint16_t result = readRegister(MA730_REG_ZERO_POSITION_MSB)<<8;
    result |= readRegister(MA730_REG_ZERO_POSITION_LSB);
    return result;
};
uint8_t MA730::getBiasCurrentTrimming() {
    return readRegister(MA730_REG_BCT);
};
bool MA730::isBiasCurrrentTrimmingX() {
    return (readRegister(MA730_REG_ET) & 0x01)==0x01;
};
bool MA730::isBiasCurrrentTrimmingY() {
    return (readRegister(MA730_REG_ET) & 0x02)==0x02;
};
uint16_t MA730::getPulsesPerTurn() {
    uint16_t result = readRegister(MA730_REG_ILIP_PPT_LSB)>>6;
    result |= ((uint16_t)readRegister(MA730_REG_PPT_MSB))<<2;
    return result+1;
};
uint8_t MA730::getIndexLength() {
    return (readRegister(MA730_REG_ILIP_PPT_LSB)>>2)&0x0F;
};
uint8_t MA730::getRotationDirection() {
    return (readRegister(MA730_REG_RD)>>7);
};
uint8_t MA730::getFieldStrengthHighThreshold() {
    return (readRegister(MA730_REG_MGLT_MGHT)>>2)&0x07;
};
uint8_t MA730::getFieldStrengthLowThreshold() {
    return (readRegister(MA730_REG_MGLT_MGHT)>>5)&0x07;
};
FieldStrength MA730::getFieldStrength() {
    return (FieldStrength)(readRegister(MA730_REG_MGH_MGL)>>6);
};



void MA730::setZero(uint16_t value) {
    writeRegister(MA730_REG_ZERO_POSITION_MSB, value>>8);
    writeRegister(MA730_REG_ZERO_POSITION_LSB, value&0x00FF);
};
void MA730::setBiasCurrentTrimming(uint8_t value) {
    writeRegister(MA730_REG_BCT, value);
};
void MA730::setBiasCurrrentTrimmingEnabled(bool Xenabled, bool Yenabled) {
    uint8_t val = Xenabled ? 0x01 : 0x00;
    val |= (Yenabled ? 0x02 : 0x00);
    writeRegister(MA730_REG_ET, val);
};
void MA730::setPulsesPerTurn(uint16_t value) {
    uint16_t pptVal = value - 1;
    writeRegister(MA730_REG_PPT_MSB, pptVal>>2);
    uint8_t val = readRegister(MA730_REG_ILIP_PPT_LSB);
    val &= 0x3F;
    val |= (pptVal&0x03)<<6;
    writeRegister(MA730_REG_ILIP_PPT_LSB, val);
};
void MA730::setIndexLength(uint8_t value) {
    uint8_t val = readRegister(MA730_REG_ILIP_PPT_LSB);
    val &= 0xC0;
    val |= ((value<<2)&0x3F);
    writeRegister(MA730_REG_ILIP_PPT_LSB, val);
};
void MA730::setRotationDirection(uint8_t value) {
    if (value==0)
        writeRegister(MA730_REG_RD, 0x00);
    else
        writeRegister(MA730_REG_RD, 0x80);
};
void MA730::setFieldStrengthThresholds(uint8_t high, uint8_t low) {
    uint8_t val = (low<<5) | (high<<2);
    writeRegister(MA730_REG_MGLT_MGHT, val);
};


uint16_t MA730::transfer16(uint16_t outValue) {
    spi->beginTransaction(settings);
    if (nCS >= 0)
        digitalWrite(nCS, LOW);
    uint16_t value = spi->transfer16(outValue);
    if (nCS >= 0)
        digitalWrite(nCS, HIGH);
    spi->endTransaction();
    return value;
};
uint8_t MA730::readRegister(uint8_t reg) {
    uint16_t cmd = 0x4000 | ((reg&0x001F)<<8);
    uint16_t value = transfer16(cmd);
    delayMicroseconds(1);
    value = transfer16(0x0000);
    return value>>8;
};
uint8_t MA730::writeRegister(uint8_t reg, uint8_t value) {
    uint16_t cmd = 0x8000 | ((reg&0x1F)<<8) | value;
    uint16_t result = transfer16(cmd);
    delay(20); // 20ms delay required
    result = transfer16(0x0000);
    return result>>8;
};