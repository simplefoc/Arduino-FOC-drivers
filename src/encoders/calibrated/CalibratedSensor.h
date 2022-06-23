#ifndef __CALIBRATEDSENSOR_H__
#define __CALIBRATEDSENSOR_H__

#include "common/base_classes/Sensor.h"
#include "BLDCMotor.h"
#include "common/base_classes/FOCMotor.h"


class CalibratedSensor: public Sensor{

public:
    // constructor of class with pointer to base class sensor and driver
    CalibratedSensor(Sensor& wrapped);
    ~CalibratedSensor();

    /*
    Override the update function
    */
    virtual void update() override;

    /**
    * Calibrate method computes the LUT for the correction
    */
    virtual void calibrate(BLDCMotor& motor);

    // voltage to run the calibration: user input
    float voltage_calibration = 1;                              

protected:

    /**
    * getSenorAngle() method of CalibratedSensor class.
    * This should call getAngle() on the wrapped instance, and then apply the correction to
    * the value returned. 
    */
    virtual float getSensorAngle() override;
    /**
    * init method of CaibratedSensor - call after calibration
    */
    virtual void init() override;
    /**
    * delegate instance of Sensor class
    */
    Sensor& _wrapped;
    
     // lut size, currently constan. Perhaps to be made variable by user?
    const int  n_lut { 128 } ;
    // create pointer for lut memory
    float* calibrationLut = new float[n_lut]();

    // Init inital angles
    float theta_actual { 0.0 };
    float elecAngle { 0.0 };
    float elec_angle { 0.0 };
    float theta_absolute_post { 0.0 };
    float theta_absolute_init { 0.0 };
    float theta_init { 0.0 };
    float avg_elec_angle { 0.0 };
};

#endif