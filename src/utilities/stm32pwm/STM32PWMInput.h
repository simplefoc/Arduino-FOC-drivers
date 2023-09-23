#pragma once

#include "Arduino.h"

#if defined(_STM32_DEF_)


class STM32PWMInput {
    public:
        STM32PWMInput(int pin);
        ~STM32PWMInput();

        int initialize();

        float getDutyCyclePercent();
        uint32_t getDutyCycleTicks();
        uint32_t getPeriodTicks();
        
        PinName _pin;
    protected:
        TIM_HandleTypeDef timer;
        bool useChannel2 = false;
};





#endif