
#include "I2CCommander.h"

I2CCommander::I2CCommander(TwoWire* wire) : _wire(wire) {

};

I2CCommander::~I2CCommander(){};

void I2CCommander::init(uint8_t address) {
    _address = address;
};



void I2CCommander::addMotor(FOCMotor* motor){
    if (numMotors<I2CCOMMANDER_MAX_MOTORS_COMMANDABLE){
        motors[numMotors] = motor;
        numMotors++;
    }
};




I2CCommander_Motor_Status I2CCommander::getMotorStatus(uint8_t motorNum){
    if (motorNum<numMotors){
        if (motors[motorNum]->zero_electric_angle==NOT_SET) // TODO detect open-loop uninitialized state!
            return I2CCommander_Motor_Status::MOT_UNINITIALIZED;
        if (motors[motorNum]->enabled==0)
            return I2CCommander_Motor_Status::MOT_DISABLED;
        if (motors[motorNum]->shaft_velocity >= I2CCOMMANDER_MIN_VELOCITY_FOR_MOTOR_MOVING)
            return I2CCommander_Motor_Status::MOT_MOVING;
        return I2CCommander_Motor_Status::MOT_IDLE;
    }
    return I2CCommander_Motor_Status::MOT_UNKNOWN;
};




bool I2CCommander::readBytes(void* valueToSet, uint8_t numBytes){
    if (_wire->available()>=numBytes){
        byte* bytes = (byte*)valueToSet;
        for (int i=0;i<numBytes;i++)
            *bytes++ = _wire->read();
        return true;
    }
    commanderror = true;
    return false;
};



void I2CCommander::writeFloat(float value){
    _wire->write((byte*)&value, 4);
};




void I2CCommander::onReceive(int numBytes){
    lastcommanderror = commanderror;
    lastcommandregister = curRegister;
    commanderror = false;
    if (numBytes>=1) { // set the current register
        curRegister = static_cast<I2CCommander_Register>(_wire->read());
    }
    if (numBytes>1) { // read from i2c and update value represented by register as well...
        if (!receiveRegister(curMotor, curRegister, numBytes))
            commanderror = true;
    }
    if (numBytes<1) 
        commanderror = true;
};



void I2CCommander::onRequest(){
    commanderror = false;
    if (!sendRegister(curMotor, curRegister))
        commanderror = true;
};





bool I2CCommander::receiveRegister(uint8_t motorNum, uint8_t registerNum, int numBytes) {
    int val;
    float floatval;
    switch (registerNum) {
        case REG_MOTOR_ADDRESS:
            val = _wire->read(); // reading one more byte is definately ok, since numBytes>1
            if (val>=0 && val<numMotors)
                curMotor = val;
            else
                commanderror = true;
            break;
        case REG_REPORT:
            if (numBytes>=3 && (numBytes&0x01)==1) { // numBytes must be odd, since we have register and n pairs of motor/register numbers
                val = (numBytes-1)/2;
                if (val>I2CCOMMANDER_MAX_REPORT_REGISTERS)
                    val = I2CCOMMANDER_MAX_REPORT_REGISTERS;
                for (int i=0;i<val;i++){
                    reportMotors[i] = _wire->read();
                    reportRegisters[i] = _wire->read();
                }
            }
            else
                commanderror = true;
            break;
        case REG_ENABLE_ALL:
            val = _wire->read();
            for (int i=0; i<numMotors; i++)
                (val>0)?motors[i]->enable():motors[i]->disable();
            break;
        case REG_ENABLE:
            val = _wire->read();
            (val>0)?motors[motorNum]->enable():motors[motorNum]->disable();
            break;
        case REG_CONTROL_MODE:
            val = _wire->read();
            if (val>=0 && val<=4) // TODO these enums don't have assigned constants
                motors[motorNum]->controller = static_cast<MotionControlType>(val);
            else
                commanderror = true;
            break;
        case REG_TORQUE_MODE:
            val = _wire->read();
            if (val>=0 && val<=2)
                motors[motorNum]->torque_controller = static_cast<TorqueControlType>(val);
            else
                commanderror = true;
            break;
        case REG_MODULATION_MODE:
            val = _wire->read();
            if (val>=0 && val<=3)
                motors[motorNum]->foc_modulation = static_cast<FOCModulationType>(val);
            else
                commanderror = true;
            break;
        case REG_TARGET:
            readBytes(&(motors[motorNum]->target), 4);
            break;
        case REG_VEL_PID_P:
            readBytes(&(motors[motorNum]->PID_velocity.P), 4);
            break;
        case REG_VEL_PID_I:
            readBytes(&(motors[motorNum]->PID_velocity.I), 4);
            break;
        case REG_VEL_PID_D:
            readBytes(&(motors[motorNum]->PID_velocity.D), 4);
            break;
        case REG_VEL_LPF_T:
            readBytes(&(motors[motorNum]->LPF_velocity.Tf), 4);
            break;
        case REG_ANG_PID_P:
            readBytes(&(motors[motorNum]->P_angle.P), 4);
            break;
        case REG_VEL_LIMIT:
            readBytes(&(motors[motorNum]->velocity_limit), 4);
            break;
        case REG_VEL_MAX_RAMP:
            readBytes(&(motors[motorNum]->PID_velocity.output_ramp), 4);
            break;
        case REG_CURQ_PID_P:
            readBytes(&(motors[motorNum]->PID_current_q.P), 4);
            break;
        case REG_CURQ_PID_I:
            readBytes(&(motors[motorNum]->PID_current_q.I), 4);
            break;
        case REG_CURQ_PID_D:
            readBytes(&(motors[motorNum]->PID_current_q.D), 4);
            break;
        case REG_CURQ_LPF_T:
            readBytes(&(motors[motorNum]->LPF_current_q.Tf), 4);
            break;
        case REG_CURD_PID_P:
            readBytes(&(motors[motorNum]->PID_current_d.P), 4);
            break;
        case REG_CURD_PID_I:
            readBytes(&(motors[motorNum]->PID_current_d.I), 4);
            break;
        case REG_CURD_PID_D:
            readBytes(&(motors[motorNum]->PID_current_d.D), 4);
            break;
        case REG_CURD_LPF_T:
            readBytes(&(motors[motorNum]->LPF_current_d.Tf), 4);
            break;
        case REG_MAX_VOLTAGE:
            readBytes(&(motors[motorNum]->voltage_limit), 4);
            break;
        case REG_MAX_CURRENT:
            readBytes(&(motors[motorNum]->current_limit), 4);
            break;
        case REG_MOTION_DOWNSAMPLE:
            readBytes(&(motors[motorNum]->motion_downsample), 4);
            break;
        case REG_ZERO_OFFSET:
            readBytes(&(motors[motorNum]->sensor_offset), 4);
            break;
        // RO registers
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
        case REG_ZERO_ELECTRIC_ANGLE:
        case REG_SENSOR_DIRECTION:
        case REG_POLE_PAIRS:
        case REG_PHASE_RESISTANCE:
        case REG_NUM_MOTORS:
        case REG_SYS_TIME:
        default: // unknown register
            return false;   
    }
    return true;
}






bool I2CCommander::sendRegister(uint8_t motorNum, uint8_t registerNum) {
        // read the current register
    switch(registerNum) {
        case REG_STATUS:
            _wire->write(curMotor);
            _wire->write((uint8_t)lastcommandregister);
            _wire->write((uint8_t)lastcommanderror+1);
            for (int i=0;(i<numMotors && i<28); i++) { // at most 28 motors, so we can fit in one packet
                uint8_t status = (uint8_t)getMotorStatus(i);
                _wire->write(status);
            }
            break;
        case REG_MOTOR_ADDRESS:
            _wire->write(curMotor);
            break;
        case REG_REPORT:
            for (int i=0;i<numReportRegisters;i++)
                if (reportRegisters[i]!=REG_REPORT) // prevent recursion
                    sendRegister(reportMotors[i], reportRegisters[i]);
            break;        
        case REG_ENABLE:
            _wire->write(motors[motorNum]->enabled);
            break;
        case REG_MODULATION_MODE:
            _wire->write(motors[motorNum]->foc_modulation);
            break;
        case REG_TORQUE_MODE:
            _wire->write(motors[motorNum]->torque_controller);
            break;
        case REG_CONTROL_MODE:
            _wire->write(motors[motorNum]->controller);
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
            _wire->write((uint32_t)motors[motorNum]->motion_downsample);
            break;            

        case REG_ZERO_ELECTRIC_ANGLE:
            writeFloat(motors[motorNum]->zero_electric_angle);
            break;
        case REG_SENSOR_DIRECTION:
            _wire->write((int8_t)motors[motorNum]->sensor_direction);
            break;
        case REG_ZERO_OFFSET:
            writeFloat(motors[motorNum]->sensor_offset);
            break;
        case REG_PHASE_RESISTANCE:
            writeFloat(motors[motorNum]->phase_resistance);
            break;
        case REG_POLE_PAIRS:
            _wire->write((uint32_t)motors[motorNum]->pole_pairs);
            break;

        case REG_SYS_TIME:
            _wire->write(millis()); 
            break;
        case REG_NUM_MOTORS:   
            _wire->write(numMotors);
            break;

        // unknown register or write only register (no read)
        case REG_ENABLE_ALL:        
        default:
            _wire->write(0); // TODO what to send in this case?
            return false;
    }
    return true;
}
