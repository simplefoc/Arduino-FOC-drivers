
#ifndef __BINARY_COMMS_DRIVER_H__
#define __BINARY_COMMS_DRIVER_H__

/*
 
 Abstracts the binary communication protocol, seperating the part that
 interacts with SimpleFOC from the comms SPI, so individual binary comms drivers
 don't have to all implement the register setting/getting functions.
 
*/

#include "../RegisterBasedCommander.h"
#include "../binary/BinaryRegisterSender.h"



class BinaryCommander : public BinaryRegisterSender, public RegisterBasedCommander {
public:
    BinaryCommander();
    virtual ~BinaryCommander();
protected:
    virtual uint8_t readBytes(void* valueToSet, uint8_t numBytes) = 0;
    uint8_t readByte(void* valueToSet) override;
    uint8_t readFloat(void* valueToSet) override;
    uint8_t readInt(void* valueToSet) override;
};



#endif

