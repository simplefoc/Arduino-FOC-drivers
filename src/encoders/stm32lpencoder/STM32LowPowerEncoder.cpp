

#include "./STM32LowPowerEncoder.h"

#if defined(_STM32_DEF_) && defined(HAL_LPTIM_MODULE_ENABLED)


extern const PinMap PinMap_LPTIM[];
extern const PinMap PinMap_LPTIMETR[];


STM32LowPowerEncoder::STM32LowPowerEncoder(unsigned int ppr, int pinA, int pinB, int pinI){
    cpr = ppr * 4; // 4x for quadrature
    _pinA = digitalPinToPinName(pinA);
    _pinB = digitalPinToPinName(pinB);
    _pinI = digitalPinToPinName(pinI);
};

void STM32LowPowerEncoder::init(){
    _encoder_handle.Instance = (LPTIM_TypeDef*)pinmap_peripheral(_pinA, PinMap_LPTIM);
    if (!IS_LPTIM_ENCODER_INSTANCE(_encoder_handle.Instance)) {
        initialized = false;
        return;
    }
    LPTIM_TypeDef* inst = (LPTIM_TypeDef*)pinmap_peripheral(_pinA, PinMap_LPTIM);
    if (inst!=_encoder_handle.Instance) {    // check the other pins are on the same timer
        initialized = false;
        return;
    }
    if (_pinI!=NC) {
        inst = (LPTIM_TypeDef*)pinmap_peripheral(_pinI, PinMap_LPTIMETR);
        if (inst!=_encoder_handle.Instance) {
            initialized = false;
            return;
        }
    }
    _encoder_handle.Init.Clock.Source = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
    _encoder_handle.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV1;
    _encoder_handle.Init.UltraLowPowerClock.Polarity = LPTIM_CLOCKPOLARITY_RISING_FALLING;
    _encoder_handle.Init.UltraLowPowerClock.SampleTime = LPTIM_CLOCKSAMPLETIME_DIRECTTRANSITION;
    if (_pinI==NC)
        _encoder_handle.Init.Trigger.Source = LPTIM_TRIGSOURCE_SOFTWARE;
    else {
        _encoder_handle.Init.Trigger.Source = LPTIM_TRIGSOURCE_0;
        _encoder_handle.Init.Trigger.ActiveEdge = LPTIM_ACTIVEEDGE_RISING;
        _encoder_handle.Init.Trigger.SampleTime = LPTIM_TRIGSAMPLETIME_DIRECTTRANSITION;        
    }
    _encoder_handle.Init.OutputPolarity = LPTIM_OUTPUTPOLARITY_HIGH;
    _encoder_handle.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE;
    _encoder_handle.Init.CounterSource = LPTIM_COUNTERSOURCE_EXTERNAL;
    _encoder_handle.Init.Input1Source = LPTIM_INPUT1SOURCE_GPIO;
    _encoder_handle.Init.Input2Source = LPTIM_INPUT2SOURCE_GPIO;
    if (HAL_LPTIM_Init(&_encoder_handle) != HAL_OK)
        initialized = false;
    else {
        if (HAL_LPTIM_Encoder_Start(&_encoder_handle, cpr) != HAL_OK)
            initialized = false;
        else {
            pinmap_pinout(_pinA, PinMap_LPTIM);
            pinmap_pinout(_pinB, PinMap_LPTIM);
            if (_pinI != NC)
                pinmap_pinout(_pinI, PinMap_LPTIMETR);
            initialized = true;
        }
    }
};


int STM32LowPowerEncoder::needsSearch(){
    return false;
};


int STM32LowPowerEncoder::hasIndex(){
    return 0; // TODO: implement index
};



float STM32LowPowerEncoder::getSensorAngle(){
    return _2PI * (_encoder_handle.Instance->CNT&0xFFFFUL) / (float)cpr;
};

#endif