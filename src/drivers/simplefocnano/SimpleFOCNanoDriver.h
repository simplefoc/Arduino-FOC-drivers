
#pragma once


#include <drivers/BLDCDriver3PWM.h>

#if defined(ARDUINO_NANO_ESP32) || defined(ARDUINO_NANO_RP2040_CONNECT) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_ARDUINO_NANO33BLE) || defined(ARDUINO_AVR_NANO)

/*
 * Default pins for the SimpleFOC Nano board
 *
 * These are the correct pins if you plug the Nano board into the SimpleFOC shield.
 * If you use jumper wires to connect, you can make your own pin choices. In this case, don't use this driver,
 * and rather use the BLDCDriver3PWM class directly.
 * 
 */


#define PIN_INU     3
#define PIN_INV     6
#define PIN_INW     9
#define PIN_ENU     4
#define PIN_ENV     7
#define PIN_ENW     8
#ifdef ARDUINO_NANO_RP2040_CONNECT
#define PIN_nSLEEP  17
#define PIN_nFAULT  20
#define PIN_nRST    21
#else
#define PIN_nSLEEP  A3
#define PIN_nFAULT  A6
#define PIN_nRST    A7
#endif
#define PIN_VBUS    A0
#define PIN_nCS     10

#define VBUS_CONV_FACTOR (22.0f/2.2f)




class SimpleFOCNanoDriver : public BLDCDriver3PWM {
public:
    SimpleFOCNanoDriver();
    ~SimpleFOCNanoDriver();

    int init() override;

    void sleep(bool sleep=true);
    void wake();
    bool isSleeping();
    
    bool isFault();
    void clearFault();
    void attachFaultInterrupt(void (*callback)());

    float getBusVoltage(float vdd_voltage, int adc_resolution);
};

#endif