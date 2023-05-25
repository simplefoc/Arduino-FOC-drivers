
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
    explicit STM32HWEncoder(unsigned int ppr, int8_t pinA, int8_t pinB, int8_t pinI=-1);

    void init() override;
    int needsSearch() override;
    int hasIndex();  // !< function returning 1 if encoder has index pin and 0 if not.

    bool initialized = false;
    uint32_t cpr;  //!< encoder cpr number
    PinName _pinA, _pinB, _pinI;
    
  protected:
    float getSensorAngle() override;
    
    TIM_HandleTypeDef encoder_handle;

};

#endif