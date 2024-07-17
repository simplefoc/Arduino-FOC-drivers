#include "MA735.h"

MA735::MA735(SPISettings settings, int nCS) : settings(settings), nCS(nCS) {

};
MA735::~MA735() {

};

void MA735::init(SPIClass* _spi) {
    spi = _spi;
    if (nCS >= 0) {
        pinMode(nCS, OUTPUT);
        digitalWrite(nCS, HIGH);
    }
};

float MA735::getCurrentAngle() {
    return (readRawAngle() * _2PI)/MA735_16BIT;//It doesn't matter that it is divided by 65536, because the raw angle fills empty data bits with empty zeros so sensor resolution doesn't affect angle calculation
}; // angle in radians, return current value

uint16_t MA735::readRawAngle() {
    uint16_t angle = transfer16(0x0000);
    return angle;
}; // 9-13bit angle value

uint16_t MA735::getZero() {
    uint16_t result = readRegister(MA735_REG_ZERO_POSITION_MSB)<<8;
    result |= readRegister(MA735_REG_ZERO_POSITION_LSB);
    return result;
};
uint8_t MA735::getBiasCurrentTrimming() {
    return readRegister(MA735_REG_BCT);
};
bool MA735::isBiasCurrrentTrimmingX() {
    return (readRegister(MA735_REG_ET) & 0x01)==0x01;
};
bool MA735::isBiasCurrrentTrimmingY() {
    return (readRegister(MA735_REG_ET) & 0x02)==0x02;
};
uint16_t MA735::getPulsesPerTurn() {
    uint16_t result = readRegister(MA735_REG_ILIP_PPT_LSB)>>6;
    result |= ((uint16_t)readRegister(MA735_REG_PPT_MSB))<<2;
    return result+1;
};
uint8_t MA735::getIndexLength() {
    return (readRegister(MA735_REG_ILIP_PPT_LSB)>>2)&0x0F;
};
uint8_t MA735::getRotationDirection() {
    return (readRegister(MA735_REG_RD)>>7);
};
uint8_t MA735::getFieldStrengthHighThreshold() {
    return (readRegister(MA735_REG_MGLT_MGHT)>>2)&0x07;
};
uint8_t MA735::getFieldStrengthLowThreshold() {
    return (readRegister(MA735_REG_MGLT_MGHT)>>5)&0x07;
};
FieldStrength MA735::getFieldStrength() {
    return (FieldStrength)(readRegister(MA735_REG_MGH_MGL)>>6);
};
uint8_t MA735::getFilterWindow() {
    return readRegister(MA735_REG_FW);
};
uint8_t MA735::getHysteresis() {
    return readRegister(MA735_REG_HYS);
};
float MA735::getResolution() {
    //All I could find in the datasheet was a table with the correlation, no function to convert Filter window to res.
    uint8_t reg = readRegister(MA735_REG_FW);
    float result;
    switch (reg) {
        case 51: 
            result = 9.0;
            break;
        case 68: 
            result = 9.5;
            break;
        case 85: 
            result = 10.0;
            break;
        case 102: 
            result = 10.5;
            break;
        case 119: 
            result = 11.0;
            break;
        case 136: 
            result = 11.5;
            break;
        case 153: 
            result = 12.0;
            break;
        case 170: 
            result = 12.5;
            break;
        case 187: 
            result = 13.0;
            break;
        default:
            result = 11.0;
            break;
    }
    return result;
};
int MA735::getUpdateTime() {
    //All I could find in the datasheet was a table with the correlation, no function to convert Filter window to update time.
    //Returns result in microseconds
    uint8_t reg = readRegister(MA735_REG_FW);
    int result;
    switch (reg) {
        case 51: 
            result = 64;
            break;
        case 68: 
            result = 128;
            break;
        case 85: 
            result = 256;
            break;
        case 102: 
            result = 512;
            break;
        case 119: 
            result = 1024;
            break;
        case 136: 
            result = 2048;
            break;
        case 153: 
            result = 4096;
            break;
        case 170: 
            result = 8192;
            break;
        case 187: 
            result = 16384;
            break;
        default:
            result = 1024;
            break;
    }
    return result;
};



void MA735::setZero(uint16_t value) {
    writeRegister(MA735_REG_ZERO_POSITION_MSB, value>>8);
    writeRegister(MA735_REG_ZERO_POSITION_LSB, value&0x00FF);
};
void MA735::setBiasCurrentTrimming(uint8_t value) {
    writeRegister(MA735_REG_BCT, value);
};
void MA735::setBiasCurrrentTrimmingEnabled(bool Xenabled, bool Yenabled) {
    uint8_t val = Xenabled ? 0x01 : 0x00;
    val |= (Yenabled ? 0x02 : 0x00);
    writeRegister(MA735_REG_ET, val);
};
void MA735::setPulsesPerTurn(uint16_t value) {
    uint16_t pptVal = value - 1;
    writeRegister(MA735_REG_PPT_MSB, pptVal>>2);
    uint8_t val = readRegister(MA735_REG_ILIP_PPT_LSB);
    val &= 0x3F;
    val |= (pptVal&0x03)<<6;
    writeRegister(MA735_REG_ILIP_PPT_LSB, val);
};
void MA735::setIndexLength(uint8_t value) {
    uint8_t val = readRegister(MA735_REG_ILIP_PPT_LSB);
    val &= 0xC0;
    val |= ((value<<2)&0x3F);
    writeRegister(MA735_REG_ILIP_PPT_LSB, val);
};
void MA735::setRotationDirection(uint8_t value) {
    if (value==0)
        writeRegister(MA735_REG_RD, 0x00);
    else
        writeRegister(MA735_REG_RD, 0x80);
};
void MA735::setFieldStrengthThresholds(uint8_t high, uint8_t low) {
    uint8_t val = (low<<5) | (high<<2);
    writeRegister(MA735_REG_MGLT_MGHT, val);
};
void MA735::setFilterWindow(uint8_t value) {
    writeRegister(MA735_REG_FW, value);
};
void MA735::setHysteresis(uint8_t value) {
    writeRegister(MA735_REG_HYS, value);
};
void MA735::setResolution(float res) {
    //All I could find in the datasheet was a table with the correlation, no function to convert Filter window to res.
    uint8_t value;
    uint8_t res_int = res * 10;//It has to be a basic type for the switch case
    switch (res_int) {
        case 90: 
            value = 51;
            break;
        case 95: 
            value = 68;
            break;
        case 100: 
            value = 85;
            break;
        case 105: 
            value = 102;
            break;
        case 110: 
            value = 119;
            break;
        case 115: 
            value = 136;
            break;
        case 120: 
            value = 153;
            break;
        case 125: 
            value = 170;
            break;
        case 130: 
            value = 187;
            break;
        default:
            value = 119;
            break;
    }
    writeRegister(MA735_REG_FW, value);
};
void MA735::setUpdateTime(int microsec) {
    //All I could find in the datasheet was a table with the correlation, no function to convert Filter window to update time.
    //time in microseconds
    uint8_t value;
    switch (microsec) {
        case 64: 
            value = 51;
            break;
        case 128: 
            value = 68;
            break;
        case 256: 
            value = 85;
            break;
        case 512: 
            value = 102;
            break;
        case 1024: 
            value = 119;
            break;
        case 2048: 
            value = 136;
            break;
        case 4096: 
            value = 153;
            break;
        case 8192: 
            value = 170;
            break;
        case 16384: 
            value = 187;
            break;
        default:
            value = 119;
            break;
    }
    writeRegister(MA735_REG_FW, value);
};


uint16_t MA735::transfer16(uint16_t outValue) {
    spi->beginTransaction(settings);
    if (nCS >= 0)
        digitalWrite(nCS, LOW);
    uint16_t value = spi->transfer16(outValue);
    if (nCS >= 0)
        digitalWrite(nCS, HIGH);
    spi->endTransaction();
    return value;
};
uint8_t MA735::readRegister(uint8_t reg) {
    uint16_t cmd = 0x4000 | ((reg&0x001F)<<8);
    uint16_t value = transfer16(cmd);
    delayMicroseconds(1);
    value = transfer16(0x0000);
    return value>>8;
};
uint8_t MA735::writeRegister(uint8_t reg, uint8_t value) {
    uint8_t write_check = readRegister(reg);
    //no need to rewrite, it is the exact same value
    if (write_check == value) {
        return write_check;
    }
    else {
        uint16_t cmd = 0x8000 | ((reg&0x1F)<<8) | value;
        uint16_t result = transfer16(cmd);
        delay(20); // 20ms delay required
        result = transfer16(0x0000);
        return result>>8;
    }
};
