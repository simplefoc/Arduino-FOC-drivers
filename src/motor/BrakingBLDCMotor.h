
#pragma once

#include "BLDCMotor.h"


enum BrakeMode : uint8_t {
    brakemode_none,
    brakemode_daxis,
    brakemode_extern
};

enum RegenMode : uint8_t {
    regenmode_none,
    regenmode_3pwm,
    regenmode_6pwm
};


class BrakingBLDCMotor : public BLDCMotor {
public:
    BrakingBLDCMotor(int pp, float R = NOT_SET, float KV = NOT_SET, BrakeMode brakeMode = brakemode_none, RegenMode regenMode = regenmode_none);
    ~BrakingBLDCMotor();

    void setBrakeMode(BrakeMode mode);
    void setRegenMode(RegenMode mode);
    void setAutoCoast(bool enable);
    void setRegenVoltage(float voltage);

    void linkDriver(BLDCDriver *driver) override;

    void loopFOC() override;
    void move(float target) override;
    void brake(float target);

protected:
    BrakeMode brakeMode = brakemode_none;
    RegenMode regenMode = regenmode_none;
    bool autoCoast = false;
    float regenVoltage = NOT_SET;

};
