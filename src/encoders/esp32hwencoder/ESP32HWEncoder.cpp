#include "ESP32HWEncoder.h"

#if defined(ESP_H) && defined(ARDUINO_ARCH_ESP32)



ESP32HWEncoder::ESP32HWEncoder(int pinA, int pinB, int32_t ppr, int pinI)
{
    #ifdef USE_ARDUINO_PINOUT
        // Handle Arduino Nano ESP32 quirks with the pin assignments
        _pinA = digitalPinToGPIO(pinA);
        _pinB = digitalPinToGPIO(pinB);
        _pinI = digitalPinToGPIO(pinI);
    #else
        _pinA = pinA;
        _pinB = pinB;
        _pinI = pinI;
    #endif

    cpr = ppr * 4; // 4x for quadrature

    pcnt_config.ctrl_gpio_num =  _pinA;
    pcnt_config.pulse_gpio_num = _pinB;
    pcnt_config.counter_l_lim = INT16_MIN;
    pcnt_config.counter_h_lim = INT16_MAX;
} 

// Interrupt handler for accumulating the pulsecounter count
void IRAM_ATTR overflowCounter(void* arg)
{
    uint8_t interruptStatus = PCNT.int_st.val;
    for (uint32_t i = 0; i < 8; i++)
    {
        if (interruptStatus & BIT(i))
        {
            int32_t set = ((overflowISR_args_t*) arg)[i].set;
            if(set != 1){ continue;}
            int32_t* count = ((overflowISR_args_t*) arg)[i].angleoverflow_val;
            
            // Add or subtract depending on the direction of the overflow
            switch (PCNT.status_unit[i].val)
            {
            case PCNT_EVT_L_LIM:
                *count += INT16_MIN;
                break;
            case PCNT_EVT_H_LIM:
                *count += INT16_MAX;
                break;
            default:
                break;
            }
            
            // Clear the interrupt
            PCNT.int_clr.val = BIT(i);
        }
    }
}

// Interrupt handler for zeroing the pulsecounter count
void IRAM_ATTR ESP32HWEncoder::indexHandler()
    {
        pcnt_counter_clear(pcnt_config.unit);
        angleOverflow = 0;
        indexFound = true;
    }

void ESP32HWEncoder::init()
{
    
    // Statically allocate and initialize the spinlock
    spinlock = portMUX_INITIALIZER_UNLOCKED;

    // find a free pulsecount unit
    for (uint8_t i = 0; i < PCNT_UNIT_MAX; i++)
    {
        if(cpr > 0){
            inv_cpr = 1.0f/cpr;
        }
        if(used_units[i] == 0){
            pcnt_config.unit = (pcnt_unit_t) i;
            if(pcnt_unit_config(&pcnt_config) == ESP_OK){
                initialized = true;
                // Serial.printf("Configured PCNT unit %d\n", i);
                used_units[i] = 1;
                break;
            }
        }

    }
    if (initialized)
    {
        // Set up the PCNT peripheral
        pcnt_set_pin(pcnt_config.unit, PCNT_CHANNEL_0, pcnt_config.ctrl_gpio_num, pcnt_config.pulse_gpio_num);
        pcnt_set_pin(pcnt_config.unit, PCNT_CHANNEL_1, pcnt_config.pulse_gpio_num, pcnt_config.ctrl_gpio_num);
        pcnt_set_mode(pcnt_config.unit, PCNT_CHANNEL_0, PCNT_COUNT_INC, PCNT_COUNT_DEC, PCNT_MODE_REVERSE, PCNT_MODE_KEEP);
        pcnt_set_mode(pcnt_config.unit, PCNT_CHANNEL_1, PCNT_COUNT_DEC, PCNT_COUNT_INC, PCNT_MODE_REVERSE, PCNT_MODE_KEEP);
        
        pcnt_counter_pause(pcnt_config.unit);
        pcnt_counter_clear(pcnt_config.unit);
        
        // Select interrupt on reaching high and low counter limit
        pcnt_event_enable(pcnt_config.unit, PCNT_EVT_L_LIM);
        pcnt_event_enable(pcnt_config.unit, PCNT_EVT_H_LIM);

        // Pass pointer to the angle accumulator and the current PCNT unit to the ISR
        overflowISR_args[pcnt_config.unit].angleoverflow_val = &angleOverflow;
        overflowISR_args[pcnt_config.unit].unit = pcnt_config.unit;
        overflowISR_args[pcnt_config.unit].set = 1;

        // Register and enable the interrupt
        pcnt_isr_register(overflowCounter, (void*)&overflowISR_args, 0, (pcnt_isr_handle_t*) NULL);
        pcnt_intr_enable(pcnt_config.unit);
        
        // Just check the last command for errors
        if(pcnt_counter_resume(pcnt_config.unit) != ESP_OK){
            initialized = false;
        }
        
        // If an index Pin is defined, create an ISR to zero the angle when the index fires
        if (hasIndex())
        {
            attachInterrupt(static_cast<u_int8_t>(_pinI), std::bind(&ESP32HWEncoder::indexHandler,this), RISING);
        }
        
        // Optionally use pullups
        if (pullup == USE_INTERN)
        {
            pinMode(static_cast<u_int8_t>(_pinA), INPUT_PULLUP);
            pinMode(static_cast<u_int8_t>(_pinB), INPUT_PULLUP);
            if (hasIndex()) {pinMode(static_cast<u_int8_t>(_pinI), INPUT_PULLUP);}
        }

    }

}

int ESP32HWEncoder::needsSearch()
{
        return !((indexFound && hasIndex()) || !hasIndex());
}

int ESP32HWEncoder::hasIndex()
{
    return _pinI != -1;
}

void ESP32HWEncoder::setCpr(int32_t ppr){
    cpr = 4*ppr;
    if(cpr > 0){
        inv_cpr = 1.0f/cpr; // Precalculate the inverse of cpr to avoid "slow" float divisions
    }
}

int32_t ESP32HWEncoder::getCpr(){
    return cpr;
}

// Change to Step/Dir counting mode. A->Step, B->Dir
void ESP32HWEncoder::setStepDirMode(){
    pcnt_set_mode(pcnt_config.unit, PCNT_CHANNEL_0, PCNT_COUNT_INC, PCNT_COUNT_DIS, PCNT_MODE_KEEP, PCNT_MODE_KEEP);
    pcnt_set_mode(pcnt_config.unit, PCNT_CHANNEL_1, PCNT_COUNT_DIS, PCNT_COUNT_DIS, PCNT_MODE_KEEP, PCNT_MODE_REVERSE);
}

// Change to default AB (quadrature) mode
void ESP32HWEncoder::setQuadratureMode(){
    pcnt_set_mode(pcnt_config.unit, PCNT_CHANNEL_0, PCNT_COUNT_INC, PCNT_COUNT_DEC, PCNT_MODE_REVERSE, PCNT_MODE_KEEP);
    pcnt_set_mode(pcnt_config.unit, PCNT_CHANNEL_1, PCNT_COUNT_DEC, PCNT_COUNT_INC, PCNT_MODE_REVERSE, PCNT_MODE_KEEP);
}

float IRAM_ATTR ESP32HWEncoder::getSensorAngle()
{
    if(!initialized){return -1.0f;}

    taskENTER_CRITICAL(&spinlock);
    // We are now in a critical section to prevent interrupts messing with angleOverflow and angleCounter

    // Retrieve the count register into a variable
    pcnt_get_counter_value(pcnt_config.unit, &angleCounter);

    // Trim the accumulator variable to prevent issues with it overflowing
    // Make the % operand behave mathematically correct (-5 modulo 4 == 3; -5 % 4 == -1)
    angleOverflow %= cpr;
    if (angleOverflow < 0){
        angleOverflow += cpr;
    }

    angleSum = (angleOverflow + angleCounter) % cpr;
    if (angleSum < 0) {
        angleSum += cpr;
    }

    taskEXIT_CRITICAL(&spinlock); // Exit critical section
    
    // Calculate the shaft angle
    return _2PI * angleSum * inv_cpr;
}

#endif