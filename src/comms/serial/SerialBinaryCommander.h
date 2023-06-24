
#pragma once

#include <Arduino.h>
#include "../RegisterReceiver.h"
#include "../telemetry/Telemetry.h"


#ifndef COMMS_MAX_REPORT_REGISTERS
#define COMMS_MAX_REPORT_REGISTERS 7
#endif


#define SERIALBINARY_FRAMETYPE_REGISTER 0x03
#define SERIALBINARY_COMMAND_READ 0x00
#define SERIALBINARY_COMMAND_WRITE 0x80


class SerialBinaryCommander : public Telemetry, public RegisterReceiver {
public:
    SerialBinaryCommander(bool echo = false);
    virtual ~SerialBinaryCommander();

    void init(Stream &serial = Serial);
    void run();


    bool echo;
protected:
    virtual void startFrame(uint8_t frameSize, uint8_t type = TELEMETRY_FRAMETYPE_DATA);
    virtual void endFrame();
    uint8_t readBytes(void* valueToSet, uint8_t numBytes);
    uint8_t writeBytes(void* valueToSend, uint8_t numBytes);

    uint8_t writeByte(uint8_t value) override;
    uint8_t writeFloat(float value) override;
    uint8_t writeInt(uint32_t value) override;

    uint8_t readByte(uint8_t* valueToSet) override;
    uint8_t readFloat(float* valueToSet) override;
    uint8_t readInt(uint32_t* valueToSet) override;

    void sendTelemetry() override;
    void sendHeader() override;

    uint8_t curMotor = 0;
    SimpleFOCRegister curRegister = REG_STATUS;
    bool commanderror = false;
    bool lastcommanderror = false;
    uint8_t lastcommandregister = REG_STATUS;

    uint8_t numReportRegisters = 0;
    uint8_t reportMotors[COMMS_MAX_REPORT_REGISTERS];
    SimpleFOCRegister reportRegisters[COMMS_MAX_REPORT_REGISTERS];

    Stream* _serial;
};
