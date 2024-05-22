# SimpleFOC Driver for ESP32 hardware encoder

This encoder driver uses the ESP32´s dedicated pulse counter hardware to efficiently count the AB(I) signals of an encoder. It also supports a Step/Dir-type mode.

Because most of the counting is done by the peripheral, it should support much higher speeds in comparison to the generic interrupt-based encoder implementation provided in the main library. 
The PCNT peripheral can count at several MHz and should not be a limiting factor here.

You can use encoders with cpr of up to 31 bits. (At this resolution, you would get about 100 counts per second if you mounted such a sensor on the earths rotational axis. Thats plenty ;-) ) 


## Status

Seems to work fine! Step/Dir mode is untested.

## Hardware Setup

You can connect the encoder to any digital input pin of the ESP32, as they all support the PCNT peripheral.

## Configuration

This is a near drop-in replacement for the standard encoder class:

```c++
#include "Arduino.h"
#include "SimpleFOC.h"
#include "SimpleFOCDrivers.h"
#include "encoders/esp32hwencoder/ESP32HWEncoder.h"

#define ENCODER_PPR 4711
#define ENCODER_PIN_A 16
#define ENCODER_PIN_B 17
#define ENCODER_PIN_I 21

ESP32HWEncoder encoder = ESP32HWEncoder(ENCODER_PIN_A, ENCODER_PIN_B, ENCODER_PPR, ENCODER_PIN_I); // The Index pin can be omitted

void setup() {
    encoder.pullup = Pullup::USE_INTERN; // optional: pullups

    encoder.setStepDirMode(); // optional: set Stepper type step/dir mode

    encoder.init();
}

void loop() {
    encoder.update(); // optional: Update manually if not using loopfoc()

    encoder.getAngle() // Access the sensor value
}
```