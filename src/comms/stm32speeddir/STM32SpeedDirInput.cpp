
#include "./STM32SpeedDirInput.h"

#if defined(_STM32_DEF_)

STM32SpeedDirInput::STM32SpeedDirInput(int pin_speed, int pin_dir) : STM32PWMInput(pin_speed) {
    _pin_speed = pin_speed;
    _pin_dir = pin_dir;
};

STM32SpeedDirInput::~STM32SpeedDirInput(){};

int STM32SpeedDirInput::init(){
    pinMode(_pin_dir, INPUT);
    return STM32PWMInput::initialize();
};


float STM32SpeedDirInput::getTargetVelocity(){
    if (_pin_dir != NOT_SET)
        direction = digitalRead(_pin_dir);
    float speed = getDutyCyclePercent();
    speed = constrain(speed, min_pwm, max_pwm);
    speed = (speed - min_pwm)/(max_pwm - min_pwm) * (max_speed - min_speed) + min_speed;
    return (direction == dir_positive_high) ? speed : -speed;
};


#endif