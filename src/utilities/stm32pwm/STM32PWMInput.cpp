
#include "./STM32PWMInput.h"

#if defined(_STM32_DEF_)



STM32PWMInput::STM32PWMInput(int pin){
    _pin = digitalPinToPinName(pin);
};


STM32PWMInput::~STM32PWMInput(){};





int STM32PWMInput::initialize(){
    pinmap_pinout(_pin, PinMap_TIM);
    uint32_t channel = STM_PIN_CHANNEL(pinmap_function(_pin, PinMap_TIM));
    timer.Instance = (TIM_TypeDef *)pinmap_peripheral(_pin, PinMap_TIM);
    timer.Init.Prescaler = 0;
    timer.Init.CounterMode = TIM_COUNTERMODE_UP;
    timer.Init.Period = 4.294967295E9;    // TODO max period, depends on which timer is used - 32 bits or 16 bits
    timer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    timer.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (channel!=1 && channel!=2) // only channels 1 & 2 supported
        return -10;
    useChannel2 = (channel==2);// remember the channel
    if (HAL_TIM_Base_Init(&timer) != HAL_OK) {
        return -1;
    }
    TIM_ClockConfigTypeDef clkCfg = { .ClockSource=TIM_CLOCKSOURCE_INTERNAL };
    if (HAL_TIM_ConfigClockSource(&timer, &clkCfg) != HAL_OK) {
        return -2;
    }
    if (HAL_TIM_IC_Init(&timer) != HAL_OK) {
        return -3;
    }

    TIM_SlaveConfigTypeDef slCfg = {
        .SlaveMode = TIM_SLAVEMODE_RESET,
        .InputTrigger = (channel==1)?TIM_TS_TI1FP1:TIM_TS_TI2FP2,
        .TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING,
        .TriggerPrescaler = TIM_ICPSC_DIV1,
        .TriggerFilter = 0
    };
    if (HAL_TIM_SlaveConfigSynchro(&timer, &slCfg) != HAL_OK) {
        return -4;
    }
    TIM_IC_InitTypeDef icCfg = {
        .ICPolarity = (channel==1)?TIM_INPUTCHANNELPOLARITY_RISING:TIM_INPUTCHANNELPOLARITY_FALLING,
        .ICSelection = (channel==1)?TIM_ICSELECTION_DIRECTTI:TIM_ICSELECTION_INDIRECTTI,
        .ICPrescaler = TIM_ICPSC_DIV1,
        .ICFilter = 0
    };
    if (HAL_TIM_IC_ConfigChannel(&timer, &icCfg, TIM_CHANNEL_1) != HAL_OK) {
        return -5;
    }
    icCfg.ICPolarity = (channel==1)?TIM_INPUTCHANNELPOLARITY_FALLING:TIM_INPUTCHANNELPOLARITY_RISING;
    icCfg.ICSelection = (channel==1)?TIM_ICSELECTION_INDIRECTTI:TIM_ICSELECTION_DIRECTTI;
    if (HAL_TIM_IC_ConfigChannel(&timer, &icCfg, TIM_CHANNEL_2) != HAL_OK) {
        return -6;
    }
    TIM_MasterConfigTypeDef mCfg = {
        .MasterOutputTrigger = TIM_TRGO_RESET,
        .MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE
    };
    if (HAL_TIMEx_MasterConfigSynchronization(&timer, &mCfg) != HAL_OK) {
        return -7;
    }
    if (HAL_TIM_IC_Start(&timer, TIM_CHANNEL_1)!=HAL_OK) {
        return -8;
    }
    if (HAL_TIM_IC_Start(&timer, TIM_CHANNEL_2)!=HAL_OK) {
        return -9;
    }
    timer.Instance->CR1 |= TIM_CR1_CEN;
    return 0;
};


float STM32PWMInput::getDutyCyclePercent(){
    uint32_t period = getPeriodTicks();
    if (period<1) return 0.0f;
    return getDutyCycleTicks() / (float)period * 100.0f;
};


uint32_t STM32PWMInput::getDutyCycleTicks(){
    if (useChannel2)
        return timer.Instance->CCR1;
    else
        return timer.Instance->CCR2;
};


uint32_t STM32PWMInput::getPeriodTicks(){
    if (useChannel2)
        return timer.Instance->CCR2;
    else
        return timer.Instance->CCR1;
};



#endif
