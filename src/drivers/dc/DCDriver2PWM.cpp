

#include "./DCDriver2PWM.h"



DCDriver2PWM::DCDriver2PWM(int pinA, int pinB, int pinEN) {
    this->pinA = pinA;
    this->pinB = pinB;
    this->pinEN = pinEN;
};



int DCDriver2PWM::init() {
    if (pinEN!=NOT_SET) {
        pinMode(pinEN, OUTPUT);
        digitalWrite(pinEN, enable_active_high ? LOW : HIGH);
    }
    pinMode(pinA, OUTPUT);
    pinMode(pinB, OUTPUT);
    params = _configure2PWM(pwm_frequency, pinA, pinB);
    initialized = (params!=SIMPLEFOC_DRIVER_INIT_FAILED);
    return params!=SIMPLEFOC_DRIVER_INIT_FAILED;
};



void DCDriver2PWM::setPwm(float U){
    // TODO off before on or hald-bridge dead-time insertion?
    if (U>0.0f) {
        U = _constrain(U, 0.0f, voltage_limit);
        U = _constrain(U/voltage_power_supply,0.0f,1.0f);
        _writeDutyCycle2PWM(U, 0, params);
    } else if (U<0.0f) {
        U = _constrain(-U, 0.0f, voltage_limit);
        U = _constrain(U/voltage_power_supply,0.0f,1.0f);
        _writeDutyCycle2PWM(0, U, params);
    } else {
        _writeDutyCycle2PWM(0, 0, params);
    }
};