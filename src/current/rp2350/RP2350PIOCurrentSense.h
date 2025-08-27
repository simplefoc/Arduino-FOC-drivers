
#pragma once

#include "common/base_classes/CurrentSense.h"
#include "bu79100g_parallel3.pio.h"

class RP2350PIOCurrentSense: public CurrentSense {
  public:
    RP2350PIOCurrentSense(float gain, uint32_t max_adc_value, int pinSCK, int pinCSB, int pinD0);
    ~RP2350PIOCurrentSense();

    int init() override;

    PhaseCurrent_s getPhaseCurrents() override;
  protected:
    uint32_t max_adc_value; //!< maximum ADC value (e.g. 4096 for 12 bit ADC)
    int pinCSB;
    int pinSCK;
    int pinD0;
    int pinD1;
    int pinD2;
    int gain_a;
    int gain_b;
    int gain_c;

};
