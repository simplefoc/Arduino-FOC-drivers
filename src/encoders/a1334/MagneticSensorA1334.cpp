
#include "./MagneticSensorA1334.h"
#include "common/foc_utils.h"
#include "common/time_utils.h"

MagneticSensorA1334::MagneticSensorA1334(int nCS, SPISettings settings) : A1334(settings, nCS) {

}


MagneticSensorA1334::~MagneticSensorA1334(){ 

}


void MagneticSensorA1334::init(SPIClass* _spi) {
    this->A1334::init(_spi);
    this->Sensor::init();
}


float MagneticSensorA1334::getSensorAngle() {
    A1334Angle angle_data = readRawAngle();
    float result = ( angle_data.angle / (float)A1334_CPR) * _2PI;
    // return the shaft angle
    return result;
}
