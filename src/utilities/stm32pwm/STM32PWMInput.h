#pragma once

#include "Arduino.h"

#if defined(_STM32_DEF_)


class STM32PWMInput {
    public:
        STM32PWMInput(int pin);
        ~STM32PWMInput();

        int init();

        float getDutyCyclePercent();
        uint32_t getDutyCycleTicks();
        uint32_t getPeriodTicks();
    protected:
        int _pin;
        TIM_HandleTypeDef timer;
};





#endif