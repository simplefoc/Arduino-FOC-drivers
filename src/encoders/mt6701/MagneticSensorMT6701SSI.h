#ifndef __MAGNETIC_SENSOR_MT6701_SSI_H__
#define __MAGNETIC_SENSOR_MT6701_SSI_H__

#include "Arduino.h"
#include "SPI.h"
#include "common/base_classes/Sensor.h"


#define MT6701_CPR 16384.0f

#define MT6701_BITORDER MSBFIRST

#define MT6701_DATA_POS 1

// Use SPI mode 2, capture on falling edge. First bit is not valid data, so have to read 25 bits to get a full SSI frame.
// SSI frame is 1 bit ignore, 14 bits angle, 4 bit status and 6 bit CRC.
static SPISettings MT6701SSISettings(1000000, MT6701_BITORDER, SPI_MODE2); // @suppress("Invalid arguments")



class MagneticSensorMT6701SSI : public Sensor {
public:
    MagneticSensorMT6701SSI(int nCS = -1, SPISettings settings = MT6701SSISettings);
    virtual ~MagneticSensorMT6701SSI();

    virtual void init(SPIClass* _spi = &SPI);

    float getSensorAngle() override; // angle in radians, return current value

protected:
    uint16_t readRawAngleSSI();

    SPISettings settings;
    SPIClass* spi;
	int nCS = -1;
};


#endif