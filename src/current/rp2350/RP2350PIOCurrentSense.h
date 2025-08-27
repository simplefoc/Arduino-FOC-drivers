
#pragma once

#include "common/base_classes/CurrentSense.h"


class RP2350PIOCurrentSense: public CurrentSense {
  public:
    RP2350PIOCurrentSense(float gain, uint32_t max_adc_value, int pinA, int pinB, int pinC = _NC);
    ~RP2350PIOCurrentSense();

    int init() override;

    PhaseCurrent_s getPhaseCurrents() override;
  protected:
    uint32_t max_adc_value; //!< maximum ADC value (e.g. 4096 for 12 bit ADC)
};
