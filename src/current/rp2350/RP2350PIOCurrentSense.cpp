    #include "RP2350PIOCurrentSense.h"

    RP2350PIOCurrentSense::RP2350PIOCurrentSense(float gain, uint32_t max_adc_value, int pinA, int pinB, int pinC) : CurrentSense() {
        this->pinA = pinA;
        this->pinB = pinB;
        this->pinC = pinC;
        this->gain_a = gain;
        this->gain_b = gain;
        this->gain_c = gain;
        this->max_adc_value = max_adc_value;
    };


    RP2350PIOCurrentSense::~RP2350PIOCurrentSense() {

    };

    int RP2350PIOCurrentSense::init() {
        // TODO check that pins are valid for PIO use (e.g. consecutive pins on same bank)
        // TODO check that driver is linked
        
        // TODO init PIO        
        // TODO init ADC via SPI
        // TODO init DMA to transfer ADC data to memory buffer
        // TODO init timer to trigger PIO conversions at required frequency (check driver settings)
        //      TDB: do we need config input to know which timer slice and channel to use? or can we pick automatically?
        // TODO start everything up

        return 0;
    };

    PhaseCurrent_s RP2350PIOCurrentSense::getPhaseCurrents() {
        PhaseCurrent_s current;
        // TODO copy values from latest ADC reading
        // TODO process raw values to get currents in mAmps
        return current;
    };



