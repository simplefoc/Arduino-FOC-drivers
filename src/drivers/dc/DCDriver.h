
#pragma once


#include "drivers/hardware_api.h"

class DCDriver{
    public:
        
        /** Initialise hardware */
        virtual int init() = 0;
        /** Enable hardware */
        virtual void enable();
        /** Disable hardware */
        virtual void disable();

        long pwm_frequency; //!< pwm frequency value in hertz
        float voltage_power_supply; //!< power supply voltage 
        float voltage_limit; //!< limiting voltage set to the motor

        int pinEN = NOT_SET;
        bool enable_active_high = true; // !< polarity of enable pin, if used
        bool initialized = false; // true if driver was successfully initialized
        void* params = 0; // pointer to hardware specific parameters of driver

        /** 
         * Set phase voltages to the harware 
         * Positive voltages are associated with the "forward" direction, negative voltages with the "reverse" direction
         * @param U voltage
        */
        virtual void setPwm(float U) = 0;        
};