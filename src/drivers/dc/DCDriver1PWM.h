
#pragma once


#include "./DCDriver.h"


/**
 * Intended to drive DC motor drivers with a single PWM control signal, with speed and direction
 * controlled by the same PWM signal.
 * Duty cycles above a given threshold are considered to be "forward", and below that threshold
 * are considered to be "reverse".
 */
class DCDriver1PWM : public DCDriver {
    public:
        DCDriver1PWM(int pinPWM, float threshold = 0.5, int pinEN = NOT_SET);

        virtual int init() override;

        /** 
         * Set phase voltages to the harware 
         * Positive voltages are associated with the "forward" direction, negative voltages with the "reverse" direction
         * @param U voltage
        */
        virtual void setPwm(float U) override;

        int pinPWM;
        bool scale_reverse = true; //!< if true, the reverse scale is full reverse, if false, the reverse scale is the same as the forward scale
        float threshold = 0.5; //!< duty cycle above which the motor is considered to be "forward"
};