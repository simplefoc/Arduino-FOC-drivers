#ifndef __VOLTAGESENSE_H__
#define __VOLTAGESENSE_H__

#include "common/lowpass_filter.h"

class VoltageSense {
  public:
    VoltageSense(float gain = 1.0f, float offset = 0.0f, float Tf = 0.1f);
    float getVoltage();
    virtual void update();

    LowPassFilter filter;
    float gain;
    float offset;

  protected:
    virtual float readRawVoltage() = 0;

    float voltage;
};



#endif