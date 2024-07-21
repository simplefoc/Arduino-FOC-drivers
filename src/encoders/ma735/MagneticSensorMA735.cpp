#include "./MagneticSensorMA735.h"
#include "common/foc_utils.h"
#include "common/time_utils.h"

MagneticSensorMA735::MagneticSensorMA735(int nCS, SPISettings settings) : MA735(settings, nCS) {

}


MagneticSensorMA735::~MagneticSensorMA735(){ 

}


void MagneticSensorMA735::init(SPIClass* _spi) {
    this->MA735::init(_spi);
    this->Sensor::init();
}


float MagneticSensorMA735::getSensorAngle() {
    float angle_data = readRawAngle();
    angle_data = ( angle_data / (float)MA735_16BIT) * _2PI;//It doesn't matter that it is divided by 65536, because the raw angle fills empty data bits with empty zeros so sensor resolution doesn't affect angle calculation
    // return the shaft angle
    return angle_data;
}
