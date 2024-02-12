#include "./MagneticSensorMT6701SSI.h"
#include "common/foc_utils.h"
#include "common/time_utils.h"

MagneticSensorMT6701SSI::MagneticSensorMT6701SSI(int nCS, SPISettings settings) : settings(settings), nCS(nCS) {

}


MagneticSensorMT6701SSI::~MagneticSensorMT6701SSI() {

}

void MagneticSensorMT6701SSI::init(SPIClass* _spi) {
    this->spi=_spi;
    if (nCS >= 0) {
        pinMode(nCS, OUTPUT);
        digitalWrite(nCS, HIGH);
    }
    this->spi->begin();
    this->Sensor::init();
}

// check 40us delay between each read?
float MagneticSensorMT6701SSI::getSensorAngle() {
    float angle_data = readRawAngleSSI();
    angle_data = ( angle_data / (float)MT6701_CPR ) * _2PI;
    // return the shaft angle
    return angle_data;
}


uint16_t MagneticSensorMT6701SSI::readRawAngleSSI() {
    spi->beginTransaction(settings);
    if (nCS >= 0)
        digitalWrite(nCS, LOW);
    uint16_t value = spi->transfer16(0x0000);
    if (nCS >= 0)
        digitalWrite(nCS, HIGH);
    spi->endTransaction();
    return (value>>MT6701_DATA_POS)&0x3FFF;
};
