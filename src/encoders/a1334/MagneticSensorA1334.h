
#ifndef __MAGNETIC_SENSOR_A1334_H__
#define __MAGNETIC_SENSOR_A1334_H__

#include "common/base_classes/Sensor.h"
#include "./A1334.h"



class MagneticSensorA1334 : public Sensor, public A1334 {
public:
	MagneticSensorA1334(int nCS = -1, SPISettings settings = A1334SPISettings);
	virtual ~MagneticSensorA1334();

    virtual float getSensorAngle() override;

	virtual void init(SPIClass* _spi = &SPI);
private:
};




#endif