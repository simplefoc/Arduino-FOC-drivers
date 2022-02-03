#include "./MagneticSensorAEAT8800Q24.h"
#include "common/foc_utils.h"
#include "common/time_utils.h"

MagneticSensorAEAT8800Q24::MagneticSensorAEAT8800Q24(int nCS, int pinNSL, SPISettings spiSettings, SPISettings ssiSettings) : AEAT8800Q24(nCS, pinNSL, spiSettings, ssiSettings) {
};


MagneticSensorAEAT8800Q24::~MagneticSensorAEAT8800Q24(){ 
};


void MagneticSensorAEAT8800Q24::init(SPIClass* _spi) {
    this->AEAT8800Q24::init(_spi);
    this->Sensor::init();
};


float MagneticSensorAEAT8800Q24::getSensorAngle() {
    float angle_data = readRawAngle();
    angle_data = ( angle_data / (float)AEAT8800Q24_CPR) * _2PI;
    // return the shaft angle
    return angle_data;
};
