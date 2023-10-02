
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



float _sin(float a) {
    a = fmod(a, _2PI);
    if (a>_PI) a -= _2PI;
    if (a<-_PI) a += _2PI;
    CORDIC->WDATA = (q31_t)((a / _PI) * 0x80000000);
    q31_t out_cos = (int32_t)CORDIC->RDATA; // read cosine result
    q31_t out_sin = (int32_t)CORDIC->RDATA; // read sine result
    return (float)out_sin / (float)0x80000000;
}

float _cos(float a) {
    a = fmod(a, _2PI);
    if (a>_PI) a -= _2PI;
    if (a<-_PI) a += _2PI;    
    CORDIC->WDATA = (q31_t)((a / _PI) * 0x80000000);
    q31_t out_cos = (int32_t)CORDIC->RDATA; // read cosine result
    q31_t out_sin = (int32_t)CORDIC->RDATA; // read sine result
    return (float)out_cos / (float)0x80000000;
}

void _sincos(float a, float* s, float* c) {
    a = fmod(a, _2PI);
    if (a>_PI) a -= _2PI;
    if (a<-_PI) a += _2PI;    
    CORDIC->WDATA = (q31_t)((a / _PI) * 0x80000000);
    q31_t out_cos = (int32_t)CORDIC->RDATA; // read cosine result
    q31_t out_sin = (int32_t)CORDIC->RDATA; // read sine result
    *c = (float)out_cos / (float)0x80000000;
    *s = (float)out_sin / (float)0x80000000;
}


#endif
