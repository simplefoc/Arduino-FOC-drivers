#pragma once

#if defined(ARDUINO_PHOQUE) 
#include "stm32g4xx_hal.h"
#if defined(HAL_DAC_MODULE_ENABLED) && defined(HAL_COMP_MODULE_ENABLED)
#include "brake/stm32/stm32_brake.hpp"

class phoque_brake : public stm32_brake
{
public:
    phoque_brake();
    void init(float threshold);
    HAL_StatusTypeDef set_threshold(float trigger_voltage);
};

#endif
#endif