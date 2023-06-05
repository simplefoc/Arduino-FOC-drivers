#ifndef __MAGNETICSENSORSC60228_H__
#define __MAGNETICSENSORSC60228_H__


#include "common/base_classes/Sensor.h"
#include "./SC60228.h"


class MagneticSensorSC60228 : public Sensor, public SC60228 {
public:
	MagneticSensorSC60228(int nCS = -1, uint32_t cpr=4096, SPISettings settings = SC60228SPISettings);
	virtual ~MagneticSensorSC60228();

    virtual float getSensorAngle() override;

	virtual void init(SPIClass* _spi = &SPI) override;
	uint32_t cpr_;
};


#endif