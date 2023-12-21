

#pragma once

#include "./Telemetry.h"
#include "../streams/TextIO.h"

class TeleplotTelemetry : public Telemetry {
    public:
        TeleplotTelemetry();
        virtual ~TeleplotTelemetry();

        void init(TextIO& _comms);

    protected:
        void sendTelemetry() override;
        void sendHeader() override;
};