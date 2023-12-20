#ifndef SIMPLEFOC_I2CCOMMANDER_H
#define SIMPLEFOC_I2CCOMMANDER_H

#include <Arduino.h>
#include <Wire.h>

#include "../SimpleFOCRegisters.h"
#include "../CommanderMaster.h"

#define I2COMMANDER_DEFAULT_MAX_REMOTE_MOTORS 4


typedef struct {
    TwoWire* wire;
    uint8_t address;    
} I2CRemoteMotor;


class I2CCommanderMaster : public CommanderMaster {

    public:
        I2CCommanderMaster(int maxMotors = I2COMMANDER_DEFAULT_MAX_REMOTE_MOTORS);
        ~I2CCommanderMaster();
        void init();
        void addI2CMotors(uint8_t i2cAddress, uint8_t motorCount, TwoWire *wire = &Wire);

        int writeRegister(int motor, SimpleFOCRegister registerNum, void* data, uint8_t size) override;
        int readRegister(int motor, SimpleFOCRegister registerNum, void* data, uint8_t size) override;
        int readLastUsedRegister(int motor, void* data, uint8_t size);

        // Motor intialization interface for convenience - think about how this will best work
        // i.e. which parameters should be set by i2c and which should be hard-coded, and where config info is saved
        // TODO bool initializeMotor(int motor);

    private:
        int maxMotors;
        int numMotors = 0;
        I2CRemoteMotor* motors;

};


#endif