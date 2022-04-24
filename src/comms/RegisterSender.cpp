
#include "./RegisterSender.h"


void RegisterSender::addMotor(FOCMotor& motor) {
    if (numMotors < TELEMETRY_MAX_MOTORS) {
        motors[numMotors] = &motor;
        numMotors++;
    }
};



bool RegisterSender::sendRegister(SimpleFOCRegister reg, uint8_t motorNum){
       // read the current register
    switch(reg) {
        // case REG_STATUS:
        //     writeByte(curMotor);
        //     writeByte((uint8_t)lastcommandregister);
        //     writeByte((uint8_t)lastcommanderror+1);
        //     for (int i=0;(i<numMotors && i<28); i++) { // at most 28 motors, so we can fit in one packet
        //         writeByte(motors[motorNum]->motor_status);
        //     }
        //     break;
        // case REG_MOTOR_ADDRESS:
        //     writeByte(curMotor);
        //     break;
        // case REG_REPORT:
        //     for (int i=0;i<numReportRegisters;i++)
        //         if (reportRegisters[i]!=REG_REPORT) // prevent recursion
        //             sendRegister(reportRegisters[i], reportMotors[i]);
        //     break;        
        case REG_ENABLE:
            writeByte(motors[motorNum]->enabled);
            break;
        case REG_MODULATION_MODE:
            writeByte(motors[motorNum]->foc_modulation);
            break;
        case REG_TORQUE_MODE:
            writeByte(motors[motorNum]->torque_controller);
            break;
        case REG_CONTROL_MODE:
            writeByte(motors[motorNum]->controller);
            break;

        case REG_TARGET:
            writeFloat(motors[motorNum]->target);
            break;
        case REG_ANGLE:
            writeFloat(motors[motorNum]->shaft_angle);
            break;
        case REG_VELOCITY:
            writeFloat(motors[motorNum]->shaft_velocity);
            break;
        case REG_SENSOR_ANGLE:
            if (motors[motorNum]->sensor)
                writeFloat(motors[motorNum]->sensor->getAngle()); // stored angle
            else
                writeFloat(motors[motorNum]->shaft_angle);
            break;

        case REG_VOLTAGE_Q:
            writeFloat(motors[motorNum]->voltage.q);
            break;
        case REG_VOLTAGE_D:
            writeFloat(motors[motorNum]->voltage.d);
            break;
        case REG_CURRENT_Q:
            writeFloat(motors[motorNum]->current.q);
            break;
        case REG_CURRENT_D:
            writeFloat(motors[motorNum]->current.d);
            break;
        case REG_CURRENT_A:
            if (motors[motorNum]->current_sense) // TODO check if current sense can be called from inside this callback function
                writeFloat(motors[motorNum]->current_sense->getPhaseCurrents().a);
            else
                writeFloat(0.0f);
            break;
        case REG_CURRENT_B:
            if (motors[motorNum]->current_sense)
                writeFloat(motors[motorNum]->current_sense->getPhaseCurrents().b);
            else
                writeFloat(0.0f);
            break;
        case REG_CURRENT_C:
            if (motors[motorNum]->current_sense)
                writeFloat(motors[motorNum]->current_sense->getPhaseCurrents().c);
            else
                writeFloat(0.0f);
            break;
        case REG_CURRENT_ABC:
            if (motors[motorNum]->current_sense) {
                PhaseCurrent_s currents = motors[motorNum]->current_sense->getPhaseCurrents();
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
            writeFloat(motors[motorNum]->PID_velocity.P);
            break;
        case REG_VEL_PID_I:
            writeFloat(motors[motorNum]->PID_velocity.I);
            break;
        case REG_VEL_PID_D:
            writeFloat(motors[motorNum]->PID_velocity.D);
            break;
        case REG_VEL_LPF_T:
            writeFloat(motors[motorNum]->LPF_velocity.Tf);
            break;
        case REG_ANG_PID_P:
            writeFloat(motors[motorNum]->P_angle.P);
            break;
        case REG_VEL_LIMIT:
            writeFloat(motors[motorNum]->velocity_limit);
            break;
        case REG_VEL_MAX_RAMP:
            writeFloat(motors[motorNum]->PID_velocity.output_ramp);
            break;

        case REG_CURQ_PID_P:
            writeFloat(motors[motorNum]->PID_current_q.P);
            break;
        case REG_CURQ_PID_I:
            writeFloat(motors[motorNum]->PID_current_q.I);
            break;
        case REG_CURQ_PID_D:
            writeFloat(motors[motorNum]->PID_current_q.D);
            break;
        case REG_CURQ_LPF_T:
            writeFloat(motors[motorNum]->LPF_current_q.Tf);
            break;
        case REG_CURD_PID_P:
            writeFloat(motors[motorNum]->PID_current_d.P);
            break;
        case REG_CURD_PID_I:
            writeFloat(motors[motorNum]->PID_current_d.I);
            break;
        case REG_CURD_PID_D:
            writeFloat(motors[motorNum]->PID_current_d.D);
            break;
        case REG_CURD_LPF_T:
            writeFloat(motors[motorNum]->LPF_current_d.Tf);
            break;

        case REG_MAX_VOLTAGE:
            writeFloat(motors[motorNum]->voltage_limit);
            break;
        case REG_MAX_CURRENT:
            writeFloat(motors[motorNum]->current_limit);
            break;
        case REG_MOTION_DOWNSAMPLE:
            writeByte((uint8_t)motors[motorNum]->motion_downsample); // TODO int will have different sizes on different platforms
                                                                    //      but using uint32 doesn't compile clean on all, e.g. RP2040
            break;            

        case REG_ZERO_ELECTRIC_ANGLE:
            writeFloat(motors[motorNum]->zero_electric_angle);
            break;
        case REG_SENSOR_DIRECTION:
            writeByte((int8_t)motors[motorNum]->sensor_direction);
            break;
        case REG_ZERO_OFFSET:
            writeFloat(motors[motorNum]->sensor_offset);
            break;
        case REG_PHASE_RESISTANCE:
            writeFloat(motors[motorNum]->phase_resistance);
            break;
        case REG_POLE_PAIRS:
            writeByte((uint8_t)motors[motorNum]->pole_pairs);
            break;

        case REG_SYS_TIME:
            // TODO how big is millis()? Same on all platforms?
            writeInt((int)millis());
            break;
        case REG_NUM_MOTORS:   
            writeByte(numMotors);
            break;

        // unknown register or write only register (no read)
        case REG_ENABLE_ALL:        
        default:
            writeByte(0); // TODO what to send in this case?
            return false;
    }
    return true;
};

