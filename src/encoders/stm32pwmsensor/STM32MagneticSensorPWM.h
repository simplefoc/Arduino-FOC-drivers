
#pragma once


#include <Arduino.h>


#if defined(_STM32_DEF_)

#include "common/base_classes/Sensor.h"
#include "utilities/stm32pwm/STM32PWMInput.h"


class STM32MagneticSensorPWM : public Sensor, public STM32PWMInput {
    public:
        STM32MagneticSensorPWM(int pin, uint32_t _min_ticks = 0, uint32_t _max_ticks = 0x0FFF);
        ~STM32MagneticSensorPWM();

        void init() override;

        uint32_t max_ticks = 0x0FFF;
        uint32_t min_ticks = 0;
        bool initialized = false;
    protected:
        float getSensorAngle() override;
};

#endif