
#ifndef __MAGNETICSENSORAS5047U_H__
#define __MAGNETICSENSORAS5047U_H__

#include "common/base_classes/Sensor.h"
#include "./AS5047U.h"

class MagneticSensorAS5047U : public Sensor, public AS5047U {
public:
	MagneticSensorAS5047U(int nCS = -1, bool fastMode = false, SPISettings settings = AS5047USPISettings);
	virtual ~MagneticSensorAS5047U();

    virtual float getSensorAngle() override;

	virtual void init(SPIClass* _spi = &SPI);
private:
    bool fastMode = false;
};

#endif
