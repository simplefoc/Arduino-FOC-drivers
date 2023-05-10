
#include "./STM32MagneticSensorPWM.h"

#if defined(_STM32_DEF_)

#include "common/foc_utils.h"


STM32MagneticSensorPWM::STM32MagneticSensorPWM(int pin, uint32_t _min_ticks, uint32_t _max_ticks) : STM32PWMInput(pin), max_ticks(_max_ticks), min_ticks(_min_ticks) {

};



STM32MagneticSensorPWM::~STM32MagneticSensorPWM(){};



void STM32MagneticSensorPWM::init(){
    initialized = (STM32PWMInput::initialize()==0);
    if(initialized)
        Sensor::init();
};



float STM32MagneticSensorPWM::getSensorAngle(){
    uint32_t ticks = getDutyCycleTicks();
    return (ticks - min_ticks) * _2PI / (max_ticks - min_ticks);
};




#endif