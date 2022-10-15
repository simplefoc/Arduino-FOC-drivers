#ifndef __MAGNETIC_SENSOR_MA330_H__
#define __MAGNETIC_SENSOR_MA330_H__


#include "common/base_classes/Sensor.h"
#include "./MA330.h"

class MagneticSensorMA330 : public Sensor, public MA330 {
public:
	MagneticSensorMA330(int nCS = -1, SPISettings settings = MA330SPISettings);
	virtual ~MagneticSensorMA330();

    virtual float getSensorAngle() override;

	virtual void init(SPIClass* _spi = &SPI);
};


#endif