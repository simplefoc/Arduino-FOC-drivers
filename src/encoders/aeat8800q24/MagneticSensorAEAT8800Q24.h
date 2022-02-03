#ifndef __MAGNETICSENSORAEAT8800Q24_H__
#define __MAGNETICSENSORAEAT8800Q24_H__


#include "common/base_classes/Sensor.h"
#include "./AEAT8800Q24.h"

class MagneticSensorAEAT8800Q24 : public Sensor, public AEAT8800Q24 {
public:
	MagneticSensorAEAT8800Q24(int nCS, int pinNSL = MOSI, SPISettings spiSettings = AEAT8800Q24SPISettings, SPISettings ssiSettings = AEAT8800Q24SSISettings);
	virtual ~MagneticSensorAEAT8800Q24();

    virtual float getSensorAngle() override;

	virtual void init(SPIClass* _spi = &SPI);
};

#endif