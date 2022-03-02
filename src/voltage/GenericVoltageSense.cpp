
#include "GenericVoltageSense.h"
#include "Arduino.h"



GenericVoltageSense::GenericVoltageSense(int pin, float fullScaleVoltage, float gain, float offset, float Tf) : VoltageSense(gain, offset, Tf), pin(pin), fullScaleVoltage(fullScaleVoltage) {
};




bool GenericVoltageSense::init(int resolution){
    pinMode(pin, INPUT_ANALOG);
    if (resolution>0) {
        analogReadResolution(resolution);
        maxValue = powl(2, resolution);
    }
    else
        maxValue = powl(2, ADC_RESOLUTION);
};





float GenericVoltageSense::readRawVoltage(){
    uint32_t val = analogRead(pin);
    val = (val / maxValue) * fullScaleVoltage;
    return val;
};

