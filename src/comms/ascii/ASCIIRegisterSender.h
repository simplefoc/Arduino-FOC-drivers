
#ifndef __ASCII_REGISTER_SENDER_H__
#define __ASCII_REGISTER_SENDER_H__


#include "Print.h"
#include "../RegisterSender.h"

class ASCIIRegisterSender : public RegisterSender {
public:
    ASCIIRegisterSender(int floatPrecision = 2);
    virtual ~ASCIIRegisterSender();
    void init(Print *print);
    uint8_t writeByte(uint8_t value) override;
    uint8_t writeFloat(float value) override;
    uint8_t writeInt(uint32_t value) override;
    virtual uint8_t writeChar(char value);

protected:
    Print* _print;
    int floatPrecision = 2;
};


#endif