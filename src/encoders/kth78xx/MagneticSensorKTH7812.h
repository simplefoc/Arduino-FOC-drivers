
#pragma once

#include "common/base_classes/Sensor.h"
#include "./KTH7812.h"

class MagneticSensorKTH7812 : public Sensor, public KTH7812 {
public:
	MagneticSensorKTH7812(int nCS = -1, bool withcrc = false, bool fastmode = false, SPISettings settings = KTH7812SPISettings);
	virtual ~MagneticSensorKTH7812();

    virtual float getSensorAngle() override;

	virtual void init(SPIClass* _spi = &SPI);
};


