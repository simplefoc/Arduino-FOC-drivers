
#ifndef __SERIAL_BINARY_COMMANDER_H__
#define __SERIAL_BINARY_COMMANDER_H__

#include <Arduino.h>
#include "../telemetry/Telemetry.h"
#include "../binary/BinaryCommander.h"

#define SERIALBINARY_FRAMETYPE_REGISTER 0x03
#define SERIALBINARY_COMMAND_READ 0x00
#define SERIALBINARY_COMMAND_WRITE 0x80


class SerialBinaryCommander : public BinaryCommander, public BinaryTelemetry {
public:
    SerialBinaryCommander(bool echo = false);
    virtual ~SerialBinaryCommander();

    void init(Stream &serial = Serial);
    void runCommander() override;

    bool echo;
protected:
    virtual void startFrame(uint8_t frameSize, uint8_t type = TELEMETRY_FRAMETYPE_DATA) override;
    virtual void endFrame() override;
    uint8_t readBytes(void* valueToSet, uint8_t numBytes) override;
    uint8_t writeBytes(void* valueToSend, uint8_t numBytes) override;

    Stream* _serial;
};




#endif