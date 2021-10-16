#ifndef __MAGNETIC_SENSOR_MA730_H__
#define __MAGNETIC_SENSOR_MA730_H__


#include "common/base_classes/Sensor.h"
#include "./MA730.h"

class MagneticSensorMA730 : public Sensor, public MA730 {
public:
	MagneticSensorMA730(int nCS = -1, SPISettings settings = MA730SPISettings);
	virtual ~MagneticSensorMA730();

    virtual float getSensorAngle() override;

	virtual void init(SPIClass* _spi = &SPI);
};


#endif