
#pragma once

#include "BLDCMotor.h"


enum BrakeMode : uint8_t {
    brakemode_none           = 0x00,    // no braking
    brakemode_passive        = 0x01,    // brake by opening low-side FETs - all drivers can do this
    brakemode_daxis          = 0x02,    // brake by applying a voltage to the D axis
    brakemode_velocity       = 0x03,    // brake by driving the motor against its current motion
    brakemode_extern         = 0x04,    // brake by switching the FET of an external brake circuit
    brakemode_regen          = 0x05,    // brake by switching motor FETs using a control loop to control voltage and current
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
    void coast();

protected:
    BrakeMode brakeMode = brakemode_none;
    RegenMode regenMode = regenmode_none;
    bool autoCoast = false;
    float regenVoltage = NOT_SET;

};
