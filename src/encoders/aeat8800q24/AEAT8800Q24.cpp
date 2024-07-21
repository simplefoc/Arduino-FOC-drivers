
#include "AEAT8800Q24.h"


AEAT8800Q24::AEAT8800Q24(int nCS, int pinNSL, SPISettings spiSettings, SPISettings ssiSettings) : nCS(nCS), pinNSL(pinNSL), spiSettings(spiSettings), ssiSettings(ssiSettings) {
};
AEAT8800Q24::~AEAT8800Q24(){
};

void AEAT8800Q24::init(SPIClass* _spi){
    spi = _spi;
    spi->beginTransaction(ssiSettings);
};


float AEAT8800Q24::getCurrentAngle() {
    return ((float)readRawAngle())/(float)AEAT8800Q24_CPR * 2.0f * (float)PI;
}; // angle in radians, return current value


// reads angle via ssi
uint16_t AEAT8800Q24::readRawAngle() {
    // digitalWrite(pinNSL, LOW); //TODO maybe we don't need it...
    // // 300ns delay
    // delayMicroseconds(1);
    uint16_t value = spi->transfer16(0x0000);
    uint8_t status = spi->transfer(0x00);
    //digitalWrite(pinNSL, HIGH);
    // // 200ns delay before next read...
    // delayMicroseconds(1);
    lastStatus.reg = status;
    return value;
};

AEAT8800Q24_Status_t AEAT8800Q24::getLastStatus() {
    return lastStatus;
}

uint16_t AEAT8800Q24::getZero(){
    uint8_t value = readRegister(AEAT8800Q24_REG_ZERO_MSB);
    return (value<<8)|readRegister(AEAT8800Q24_REG_ZERO_LSB);
};
void AEAT8800Q24::setZero(uint16_t value){
    writeRegister(AEAT8800Q24_REG_ZERO_MSB, (value>>8)&0xFF);
    writeRegister(AEAT8800Q24_REG_ZERO_LSB, value&0xFF);
};

AEAT8800Q24_CONF0_t AEAT8800Q24::getConf0(){
    AEAT8800Q24_CONF0_t result;
    result.reg = readRegister(AEAT8800Q24_REG_CONF0);
    return result;
};
void AEAT8800Q24::setConf0(AEAT8800Q24_CONF0_t value){
    writeRegister(AEAT8800Q24_REG_CONF0, value.reg);
};

AEAT8800Q24_CONF1_t AEAT8800Q24::getConf1(){
    AEAT8800Q24_CONF1_t result;
    result.reg = readRegister(AEAT8800Q24_REG_CONF1);
    return result;
};
void AEAT8800Q24::setConf1(AEAT8800Q24_CONF1_t value){
    writeRegister(AEAT8800Q24_REG_CONF1, value.reg);
};

AEAT8800Q24_CONF2_t AEAT8800Q24::getConf2(){
    AEAT8800Q24_CONF2_t result;
    result.reg = readRegister(AEAT8800Q24_REG_CONF2);
    return result;
};
void AEAT8800Q24::setConf2(AEAT8800Q24_CONF2_t value){
    writeRegister(AEAT8800Q24_REG_CONF2, value.reg);
};




uint16_t AEAT8800Q24::transfer16SPI(uint16_t outValue) {
    // delay 1us between switching the CS line to SPI
    delayMicroseconds(1);
    spi->endTransaction();
    spi->beginTransaction(spiSettings);
    if (nCS >= 0)
        digitalWrite(nCS, LOW);    
    uint16_t value = spi->transfer16(outValue);
    if (nCS >= 0)
        digitalWrite(nCS, HIGH);
    spi->endTransaction();
    // delay 1us between switching the CS line to SSI
    delayMicroseconds(1);
    spi->beginTransaction(ssiSettings);
    return value;
};
uint8_t AEAT8800Q24::readRegister(uint8_t reg) {
    uint16_t cmd = 0x8000 | ((reg&0x001F)<<8);
    uint16_t value = transfer16SPI(cmd);
    return value&0x00FF;
};
void AEAT8800Q24::writeRegister(uint8_t reg, uint8_t value) {
    uint16_t cmd = 0x4000 | ((reg&0x1F)<<8) | value;
    /*uint16_t result =*/ transfer16SPI(cmd);
};