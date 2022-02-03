
#ifndef __MAGNETIC_SENSOR_TLE5012B_H__
#define __MAGNETIC_SENSOR_TLE5012B_H__

#include "./STM32TLE5012B.h"

#if defined(_STM32_DEF_)

#include "common/base_classes/Sensor.h"




class MagneticSensorTLE5012B : public Sensor, public TLE5012B {
    public:
        MagneticSensorTLE5012B(int data, int sck, int nCS);
        ~MagneticSensorTLE5012B();
        virtual void init() override;
        virtual float getSensorAngle() override;
};



#endif
#endif
