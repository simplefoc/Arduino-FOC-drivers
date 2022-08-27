
#pragma once

#include "common/base_classes/FOCMotor.h"
#include "drivers/dc/DCDriver.h"

/**
 * Class implementing control of DC motors.
 * 
 * SimpleFOC is really for FOC, but it can also be interesting to use its other functionality to control DC motors.
 * This class implements closed-loop control of DC motors based on a position sensor on the motor shaft.
 * 
 * Open-loop control is not implemented. If you want to do open-loop voltage control of DC motors, you can directly
 * use the DCDriver classes. Open-loop velocity or position modes are not supported.
 * 
 */
class DCMotor : public FOCMotor {
public:
    DCMotor();

    /**
     * Function linking a motor and a pwm driver 
     * 
     * @param driver DCDriver class implementing all the hardware specific functions necessary PWM setting
     */
    void linkDriver(DCDriver* driver);

    /** 
      * DCDriver link
      */
    DCDriver* driver;
    /**
     * Voltage set-point
     */
    float voltage_sp = 0.0f;
    
    /**  Motor hardware init function */
  	void init() override;
    /** Motor disable function */
  	void disable() override;
    /** Motor enable function */
    void enable() override;


    /**
     * Function executing the control loops set by the controller parameter of the DCMotor.
     * 
     * @param target  Either voltage, angle or velocity based on the motor.controller
     *                If it is not set the motor will use the target set in its variable motor.target
     * 
     * This function doesn't need to be run upon each loop execution - depends of the use case
     */
    void move(float target = NOT_SET) override;

  /**
    * @param U Voltage to set to the motor
    */
    void setPhaseVoltage(float U);

    // not implemented for DC motors
    virtual int initFOC(float zero_electric_offset = NOT_SET , Direction sensor_direction = Direction::CW) override; 
    // not implemented for DC motors
    virtual void loopFOC() override;

};