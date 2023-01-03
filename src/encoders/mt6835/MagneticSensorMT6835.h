
#pragma once

#include "common/base_classes/Sensor.h"
#include "./MT6835.h"

class MagneticSensorMT6835 : public Sensor, public MT6835 {
public:
	MagneticSensorMT6835(int nCS = -1, SPISettings settings = MT6835SPISettings);
	virtual ~MagneticSensorMT6835();

    virtual float getSensorAngle() override;

	virtual void init(SPIClass* _spi = &SPI);
};


