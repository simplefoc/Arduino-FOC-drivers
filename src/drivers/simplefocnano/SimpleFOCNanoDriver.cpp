
#include "./SimpleFOCNanoDriver.h"

#if defined(ARDUINO_NANO_ESP32) || defined(ARDUINO_NANO_RP2040_CONNECT) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_ARDUINO_NANO33BLE) || defined(ARDUINO_AVR_NANO)

SimpleFOCNanoDriver::SimpleFOCNanoDriver() : BLDCDriver3PWM(PIN_INU, PIN_INV, PIN_INW, PIN_ENU, PIN_ENV, PIN_ENW) {
    // nothing to do here
};


SimpleFOCNanoDriver::~SimpleFOCNanoDriver() {
    // nothing to do here
};

int SimpleFOCNanoDriver::init() {
    pinMode(PIN_nSLEEP, OUTPUT);
    pinMode(PIN_nRST, OUTPUT);
    pinMode(PIN_nFAULT, INPUT_PULLUP);
    pinMode(PIN_VBUS, INPUT);
    this->sleep(false);
    digitalWrite(PIN_nRST, HIGH);
    return BLDCDriver3PWM::init();
};

void SimpleFOCNanoDriver::sleep(bool sleep) {
    digitalWrite(PIN_nSLEEP, !sleep);
};


void SimpleFOCNanoDriver::wake() {
    this->sleep(false);
};


bool SimpleFOCNanoDriver::isSleeping() {
    return digitalRead(PIN_nSLEEP)==LOW;
};



bool SimpleFOCNanoDriver::isFault() {
    return !digitalRead(PIN_nFAULT);
};


void SimpleFOCNanoDriver::clearFault() {
    if (!this->isFault()) return;
    disable();
    digitalWrite(PIN_nRST, LOW);
    delayMicroseconds(100);
    digitalWrite(PIN_nRST, HIGH);
    enable();
};


void SimpleFOCNanoDriver::attachFaultInterrupt(void (*callback)()) {
    attachInterrupt(digitalPinToInterrupt(PIN_nFAULT), callback, FALLING);
};



float SimpleFOCNanoDriver::getBusVoltage(float vdd_voltage, int adc_resolution) {
    float sum = 0.0;
    for(int i = 0; i < 500; i++) {
        sum += analogRead(PIN_VBUS);
    }

    return sum / 500.0 * VBUS_CONV_FACTOR * (vdd_voltage / adc_resolution);
};

#endif
