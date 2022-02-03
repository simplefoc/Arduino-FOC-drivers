
#include "./MagneticSensorTLE5012B.h"

#if defined(_STM32_DEF_)

MagneticSensorTLE5012B::MagneticSensorTLE5012B(int data, int sck, int nCS) : TLE5012B(data, sck, nCS) { };
MagneticSensorTLE5012B::~MagneticSensorTLE5012B(){ };

void MagneticSensorTLE5012B::init() {
    this->TLE5012B::init();
    this->Sensor::init();
};

float MagneticSensorTLE5012B::getSensorAngle() {
    return getCurrentAngle();
};

#endif
