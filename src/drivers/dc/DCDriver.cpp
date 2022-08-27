
#include "./DCDriver.h"


void DCDriver::enable() {
    if (pinEN!=NOT_SET)
        digitalWrite(pinEN, enable_active_high ? HIGH : LOW);
};


void DCDriver::disable() {
    setPwm(0);
    if (pinEN!=NOT_SET)
        digitalWrite(pinEN, enable_active_high ? LOW : HIGH);
};