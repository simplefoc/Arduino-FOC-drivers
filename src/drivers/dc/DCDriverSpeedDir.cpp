


#include "./DCDriverSpeedDir.h"


DCDriverSpeedDir::DCDriverSpeedDir(int pinPWM, int pinDIR, int pinEN) {
    this->pinPWM = pinPWM;
    this->pinDIR = pinDIR;
    this->pinEN = pinEN;
};


int DCDriverSpeedDir::init() {
    if (pinEN!=NOT_SET) {
        pinMode(pinEN, OUTPUT);
        digitalWrite(pinEN, enable_active_high ? LOW : HIGH);
    }
    pinMode(pinDIR, OUTPUT);
    digitalWrite(pinDIR, HIGH);
    pinMode(pinPWM, OUTPUT);
    params = _configure1PWM(pwm_frequency, pinPWM);
    initialized = (params!=SIMPLEFOC_DRIVER_INIT_FAILED);
    return params!=SIMPLEFOC_DRIVER_INIT_FAILED;
};



void DCDriverSpeedDir::setPwm(float U){
    if (U>0.0f) {
        U = _constrain(U, 0.0f, voltage_limit);
        U = _constrain(U/voltage_power_supply,0.0f,1.0f);
        _writeDutyCycle1PWM(U, params);
        digitalWrite(pinDIR, HIGH);
    } else if (U<0.0f) {
        U = _constrain(-U, 0.0f, voltage_limit);
        U = _constrain(U/voltage_power_supply,0.0f,1.0f);
        _writeDutyCycle1PWM(U, params);
        digitalWrite(pinDIR, LOW);
    } else {
        _writeDutyCycle1PWM(0, params);
    }
};