
#ifndef __MAGNETICSENSORAS5047PD_H__
#define __MAGNETICSENSORAS5047PD_H__

#include "common/base_classes/Sensor.h"
#include "./AS5047.h"

class MagneticSensorAS5047 : public Sensor, public AS5047 {
public:
	MagneticSensorAS5047(int nCS = -1, bool fastMode = false, SPISettings settings = AS5047SPISettings);
	virtual ~MagneticSensorAS5047();

    virtual float getSensorAngle() override;

	virtual void init(SPIClass* _spi = &SPI);
private:
    bool fastMode = false;
};

#endif
