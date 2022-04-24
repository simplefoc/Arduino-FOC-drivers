
#ifndef __SERIALASCIITELEMETRY_H__
#define __SERIALASCIITELEMETRY_H__

#include "Arduino.h"
#include "../telemetry/Telemetry.h"
#include "../ascii/ASCIIRegisterSender.h"

class SerialASCIITelemetry : public ASCIIRegisterSender, public ASCIITelemetry {
public:
    SerialASCIITelemetry(int floatPrecision = 2);
    virtual ~SerialASCIITelemetry();

    void init(Print* print = &Serial);
};





#endif
