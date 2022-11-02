
#include "BrakingBLDCMotor.h"


BrakingBLDCMotor::BrakingBLDCMotor(int pp, float R, float KV, BrakeMode brakeMode, RegenMode regenMode) : BLDCMotor(pp, R, KV) {
    this->brakeMode = brakeMode;
    this->regenMode = regenMode;
};


BrakingBLDCMotor::~BrakingBLDCMotor() {
    // nothing to do
};


void BrakingBLDCMotor::setBrakeMode(BrakeMode mode) {
    this->brakeMode = mode;
};


void BrakingBLDCMotor::setRegenMode(RegenMode mode) {
    this->regenMode = mode;
};


void BrakingBLDCMotor::setAutoCoast(bool enable) {
    this->autoCoast = enable;
};


void BrakingBLDCMotor::setRegenVoltage(float voltage) {
    this->regenVoltage = voltage;
};


void BrakingBLDCMotor::linkDriver(BLDCDriver *driver) {
    // TODO implement sanity check - check driver for correct mode
    BLDCMotor::linkDriver(driver);
};


void BrakingBLDCMotor::loopFOC() {
    // TODO implement regen mode
    BLDCMotor::loopFOC();
};


void BrakingBLDCMotor::move(float target) {
    // TODO implement brake mode
    BLDCMotor::move(target);
};


void BrakingBLDCMotor::brake(float target) {
    // TODO implement active braking
};

