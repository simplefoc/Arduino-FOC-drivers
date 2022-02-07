
#ifndef __MAGNETIC_SENSOR_AS5145_H__
#define __MAGNETIC_SENSOR_AS5145_H__

#include "Arduino.h"
#include "SPI.h"
#include "common/base_classes/Sensor.h"

#ifndef MSBFIRST
#define MSBFIRST BitOrder::MSBFIRST
#endif

#define AS5145_BITORDER MSBFIRST
#define AS5145_CPR 4096.0f
#define _2PI 6.28318530718f


static SPISettings AS5145SSISettings(1000000, AS5145_BITORDER, SPI_MODE2); // @suppress("Invalid arguments")


class MagneticSensorAS5145 : public Sensor {
public:
	MagneticSensorAS5145(SPISettings settings = AS5145SSISettings);
	virtual ~MagneticSensorAS5145();

    virtual float getSensorAngle() override;

	virtual void init(SPIClass* _spi = &SPI);

    uint16_t readRawAngleSSI();
	
private:
	SPIClass* spi;
	SPISettings settings;
};



#endif