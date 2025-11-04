
#include "./STM32G4CORDICTrigFunctions.h"

#ifdef HAL_CORDIC_MODULE_ENABLED

#include "Arduino.h"
//#include "stm32g4xx_hal_cordic.h"
#include "stm32g4xx_ll_cordic.h"
#include "stm32g4xx_ll_rcc.h"
#include "stm32g4xx_ll_bus.h"
#include "common/foc_utils.h"
#include "arm_math.h"

CORDIC_HandleTypeDef thisCordic;

bool SimpleFOC_CORDIC_Config(void){
    //__HAL_RCC_CORDIC_CLK_ENABLE();
    // CORDIC_ConfigTypeDef sConfig;
    // thisCordic.Instance = CORDIC;
    // if (HAL_CORDIC_Init(&thisCordic) != HAL_OK) {
    //     Error_Handler();
    //     return false;
    // }

    // sConfig.Function = CORDIC_FUNCTION_COSINE;  
    // sConfig.Precision = CORDIC_PRECISION_6CYCLES;
    // sConfig.Scale = CORDIC_SCALE_0;
    // sConfig.NbWrite = CORDIC_NBWRITE_1;
    // sConfig.NbRead = CORDIC_NBREAD_2;
    // sConfig.InSize = CORDIC_INSIZE_32BITS;
    // sConfig.OutSize = CORDIC_OUTSIZE_32BITS;
    // if (HAL_CORDIC_Configure(&thisCordic, &sConfig) != HAL_OK) {
    //     /* Channel Configuration Error */
    //     Error_Handler();
    //     return false;
    // }

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_CORDIC);
    LL_CORDIC_Config(CORDIC, LL_CORDIC_FUNCTION_COSINE, /* cosine function */
                   LL_CORDIC_PRECISION_6CYCLES,       /* max precision for q1.31 cosine */
                   LL_CORDIC_SCALE_0,                 /* no scale */
                   LL_CORDIC_NBWRITE_1,               /* One input data: angle. Second input data (modulus) is 1 after cordic reset */
                   LL_CORDIC_NBREAD_2,                /* Two output data: cosine, then sine */
                   LL_CORDIC_INSIZE_32BITS,           /* q1.31 format for input data */
                   LL_CORDIC_OUTSIZE_32BITS);         /* q1.31 format for output data */
    return true;
};

static bool arg2_modified = false;


float _sin(float a) {
    float s, c;
    _sincos(a, &s, &c);
    return s;
}

float _cos(float a) {
    float s, c;
    _sincos(a, &s, &c);
    return c;
}

void _sincos(float a, float* s, float* c) {
    a = fmod(a, _2PI);
    if (a>_PI) a -= _2PI;
    if (a<-_PI) a += _2PI;
    LL_CORDIC_SetFunction(CORDIC, LL_CORDIC_FUNCTION_COSINE);
    CORDIC->WDATA = (q31_t)((a / _PI) * 0x80000000);
    if (arg2_modified)
    {
        CORDIC->WDATA = (q31_t)((1.f) * 0x80000000);
    }
    q31_t out_cos = (int32_t)CORDIC->RDATA; // read cosine result
    q31_t out_sin = (int32_t)CORDIC->RDATA; // read sine result
    *c = (float)out_cos / (float)0x80000000;
    *s = (float)out_sin / (float)0x80000000;
    if (arg2_modified)
    {
        LL_CORDIC_SetNbWrite(CORDIC, LL_CORDIC_NBWRITE_1);
        arg2_modified = false;
    }
}

void _atan2mod(float y, float x, float* phase, float* modulus)
{
    float absx = abs(x), absy = abs(y);
    float xbigger = abs(x) >= abs(y);
    float scaler = (xbigger ? absx : absy) * 1.5f; //multiply scaler by 1.5 to avoid saturating modulus
    LL_CORDIC_SetFunction(CORDIC, LL_CORDIC_FUNCTION_PHASE);
    if (!arg2_modified)
    {
        arg2_modified = true;
        LL_CORDIC_SetNbWrite(CORDIC, LL_CORDIC_NBWRITE_2);
    }
    CORDIC->WDATA = (q31_t)((x / scaler) * 0x80000000);
    CORDIC->WDATA = (q31_t)((y / scaler) * 0x80000000);
    q31_t out_phase = (int32_t)CORDIC->RDATA; // read phase result
    q31_t out_modulus = (int32_t)CORDIC->RDATA; // read modulus result
    *phase = (float)out_phase * _PI / (float)0x80000000;
    *modulus = (float)out_modulus / (float)0x80000000 * scaler;
}

float _atan2(float y, float x)
{
    float phase, modulus;
    _atan2mod(y, x, &phase, &modulus);
    return phase;
}

#endif
