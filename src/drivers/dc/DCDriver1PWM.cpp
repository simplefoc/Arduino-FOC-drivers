

#include "./DCDriver1PWM.h"

DCDriver1PWM::DCDriver1PWM(int pinPWM, float threshold, int pinEN) {
    this->pinPWM = pinPWM;
    this->threshold = threshold;
    this->pinEN = pinEN;
};


int DCDriver1PWM::init() {
    if (pinEN!=NOT_SET) {
        pinMode(pinEN, OUTPUT);
        digitalWrite(pinEN, enable_active_high ? LOW : HIGH);
    }
    pinMode(pinPWM, OUTPUT);
    params = _configure1PWM(pwm_frequency, pinPWM);
    initialized = (params!=SIMPLEFOC_DRIVER_INIT_FAILED);
    return params!=SIMPLEFOC_DRIVER_INIT_FAILED;
};


void DCDriver1PWM::setPwm(float U){
    U = _constrain(U, -voltage_limit, voltage_limit);
    if (U>0.0f) {
        U = ( U/voltage_power_supply ) * (1.0f-threshold);
        U = _constrain(U,threshold,1.0f);
        _writeDutyCycle1PWM(U, params);
    } else if (U<0.0f) {
        if (!scale_reverse)
            U = ( -U/voltage_power_supply ) * (1.0f-threshold); // same scale as forward
        else
            U = ( -U/voltage_power_supply ) * (threshold);      // full reverse scale
        U = _constrain(U,threshold,1.0f);
        _writeDutyCycle1PWM(U, params);
    } else {
        _writeDutyCycle1PWM(threshold, params);
    }
};