
#include "./MagneticSensorAS5047U.h"
#include "common/foc_utils.h"
#include "common/time_utils.h"

MagneticSensorAS5047U::MagneticSensorAS5047U(int nCS, bool fastMode, SPISettings settings) : AS5047U(settings, nCS), fastMode(fastMode) {

}


MagneticSensorAS5047U::~MagneticSensorAS5047U(){ 

}


void MagneticSensorAS5047U::init(SPIClass* _spi) {
    this->AS5047U::init(_spi);
    this->Sensor::init();
}


float MagneticSensorAS5047U::getSensorAngle() {
    float angle_data = readRawAngle();
    if (!fastMode) // read again to ensure current value
        angle_data = readRawAngle();
    angle_data = ( angle_data / (float)AS5047U_CPR) * _2PI;
    // return the shaft angle
    return angle_data;
}
