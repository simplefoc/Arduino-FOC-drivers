#pragma once

#include "Arduino.h"

#if defined(_STM32_DEF_)


class STM32PWMInput {
    public:
        /**
         * StM32PWMInput constructor
         * 
         * @param pin - the pin number to read the PWM signal from
         * @param pwm_freq - the frequency of the PWM signal (default is 1kHz)
         * 
         * This class is used to read PWM signals from a pin using the STM32 HAL library.
         * IMPORTANT!
         *     This class can only be used with the pins that are assocaited with some timer, 
         *     and only if they are associated to the channels 1 or 2. The timer can not be 
         *     used for other purposes, like motor control. 
         */
        STM32PWMInput(int pin, uint32_t pwm_freq = 1000);
        ~STM32PWMInput();

        int initialize();

        /**
         * Get the duty cycle of the PWM signal as a percentage.
         * 
         * @return float - the duty cycle in percent
         */
        float getDutyCyclePercent();
        /**
         * Get the duty cycle of the PWM signal in ticks.
         * 
         * @return uint32_t - the duty cycle in ticks
         */
        uint32_t getDutyCycleTicks();
        /**
         * Get the period of the PWM signal in ticks.
         * 
         * @return uint32_t - the period in ticks
         */
        uint32_t getPeriodTicks();
        
        PinName _pin; // the pin to read the PWM signal from 
        uint32_t _pwm_freq; // the frequency of the PWM signal

    protected:
        TIM_HandleTypeDef timer; // the timer handle for the PWM input
        bool useChannel2 = false; // whether to use channel 2 or not, default is channel 1
};





#endif