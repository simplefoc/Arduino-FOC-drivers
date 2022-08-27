
#pragma once


#include "./DCDriver.h"


/**
 * Intended to drive DC motor drivers with two PWM control signals, i.e. independent control of
 * the half-bridges.
 * Speed is determined by duty cycle. By convention the on-state half-bridge is driven with PWM, and
 * the off-state half-bridge is held low.
 */
class DCDriver2PWM : public DCDriver {
    public:
        DCDriver2PWM(int pinA, int pinB, int pinEN = NOT_SET);

        virtual int init() override;

        /** 
         * Set phase voltages to the harware 
         * Positive voltages are associated with the "forward" direction, negative voltages with the "reverse" direction
         * @param U voltage
        */
        virtual void setPwm(float U) override;

        int pinA;
        int pinB;

};