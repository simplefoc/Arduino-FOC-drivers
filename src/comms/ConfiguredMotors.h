
#ifndef __CONFIGUREDMOTORS_H__
#define __CONFIGUREDMOTORS_H__

#include "common/base_classes/FOCMotor.h"

#ifndef TELEMETRY_MAX_MOTORS
#define TELEMETRY_MAX_MOTORS 4
#endif



class ConfiguredMotors {
public:
    void addMotor(FOCMotor& motor);
protected:
    FOCMotor* motors[TELEMETRY_MAX_MOTORS];
    uint8_t numMotors = 0;
};


#endif
