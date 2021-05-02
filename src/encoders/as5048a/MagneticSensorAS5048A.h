
#ifndef __MAGNETICSENSORAS5048A_H__
#define __MAGNETICSENSORAS5048A_H__

#include "common/base_classes/Sensor.h"
#include "./AS5048A.h"

class MagneticSensorAS5048A : public Sensor, public AS5048A {
public:
	MagneticSensorAS5048A(int nCS = -1, bool fastMode = false, SPISettings settings = AS5048SPISettings);
	virtual ~MagneticSensorAS5048A();

    virtual float getAngle() override;
    virtual float getVelocity() override;

	virtual void init(SPIClass* _spi = &SPI);
private:
    bool fastMode = false;
    float angle_data_prev=0;
    float angle_curr=0;
    long angle_curr_ts=0;
    float full_rotation_offset=0;   // TODO this is problematic as it causes angle counting to stop after a relatively short time, when the precision of the full_rotation
                                    // is insufficient to capture the angle-delta, no further incrementation happends

    float angle_prev=0; //!< angle in previous velocity calculation step
    long velocity_calc_timestamp=0; //!< last velocity calculation timestamp
};

#endif
