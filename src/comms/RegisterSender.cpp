
#include "./RegisterSender.h"
#include "common/foc_utils.h"
#include "BLDCMotor.h"

bool RegisterSender::sendRegister(SimpleFOCRegister reg, FOCMotor* motor){
    // write a register value for the given motor
    switch(reg) {
        case REG_STATUS:
            writeByte(motor->motor_status);
            break;
        case REG_ENABLE:
            writeByte(motor->enabled);
            break;
        case REG_MODULATION_MODE:
            writeByte(motor->foc_modulation);
            break;
        case REG_TORQUE_MODE:
            writeByte(motor->torque_controller);
            break;
        case REG_CONTROL_MODE:
            writeByte(motor->controller);
            break;
        case REG_TARGET:
            writeFloat(motor->target);
            break;
        case REG_ANGLE:
            writeFloat(motor->shaft_angle);
            break;
        case REG_POSITION:
            if (motor->sensor) {
                writeInt(motor->sensor->getFullRotations());
                writeFloat(motor->sensor->getMechanicalAngle());
            }
            else {
                writeInt(motor->shaft_angle/_2PI);
                writeFloat(fmod(motor->shaft_angle, _2PI));
            }
            break;
        case REG_VELOCITY:
            writeFloat(motor->shaft_velocity);
            break;
        case REG_SENSOR_ANGLE:
            if (motor->sensor)
                writeFloat(motor->sensor->getAngle()); // stored angle
            else
                writeFloat(motor->shaft_angle);
            break;

        case REG_VOLTAGE_Q:
            writeFloat(motor->voltage.q);
            break;
        case REG_VOLTAGE_D:
            writeFloat(motor->voltage.d);
            break;
        case REG_CURRENT_Q:
            writeFloat(motor->current.q);
            break;
        case REG_CURRENT_D:
            writeFloat(motor->current.d);
            break;
        case REG_CURRENT_A:
            if (motor->current_sense)
                writeFloat(motor->current_sense->getPhaseCurrents().a);
            else
                writeFloat(0.0f);
            break;
        case REG_CURRENT_B:
            if (motor->current_sense)
                writeFloat(motor->current_sense->getPhaseCurrents().b);
            else
                writeFloat(0.0f);
            break;
        case REG_CURRENT_C:
            if (motor->current_sense)
                writeFloat(motor->current_sense->getPhaseCurrents().c);
            else
                writeFloat(0.0f);
            break;
        case REG_CURRENT_ABC:
            if (motor->current_sense) {
                PhaseCurrent_s currents = motor->current_sense->getPhaseCurrents();
                writeFloat(currents.a);
                writeFloat(currents.b);
                writeFloat(currents.c);
            }
            else {
                writeFloat(0.0f);
                writeFloat(0.0f);
                writeFloat(0.0f);
            }
            break;
        case REG_VEL_PID_P:
            writeFloat(motor->PID_velocity.P);
            break;
        case REG_VEL_PID_I:
            writeFloat(motor->PID_velocity.I);
            break;
        case REG_VEL_PID_D:
            writeFloat(motor->PID_velocity.D);
            break;
        case REG_VEL_LPF_T:
            writeFloat(motor->LPF_velocity.Tf);
            break;
        case REG_ANG_PID_P:
            writeFloat(motor->P_angle.P);
            break;
        case REG_VEL_LIMIT:
            writeFloat(motor->velocity_limit);
            break;
        case REG_VEL_MAX_RAMP:
            writeFloat(motor->PID_velocity.output_ramp);
            break;

        case REG_CURQ_PID_P:
            writeFloat(motor->PID_current_q.P);
            break;
        case REG_CURQ_PID_I:
            writeFloat(motor->PID_current_q.I);
            break;
        case REG_CURQ_PID_D:
            writeFloat(motor->PID_current_q.D);
            break;
        case REG_CURQ_LPF_T:
            writeFloat(motor->LPF_current_q.Tf);
            break;
        case REG_CURD_PID_P:
            writeFloat(motor->PID_current_d.P);
            break;
        case REG_CURD_PID_I:
            writeFloat(motor->PID_current_d.I);
            break;
        case REG_CURD_PID_D:
            writeFloat(motor->PID_current_d.D);
            break;
        case REG_CURD_LPF_T:
            writeFloat(motor->LPF_current_d.Tf);
            break;

        case REG_VOLTAGE_LIMIT:
            writeFloat(motor->voltage_limit);
            break;
        case REG_CURRENT_LIMIT:
            writeFloat(motor->current_limit);
            break;
        case REG_MOTION_DOWNSAMPLE:
            writeByte((uint8_t)motor->motion_downsample);
            break;
        case REG_DRIVER_VOLTAGE_LIMIT:
            writeFloat(((BLDCMotor*)motor)->driver->voltage_limit);
            break;
        case REG_PWM_FREQUENCY:
            writeInt(((BLDCMotor*)motor)->driver->pwm_frequency);
            break;

        case REG_ZERO_ELECTRIC_ANGLE:
            writeFloat(motor->zero_electric_angle);
            break;
        case REG_SENSOR_DIRECTION:
            writeByte((int8_t)motor->sensor_direction);
            break;
        case REG_ZERO_OFFSET:
            writeFloat(motor->sensor_offset);
            break;
        case REG_PHASE_RESISTANCE:
            writeFloat(motor->phase_resistance);
            break;
        case REG_KV:
            writeFloat(motor->KV_rating);
            break;
        case REG_INDUCTANCE:
            writeFloat(motor->phase_inductance);
            break;
        case REG_POLE_PAIRS:
            writeByte((uint8_t)motor->pole_pairs);
            break;

        case REG_SYS_TIME:
            // TODO how big is millis()? Same on all platforms?
            writeInt((int)millis());
            break;
        // unknown register or write only register (no read) or can't handle in superclass
        case REG_NUM_MOTORS:   
        case REG_REPORT:
        case REG_MOTOR_ADDRESS:
        case REG_ENABLE_ALL:        
        default:
            writeByte(0); // TODO what to send in this case?
            return false;
    }
    return true;
};

