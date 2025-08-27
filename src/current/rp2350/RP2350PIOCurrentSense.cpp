    #include "RP2350PIOCurrentSense.h"

    RP2350PIOCurrentSense::RP2350PIOCurrentSense(float gain, uint32_t max_adc_value, int pinSCK, int pinCSB, int pinD0, int pinTRIG) : CurrentSense() {
        this->pinSCK = pinSCK;
        this->pinCSB = pinCSB;
        this->pinD0 = pinD0;
        this->pinD1 = pinD0+1;
        this->pinD2 = pinD0+2;
        this->pinTRIG = pinTRIG;
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
        
        // Done init PIO        
        // TODO init ADC via SPI
        // TODO init DMA to transfer ADC data to memory buffer
        // TODO init timer to trigger PIO conversions at required frequency (check driver settings)
        //      TDB: do we need config input to know which timer slice and channel to use? or can we pick automatically?
        // TODO start everything up

        
        float sck_hz = 20e6;
        PIO pio = pio0; 
        int sm = pio_claim_unused_sm(pio0, false);
        if (sm < 0) { pio = pio1; sm = pio_claim_unused_sm(pio1, true); }

        // --- patch program instructions with chosen trigger pin ---
        size_t prog_len = bu79100g_parallel3_program.length;
        uint16_t insns[prog_len];
        memcpy(insns, bu79100g_parallel3_program_instructions, sizeof(insns));
        insns[1] = (insns[1] & ~0x1Fu) | (this->pinTRIG & 0x1Fu);
        insns[2] = (insns[2] & ~0x1Fu) | (this->pinTRIG & 0x1Fu);
        struct pio_program prog = bu79100g_parallel3_program; // copy metadata
        prog.instructions = insns;
    
        uint off = pio_add_program(pio, &prog);
        pio_sm_config c = bu79100g_parallel3_program_get_default_config(off);

        // Map pins to the SM
        sm_config_set_in_pins(&c, this->pinD0);                // reads D0..D2
        sm_config_set_set_pins(&c, this->pinCSB, 1);           // CSB (1 pin)
        sm_config_set_sideset_pins(&c, this->pinSCK);          // SCK (sideset)

        // Put pins into PIO control
        pio_gpio_init(pio, this->pinSCK);
        pio_gpio_init(pio, this->pinCSB);
        pio_gpio_init(pio, this->pinD0);
        pio_gpio_init(pio, this->pinD1);
        pio_gpio_init(pio, this->pinD2);

        // Directions (from the SM’s point of view)
        pio_sm_set_consecutive_pindirs(pio, sm, this->pinSCK, 1, true);   // SCK out
        pio_sm_set_consecutive_pindirs(pio, sm, this->pinCSB, 1, true);   // CS out
        pio_sm_set_consecutive_pindirs(pio, sm, this->pinD0,  3, false);  // D0..D2 in

        // Shift config: right, autopush every 24 bits (two pushes per conversion)
        sm_config_set_in_shift(&c, true, true, 24);

        // SCK ≈ clk_sys / (2 * clkdiv) because each SCK period = 2 instructions
        float div = (float)clock_get_hz(clk_sys) / (2.0f * sck_hz);
        sm_config_set_clkdiv(&c, div);

        // Init & start the SM
        pio_sm_init(pio, sm, off, &c);
        pio_sm_set_enabled(pio, sm, true);

        return 0;
    };

    PhaseCurrent_s RP2350PIOCurrentSense::getPhaseCurrents() {
        PhaseCurrent_s current;
        // TODO copy values from latest ADC reading
        // TODO process raw values to get currents in mAmps
        return current;
    };



