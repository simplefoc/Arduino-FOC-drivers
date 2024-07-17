#ifndef __MAGNETIC_SENSOR_MA735_H__
#define __MAGNETIC_SENSOR_MA735_H__


#include "common/base_classes/Sensor.h"
#include "./MA735.h"

class MagneticSensorMA735 : public Sensor, public MA735 {
public:
	MagneticSensorMA735(int nCS = -1, SPISettings settings = MA735SPISettings);
	virtual ~MagneticSensorMA735();

    virtual float getSensorAngle() override;

	virtual void init(SPIClass* _spi = &SPI);
};


#endif
