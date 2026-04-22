#include "STM32HWEncoder.h"

#if defined(_STM32_DEF_)

#include "drivers/hardware_specific/stm32/stm32_mcu.h"

/*
  HardwareEncoder(int cpr)
*/
STM32HWEncoder::STM32HWEncoder(unsigned int _ppr, int pinA, int pinB, int pinI) {
    cpr = _ppr * 4; // 4x for quadrature
    _pinA = digitalPinToPinName(pinA);
    _pinB = digitalPinToPinName(pinB);
    _pinI = digitalPinToPinName(pinI);
    index_found = false;
}

// function returns encoder handle
TIM_HandleTypeDef STM32HWEncoder::getEncoderTimerHandle() { return encoder_handle; }

/*
  Shaft angle calculation
*/
float STM32HWEncoder::getSensorAngle() { 
    return _2PI * encoder_handle.Instance->CNT / static_cast<float>(cpr);
}
/* 
  Set the current angle using CNT register
*/
void STM32HWEncoder::setCurrentAngle(float set_angle) {
    encoder_handle.Instance->CNT = set_angle * cpr / _2PI;
}
/*
  Modify encoder count directly
*/
void STM32HWEncoder::setEncoderCount(uint32_t ecount) {
    encoder_handle.Instance->CNT = ecount;
}

// getter for index pin
int STM32HWEncoder::needsSearch() { return hasIndex() && !index_found; }

// private function used to determine if encoder has index
int STM32HWEncoder::hasIndex() { return (_pinI!=NC); }

// encoder initialisation of the hardware pins
void STM32HWEncoder::init() {
    // GPIO configuration
    TIM_TypeDef *InstanceA = (TIM_TypeDef *)pinmap_peripheral(_pinA, PinMap_TIM);
    if (!IS_TIM_ENCODER_INTERFACE_INSTANCE(InstanceA)) {
        initialized = false;
        SIMPLEFOC_DEBUG("STM32HWEncoder pin A doesn't support encoder interface");
        return;
    }
    TIM_TypeDef *InstanceB = (TIM_TypeDef *)pinmap_peripheral(_pinB, PinMap_TIM);
    if (InstanceA != InstanceB) {
        initialized = false;
        SIMPLEFOC_DEBUG("STM32HWEncoder pin B is not in the same timer as A");
        return;
    }
    pinmap_pinout(_pinA, PinMap_TIM);
    pinmap_pinout(_pinB, PinMap_TIM);

    // TODO check width:
    //IS_TIM_32B_COUNTER_INSTANCE(InstanceA);

    // set up timer for encoder
    encoder_handle.Init.Period = cpr - 1;
    encoder_handle.Init.Prescaler = 0;
    encoder_handle.Init.ClockDivision = 0;
    encoder_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    encoder_handle.Init.RepetitionCounter = 0;
    encoder_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    TIM_Encoder_InitTypeDef encoder_config;

    encoder_config.EncoderMode = TIM_ENCODERMODE_TI12;

    encoder_config.IC1Polarity = TIM_ICPOLARITY_RISING;
    encoder_config.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    encoder_config.IC1Prescaler = TIM_ICPSC_DIV1;
    encoder_config.IC1Filter = 0;

    encoder_config.IC2Polarity = TIM_ICPOLARITY_RISING;
    encoder_config.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    encoder_config.IC2Prescaler = TIM_ICPSC_DIV1;
    encoder_config.IC2Filter = 0;

    encoder_handle.Instance = InstanceA; // e.g. TIM4;
    enableTimerClock(&encoder_handle);

    if (HAL_TIM_Encoder_Init(&encoder_handle, &encoder_config) != HAL_OK) {
        initialized = false;
        SIMPLEFOC_DEBUG("Couldn't initialize timer in encoder mode!");
        return;
    }

    // TODO: figure out way to check if Index pin is specifically ETR line
    //if(IS_TIM_ETR_INSTANCE())
    
    // Encoder index configuration
    //TIMEx_EncoderIndexConfigTypeDef encoder_indexconfig = {0};
    //encoder_indexconfig.Polarity = TIM_ENCODERINDEX_POLARITY_NONINVERTED;
    //encoder_indexconfig.Prescaler = TIM_ENCODERINDEX_PRESCALER_DIV1;
    //encoder_indexconfig.Filter = 0;
    //encoder_indexconfig.FirstIndexEnable = ENABLE;
    //encoder_indexconfig.Position = TIM_ENCODERINDEX_POSITION_00;
    //encoder_indexconfig.Direction = TIM_ENCODERINDEX_DIRECTION_UP_DOWN; // Double check this is ok
    //if (HAL_TIMEx_ConfigEncoderIndex(&encoder_handle, &encoder_indexconfig) != HAL_OK)
    //{
    //    SIMPLEFOC_DEBUG("Couldn't configure encoder index pin");
    //    initialized = false;
    //    return;
    //}

    // If index pin provided attach interrupt to reset the counter 
    // on index pulse
    if(hasIndex())
    {
        attachInterrupt(digitalPinToInterrupt(pinNametoDigitalPin(_pinI)), [this]() {
            encoder_handle.Instance->CNT = 0; // reset counter
            index_found = true;
        }, index_polarity);
    }

    if (HAL_TIM_Encoder_Start(&encoder_handle, TIM_CHANNEL_1) != HAL_OK) {
        initialized = false;
        return;
    }

    stm32_reserveTimer(encoder_handle.Instance);
    initialized = true;
}

#endif