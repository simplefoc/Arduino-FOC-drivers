
#include "./MagneticSensorSC60228.h"
#include "common/foc_utils.h"
#include "common/time_utils.h"

MagneticSensorSC60228::MagneticSensorSC60228(int nCS, SPISettings settings) : SC60228(settings, nCS){
    // nix
};
MagneticSensorSC60228::~MagneticSensorSC60228(){ };



float MagneticSensorSC60228::getSensorAngle(){
    SC60228Angle angle_data = readRawAngle();
    float result = ( angle_data.angle / (float)SC60228_CPR ) * _2PI;
    return result;
};



void MagneticSensorSC60228::init(SPIClass* _spi){
    this->SC60228::init(_spi);
    this->Sensor::init();
};