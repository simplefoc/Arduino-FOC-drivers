
#include "./DCMotor.h"


DCMotor::DCMotor() {
    torque_controller = TorqueControlType::voltage;
    pole_pairs = 1; // DC motors considered to have 1PP for purposes of shaft-angle calculations
};


void DCMotor::linkDriver(DCDriver* driver) {
    this->driver = driver;
};


void DCMotor::init() {
    if (!driver || !driver->initialized) {
        motor_status = FOCMotorStatus::motor_init_failed;
        SIMPLEFOC_DEBUG("MOT: Init not possible, driver not initialized");
        return;
    }
    if (!sensor) {
        motor_status = FOCMotorStatus::motor_init_failed;
        SIMPLEFOC_DEBUG("MOT: Init not possible, sensor not initialized");
        return;
    }

    motor_status = FOCMotorStatus::motor_initializing;
    SIMPLEFOC_DEBUG("MOT: Init");

    // sanity check for the voltage limit configuration
    if(voltage_limit > driver->voltage_limit) voltage_limit =  driver->voltage_limit;
    // constrain voltage for sensor alignment
    if(voltage_sensor_align > voltage_limit) voltage_sensor_align = voltage_limit;

    // update the controller limits
    if(current_sense){
        // current control loop controls voltage
        PID_current_q.limit = voltage_limit;
        PID_current_d.limit = voltage_limit;
    }
    if(_isset(phase_resistance) || torque_controller != TorqueControlType::voltage){
        // velocity control loop controls current
        PID_velocity.limit = current_limit;
    }else{
        // velocity control loop controls the voltage
        PID_velocity.limit = voltage_limit;
    }
    P_angle.limit = velocity_limit;

    _delay(50);
    // enable motor
    SIMPLEFOC_DEBUG("MOT: Enable driver.");
    enable();
    _delay(50);
    motor_status = FOCMotorStatus::motor_ready;
};


void DCMotor::disable() {
    // set zero to PWM
    driver->setPwm(0);
    // disable the driver
    driver->disable();
    // motor status update
    enabled = 0;
};


void DCMotor::enable() {
    // enable the driver
    driver->enable();
    // set zero to PWM
    driver->setPwm(0);
    // motor status update
    enabled = 1;
};


void DCMotor::move(float target) {
    // get angular velocity
    if (sensor) sensor->update();
    shaft_velocity = shaftVelocity();

    // open-loop not supported
    if( controller==MotionControlType::angle_openloop || controller==MotionControlType::velocity_openloop ) return;
    
    // if disabled do nothing
    if(!enabled) return;

    // get angle
    shaft_angle = shaftAngle();
    shaft_velocity = shaftVelocity();
    switch (controller) {
        case MotionControlType::torque:
            if (torque_controller == TorqueControlType::voltage)
                voltage_sp = target;
            else
                voltage_sp = 0.0f; // TODO implement the other torque modes, and current control loop
            break;
        case MotionControlType::angle:
            // angle set point
            // include angle loop
            shaft_angle_sp = target;
            shaft_velocity_sp = P_angle( shaft_angle_sp - shaft_angle );
            voltage_sp = PID_velocity(shaft_velocity_sp - shaft_velocity);
            break;
        case MotionControlType::velocity:
            // velocity set point
            // inlcude velocity loop
            shaft_velocity_sp = target;
            voltage_sp = PID_velocity(shaft_velocity_sp - shaft_velocity);
        break;
    }
    // set the voltage to the motor
    setPhaseVoltage(voltage_sp);
};


void DCMotor::setPhaseVoltage(float U) {
    if (enabled==1)
        driver->setPwm(U);
};