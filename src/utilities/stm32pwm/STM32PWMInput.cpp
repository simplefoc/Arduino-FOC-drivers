
#include "./STM32PWMInput.h"
#include <SimpleFOC.h>
#include "communication/SimpleFOCDebug.h"

#if defined(_STM32_DEF_)



STM32PWMInput::STM32PWMInput(int pin, uint32_t pwm_freq){
    _pin = digitalPinToPinName(pin);
    _pwm_freq = pwm_freq;
};


STM32PWMInput::~STM32PWMInput(){};





int STM32PWMInput::initialize(){
    pinmap_pinout(_pin, PinMap_TIM);
    uint32_t channel = STM_PIN_CHANNEL(pinmap_function(_pin, PinMap_TIM));
    timer.Instance = (TIM_TypeDef *)pinmap_peripheral(_pin, PinMap_TIM);
    timer.Init.CounterMode = TIM_COUNTERMODE_UP;
    // Check if timer is 16 or 32 bit and set max period accordingly
    if (IS_TIM_32B_COUNTER_INSTANCE(timer.Instance)) {
        timer.Init.Period = 0xFFFFFFFF; // 32-bit timer max
    } else {
        timer.Init.Period = 0xFFFF; // 16-bit timer max
    }
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

    // Check if the timer period is longer than the PWM period 
    // if it isnt set the perescaler to make it longer
    // Calculate timer clock frequency
    uint32_t timer_clk = HAL_RCC_GetPCLK1Freq();
    if (IS_TIM_CLOCK_DIVISION_INSTANCE(timer.Instance)) {
        // If APB1 prescaler > 1, timer clock is doubled
        if ((RCC->CFGR & RCC_CFGR_PPRE1) != RCC_CFGR_PPRE1_DIV1) {
            timer_clk *= 2;
        }
    }

    // Calculate required period (in timer ticks) for one PWM period
    uint32_t desired_period_ticks = timer_clk / _pwm_freq;

    // Check if timer's max period can fit the desired period
    uint32_t max_period = (IS_TIM_32B_COUNTER_INSTANCE(timer.Instance)) ? 0xFFFFFFFF : 0xFFFF;
    uint32_t prescaler = 1;
    if (desired_period_ticks > max_period) {
        prescaler = (desired_period_ticks + max_period - 1) / max_period;
        if (prescaler > 0xFFFF) prescaler = 0xFFFF; // limit to 16-bit prescaler
    }

    // Set the prescaler to achieve the desired period
    LL_TIM_SetPrescaler(timer.Instance, prescaler);

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
