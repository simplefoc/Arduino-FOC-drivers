#ifndef __CALIBRATEDSENSOR_H__
#define __CALIBRATEDSENSOR_H__

#include "common/base_classes/Sensor.h"
#include "BLDCMotor.h"
#include "common/base_classes/FOCMotor.h"
#include "common/foc_utils.h"


class CalibratedSensor: public Sensor{

public:
    /**
     * @brief Constructor of class with pointer to base class sensor and driver
     * @param wrapped the wrapped sensor which needs calibration
     * @param n_lut the number of entries in the lut
     * @param lut the look up table (if null, the lut will be allocated on the heap)
     */
    CalibratedSensor(Sensor& wrapped, int n_lut = 200, float* lut = nullptr);

    ~CalibratedSensor();

    /*
    Override the update function
    */
    virtual void update() override;

    /**
    * Calibrate method computes the LUT for the correction
    */
    virtual void calibrate(FOCMotor& motor, int settle_time_ms = 30);

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

    void alignSensor(FOCMotor &motor);
    void filter_error(float* error, float &error_mean, int n_ticks, int window);
    
     // lut size - settable by the user
    int  n_lut { 200 } ;
    // pointer for lut memory 
    // depending on the size of the lut
    // will be allocated in the calibrate function if not given.
    bool allocated;
    float* calibrationLut;
};

#endif
