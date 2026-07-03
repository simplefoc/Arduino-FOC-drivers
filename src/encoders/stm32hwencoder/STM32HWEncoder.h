
#pragma once

#include <Arduino.h>


#if defined(_STM32_DEF_)

#include <HardwareTimer.h>
#include "common/base_classes/Sensor.h"
#include "common/foc_utils.h"

class STM32HWEncoder : public Sensor {
  public:
    /**
    Encoder class constructor
    @param ppr  impulses per rotation  (cpr=ppr*4)
    */
    explicit STM32HWEncoder(unsigned int ppr, int pinA, int pinB, int pinI=-1);

    void init() override;
    int needsSearch() override;
    int hasIndex();  // !< function returning 1 if encoder has index pin and 0 if not.
    void setCurrentAngle(float set_angle); // !< helper function for setting the angle by using the count register
    void setEncoderCount(uint32_t ecount); // !< function for setting the count register
    TIM_HandleTypeDef getEncoderTimerHandle(); // !< function for getting the encoder timer handle

    bool initialized = false;
    uint32_t cpr;  //!< encoder cpr number
    PinName _pinA, _pinB, _pinI;
    bool index_found;
    uint32_t index_polarity = RISING;
    // Use TIM_ENCODERMODE_CLOCKPLUSDIRECTION_X1 for step/dir counting (pinA ch1 dir, pinB ch2 step)
    uint32_t encoder_mode = TIM_ENCODERMODE_TI12; // Must be set before calling init
    
  protected:
    float getSensorAngle() override;
    
    TIM_HandleTypeDef encoder_handle;

};

#endif