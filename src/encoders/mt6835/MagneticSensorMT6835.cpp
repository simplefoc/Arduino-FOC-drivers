
#include "MagneticSensorMT6835.h"


MagneticSensorMT6835::MagneticSensorMT6835(int nCS, SPISettings settings) : MT6835(settings, nCS), Sensor() {
    // nix
};


MagneticSensorMT6835::~MagneticSensorMT6835() {
    // nix
};


float MagneticSensorMT6835::getSensorAngle() {
    return getCurrentAngle();
};


void MagneticSensorMT6835::init(SPIClass* _spi) {
    this->MT6835::init(_spi);
    this->Sensor::init();
};

