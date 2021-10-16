#include "./MagneticSensorMA730.h"
#include "common/foc_utils.h"
#include "common/time_utils.h"

MagneticSensorMA730::MagneticSensorMA730(int nCS, SPISettings settings) : MA730(settings, nCS) {

}


MagneticSensorMA730::~MagneticSensorMA730(){ 

}


void MagneticSensorMA730::init(SPIClass* _spi) {
    this->MA730::init(_spi);
    this->Sensor::init();
}


float MagneticSensorMA730::getSensorAngle() {
    float angle_data = readRawAngle();
    angle_data = ( angle_data / (float)MA730_CPR) * _2PI;
    // return the shaft angle
    return angle_data;
}
