
#pragma once

#include "Arduino.h"
#include "../telemetry/Telemetry.h"

class SerialASCIITelemetry : public Telemetry {
public:
    SerialASCIITelemetry(int floatPrecision = 2);
    virtual ~SerialASCIITelemetry();

    void init(Print* print = &Serial);

protected:
    uint8_t writeByte(uint8_t value) override;
    uint8_t writeFloat(float value) override;
    uint8_t writeInt(uint32_t value) override;
    uint8_t writeChar(char value);

    void sendTelemetry() override;
    void sendHeader() override;    

    Print* _print;
    int floatPrecision = 2;
};


