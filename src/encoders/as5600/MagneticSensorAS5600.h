
#pragma once


#include "./AS5600.h"
#include "common/base_classes/Sensor.h"


class MagneticSensorAS5600 : public Sensor, public AS5600 {

public:
    MagneticSensorAS5600(uint8_t _address = 0x36);
    ~MagneticSensorAS5600();

    virtual void init(TwoWire* wire = &Wire);

    virtual float getSensorAngle() override;    

protected:

};

