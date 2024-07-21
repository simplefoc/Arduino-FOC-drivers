

#pragma once

#include "./Telemetry.h"
#include "../streams/TextIO.h"

class SimpleTelemetry : public Telemetry {
    public:
        SimpleTelemetry();
        virtual ~SimpleTelemetry();

        void init(TextIO& _comms);

    protected:
        void sendTelemetry() override;
        void sendHeader() override;
};