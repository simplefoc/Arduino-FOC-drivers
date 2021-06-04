
#include "./MagneticSensorAS5048A.h"
#include "common/foc_utils.h"
#include "common/time_utils.h"

MagneticSensorAS5048A::MagneticSensorAS5048A(int nCS, bool fastMode, SPISettings settings) : AS5048A(settings, nCS), fastMode(fastMode) {

}

MagneticSensorAS5048A::~MagneticSensorAS5048A(){ 

}
void MagneticSensorAS5048A::init(SPIClass* _spi) {
    this->AS5048A::init(_spi);
    this->Sensor::init();
}

float MagneticSensorAS5048A::getSensorAngle() {
    float angle_data = readRawAngle();
    if (!fastMode) // read again to ensure current value
        angle_data = readRawAngle();

    angle_data = ( angle_data / (float)AS5048A_CPR ) * _2PI;
    // return the shaft angle
    return angle_data;
}
