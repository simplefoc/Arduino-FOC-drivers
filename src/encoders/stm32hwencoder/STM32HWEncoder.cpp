#include "STM32HWEncoder.h"

#if defined(_STM32_DEF_)

/*
  HardwareEncoder(int cpr)
*/
STM32HWEncoder::STM32HWEncoder(unsigned int _ppr, int8_t pinA, int8_t pinB, int8_t pinI) {
    rotations_per_overflow = 0;
    ticks_per_overflow = 0;

    overflow_count = 0;
    count = 0;
    prev_count = 0;
    prev_overflow_count = 0;
    pulse_timestamp = 0;

    cpr = _ppr * 4; // 4x for quadrature

    // velocity calculation variables
    prev_timestamp = getCurrentMicros();
    pulse_timestamp = getCurrentMicros();

    _pinA = pinA;
    _pinB = pinB;
    _pinI = pinI;
}



void STM32HWEncoder::update() {
    // handle overflow of the 16-bit counter here
    // must be called at least twice per traversal of overflow range
    // TODO(conroy-cheers): investigate performance impact
    prev_count = count;
    count = encoder_handle.Instance->CNT;

    prev_timestamp = pulse_timestamp;
    pulse_timestamp = getCurrentMicros();

    prev_overflow_count = overflow_count;
    if (prev_count > (ticks_per_overflow - overflow_margin) &&
        prev_count <= ticks_per_overflow && count < overflow_margin)
        ++overflow_count;
    if (prev_count >= 0 && prev_count < overflow_margin &&
        count >= (ticks_per_overflow - overflow_margin))
        --overflow_count;
}



/*
  Shaft angle calculation
*/
float STM32HWEncoder::getSensorAngle() { return getAngle(); }

float STM32HWEncoder::getMechanicalAngle() {
    return _2PI * (count % static_cast<int>(cpr)) / static_cast<float>(cpr);
}
float STM32HWEncoder::getAngle() {
    return _2PI * (count / static_cast<float>(cpr) +
                   overflow_count * rotations_per_overflow);
}
double STM32HWEncoder::getPreciseAngle() {
    return _2PI * (count / static_cast<double>(cpr) +
                   overflow_count * rotations_per_overflow);
}
int32_t STM32HWEncoder::getFullRotations() {
    return count / static_cast<int>(cpr) +
           overflow_count * rotations_per_overflow;
}

/*
  Shaft velocity calculation
*/
float STM32HWEncoder::getVelocity() {
    // sampling time calculation
    float dt = (pulse_timestamp - prev_timestamp) * 1e-6f;
    // quick fix for strange cases (micros overflow)
    if (dt <= 0 || dt > 0.5f)
        dt = 1e-3f;

    // time from last impulse
    int32_t overflow_diff = overflow_count - prev_overflow_count;
    int32_t dN = (count - prev_count) + (ticks_per_overflow * overflow_diff);

    float pulse_per_second = dN / dt;

    // velocity calculation
    return pulse_per_second / (static_cast<float>(cpr)) * _2PI;
}

// getter for index pin
int STM32HWEncoder::needsSearch() { return false; }

// private function used to determine if encoder has index
int STM32HWEncoder::hasIndex() { return 0; }

// encoder initialisation of the hardware pins
// and calculation variables
void STM32HWEncoder::init() {
    // overflow handling
    rotations_per_overflow = 0xFFFF / cpr;
    ticks_per_overflow = cpr * rotations_per_overflow;

    // set up GPIO
    GPIO_InitTypeDef gpio;

    PinName pinA = digitalPinToPinName(_pinA);
    TIM_TypeDef *InstanceA = (TIM_TypeDef *)pinmap_peripheral(pinA, PinMap_PWM);
    gpio.Pin = digitalPinToBitMask(_pinA);
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_MEDIUM;
    #ifndef STM32F1xx_HAL_GPIO_H
    gpio.Alternate = pinmap_function(pinA, PinMap_PWM);
    #endif
    HAL_GPIO_Init(digitalPinToPort(_pinA), &gpio);

    // lets assume pinB is on the same timer as pinA... otherwise it can't work but the API currently doesn't allow us to fail gracefully
    gpio.Pin = digitalPinToBitMask(_pinB);
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_MEDIUM;
    #ifndef STM32F1xx_HAL_GPIO_H
    gpio.Alternate = pinmap_function(digitalPinToPinName(_pinB), PinMap_PWM);
    #endif
    HAL_GPIO_Init(digitalPinToPort(_pinB), &gpio);

    // set up timer for encoder
    encoder_handle.Init.Period = ticks_per_overflow - 1;
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
        _Error_Handler(__FILE__, __LINE__);
    }

    HAL_TIM_Encoder_Start(&encoder_handle, TIM_CHANNEL_1);

    // counter setup
    overflow_count = 0;
    count = 0;
    prev_count = 0;
    prev_overflow_count = 0;

    prev_timestamp = getCurrentMicros();
    pulse_timestamp = getCurrentMicros();
}

#endif