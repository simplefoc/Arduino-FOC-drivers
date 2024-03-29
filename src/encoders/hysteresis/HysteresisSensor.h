
#pragma once


#include "Arduino.h"
#include "common/base_classes/FOCMotor.h"
#include "common/base_classes/Sensor.h"

class HysteresisSensor : public Sensor {
public:
    HysteresisSensor(Sensor& wrapped, float amount = 0.0125f);

    float getSensorAngle() override;

    void init() override;

    float _amount;
protected:
    Sensor& _wrapped;
    float _window;

};

