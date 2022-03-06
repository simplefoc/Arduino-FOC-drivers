
#ifndef __BINARY_COMMS_DRIVER_H__
#define __BINARY_COMMS_DRIVER_H__

/*
 
 Abstracts the binary communication protocol, seperating the part that
 interacts with SimpleFOC from the comms SPI, so individual binary comms drivers
 don't have to all implement the register setting/getting functions.
 
*/

#include "../RegisterBasedCommander.h"



class BinaryCommander : public RegisterBasedCommander {
protected:
    virtual bool sendRegister(SimpleFOCRegister reg, uint8_t motorNum = 0) override;
    virtual void receiveRegister(SimpleFOCRegister reg, uint8_t motorNum = 0) override;
    virtual void setRegister(SimpleFOCRegister reg, void* value, uint8_t motorNum = 0) override;

    virtual uint8_t writeBytes(void* valueToWrite, uint8_t numBytes) = 0;
    virtual uint8_t writeByte(uint8_t value);
    virtual uint8_t writeFloat(float value);
    virtual uint8_t writeInt(uint32_t value);
    virtual uint8_t readBytes(void* valueToSet, uint8_t numBytes) = 0;
    virtual uint8_t readByte(void* valueToSet);
    virtual uint8_t readFloat(void* valueToSet);
    virtual uint8_t readInt(void* valueToSet);

};



#endif

