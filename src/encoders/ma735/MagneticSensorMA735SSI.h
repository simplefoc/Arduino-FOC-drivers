#ifndef __MAGNETIC_SENSOR_MA735SSI_H__
#define __MAGNETIC_SENSOR_MA735SSI_H__


#include "common/base_classes/Sensor.h"
#include "./MA735.h"

class MagneticSensorMA735SSI : public Sensor {
public:
	MagneticSensorMA735SSI(SPISettings settings = MA735SSISettings);
	virtual ~MagneticSensorMA735SSI();

    virtual float getSensorAngle() override;

	virtual void init(SPIClass* _spi = &SPI);

    uint16_t readRawAngleSSI();
	
private:
	SPIClass* spi;
	SPISettings settings;
};


#endif
