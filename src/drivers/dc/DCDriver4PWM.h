
#pragma once


#include "./DCDriver.h"


/**
 * Intended to drive DC motor drivers with independent control of all four FETs of the 
 * two half-bridges.
 * Speed is determined by duty cycle. By convention the on-state half-bridge is driven with PWM, and
 * the off-state half-bridge is held low.
 */
class DCDriver4PWM : public DCDriver {
    public:
        DCDriver4PWM(int pinA_h, int pinA_l, int pinB_h, int pinB_l, int pinEN = NOT_SET);

        virtual int init() override;

        /** 
         * Set phase voltages to the harware 
         * Positive voltages are associated with the "forward" direction, negative voltages with the "reverse" direction
         * @param U voltage
        */
        virtual void setPwm(float U) override;
        
        int pinA_h;
        int pinA_l;
        int pinB_h;
        int pinB_l;

};