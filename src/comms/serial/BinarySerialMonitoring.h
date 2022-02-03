
#ifndef __BINARY_SERIAL_MONITORING_H__
#define __BINARY_SERIAL_MONITORING_H__

#include "Arduino.h"
#include "common/base_classes/FOCMotor.h"
#include "../CommanderRegisters.h"

class BinarySerialMonitoring : public Monitoring {
public:
    BinarySerialMonitoring();
    ~BinarySerialMonitoring();

    void init(Stream& serial);
    void addMotor(FOCMotor& motor);
    void setMonitoringRegisters(uint8_t numRegisters, uint8_t* registers, uint8_t* motors = NULL);

    void run() override;
    void startFrame() override;
    void endFrame() override;
    void writeFrame(uint8_t* data, uint8_t size) override;
    void writeFrameFloat(float data) override;
    void writeFrameInt(int data) override;

};


#endif
