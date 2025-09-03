
#pragma once

#include "common/base_classes/CurrentSense.h"
#include "bu79100g_parallel3.pio.h"
#include "hardware/dma.h"
#include "hardware/sync.h"

class RP2350PIOCurrentSense: public CurrentSense {
  public:
    RP2350PIOCurrentSense(float gain, uint32_t max_adc_value, int pinSCK, int pinCSB, int pinD0, int pinTRIG);
    ~RP2350PIOCurrentSense();

    int init() override;

    PhaseCurrent_s getPhaseCurrents() override;
  
    static constexpr uint32_t RING_WORDS    = 64;   // ring span (needs to be a power of two)
    static constexpr uint32_t RING_BYTES = RING_WORDS * 4;

    // Buffer base must be aligned to ring span for write-ring:
    alignas(RING_BYTES) volatile uint32_t buff[RING_WORDS];

    // Single word used by DMA B to rearm A:
    alignas(4) volatile uint32_t reload_count = RING_WORDS;

    int dma_a = -1; // PIO RX -> ring (streamer)
    int dma_b = -1; // reloader 

    uint32_t max_adc_value; //!< maximum ADC value (e.g. 4096 for 12 bit ADC)
    int pinCSB;
    int pinSCK;
    int pinD0;
    int pinD1;
    int pinD2;
    int pinTRIG;
    float gain_a;
    float gain_b;
    float gain_c;
  protected: //For debug, all public
};
