
#ifndef __GENERICVOLTAGESENSE_H__
#define __GENERICVOLTAGESENSE_H__

#include "./VoltageSense.h"
#include "common/foc_utils.h"

class GenericVoltageSense : public VoltageSense {
  public:
    GenericVoltageSense(int pin, float gain = 1.0f, float offset = 0.0f, float Tf = 0.1f, float fullScaleVoltage = 3.3f);
    bool init(int resolution = -1);
    float fullScaleVoltage;
  protected:
    float readRawVoltage() override;
    int maxValue;
    int pin;
};





#endif
