#include "./MagneticSensorMA330.h"
#include "common/foc_utils.h"
#include "common/time_utils.h"

MagneticSensorMA330::MagneticSensorMA330(int nCS, SPISettings settings) : MA330(settings, nCS) {

}


MagneticSensorMA330::~MagneticSensorMA330(){ 

}


void MagneticSensorMA330::init(SPIClass* _spi) {
    this->MA330::init(_spi);
    this->Sensor::init();
}


float MagneticSensorMA330::getSensorAngle() {
    float angle_data = readRawAngle();
    angle_data = ( angle_data / (float)MA330_CPR) * _2PI;
    // return the shaft angle
    return angle_data;
}
