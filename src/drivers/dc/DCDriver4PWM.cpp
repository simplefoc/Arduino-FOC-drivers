

#include "./DCDriver4PWM.h"


DCDriver4PWM::DCDriver4PWM(int pinA_h, int pinA_l, int pinB_h, int pinB_l, int pinEN) {
    this->pinA_h = pinA_h;
    this->pinA_l = pinA_l;
    this->pinB_h = pinB_h;
    this->pinB_l = pinB_l;
    this->pinEN = pinEN;
};



int DCDriver4PWM::init() {
    if (pinEN!=NOT_SET) {
        pinMode(pinEN, OUTPUT);
        digitalWrite(pinEN, enable_active_high ? LOW : HIGH);
    }
    pinMode(pinA_h, OUTPUT);
    pinMode(pinA_l, OUTPUT);
    pinMode(pinB_h, OUTPUT);
    pinMode(pinB_l, OUTPUT);
    params = _configure4PWM(pwm_frequency, pinA_h, pinA_l, pinB_h, pinB_l);
    initialized = (params!=SIMPLEFOC_DRIVER_INIT_FAILED);
    return params!=SIMPLEFOC_DRIVER_INIT_FAILED;
};



void DCDriver4PWM::setPwm(float U) {
    // TODO off before on or hald-bridge dead-time insertion?
    // TODO dead-time insertion
    if (U>0.0f) {
        U = _constrain(U, 0.0f, voltage_limit);
        U = _constrain(U/voltage_power_supply,0.0f,1.0f);
        _writeDutyCycle4PWM(U, 0.0f, 0.0f, 1.0f, params);
    } else if (U<0.0f) {
        U = _constrain(-U, 0.0f, voltage_limit);
        U = _constrain(U/voltage_power_supply,0.0f,1.0f);
        _writeDutyCycle4PWM(0.0f, 1.0f, U, 0.0f, params);
    } else {
        _writeDutyCycle4PWM(0.0f, 0.0f, 0.0f, 0.0f, params);
    }
};