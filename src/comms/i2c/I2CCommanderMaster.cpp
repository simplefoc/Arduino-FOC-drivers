
#include "I2CCommanderMaster.h"


I2CCommanderMaster::I2CCommanderMaster(int maxMotors)  : maxMotors(maxMotors), motors(new I2CRemoteMotor[maxMotors]) {    
};



I2CCommanderMaster::~I2CCommanderMaster(){
};



void I2CCommanderMaster::init(){
};


// TODO handle multiple motors per target
void I2CCommanderMaster::addI2CMotors(uint8_t i2cAddress, uint8_t motorCount, TwoWire *wire){
    for (int i=0;i<motorCount;i++){
        if (numMotors<maxMotors){
            motors[numMotors] = I2CRemoteMotor{ .wire=wire, .address=i2cAddress };
            numMotors++;
        }
    }
};




int I2CCommanderMaster::writeRegister(int motor, SimpleFOCRegister registerNum, void* data, uint8_t size){
    motors[motor].wire->beginTransmission(motors[motor].address);
    motors[motor].wire->write((uint8_t)registerNum);
    motors[motor].wire->write((uint8_t*)data, size);
    motors[motor].wire->endTransmission();
    return size;
};


int I2CCommanderMaster::readRegister(int motor, SimpleFOCRegister registerNum, void* data, uint8_t size){
    motors[motor].wire->beginTransmission(motors[motor].address);
    int numWrite = motors[motor].wire->write((uint8_t)registerNum); // TODO check return value
    motors[motor].wire->endTransmission();
    if (numWrite==1)
        return readLastUsedRegister(motor, data, size);
    return 0;
};


int I2CCommanderMaster::readLastUsedRegister(int motor, void* data, uint8_t size){
    int numRead = motors[motor].wire->requestFrom(motors[motor].address, size);
    if (numRead==size)
        motors[motor].wire->readBytes((uint8_t*)data, size);
    else {
        return 0;
    }
    return numRead;
};