

#include "./VoltageSense.h"



VoltageSense::VoltageSense(float gain, float offset, float Tf) : filter(Tf), gain(gain), offset(offset) {
    voltage = 0.0f;
};



float VoltageSense::getVoltage(){
    return voltage;
};



void VoltageSense::update(){
    float v = readRawVoltage();
    v = (v - offset) * gain;
    voltage = filter(v);
};
