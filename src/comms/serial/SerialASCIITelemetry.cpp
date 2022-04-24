
#include "./SerialASCIITelemetry.h"

SerialASCIITelemetry::SerialASCIITelemetry(int floatPrecision) : ASCIIRegisterSender(floatPrecision), ASCIITelemetry(this) {
    // nix
};

SerialASCIITelemetry::~SerialASCIITelemetry(){

};

void SerialASCIITelemetry::init(Print* print){
    this->ASCIIRegisterSender::init(print);
    this->Telemetry::init();
};