#ifndef __MAGNETIC_SENSOR_MA730SSI_H__
#define __MAGNETIC_SENSOR_MA730SSI_H__


#include "common/base_classes/Sensor.h"
#include "./MA730.h"

class MagneticSensorMA730SSI : public Sensor {
public:
	MagneticSensorMA730SSI(SPISettings settings = MA730SSISettings);
	virtual ~MagneticSensorMA730SSI();

    virtual float getSensorAngle() override;

	virtual void init(SPIClass* _spi = &SPI);

    uint16_t readRawAngleSSI();
	
private:
	SPIClass* spi;
	SPISettings settings;
};


#endif