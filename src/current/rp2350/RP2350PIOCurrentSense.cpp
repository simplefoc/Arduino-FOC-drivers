    #include "RP2350PIOCurrentSense.h"
    #if defined(ARDUINO_ARCH_RP2040)
    RP2350PIOCurrentSense::RP2350PIOCurrentSense(PIO pio, float gain, uint32_t max_adc_value, int pinSCK, int pinCSB, int pinD0, int pinTRIG) : CurrentSense() {
        this->pio = pio;
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
        // TODO implement offset and calibration like other current sensor classes.
        float sck_hz = 20e6;
        int sm = pio_claim_unused_sm(this->pio, true);
        //if (sm < 0) { pio = pio1; sm = pio_claim_unused_sm(pio1, true); } //For now, let say we have to use PIO0, this is simpler for quick DMA setup
        
        // --- patch program instructions with chosen trigger pin ---
        size_t prog_len = bu79100g_parallel3_program.length;
        uint16_t insns[prog_len];
        memcpy(insns, bu79100g_parallel3_program_instructions, sizeof(insns));
        insns[1] = (insns[1] & ~0x1Fu) | (this->pinTRIG & 0x1Fu);
        insns[2] = (insns[2] & ~0x1Fu) | (this->pinTRIG & 0x1Fu);
        struct pio_program prog = bu79100g_parallel3_program; // copy metadata
        prog.instructions = insns;
    
        uint off = pio_add_program(this->pio, &prog);
        pio_sm_config c = bu79100g_parallel3_program_get_default_config(off);

        // Map pins to the SM
        sm_config_set_in_pins(&c, this->pinD0);                // reads D0..D2
        sm_config_set_set_pins(&c, this->pinCSB, 1);           // CSB (1 pin)
        sm_config_set_sideset_pins(&c, this->pinSCK);          // SCK (sideset)

        // Put pins into PIO control
        pio_gpio_init(this->pio, this->pinSCK);
        pio_gpio_init(this->pio, this->pinCSB);
        pio_gpio_init(this->pio, this->pinD0);
        pio_gpio_init(this->pio, this->pinD1);
        pio_gpio_init(this->pio, this->pinD2);

        // Directions (from the SM’s point of view)
        pio_sm_set_consecutive_pindirs(pio, sm, this->pinSCK, 1, true);   // SCK out
        pio_sm_set_consecutive_pindirs(pio, sm, this->pinCSB, 1, true);   // CS out
        pio_sm_set_consecutive_pindirs(pio, sm, this->pinD0,  3, false);  // D0..D2 in

        // Shift config: right, autopush every 32 bits (two pushes per conversion)
        sm_config_set_in_shift(&c, false, true, 32);

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

    PhaseCurrent_s RP2350PIOCurrentSense::getPhaseCurrents() {
        PhaseCurrent_s current;

        const uintptr_t base = (uintptr_t)buff;
        //Get the index the DMA is about to write
        const uint32_t i_dma = (dma_hw->ch[dma_a].write_addr - base)>>2;
        //For a safe read, get the one that is an even number and at least <2.wi, manage looping
        const uint32_t i_last = (i_dma <= 1) ? RING_WORDS -2 : ((i_dma / 2)*2 - 2);
        //copy them quickly (before any print!)
        const uint32_t w0 = buff[i_last];
        const uint32_t w1 = buff[i_last+1];

        //Reconstruct the 3 current from interleaved data
        uint32_t a,b,c = 0;
        uint32_t g;

        //g = (w0 >> 28) & 0xFu; a = (a<<1)|((g>>0)&1u); b = (b<<1)|((g>>1)&1u); c = (c<<1)|((g>>2)&1u); d = (d<<1)|((g>>3)&1u);
        //g = (w0 >> 24) & 0xFu; a = (a<<1)|((g>>0)&1u); b = (b<<1)|((g>>1)&1u); c = (c<<1)|((g>>2)&1u); d = (d<<1)|((g>>3)&1u);
        //g = (w0 >> 20) & 0xFu; a = (a<<1)|((g>>0)&1u); b = (b<<1)|((g>>1)&1u); c = (c<<1)|((g>>2)&1u); d = (d<<1)|((g>>3)&1u);
        //g = (w0 >> 16) & 0xFu; a = (a<<1)|((g>>0)&1u); b = (b<<1)|((g>>1)&1u); c = (c<<1)|((g>>2)&1u); d = (d<<1)|((g>>3)&1u);
        g = (w0 >> 12) & 0xFu; a = (a<<1)|((g>>0)&1u); b = (b<<1)|((g>>1)&1u); c = (c<<1)|((g>>2)&1u);// d = (d<<1)|((g>>3)&1u);
        g = (w0 >>  8) & 0xFu; a = (a<<1)|((g>>0)&1u); b = (b<<1)|((g>>1)&1u); c = (c<<1)|((g>>2)&1u);// d = (d<<1)|((g>>3)&1u);
        g = (w0 >>  4) & 0xFu; a = (a<<1)|((g>>0)&1u); b = (b<<1)|((g>>1)&1u); c = (c<<1)|((g>>2)&1u);// d = (d<<1)|((g>>3)&1u);
        g = (w0 >>  0) & 0xFu; a = (a<<1)|((g>>0)&1u); b = (b<<1)|((g>>1)&1u); c = (c<<1)|((g>>2)&1u);// d = (d<<1)|((g>>3)&1u);

        g = (w1 >> 28) & 0xFu; a = (a<<1)|((g>>0)&1u); b = (b<<1)|((g>>1)&1u); c = (c<<1)|((g>>2)&1u);// d = (d<<1)|((g>>3)&1u);
        g = (w1 >> 24) & 0xFu; a = (a<<1)|((g>>0)&1u); b = (b<<1)|((g>>1)&1u); c = (c<<1)|((g>>2)&1u);// d = (d<<1)|((g>>3)&1u);
        g = (w1 >> 20) & 0xFu; a = (a<<1)|((g>>0)&1u); b = (b<<1)|((g>>1)&1u); c = (c<<1)|((g>>2)&1u);// d = (d<<1)|((g>>3)&1u);
        g = (w1 >> 16) & 0xFu; a = (a<<1)|((g>>0)&1u); b = (b<<1)|((g>>1)&1u); c = (c<<1)|((g>>2)&1u);// d = (d<<1)|((g>>3)&1u);
        g = (w1 >> 12) & 0xFu; a = (a<<1)|((g>>0)&1u); b = (b<<1)|((g>>1)&1u); c = (c<<1)|((g>>2)&1u);// d = (d<<1)|((g>>3)&1u);
        g = (w1 >>  8) & 0xFu; a = (a<<1)|((g>>0)&1u); b = (b<<1)|((g>>1)&1u); c = (c<<1)|((g>>2)&1u);// d = (d<<1)|((g>>3)&1u);
        g = (w1 >>  4) & 0xFu; a = (a<<1)|((g>>0)&1u); b = (b<<1)|((g>>1)&1u); c = (c<<1)|((g>>2)&1u);// d = (d<<1)|((g>>3)&1u);
        g = (w1 >>  0) & 0xFu; a = (a<<1)|((g>>0)&1u); b = (b<<1)|((g>>1)&1u); c = (c<<1)|((g>>2)&1u);// d = (d<<1)|((g>>3)&1u);

        current.a = a * gain_a;
        current.b = b * gain_b;
        current.c = c * gain_c;
        
        return current;
    };
    #endif


