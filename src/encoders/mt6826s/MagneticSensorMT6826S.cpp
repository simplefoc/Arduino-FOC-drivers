
#include "MagneticSensorMT6826S.h"


MagneticSensorMT6826S::MagneticSensorMT6826S(int nCS, SPISettings settings) : Sensor(), MT6826S(settings, nCS) {
    // nix
};


MagneticSensorMT6826S::~MagneticSensorMT6826S() {
    // nix
};


float MagneticSensorMT6826S::getSensorAngle() {
    return getCurrentAngle();
};


void MagneticSensorMT6826S::init(SPIClass* _spi) {
    this->MT6826S::init(_spi);
    this->Sensor::init();
};

