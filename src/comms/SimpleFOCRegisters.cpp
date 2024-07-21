
#include "./SimpleFOCRegisters.h"
#include "BLDCMotor.h"
#include "./telemetry/Telemetry.h"


SimpleFOCRegisters::SimpleFOCRegisters(){};
SimpleFOCRegisters::~SimpleFOCRegisters(){};


// write the register value(s) for the given motor to the given comms object
bool SimpleFOCRegisters::registerToComms(RegisterIO& comms, uint8_t reg, FOCMotor* motor){
    switch(reg) {
        case SimpleFOCRegister::REG_STATUS:
            comms << (uint8_t)motor->motor_status;
            break;
        case SimpleFOCRegister::REG_ENABLE:
            comms << (uint8_t)motor->enabled;
            break;
        case SimpleFOCRegister::REG_MODULATION_MODE:
            comms << (uint8_t)motor->foc_modulation;
            break;
        case SimpleFOCRegister::REG_TORQUE_MODE:
            comms << (uint8_t)motor->torque_controller;
            break;
        case SimpleFOCRegister::REG_CONTROL_MODE:
            comms << (uint8_t)motor->controller;
            break;
        case SimpleFOCRegister::REG_TARGET:
            comms << motor->target;
            break;
        case SimpleFOCRegister::REG_ANGLE:
            comms << motor->shaft_angle;
            break;
        case SimpleFOCRegister::REG_POSITION:
            if (motor->sensor) {
                comms << (uint32_t)motor->sensor->getFullRotations(); // TODO fix me!
                comms << motor->sensor->getMechanicalAngle();
            }
            else {
                comms << (uint32_t)motor->shaft_angle/_2PI;
                comms << (float)fmod(motor->shaft_angle, _2PI);
            }
            break;
        case SimpleFOCRegister::REG_VELOCITY:
            comms << motor->shaft_velocity;
            break;
        case SimpleFOCRegister::REG_SENSOR_ANGLE:
            if (motor->sensor)
                comms << motor->sensor->getAngle(); // stored angle
            else
                comms << motor->shaft_angle;
            break;
        case SimpleFOCRegister::REG_SENSOR_MECHANICAL_ANGLE:
            if (motor->sensor)
                comms << motor->sensor->getMechanicalAngle(); // stored angle
            else
                comms << motor->shaft_angle;
            break;
        case SimpleFOCRegister::REG_SENSOR_VELOCITY:
            if (motor->sensor)
                comms << motor->sensor->getVelocity(); // stored angle
            else
                comms << motor->shaft_velocity;
            break;
        case SimpleFOCRegister::REG_SENSOR_TIMESTAMP:
            if (motor->sensor)
                //comms << (uint32_t)motor->sensor->angle_prev_ts; // TODO stored angle, make it a public or make this friend class
                comms << (uint32_t)0;
            else
                comms << (uint32_t)0;
            break;

        case SimpleFOCRegister::REG_TELEMETRY_REG:
            if (Telemetry::num_telemetry > 0){
                Telemetry* telemetry = Telemetry::telemetries[Telemetry::telemetry_ctrl];
                comms << telemetry->numRegisters;
                for (uint8_t i=0; i<telemetry->numRegisters; i++) {
                    comms << telemetry->registers_motor[i];
                    comms << telemetry->registers[i];
                }
                telemetry->headerSent = false;
            }
            else {
                comms << (uint32_t)0;
            }
            break;
        case SimpleFOCRegister::REG_TELEMETRY_CTRL:
            comms << (Telemetry::telemetry_ctrl);
            break;
        case SimpleFOCRegister::REG_TELEMETRY_DOWNSAMPLE:
            if (Telemetry::num_telemetry > 0)
                comms << (uint32_t)(Telemetry::telemetries[Telemetry::telemetry_ctrl]->downsample);
            else
                comms << (uint32_t)0;
            break;
        case SimpleFOCRegister::REG_ITERATIONS_SEC:
            if (Telemetry::num_telemetry > 0)
                comms << (Telemetry::telemetries[0]->last_iterations);
            else
                comms << (uint32_t)0;
            break;


        case SimpleFOCRegister::REG_PHASE_VOLTAGE:
            comms << ((BLDCMotor*)motor)->Ua;
            comms << ((BLDCMotor*)motor)->Ub;
            comms << ((BLDCMotor*)motor)->Uc;
            break;
        case SimpleFOCRegister::REG_PHASE_STATE:
            // TODO implement - need to refactor BLDCDriver to make it accessible
            break;
        case SimpleFOCRegister::REG_DRIVER_ENABLE: // write only at the moment
            return false;

        case SimpleFOCRegister::REG_VOLTAGE_Q:
            comms << motor->voltage.q;
            break;
        case SimpleFOCRegister::REG_VOLTAGE_D:
            comms << motor->voltage.d;
            break;
        case SimpleFOCRegister::REG_CURRENT_Q:
            comms << motor->current.q;
            break;
        case SimpleFOCRegister::REG_CURRENT_D:
            comms << motor->current.d;
            break;
        case SimpleFOCRegister::REG_CURRENT_A:
            if (motor->current_sense)
                comms << motor->current_sense->getPhaseCurrents().a;
            else
                comms << 0.0f;
            break;
        case SimpleFOCRegister::REG_CURRENT_B:
            if (motor->current_sense)
                comms << motor->current_sense->getPhaseCurrents().b;
            else
                comms << 0.0f;
            break;
        case SimpleFOCRegister::REG_CURRENT_C:
            if (motor->current_sense)
                comms << motor->current_sense->getPhaseCurrents().c;
            else
                comms << 0.0f;
            break;
        case SimpleFOCRegister::REG_CURRENT_ABC:
            if (motor->current_sense) {
                PhaseCurrent_s currents = motor->current_sense->getPhaseCurrents();
                comms << currents.a;
                comms << currents.b;
                comms << currents.c;
            }
            else {
                comms << 0.0f;
                comms << 0.0f;
                comms << 0.0f;
            }
            break;
        case SimpleFOCRegister::REG_VEL_PID_P:
            comms << motor->PID_velocity.P;
            break;
        case SimpleFOCRegister::REG_VEL_PID_I:
            comms << motor->PID_velocity.I;
            break;
        case SimpleFOCRegister::REG_VEL_PID_D:
            comms << motor->PID_velocity.D;
            break;
        case SimpleFOCRegister::REG_VEL_PID_LIM:
            comms << motor->PID_velocity.limit;
            break;
        case SimpleFOCRegister::REG_VEL_PID_RAMP:
            comms << motor->PID_velocity.output_ramp;
            break;
        case SimpleFOCRegister::REG_VEL_LPF_T:
            comms << motor->LPF_velocity.Tf;
            break;
        case SimpleFOCRegister::REG_ANG_PID_P:
            comms << motor->P_angle.P;
            break;
        case SimpleFOCRegister::REG_ANG_PID_I:
            comms << motor->P_angle.I;
            break;
        case SimpleFOCRegister::REG_ANG_PID_D:
            comms << motor->P_angle.D;
            break;
        case SimpleFOCRegister::REG_ANG_PID_LIM:
            comms << motor->P_angle.limit;
            break;
        case SimpleFOCRegister::REG_ANG_PID_RAMP:
            comms << motor->P_angle.output_ramp;
            break;
        case SimpleFOCRegister::REG_ANG_LPF_T:
            comms << motor->LPF_angle.Tf;
            break;

        case SimpleFOCRegister::REG_CURQ_PID_P:
            comms << motor->PID_current_q.P;
            break;
        case SimpleFOCRegister::REG_CURQ_PID_I:
            comms << motor->PID_current_q.I;
            break;
        case SimpleFOCRegister::REG_CURQ_PID_D:
            comms << motor->PID_current_q.D;
            break;
        case SimpleFOCRegister::REG_CURQ_PID_LIM:
            comms << motor->PID_current_q.limit;
            break;
        case SimpleFOCRegister::REG_CURQ_PID_RAMP:
            comms << motor->PID_current_q.output_ramp;
            break;
        case SimpleFOCRegister::REG_CURQ_LPF_T:
            comms << motor->LPF_current_q.Tf;
            break;
        case SimpleFOCRegister::REG_CURD_PID_P:
            comms << motor->PID_current_d.P;
            break;
        case SimpleFOCRegister::REG_CURD_PID_I:
            comms << motor->PID_current_d.I;
            break;
        case SimpleFOCRegister::REG_CURD_PID_D:
            comms << motor->PID_current_d.D;
            break;
        case SimpleFOCRegister::REG_CURD_PID_LIM:
            comms << motor->PID_current_d.limit;
            break;
        case SimpleFOCRegister::REG_CURD_PID_RAMP:
            comms << motor->PID_current_d.output_ramp;
            break;
        case SimpleFOCRegister::REG_CURD_LPF_T:
            comms << motor->LPF_current_d.Tf;
            break;

        case SimpleFOCRegister::REG_VOLTAGE_LIMIT:
            comms << motor->voltage_limit;
            break;
        case SimpleFOCRegister::REG_CURRENT_LIMIT:
            comms << motor->current_limit;
            break;
        case SimpleFOCRegister::REG_VELOCITY_LIMIT:
            comms << motor->velocity_limit;
            break;
        case SimpleFOCRegister::REG_MOTION_DOWNSAMPLE:
            comms << (uint8_t)motor->motion_downsample;
            break;
        case SimpleFOCRegister::REG_DRIVER_VOLTAGE_LIMIT:
            comms << ((BLDCMotor*)motor)->driver->voltage_limit;  // TODO handle stepper motors
            break;
        case SimpleFOCRegister::REG_DRIVER_VOLTAGE_PSU:
            comms << (((BLDCMotor*)motor)->driver->voltage_power_supply);
            return true;
        case SimpleFOCRegister::REG_VOLTAGE_SENSOR_ALIGN:
            comms << (((BLDCMotor*)motor)->voltage_sensor_align);
            return true;            
        case SimpleFOCRegister::REG_PWM_FREQUENCY:
            comms << (uint32_t)((BLDCMotor*)motor)->driver->pwm_frequency; // TODO handle stepper motors
            break;

        case SimpleFOCRegister::REG_ZERO_ELECTRIC_ANGLE:
            comms << motor->zero_electric_angle;
            break;
        case SimpleFOCRegister::REG_SENSOR_DIRECTION:
            comms << (uint8_t)motor->sensor_direction;
            break;
        case SimpleFOCRegister::REG_ZERO_OFFSET:
            comms << motor->sensor_offset;
            break;
        case SimpleFOCRegister::REG_PHASE_RESISTANCE:
            comms << motor->phase_resistance;
            break;
        case SimpleFOCRegister::REG_KV:
            comms << motor->KV_rating;
            break;
        case SimpleFOCRegister::REG_INDUCTANCE:
            comms << motor->phase_inductance;
            break;
        case SimpleFOCRegister::REG_POLE_PAIRS:
            comms << (uint8_t)motor->pole_pairs;
            break;

        // case SimpleFOCRegister::REG_CURA_GAIN:
        //     if (motor->current_sense)
        //         comms << motor->current_sense->gain_a;
        //     else
        //         comms << 0.0f;
        //     break;
        // case SimpleFOCRegister::REG_CURB_GAIN:
        //     if (motor->current_sense)
        //         comms << motor->current_sense->gain_b;
        //     else
        //         comms << 0.0f;
        //     break;
        // case SimpleFOCRegister::REG_CURC_GAIN:
        //     if (motor->current_sense)
        //         comms << motor->current_sense->gain_c;
        //     else
        //         comms << 0.0f;
        //     break;
        // case SimpleFOCRegister::REG_CURA_OFFSET:
        //     if (motor->current_sense)
        //         comms << motor->current_sense->offset_a;
        //     else
        //         comms << 0.0f;
        //     break;
        // case SimpleFOCRegister::REG_CURB_OFFSET:
        //     if (motor->current_sense)
        //         comms << motor->current_sense->offset_b;
        //     else
        //         comms << 0.0f;
        //     break;
        // case SimpleFOCRegister::REG_CURC_OFFSET:
        //     if (motor->current_sense)
        //         comms << motor->current_sense->offset_c;
        //     else
        //         comms << 0.0f;
        //     break;

        case SimpleFOCRegister::REG_SYS_TIME:
            // TODO how big is millis()? Same on all platforms?
            comms << (uint32_t)(int)millis();
            break;
        // unknown register or write only register (no read) or can't handle in superclass
        case SimpleFOCRegister::REG_NUM_MOTORS:
        case SimpleFOCRegister::REG_MOTOR_ADDRESS:
        case SimpleFOCRegister::REG_ENABLE_ALL:
        default:
            return false;
    }
    return true;
};


// read the register value(s) for the given motor from the given comms object
bool SimpleFOCRegisters::commsToRegister(RegisterIO& comms, uint8_t reg, FOCMotor* motor) {
    uint32_t val32;
    uint8_t val8 = 0;
    switch(reg) {
        case REG_ENABLE:
            comms >> val8;
            (val8>0)?motor->enable():motor->disable();            
            return true;
        case SimpleFOCRegister::REG_MODULATION_MODE:
            comms >> val8;
            motor->foc_modulation = static_cast<FOCModulationType>(val8);
            return true;
        case SimpleFOCRegister::REG_TORQUE_MODE:
            comms >> val8;
            motor->torque_controller = static_cast<TorqueControlType>(val8);
            return true;
        case SimpleFOCRegister::REG_CONTROL_MODE:
            comms >> val8;
            motor->controller = static_cast<MotionControlType>(val8);
            return true;
        case SimpleFOCRegister::REG_TARGET:
            comms >> (motor->target);
            return true;

        case SimpleFOCRegister::REG_TELEMETRY_DOWNSAMPLE:
            comms >> val32;
            if (Telemetry::telemetry_ctrl < Telemetry::num_telemetry)
                Telemetry::telemetries[Telemetry::telemetry_ctrl]->downsample = (uint16_t)val32;
            return true;
        case SimpleFOCRegister::REG_TELEMETRY_CTRL:
            comms >> val8;
            if (val8 < Telemetry::num_telemetry)
                Telemetry::telemetry_ctrl = val8;
            return true;
        case SimpleFOCRegister::REG_TELEMETRY_REG:
            if (Telemetry::telemetry_ctrl < Telemetry::num_telemetry){
                Telemetry* telemetry = Telemetry::telemetries[Telemetry::telemetry_ctrl];
                uint8_t numRegisters;
                comms >> numRegisters;
                uint8_t registers[numRegisters];
                uint8_t motors[numRegisters];
                for (uint8_t i=0; i<numRegisters; i++) {
                    comms >> motors[i];
                    comms >> registers[i];
                }
                telemetry->setTelemetryRegisters(numRegisters, registers, motors);
            }
            return true;

        case SimpleFOCRegister::REG_PHASE_VOLTAGE:
            float va, vb, vc;
            comms >> va >> vb >> vc;
            if (motor->enabled) {
                motor->disable();
                ((BLDCMotor*)motor)->driver->enable();
            }
            ((BLDCMotor*)motor)->driver->setPwm(va, vb, vc);
            return true;
        case SimpleFOCRegister::REG_PHASE_STATE:
            uint8_t sa, sb, sc;
            comms >> sa >> sb >> sc;
            ((BLDCMotor*)motor)->driver->setPhaseState((PhaseState)sa, (PhaseState)sb, (PhaseState)sc);
            return true;
        case SimpleFOCRegister::REG_DRIVER_ENABLE:
            comms >> val8;
            if (val8>0)
                ((BLDCMotor*)motor)->driver->enable();
            else
                ((BLDCMotor*)motor)->driver->disable();
            return true;

        case SimpleFOCRegister::REG_VEL_PID_P:
            comms >> (motor->PID_velocity.P);
            return true;
        case SimpleFOCRegister::REG_VEL_PID_I:
            comms >> (motor->PID_velocity.I);
            return true;
        case SimpleFOCRegister::REG_VEL_PID_D:
            comms >> (motor->PID_velocity.D);
            return true;
        case SimpleFOCRegister::REG_VEL_PID_LIM:
            comms >> (motor->PID_velocity.limit);
            return true;
        case SimpleFOCRegister::REG_VEL_PID_RAMP:
            comms >> (motor->PID_velocity.output_ramp);
            return true;
        case SimpleFOCRegister::REG_VEL_LPF_T:
            comms >> (motor->LPF_velocity.Tf);
            return true;
        case SimpleFOCRegister::REG_ANG_PID_P:
            comms >> (motor->P_angle.P);
            return true;
        case SimpleFOCRegister::REG_ANG_PID_I:
            comms >> (motor->P_angle.I);
            return true;
        case SimpleFOCRegister::REG_ANG_PID_D:
            comms >> (motor->P_angle.D);
            return true;
        case SimpleFOCRegister::REG_ANG_PID_LIM:
            comms >> (motor->P_angle.limit);
            return true;
        case SimpleFOCRegister::REG_ANG_PID_RAMP:
            comms >> (motor->P_angle.output_ramp);
            return true;
        case SimpleFOCRegister::REG_ANG_LPF_T:
            comms >> (motor->LPF_angle.Tf);
            return true;

        case SimpleFOCRegister::REG_CURQ_PID_P:
            comms >> (motor->PID_current_q.P);
            return true;
        case SimpleFOCRegister::REG_CURQ_PID_I:
            comms >> (motor->PID_current_q.I);
            return true;
        case SimpleFOCRegister::REG_CURQ_PID_D:
            comms >> (motor->PID_current_q.D);
            return true;
        case SimpleFOCRegister::REG_CURQ_PID_LIM:
            comms >> (motor->PID_current_q.limit);
            return true;
        case SimpleFOCRegister::REG_CURQ_PID_RAMP:
            comms >> (motor->PID_current_q.output_ramp);
            return true;
        case SimpleFOCRegister::REG_CURQ_LPF_T:
            comms >> (motor->LPF_current_q.Tf);
            return true;
        case SimpleFOCRegister::REG_CURD_PID_P:
            comms >> (motor->PID_current_d.P);
            return true;
        case SimpleFOCRegister::REG_CURD_PID_I:
            comms >> (motor->PID_current_d.I);
            return true;
        case SimpleFOCRegister::REG_CURD_PID_D:
            comms >> (motor->PID_current_d.D);
            return true;
        case SimpleFOCRegister::REG_CURD_PID_LIM:
            comms >> (motor->PID_current_d.limit);
            return true;
        case SimpleFOCRegister::REG_CURD_PID_RAMP:
            comms >> (motor->PID_current_d.output_ramp);
            return true;
        case SimpleFOCRegister::REG_CURD_LPF_T:
            comms >> (motor->LPF_current_d.Tf);
            return true;

        case SimpleFOCRegister::REG_VOLTAGE_LIMIT:
            comms >> (motor->voltage_limit);
            if (motor->phase_resistance==NOT_SET){
                motor->PID_velocity.limit = motor->voltage_limit;
                //if (motor->controller==MotionControlType::angle_nocascade)
                //    motor->P_angle.limit = motor->voltage_limit;
            }
            return true;
        case SimpleFOCRegister::REG_CURRENT_LIMIT:
            comms >> (motor->current_limit);
            if (motor->phase_resistance!=NOT_SET) {
                motor->PID_velocity.limit = motor->current_limit;
                //if (motor->controller==MotionControlType::angle_nocascade)
                //    motor->P_angle.limit = motor->current_limit;
            }
            return true;
        case SimpleFOCRegister::REG_VELOCITY_LIMIT:
            comms >> (motor->velocity_limit);
            //if (motor->controller!=MotionControlType::angle_nocascade)
            //    motor->P_angle.limit = motor->velocity_limit;
            return true;
        case SimpleFOCRegister::REG_MOTION_DOWNSAMPLE:
            comms >> val8;
            motor->motion_downsample = val8; // change to use 4 bytes?
            return true;
        case SimpleFOCRegister::REG_DRIVER_VOLTAGE_LIMIT:
            comms >> (((BLDCMotor*)motor)->driver->voltage_limit);
            return true;
        case SimpleFOCRegister::REG_DRIVER_VOLTAGE_PSU:
            comms >> (((BLDCMotor*)motor)->driver->voltage_power_supply);
            return true;
        case SimpleFOCRegister::REG_VOLTAGE_SENSOR_ALIGN:
            comms >> (((BLDCMotor*)motor)->voltage_sensor_align);
            return true;
        case SimpleFOCRegister::REG_PWM_FREQUENCY:
            comms >> val32;
            ((BLDCMotor*)motor)->driver->pwm_frequency = val32;
            return true;

        case SimpleFOCRegister::REG_ZERO_ELECTRIC_ANGLE:
            comms >> (motor->zero_electric_angle);
            return true;
        case SimpleFOCRegister::REG_SENSOR_DIRECTION:
            comms >> val8;
            motor->sensor_direction = static_cast<Direction>(val8);
            return true;
        case SimpleFOCRegister::REG_ZERO_OFFSET:
            comms >> (motor->sensor_offset);
            return true;
        case SimpleFOCRegister::REG_PHASE_RESISTANCE:
            comms >> (motor->phase_resistance);
            return true;
        case SimpleFOCRegister::REG_KV:
            comms >> (motor->KV_rating);
            return true;
        case SimpleFOCRegister::REG_INDUCTANCE:
            comms >> (motor->phase_inductance);
            return true;
        case SimpleFOCRegister::REG_POLE_PAIRS:
            comms >> val8;
            motor->pole_pairs = val8;
            return true;

        // case SimpleFOCRegister::REG_CURA_GAIN:
        //     comms >> va;
        //     if (motor->current_sense)
        //         motor->current_sense->gain_a = va;
        //     return true;
        // case SimpleFOCRegister::REG_CURB_GAIN:
        //     comms >> vb;
        //     if (motor->current_sense)
        //         motor->current_sense->gain_b = vb;
        //     return true;
        // case SimpleFOCRegister::REG_CURC_GAIN:
        //     comms >> vc;
        //     if (motor->current_sense)
        //         motor->current_sense->gain_c = vc;
        //     return true;
        // case SimpleFOCRegister::REG_CURA_OFFSET:
        //     comms >> va;
        //     if (motor->current_sense)
        //         motor->current_sense->offset_a = va;
        //     return true;
        // case SimpleFOCRegister::REG_CURB_OFFSET:
        //     comms >> vb;
        //     if (motor->current_sense)
        //         motor->current_sense->offset_b = vb;
        //     return true;
        // case SimpleFOCRegister::REG_CURC_OFFSET:
        //     comms >> vc;
        //     if (motor->current_sense)
        //         motor->current_sense->offset_c = vc;
        //     return true;

        // unknown register or read-only register (no write) or can't handle in superclass
        case SimpleFOCRegister::REG_ITERATIONS_SEC:
        case SimpleFOCRegister::REG_STATUS:
        case SimpleFOCRegister::REG_ANGLE:
        case SimpleFOCRegister::REG_POSITION:
        case SimpleFOCRegister::REG_VELOCITY:
        case SimpleFOCRegister::REG_SENSOR_ANGLE:
        case SimpleFOCRegister::REG_SENSOR_MECHANICAL_ANGLE:
        case SimpleFOCRegister::REG_SENSOR_VELOCITY:
        case SimpleFOCRegister::REG_SENSOR_TIMESTAMP:
        case SimpleFOCRegister::REG_VOLTAGE_Q:
        case SimpleFOCRegister::REG_VOLTAGE_D:
        case SimpleFOCRegister::REG_CURRENT_Q:
        case SimpleFOCRegister::REG_CURRENT_D:
        case SimpleFOCRegister::REG_CURRENT_A:
        case SimpleFOCRegister::REG_CURRENT_B:
        case SimpleFOCRegister::REG_CURRENT_C:
        case SimpleFOCRegister::REG_CURRENT_ABC:
        case SimpleFOCRegister::REG_SYS_TIME:
        case SimpleFOCRegister::REG_NUM_MOTORS:
        case SimpleFOCRegister::REG_MOTOR_ADDRESS:
        case SimpleFOCRegister::REG_ENABLE_ALL:
        default:
            return false;
    }
    return false;
};



// return the size of the register values, when output to comms, in bytes
uint8_t SimpleFOCRegisters::sizeOfRegister(uint8_t reg){
    switch (reg) {
        case SimpleFOCRegister::REG_TARGET:
        case SimpleFOCRegister::REG_ANGLE:
        case SimpleFOCRegister::REG_VELOCITY:
        case SimpleFOCRegister::REG_SENSOR_ANGLE:
        case SimpleFOCRegister::REG_SENSOR_MECHANICAL_ANGLE:
        case SimpleFOCRegister::REG_SENSOR_VELOCITY:
        case SimpleFOCRegister::REG_SENSOR_TIMESTAMP:        
        case SimpleFOCRegister::REG_VOLTAGE_Q:
        case SimpleFOCRegister::REG_VOLTAGE_D:
        case SimpleFOCRegister::REG_CURRENT_Q:
        case SimpleFOCRegister::REG_CURRENT_D:
        case SimpleFOCRegister::REG_CURRENT_A:
        case SimpleFOCRegister::REG_CURRENT_B:
        case SimpleFOCRegister::REG_CURRENT_C:
        case SimpleFOCRegister::REG_VEL_PID_P:
        case SimpleFOCRegister::REG_VEL_PID_I:
        case SimpleFOCRegister::REG_VEL_PID_D:
        case SimpleFOCRegister::REG_VEL_PID_LIM:
        case SimpleFOCRegister::REG_VEL_PID_RAMP:
        case SimpleFOCRegister::REG_VEL_LPF_T:
        case SimpleFOCRegister::REG_ANG_PID_P:
        case SimpleFOCRegister::REG_ANG_PID_I:
        case SimpleFOCRegister::REG_ANG_PID_D:
        case SimpleFOCRegister::REG_ANG_PID_LIM:
        case SimpleFOCRegister::REG_ANG_PID_RAMP:
        case SimpleFOCRegister::REG_ANG_LPF_T:
        case SimpleFOCRegister::REG_CURQ_PID_P:
        case SimpleFOCRegister::REG_CURQ_PID_I:
        case SimpleFOCRegister::REG_CURQ_PID_D:
        case SimpleFOCRegister::REG_CURQ_PID_LIM:
        case SimpleFOCRegister::REG_CURQ_PID_RAMP:
        case SimpleFOCRegister::REG_CURQ_LPF_T:
        case SimpleFOCRegister::REG_CURD_PID_P:
        case SimpleFOCRegister::REG_CURD_PID_I:
        case SimpleFOCRegister::REG_CURD_PID_D:
        case SimpleFOCRegister::REG_CURD_PID_LIM:
        case SimpleFOCRegister::REG_CURD_PID_RAMP:
        case SimpleFOCRegister::REG_CURD_LPF_T:
        case SimpleFOCRegister::REG_VOLTAGE_LIMIT:
        case SimpleFOCRegister::REG_CURRENT_LIMIT:
        case SimpleFOCRegister::REG_VELOCITY_LIMIT:
        case SimpleFOCRegister::REG_DRIVER_VOLTAGE_LIMIT:
        case SimpleFOCRegister::REG_DRIVER_VOLTAGE_PSU:
        case SimpleFOCRegister::REG_VOLTAGE_SENSOR_ALIGN:
        case SimpleFOCRegister::REG_PWM_FREQUENCY:
        case SimpleFOCRegister::REG_ZERO_ELECTRIC_ANGLE:
        case SimpleFOCRegister::REG_ZERO_OFFSET:
        case SimpleFOCRegister::REG_PHASE_RESISTANCE:
        case SimpleFOCRegister::REG_KV:
        case SimpleFOCRegister::REG_INDUCTANCE:
        case SimpleFOCRegister::REG_TELEMETRY_DOWNSAMPLE:
        case SimpleFOCRegister::REG_ITERATIONS_SEC:
        case SimpleFOCRegister::REG_CURA_GAIN:
        case SimpleFOCRegister::REG_CURB_GAIN:
        case SimpleFOCRegister::REG_CURC_GAIN:
        case SimpleFOCRegister::REG_CURA_OFFSET:
        case SimpleFOCRegister::REG_CURB_OFFSET:
        case SimpleFOCRegister::REG_CURC_OFFSET:     
            return 4;
        case SimpleFOCRegister::REG_SYS_TIME:
            return 4; // TODO how big is millis()? Same on all platforms?
        case SimpleFOCRegister::REG_MOTION_DOWNSAMPLE:
        case SimpleFOCRegister::REG_SENSOR_DIRECTION:
        case SimpleFOCRegister::REG_POLE_PAIRS:
        case SimpleFOCRegister::REG_STATUS:
        case SimpleFOCRegister::REG_ENABLE:
        case SimpleFOCRegister::REG_MODULATION_MODE:
        case SimpleFOCRegister::REG_TORQUE_MODE:
        case SimpleFOCRegister::REG_CONTROL_MODE:
        case SimpleFOCRegister::REG_NUM_MOTORS:   
        case SimpleFOCRegister::REG_MOTOR_ADDRESS:
        case SimpleFOCRegister::REG_TELEMETRY_CTRL:
            return 1;
        case SimpleFOCRegister::REG_POSITION:
            return 8;
        case SimpleFOCRegister::REG_CURRENT_ABC:
        case SimpleFOCRegister::REG_PHASE_VOLTAGE:
            return 12;
        case SimpleFOCRegister::REG_PHASE_STATE:
            return 3;
        case SimpleFOCRegister::REG_TELEMETRY_REG:
            if (Telemetry::num_telemetry > 0) {
                Telemetry* telemetry = Telemetry::telemetries[Telemetry::telemetry_ctrl];
                return 2*telemetry->numRegisters + 1;
            }
            else
                return 1;
        case SimpleFOCRegister::REG_DRIVER_ENABLE:
        case SimpleFOCRegister::REG_ENABLE_ALL: // write-only
        default: // unknown register or write only register (no output) or can't handle in superclass
            return 0;
    }
};


SimpleFOCRegisters* SimpleFOCRegisters::regs = new SimpleFOCRegisters();
