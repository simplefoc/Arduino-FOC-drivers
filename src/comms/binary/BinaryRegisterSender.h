
#ifndef __BINARY_REGISTER_SENDER_H__
#define __BINARY_REGISTER_SENDER_H__


#include "../RegisterSender.h"



class BinaryRegisterSender : public RegisterSender {
    virtual uint8_t writeBytes(void* valueToWrite, uint8_t numBytes) = 0;
    uint8_t writeByte(uint8_t value) override;
    uint8_t writeFloat(float value) override;
    uint8_t writeInt(uint32_t value) override;
};


#endif
