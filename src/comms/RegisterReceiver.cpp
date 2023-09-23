
#include "./RegisterReceiver.h"
#include "BLDCMotor.h"


void RegisterReceiver::setRegister(SimpleFOCRegister reg, FOCMotor* motor){
    // write a register value for the given motor
    switch(reg) {
        case REG_ENABLE:
            readByte((uint8_t*)&(motor->enabled));
            break;
        case REG_MODULATION_MODE:
            readByte((uint8_t*)&(motor->foc_modulation));
            break;
        case REG_TORQUE_MODE:
            readByte((uint8_t*)&(motor->torque_controller));
            break;
        case REG_CONTROL_MODE:
            readByte((uint8_t*)&(motor->controller));
            break;
        case REG_TARGET:
            readFloat(&(motor->target));
            break;
        case REG_VEL_PID_P:
            readFloat(&(motor->PID_velocity.P));
            break;
        case REG_VEL_PID_I:
            readFloat(&(motor->PID_velocity.I));
            break;
        case REG_VEL_PID_D:
            readFloat(&(motor->PID_velocity.D));
            break;
        case REG_VEL_LPF_T:
            readFloat(&(motor->LPF_velocity.Tf));
            break;
        case REG_ANG_PID_P:
            readFloat(&(motor->P_angle.P));
            break;
        case REG_VEL_LIMIT:
            readFloat(&(motor->velocity_limit));
            break;
        case REG_VEL_MAX_RAMP:
            readFloat(&(motor->PID_velocity.output_ramp));
            break;

        case REG_CURQ_PID_P:
            readFloat(&(motor->PID_current_q.P));
            break;
        case REG_CURQ_PID_I:
            readFloat(&(motor->PID_current_q.I));
            break;
        case REG_CURQ_PID_D:
            readFloat(&(motor->PID_current_q.D));
            break;
        case REG_CURQ_LPF_T:
            readFloat(&(motor->LPF_current_q.Tf));
            break;
        case REG_CURD_PID_P:
            readFloat(&(motor->PID_current_d.P));
            break;
        case REG_CURD_PID_I:
            readFloat(&(motor->PID_current_d.I));
            break;
        case REG_CURD_PID_D:
            readFloat(&(motor->PID_current_d.D));
            break;
        case REG_CURD_LPF_T:
            readFloat(&(motor->LPF_current_d.Tf));
            break;

        case REG_VOLTAGE_LIMIT:
            readFloat(&(motor->voltage_limit));
            break;
        case REG_CURRENT_LIMIT:
            readFloat(&(motor->current_limit));
            break;
        case REG_MOTION_DOWNSAMPLE:
            readByte((uint8_t*)&(motor->motion_downsample));
            break;
        case REG_DRIVER_VOLTAGE_LIMIT:
            readFloat(&(((BLDCMotor*)motor)->driver->voltage_limit));
            break;
        case REG_PWM_FREQUENCY:
            readInt((uint32_t*)&(((BLDCMotor*)motor)->driver->pwm_frequency));
            break;

        case REG_ZERO_ELECTRIC_ANGLE:
            readFloat(&(motor->zero_electric_angle));
            break;
        case REG_SENSOR_DIRECTION:
            readByte((uint8_t*)&(motor->sensor_direction));
            break;
        case REG_ZERO_OFFSET:
            readFloat(&(motor->sensor_offset));
            break;
        case REG_PHASE_RESISTANCE:
            readFloat(&(motor->phase_resistance));
            break;
        case REG_KV:
            readFloat(&(motor->KV_rating));
            break;
        case REG_INDUCTANCE:
            readFloat(&(motor->phase_inductance));
            break;
        case REG_POLE_PAIRS:
            readByte((uint8_t*)&(motor->pole_pairs));
            break;
        // unknown register or read-only register (no write) or can't handle in superclass
        case REG_STATUS:
        case REG_ANGLE:
        case REG_POSITION:
        case REG_VELOCITY:
        case REG_SENSOR_ANGLE:
        case REG_VOLTAGE_Q:
        case REG_VOLTAGE_D:
        case REG_CURRENT_Q:
        case REG_CURRENT_D:
        case REG_CURRENT_A:
        case REG_CURRENT_B:
        case REG_CURRENT_C:
        case REG_CURRENT_ABC:
        case REG_SYS_TIME:
        case REG_NUM_MOTORS:
        case REG_MOTOR_ADDRESS:
        case REG_ENABLE_ALL:
        case REG_REPORT:
        default:
            break;
    }
};

