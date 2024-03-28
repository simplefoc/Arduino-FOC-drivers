
#pragma once

#include "common/base_classes/FOCMotor.h"

/**
 * Trapezoidal planner
 * 
 * Very simple class to demonstrate higher level control on top of the FOC library.
 * 
 * This class is used to generate trapezoidal velocity profiles for the motor. 
 * Configure the motor in MotionControlMode::velocity. Link the motor to the planner,
 * and set the target angle. The planner will take care of controlling the motor
 * velocity to follow a trapezoidal profile.
 * Call the run() method in the loop, and use the return value to update the motor
 * target velocity. Call the trapezoidal planner less frequently than the motor.move()
 * method, to ensure the motor has time to reach the target velocity.
 * 
 */
class TrapezoidalPlanner {
public:
    TrapezoidalPlanner(float max, float accel, float decel = NOT_SET, float min = NOT_SET);
    ~TrapezoidalPlanner();

    void linkMotor(FOCMotor& motor);
    void setTarget(float target);
    float getTarget() { return target; };

    float run();

    float arrived_distance = 0.02f;
    float arrived_velocity = 0.2f;
    float max_velocity;
    float min_velocity;
    float accel;
    float decel;

protected:
    FOCMotor* motor;
    float target = NOT_SET;
    float last_target = NOT_SET;
    float start_ang = 0.0f;
};
