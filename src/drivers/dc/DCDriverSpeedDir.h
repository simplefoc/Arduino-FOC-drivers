
#pragma once


#include "./DCDriver.h"


/**
 * Intended to drive DC motor drivers with one PWM speed control signal, and one direction control signal.
 * Speed is determined by PWM duty cycle, and direction is determined by the state of the direction control 
 * signal.
 */
class DCDriverSpeedDir : public DCDriver {
    public:
        DCDriverSpeedDir(int pinPWM, int pinDIR, int pinEN = NOT_SET);

        virtual int init() override;

        /** 
         * Set phase voltages to the harware 
         * Positive voltages are associated with the "forward" direction, negative voltages with the "reverse" direction
         * @param U voltage
        */
        virtual void setPwm(float U) override;

        int pinPWM;
        int pinDIR;
};