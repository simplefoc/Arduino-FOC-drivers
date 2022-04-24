#ifndef __TELEMETRY_H__
#define __TELEMETRY_H__


#include "../SimpleFOCRegisters.h"
#include "../ascii/ASCIIRegisterSender.h"
#include "../binary/BinaryRegisterSender.h"

#ifndef TELEMETRY_MAX_REGISTERS
#define TELEMETRY_MAX_REGISTERS 8
#endif


enum : uint8_t {
    TELEMETRY_FRAMETYPE_DATA = 0x01,
    TELEMETRY_FRAMETYPE_HEADER = 0x02
} TelemetryFrameType;




class Telemetry {
public:
    Telemetry();
    virtual ~Telemetry();
    virtual void init();
    void setTelemetryRegisters(uint8_t numRegisters, uint8_t* registers, uint8_t* motors = NULL);
    virtual void sendTelemetry() = 0;
protected:
    virtual void sendHeader() = 0;

    uint8_t numRegisters;
    uint8_t registers[TELEMETRY_MAX_REGISTERS];
    uint8_t registers_motor[TELEMETRY_MAX_REGISTERS];
    uint8_t frameSize;
};



class BinaryTelemetry : public Telemetry {
public:
    BinaryTelemetry(RegisterSender* sender);
    virtual ~BinaryTelemetry();
    void sendTelemetry() override;
    void sendHeader() override;
    virtual void startFrame(uint8_t frameSize, uint8_t type = TELEMETRY_FRAMETYPE_DATA) = 0;
    virtual void endFrame() = 0;
protected:
    RegisterSender* sender;
};






class ASCIITelemetry : public Telemetry {
public:
    ASCIITelemetry(ASCIIRegisterSender* sender);
    virtual ~ASCIITelemetry();
    void sendTelemetry() override;
    void sendHeader() override;
protected:
    ASCIIRegisterSender* sender;
};






#endif