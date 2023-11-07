#include "STM32HWEncoder.h"

#if defined(_STM32_DEF_)

/*
  HardwareEncoder(int cpr)
*/
STM32HWEncoder::STM32HWEncoder(unsigned int _ppr, int8_t pinA, int8_t pinB, int8_t pinI) {
    cpr = _ppr * 4; // 4x for quadrature
    _pinA = digitalPinToPinName(pinA);
    _pinB = digitalPinToPinName(pinB);
    _pinI = digitalPinToPinName(pinI);
}

/*
  Shaft angle calculation
*/
float STM32HWEncoder::getSensorAngle() { 
    return _2PI * encoder_handle.Instance->CNT / static_cast<float>(cpr);
}

// getter for index pin
int STM32HWEncoder::needsSearch() { return false; }

// private function used to determine if encoder has index
int STM32HWEncoder::hasIndex() { return 0; }

// encoder initialisation of the hardware pins
void STM32HWEncoder::init() {
    // GPIO configuration
    TIM_TypeDef *InstanceA = (TIM_TypeDef *)pinmap_peripheral(_pinA, PinMap_TIM);
    if (!IS_TIM_ENCODER_INTERFACE_INSTANCE(InstanceA)) {
        initialized = false;
        return;
    }
    TIM_TypeDef *InstanceB = (TIM_TypeDef *)pinmap_peripheral(_pinB, PinMap_TIM);
    if (InstanceA != InstanceB) {
        initialized = false;
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
        return;
    }

    if (HAL_TIM_Encoder_Start(&encoder_handle, TIM_CHANNEL_1) != HAL_OK) {
        initialized = false;
        return;
    }

    initialized = true;
}

#endif