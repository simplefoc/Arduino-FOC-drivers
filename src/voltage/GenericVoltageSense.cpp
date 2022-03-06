
#include "GenericVoltageSense.h"
#include "Arduino.h"



GenericVoltageSense::GenericVoltageSense(int pin, float fullScaleVoltage, float gain, float offset, float Tf) : VoltageSense(gain, offset, Tf), fullScaleVoltage(fullScaleVoltage), pin(pin) {
};




bool GenericVoltageSense::init(int resolution){
    pinMode(pin, INPUT);
    if (resolution>0) {
        analogReadResolution(resolution);
        maxValue = powl(2, resolution);
    }
    else
        maxValue = powl(2, ADC_RESOLUTION);
    return true;
};





float GenericVoltageSense::readRawVoltage(){
    uint32_t val = analogRead(pin);
    val = (val / maxValue) * fullScaleVoltage;
    return val;
};

