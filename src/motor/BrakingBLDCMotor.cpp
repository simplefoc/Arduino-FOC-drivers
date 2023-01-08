
#include "BrakingBLDCMotor.h"


BrakingBLDCMotor::BrakingBLDCMotor(int pp, float R, float KV, enum BrakeMode brakeMode, enum RegenMode regenMode) : BLDCMotor(pp, R, KV) {
    this->brakeMode = brakeMode;
    this->regenMode = regenMode;
};


BrakingBLDCMotor::~BrakingBLDCMotor() {
    // nothing to do
};


void BrakingBLDCMotor::setBrakeMode(enum BrakeMode mode) {
    this->brakeMode = mode;
};


void BrakingBLDCMotor::setRegenMode(enum RegenMode mode) {
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
    driver->setPhaseState(PhaseState::PHASE_ON, PhaseState::PHASE_ON, PhaseState::PHASE_ON);
    BLDCMotor::move(target);
};


void BrakingBLDCMotor::brake(float target) {
    // TODO this code is very unfinished, and should be considered conceptual! It certainly hasn't been tested, and may be completely wrong!
    switch (brakeMode) {
        case brakemode_passive:
            driver->setPhaseState(PhaseState::PHASE_LO, PhaseState::PHASE_LO, PhaseState::PHASE_LO);
            driver->setPwm(target, target, target); // target expressed as voltage, is this what we want?
            break;
        case brakemode_velocity: // maybe we should give this another name
            driver->setPhaseState(PhaseState::PHASE_ON, PhaseState::PHASE_ON, PhaseState::PHASE_ON);
            this->target = target; // target expressed as velocity, is this what we want?
            // FOC loop will automatically attempt to reach target velocity
            break;
        case brakemode_extern:
            // open high-side FETs ?
            driver->setPhaseState(PhaseState::PHASE_HI, PhaseState::PHASE_HI, PhaseState::PHASE_HI);
            driver->setPwm(driver->voltage_power_supply, driver->voltage_power_supply, driver->voltage_power_supply);
            // TODO implement external brake circuit
            // brakeDriver->setPwm(target);
            break;
        case brakemode_regen:
            // TODO this has to be implemented in the FOC loop
        case brakemode_daxis:
            // TODO this has to be implemented in the FOC loop
        default:
        // not implemented :-(
        case brakemode_none:
            break;
    }
};

void BrakingBLDCMotor::coast(bool coast) {
    if (coast) // next call to loopFOC() will set the phase state to PHASE_OFF
        driver->setPhaseState(PhaseState::PHASE_OFF, PhaseState::PHASE_OFF, PhaseState::PHASE_OFF);
    else
        driver->setPhaseState(PhaseState::PHASE_ON, PhaseState::PHASE_ON, PhaseState::PHASE_ON);
};