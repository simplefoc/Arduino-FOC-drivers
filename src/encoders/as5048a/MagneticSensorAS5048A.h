
#ifndef __MAGNETICSENSORAS5048A_H__
#define __MAGNETICSENSORAS5048A_H__

#include "common/base_classes/Sensor.h"
#include "./AS5048A.h"

class MagneticSensorAS5048A : public Sensor, public AS5048A {
public:
	MagneticSensorAS5048A(int nCS = -1, bool fastMode = false, SPISettings settings = AS5048SPISettings);
	virtual ~MagneticSensorAS5048A();

    virtual float getSensorAngle() override;

	virtual void init(SPIClass* _spi = &SPI);
protected:
    bool fastMode = false;
};

#endif
