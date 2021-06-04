/*
 * PreciseMagneticSensorAS5048A.h
 *
 *  Created on: 1 May 2021
 *      Author: runger
 */

#ifndef LIBRARIES_ARDUNIO_FOC_DRIVERS_SRC_ENCODERS_AS5048A_PRECISEMAGNETICSENSORAS5048A_H_
#define LIBRARIES_ARDUNIO_FOC_DRIVERS_SRC_ENCODERS_AS5048A_PRECISEMAGNETICSENSORAS5048A_H_

#include "common/base_classes/Sensor.h"
#include "./AS5048A.h"
#include "utilities/PreciseAngle.h"

class PreciseMagneticSensorAS5048A : public Sensor, public AS5048A {
public:
	PreciseMagneticSensorAS5048A(int nCS = -1, bool fastMode = false, SPISettings settings = AS5048SPISettings);
	virtual ~PreciseMagneticSensorAS5048A();

    virtual float getSensorAngle() override;
    virtual float getVelocity() override;

	virtual void init(SPIClass* _spi = &SPI) override;

protected:
    bool fastMode = false;
    PreciseAngle previous_angle = PreciseAngle();
    PreciseAngle current_angle = PreciseAngle();
    unsigned long previous_ts = 0;
    unsigned long current_ts = 1;
};

#endif /* LIBRARIES_ARDUNIO_FOC_DRIVERS_SRC_ENCODERS_AS5048A_PRECISEMAGNETICSENSORAS5048A_H_ */
