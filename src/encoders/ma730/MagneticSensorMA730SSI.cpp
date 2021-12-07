#include "./MagneticSensorMA730SSI.h"
#include "common/foc_utils.h"
#include "common/time_utils.h"

MagneticSensorMA730SSI::MagneticSensorMA730SSI(SPISettings settings) : settings(settings) {

}


MagneticSensorMA730SSI::~MagneticSensorMA730SSI(){ 

}


void MagneticSensorMA730SSI::init(SPIClass* _spi) {
    this->spi=_spi;
    this->Sensor::init();
}

// check 40us delay between each read?
float MagneticSensorMA730SSI::getSensorAngle() {
    float angle_data = readRawAngle();
    angle_data = ( angle_data / (float)MA730_CPR) * _2PI;
    // return the shaft angle
    return angle_data;
}



uint16_t MagneticSensorMA730SSI::readRawAngle() {
    spi->beginTransaction(settings);
    uint16_t value = spi->transfer16(0x0000);
    uint8_t parity = spi->transfer(0x00);
    // TODO: check parity
    spi->endTransaction();
    return value>>2;
}; // 14bit angle value
