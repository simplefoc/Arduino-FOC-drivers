

#include "./TrapezoidalPlanner.h"


TrapezoidalPlanner::TrapezoidalPlanner(float max, float accel, float decel, float min){
    this->max_velocity = max;
    this->accel = accel;
    this->decel = (decel!=NOT_SET)?decel:accel;
    this->min_velocity = (min!=NOT_SET)?min:0.2f;
};


TrapezoidalPlanner::~TrapezoidalPlanner(){};



void TrapezoidalPlanner::linkMotor(FOCMotor& motor){
    this->motor = &motor;
};



void TrapezoidalPlanner::setTarget(float target){
    this->target = target;
    this->start_ang = motor->shaft_angle;
    this->last_target = target;
};



float TrapezoidalPlanner::run(){
    // given the just current position and the target position, calculate the desired velocity
    // assume we're within our acceleration capabilities
    float d = target - motor->shaft_angle;
    float dd = motor->shaft_angle - start_ang;
    // if the target is reached, return 0
    if (abs(d) < arrived_distance && abs(motor->shaft_velocity) < arrived_velocity) return 0;

    // how fast can we be going, based on deceleration?
    float max_v_d = sqrt(2.0f * decel * fabs(d));
    // how fast can we be going, based on accelleration?
    float max_v_dd = sqrt(2.0f * accel * fabs(dd));
    float max_v = min(max_v_d, max_v_dd);
    // how fast can we be going, based on max speed?
    max_v = min(max_v, max_velocity);
    max_v = max(max_v, min_velocity);
    // let's go that speed :-)
    return max_v * _sign(d);
};


