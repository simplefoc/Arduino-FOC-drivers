
#pragma once

#include "common/base_classes/Sensor.h"
#include "./MT6826S.h"

class MagneticSensorMT6826S : public Sensor, public MT6826S {
public:
	MagneticSensorMT6826S(int nCS = -1, SPISettings settings = MT6826SSPISettings);
	virtual ~MagneticSensorMT6826S();

    virtual float getSensorAngle() override;

	virtual void init(SPIClass* _spi = &SPI);
};


