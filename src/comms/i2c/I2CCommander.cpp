
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





uint8_t I2CCommander::readBytes(void* valueToSet, uint8_t numBytes){
    if (_wire->available()>=numBytes){
        byte* bytes = (byte*)valueToSet;
        for (int i=0;i<numBytes;i++)
            *bytes++ = _wire->read();
        return numBytes;
    }
    commanderror = true;
    return 0;
};


RegisterIO& I2CCommander::operator>>(uint8_t& valueToSet) {
    readBytes(&valueToSet, 1);
    return *this;
};
RegisterIO& I2CCommander::operator>>(float& valueToSet) {
    readBytes(&valueToSet, 4);
    return *this;
};
RegisterIO& I2CCommander::operator>>(uint32_t& valueToSet) {
    readBytes(&valueToSet, 4);
    return *this;
};

RegisterIO& I2CCommander::operator<<(uint8_t value) {
    _wire->write(value);
    return *this;
};
RegisterIO& I2CCommander::operator<<(float value) {
    _wire->write((byte*)&value, 4);
    return *this;
};
RegisterIO& I2CCommander::operator<<(uint32_t value) {
    _wire->write((byte*)&value, 4);
    return *this;
};        


void I2CCommander::onReceive(int numBytes){
    lastcommanderror = commanderror;
    lastcommandregister = curRegister;
    commanderror = false;
    if (numBytes>=1) { // set the current register
        curRegister = static_cast<SimpleFOCRegister>(_wire->read());
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




/*
Reads values from I2C bus and updates the motor's values.

Currently this isn't really thread-safe, but works ok in practice on 32-bit MCUs.

Do not use on 8-bit architectures where the 32 bit writes may not be atomic!

Plan to make this safe: the writes should be buffered, and not actually executed 
until in the main loop by calling commander->run();
the run() method disables interrupts while the updates happen.
*/
bool I2CCommander::receiveRegister(uint8_t motorNum, uint8_t registerNum, int numBytes) {
    int val;
    switch (registerNum) {
        case REG_MOTOR_ADDRESS:
            val = _wire->read(); // reading one more byte is definately ok, since numBytes>1
            if (val>=0 && val<numMotors)
                curMotor = val;
            else
                commanderror = true;
            break;
        case I2CCOMMANDER_REG_REPORT:
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
        default: // unknown register
            return SimpleFOCRegisters::regs->commsToRegister(*this, registerNum, motors[motorNum]);
    }
    return true;
}





/*
    Reads values from motor/sensor and writes them to I2C bus. Intended to be run
    from the Wire.onRequest interrupt.

    Assumes atomic 32 bit reads. On 8-bit arduino this assumption does not hold and this
    code is not safe on those platforms. You might read "half-written" floats.
    
    A solution might be to maintain a complete set of shadow registers  in the commander
    class, and update them in the run() method (which runs with interrupts off). Not sure
    of the performance impact of all those 32 bit read/writes though. In any case, since
    I use only 32 bit MCUs I'll leave it as an excercise to the one who needs it. ;-)

    On 32 bit platforms the implication is that reads will occur atomically, so data will
    be intact, but they can occur at any time during motor updates, so different values might
    not be in a fully consistent state (i.e. phase A current might be from the current iteration
    but phase B current from the previous iteration).
*/
bool I2CCommander::sendRegister(uint8_t motorNum, uint8_t registerNum) {
        // read the current register
    switch(registerNum) {
        case REG_STATUS:
            _wire->write(curMotor);
            _wire->write((uint8_t)lastcommandregister);
            _wire->write((uint8_t)lastcommanderror+1);
            for (int i=0;(i<numMotors && i<28); i++) { // at most 28 motors, so we can fit in one packet
                _wire->write(motors[i]->motor_status);
            }
            break;
        case REG_MOTOR_ADDRESS:
            _wire->write(curMotor);
            break;
        case I2CCOMMANDER_REG_REPORT:
            for (int i=0;i<numReportRegisters;i++)
                SimpleFOCRegisters::regs->registerToComms(*this, reportRegisters[i], motors[reportMotors[i]]); // send any normal register
            break;
        case REG_NUM_MOTORS:
            _wire->write(numMotors);
            break;
        // write only register
        case REG_ENABLE_ALL:        
            return false;
        // unknown register - not handled here
        default:
            return SimpleFOCRegisters::regs->registerToComms(*this, registerNum, motors[motorNum]);
    }
    return true;
}
