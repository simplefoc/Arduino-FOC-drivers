#include "./MagneticSensorAS5145.h"
#include "common/foc_utils.h"
#include "common/time_utils.h"

MagneticSensorAS5145::MagneticSensorAS5145(SPISettings settings) : settings(settings) {

}


MagneticSensorAS5145::~MagneticSensorAS5145() {

}

void MagneticSensorAS5145::init(SPIClass* _spi) {
    this->spi=_spi;
    this->Sensor::init();
}

// check 40us delay between each read?
float MagneticSensorAS5145::getSensorAngle() {
    float angle_data = readRawAngleSSI();
    angle_data = ( (float)angle_data / AS5145_CPR ) * _2PI;
    // return the shaft angle
    return angle_data;
}


uint16_t MagneticSensorAS5145::readRawAngleSSI() {
    spi->beginTransaction(settings);
    uint16_t value = spi->transfer16(0x0000);
    //uint16_t parity = spi->transfer(0x00);
    spi->endTransaction();
    return (value>>3)&0x1FFF; // TODO this isn't what I expected from the datasheet... maybe there's a leading 0 bit?
}; // 12bit angle value
