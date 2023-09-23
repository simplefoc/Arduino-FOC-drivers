

#include "./MagneticSensorAS5600.h"
#include "common/foc_utils.h"

MagneticSensorAS5600::MagneticSensorAS5600(uint8_t _address) : AS5600(_address) {};
MagneticSensorAS5600::~MagneticSensorAS5600() {};

void MagneticSensorAS5600::init(TwoWire* wire) {
  AS5600::init(wire);
  Sensor::init();
};


float MagneticSensorAS5600::getSensorAngle() {
    uint16_t raw = readRawAngle();
    return raw / AS5600_CPR * _2PI;
};