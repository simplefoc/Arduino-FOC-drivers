#include "STM32HWEncoder.h"

#if defined(_STM32_DEF_)

/*
  HardwareEncoder(int cpr)
*/
STM32HWEncoder::STM32HWEncoder(unsigned int _ppr, int8_t pinA, int8_t pinB, int8_t pinI)
{
    cpr = _ppr * 4; // 4x for quadrature
    _pinA = digitalPinToPinName(pinA);
    _pinB = digitalPinToPinName(pinB);
    _pinI = digitalPinToPinName(pinI);
}

/*
  Shaft angle calculation
*/
float STM32HWEncoder::getSensorAngle()
{
    return _2PI * encoder_handle.Instance->CNT / static_cast<float>(cpr);
}

// getter for index pin
int STM32HWEncoder::needsSearch() { return false; }

// Used to tell if the encoder selected has a hardware TIM_ETR pin.
int STM32HWEncoder::hasHardwareIndex(TIM_TypeDef tim)
{
    /**
     * As far as I can tell, checking F1-7, G4, H7 series parts,
     * these are the only timers which support TIM_ETR indexing.
     * 1-5,8,20-24
     * So we check which timer in use then check if that timer on that
     * chip has hardware index (TIM_ETR).
     */
    switch (tim)
    {
    case TIM1:
#ifdef TIM_TIM1_ETR_GPIO
        return 1;
#endif
        break;

    case TIM2:
#ifdef TIM_TIM2_ETR_GPIO
        return 1;
#endif
        break;

    case TIM3:
#ifdef TIM_TIM3_ETR_GPIO
        return 1;
#endif
        break;

    case TIM4:
#ifdef TIM_TIM4_ETR_GPIO
        return 1;
#endif
        break;

    case TIM5:
#ifdef TIM_TIM5_ETR_GPIO
        return 1;
#endif
        break;

    case TIM8:
#ifdef TIM_TIM8_ETR_GPIO
        return 1;
#endif
        break;

    case TIM20:
#ifdef TIM_TIM20_ETR_GPIO
        return 1;
#endif
        break;

    case TIM21:
#ifdef TIM_TIM21_ETR_GPIO
        return 1;
#endif
        break;

    case TIM22:
#ifdef TIM_TIM22_ETR_GPIO
        return 1;
#endif
        break;

    case TIM23:
#ifdef TIM_TIM23_ETR_GPIO
        return 1;
#endif
        break;

    case TIM24:
#ifdef TIM_TIM24_ETR_GPIO
        return 1;
#endif
        break;

    default:
        break;
    }
    return 0;
}

void STM32HWEncoder::initIndex(TIM_TypeDef tim)
{
    if(hasHardwareIndex(tim))
    {
        TIM_MasterConfigTypeDef sMasterConfig = {0};
        TIMEx_EncoderIndexConfigTypeDef sEncoderIndexConfig = {0};

        sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
        sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
        if (HAL_TIMEx_MasterConfigSynchronization(&tim, &sMasterConfig) != HAL_OK)
        {
            Error_Handler();
        }
        sEncoderIndexConfig.Polarity = TIM_ENCODERINDEX_POLARITY_NONINVERTED;
        sEncoderIndexConfig.Prescaler = TIM_ENCODERINDEX_PRESCALER_DIV1;
        sEncoderIndexConfig.Filter = 0;
        sEncoderIndexConfig.FirstIndexEnable = DISABLE;
        sEncoderIndexConfig.Position = TIM_ENCODERINDEX_POSITION_00;
        sEncoderIndexConfig.Direction = TIM_ENCODERINDEX_DIRECTION_UP_DOWN;
        if (HAL_TIMEx_ConfigEncoderIndex(&tim, &sEncoderIndexConfig) != HAL_OK)
        {
            Error_Handler();
        }
    }
    else
    {

    }
}

// encoder initialisation of the hardware pins
void STM32HWEncoder::init()
{
    // GPIO configuration
    TIM_TypeDef *InstanceA = (TIM_TypeDef *)pinmap_peripheral(_pinA, PinMap_TIM);
    TIM_TypeDef *InstanceB = (TIM_TypeDef *)pinmap_peripheral(_pinB, PinMap_TIM);
    if (InstanceA != InstanceB)
    {
        initialized = false;
        return;
    }
    pinmap_pinout(_pinA, PinMap_TIM);
    pinmap_pinout(_pinB, PinMap_TIM);

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

    if (HAL_TIM_Encoder_Init(&encoder_handle, &encoder_config) != HAL_OK)
    {
        initialized = false;
        return;
    }

    if (HAL_TIM_Encoder_Start(&encoder_handle, TIM_CHANNEL_1) != HAL_OK)
    {
        initialized = false;
        return;
    }

    initialized = true;
}

#endif