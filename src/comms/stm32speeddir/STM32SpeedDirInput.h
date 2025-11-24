
#pragma once

#include "Arduino.h"

#if defined(_STM32_DEF_)

#include "common/foc_utils.h"
#include "utilities/stm32pwm/STM32PWMInput.h"

class STM32SpeedDirInput : public STM32PWMInput {
    public:

        /**
         * STM32SpeedDirInput constructor
         * 
         * @param pin_speed - the pin number to read the speed PWM signal from
         * @param pin_dir - the pin number to read the direction signal from (default is NOT_SET)
         * @param pwm_freq - the frequency of the PWM signal (default is 1kHz)
         * 
         * This class is used to read speed and direction signals from a pin using the STM32 HAL library.
         * IMPORTANT!
         *      This class can only be used with the pins that are associated with some timer,
         *      and only if they are associated to the channels 1 or 2. The timer can not be
         *      used for other purposes, like motor control.
         */
        STM32SpeedDirInput(int pin_speed, int pin_dir = NOT_SET, uint32_t pwm_freq = 1000);
        ~STM32SpeedDirInput();

        int init();
        float getTargetVelocity();

        float min_speed = 0;            // min speed in rad/s
        float max_speed = 100;          // max speed in rad/s
        float min_pwm = 5.0;            // min duty cycle in %
        float max_pwm = 95.0;           // max duty cycle in %
        bool dir_positive_high = true;  // true if the direction pin is high when the motor is spinning in the positive direction
        bool direction = true;          // direction of rotation, default positive
    protected:
        int _pin_speed;
        int _pin_dir;
};


#endif