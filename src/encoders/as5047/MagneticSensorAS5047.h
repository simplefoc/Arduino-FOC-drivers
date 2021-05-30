
#ifndef __MAGNETICSENSORAS5047PD_H__
#define __MAGNETICSENSORAS5047PD_H__

#include "common/base_classes/Sensor.h"
#include "./AS5047.h"

class MagneticSensorAS5047 : public Sensor, public AS5047 {
public:
	MagneticSensorAS5047(int nCS = -1, bool fastMode = false, SPISettings settings = AS5047SPISettings);
	virtual ~MagneticSensorAS5047();

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
