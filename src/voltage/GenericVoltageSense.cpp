
#include "GenericVoltageSense.h"
#include "Arduino.h"



GenericVoltageSense::GenericVoltageSense(int pin, float gain, float offset, float Tf, float fullScaleVoltage) : VoltageSense(gain, offset, Tf), fullScaleVoltage(fullScaleVoltage), pin(pin) {
};

#ifndef INPUT_ANALOG
#define INPUT_ANALOG INPUT
#endif

#ifndef ADC_RESOLUTION
#define ADC_RESOLUTION 10
#endif


bool GenericVoltageSense::init(int resolution){
    pinMode(pin, INPUT_ANALOG);
#if !defined(ARDUINO_ARCH_AVR) && !defined(ARDUINO_ARCH_MEGAAVR)
    if (resolution>0) {
        analogReadResolution(resolution);
        maxValue = _powtwo(resolution);
    }
    else {
        maxValue = _powtwo(ADC_RESOLUTION);
    }
#else
    maxValue = _powtwo(ADC_RESOLUTION);
#endif
    return true;
};





float GenericVoltageSense::readRawVoltage(){
    float val = analogRead(pin);
    val = (val / (float)maxValue) * fullScaleVoltage;
    return val;
};

