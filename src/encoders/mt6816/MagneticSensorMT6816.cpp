
#include "common/foc_utils.h"
#include "common/time_utils.h"
#include "MagneticSensorMT6816.h"


MagneticSensorMT6816::MagneticSensorMT6816(int nCS, SPISettings settings) : MT6816(settings, nCS) {
}

MagneticSensorMT6816::~MagneticSensorMT6816(){
}

void MagneticSensorMT6816::init(SPIClass* _spi) {
    this->MT6816::init(_spi);
    this->Sensor::init();
}

float MagneticSensorMT6816::getSensorAngle() {
    uint16_t raw_angle_data = readRawAngle();

    if (this->MT6816::isNoMagneticReading()) {
        return 0;
    }

    return static_cast<float>(raw_angle_data) / MT6816_CPR * _2PI;
}