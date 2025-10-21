
#include "MagneticSensorKTH7812.h"


MagneticSensorKTH7812::MagneticSensorKTH7812(int nCS, bool withcrc, bool fastmode, SPISettings settings) : Sensor(), KTH7812(settings, nCS, fastmode, withcrc) {
    // nix
};


MagneticSensorKTH7812::~MagneticSensorKTH7812() {
    // nix
};


float MagneticSensorKTH7812::getSensorAngle() {
    return getCurrentAngle();
};


void MagneticSensorKTH7812::init(SPIClass* _spi) {
    this->KTH7812::init(_spi);
    this->Sensor::init();
};

