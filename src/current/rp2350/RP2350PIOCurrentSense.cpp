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
        // TODO init ADC via SPI (Only for ADC with MOSI input)
        // Done init DMA to transfer ADC data to memory buffer
        // TODO init timer to trigger PIO conversions at required frequency (check driver settings)
        //      TDB: do we need config input to know which timer slice and channel to use? or can we pick automatically?
        // TODO start everything up

        float sck_hz = 20e6;
        PIO pio = pio0; 
        int sm = pio_claim_unused_sm(pio0, true);
        //if (sm < 0) { pio = pio1; sm = pio_claim_unused_sm(pio1, true); } //For now, let say we have to use PIO0, this is simpler for quick DMA setup

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

        // Shift config: right, autopush every 32 bits (two pushes per conversion)
        sm_config_set_in_shift(&c, true, true, 32);

        // SCK ≈ clk_sys / (2 * clkdiv) because each SCK period = 2 instructions
        float div = (float)clock_get_hz(clk_sys) / (2.0f * sck_hz);
        sm_config_set_clkdiv(&c, div);

        // Init the SM
        pio_sm_init(pio, sm, off, &c);

       //DMA Setup
       //dma_a is set to read from PIO RX FIFO and write to 'buff' buffer memory
        dma_a = dma_claim_unused_channel(true);
        dma_b = dma_claim_unused_channel(true);
        // ---------------------- DMA A: PIO RX FIFO -> buff[] with WRITE ring ----------------------
        dma_channel_config ca = dma_channel_get_default_config(dma_a);
        channel_config_set_read_increment(&ca, false);
        channel_config_set_write_increment(&ca, true);
        channel_config_set_transfer_data_size(&ca, DMA_SIZE_32);
        channel_config_set_dreq(&ca, DREQ_PIO0_RX0 + sm);
        channel_config_set_chain_to(&ca, dma_b);  // A -> B when count hits 0

        // Enable WRITE-side ring over the whole buffer span (power-of-two bytes)
        // size_bits = log2(RING_BYTES)
        channel_config_set_ring(&ca, /*write=*/true, /*size_bits=*/__builtin_ctz(RING_BYTES));

        dma_channel_set_config(dma_a, &ca, false);
        dma_channel_set_read_addr(dma_a, &pio->rxf[sm], false);
        dma_channel_set_write_addr(dma_a, (void*)buff, false);
        dma_channel_set_trans_count(dma_a, RING_WORDS, false);

        // ---------------------- DMA B: rearm A with ONE 32-bit write ----------------------
        dma_channel_config cb = dma_channel_get_default_config(dma_b);
        channel_config_set_read_increment(&cb, false);
        channel_config_set_write_increment(&cb, false);
        channel_config_set_transfer_data_size(&cb, DMA_SIZE_32);

        // Write 1 word to A.AL1_TRANSFER_COUNT_TRIG (this sets count and re-starts A)
        dma_channel_configure(
            dma_b, &cb,
            (void*)&dma_hw->ch[dma_a].al1_transfer_count_trig,
            (const void*)&reload_count,
            1, false
        );

        // Go!
        dma_channel_start(dma_a);
        pio_sm_set_enabled(pio, sm, true);

        return 0;
    };

    void extract_bit_interleaved(const uint32_t w0, const uint32_t w1, uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d) {
        *a = 0;
        *b = 0;
        *c = 0;
        *d = 0;
        for (int i = 0; i < 64; i += 4) {
            uint32_t w = (i < 32) ? w1 : w0;
            int shift = 28 - (i % 32);  // 28, 24, ..., 0 for each group of 4 bits

            uint32_t group = (w >> shift) & 0xF;  // extract aN bN cN dN

            *a = (*a << 1) | ((group >> 0) & 0x1);
            *b = (*b << 1) | ((group >> 1) & 0x1);
            *c = (*c << 1) | ((group >> 2) & 0x1);
            *d = (*d << 1) | ((group >> 3) & 0x1);
        }
    }

    PhaseCurrent_s RP2350PIOCurrentSense::getPhaseCurrents() {
        PhaseCurrent_s current;

        const uintptr_t base = (uintptr_t)buff;
        //Get the index the DMA is about to write
        const uint32_t i_dma = (dma_hw->ch[dma_a].write_addr - base)>>2;
        //For a safe read, get the one that is an even number and at least <2.wi, manage looping
        const uint32_t i_last = (i_dma <= 1) ? RING_WORDS -2 : ((i_dma / 2)*2 - 2);
        //copy them quickly (before print!)
        const uint32_t w0 = buff[i_last];
        const uint32_t w1 = buff[i_last+1];
        //Reconstruct the 3 current from interleaved data
        uint32_t a,b,c,d = 0;
        extract_bit_interleaved(w0,w1, &a, &b, &c, &d);    

        current.a = (0x00000fff & a) * gain_a;
        current.b = (0x00000fff & b) * gain_b;
        current.c = (0x00000fff & c) * gain_c;
        
        return current;
    };



