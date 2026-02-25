#ifndef __CALIBRATEDSENSOR_H__
#define __CALIBRATEDSENSOR_H__

#include "common/base_classes/Sensor.h"
#include "BLDCMotor.h"
#include "common/base_classes/FOCMotor.h"
#include "common/foc_utils.h"
#include "communication/SimpleFOCDebug.h"

// LUT quantization constants for uint16_t encoding
// Maps radians in range [-PI, PI] to uint16_t [0, 65535]
// The real error is probably going to be much smaller than that 
// range so in some cases it might make sense to
// raise the LUT_SCALE constants to improve resolution.
#define LUT_SCALE 10430.2191955f  // 65535 / (2*PI)  ~ 
#define LUT_OFFSET 32767.0f       // center value for symmetric range

class CalibratedSensor: public Sensor{

public:
    /**
     * @brief Constructor of class with pointer to base class sensor and driver
     * @param wrapped the wrapped sensor which needs calibration
     * @param n_lut the number of entries in the lut
     * @param lut the look up table (if null, the lut will be allocated on the heap)
     */
    CalibratedSensor(Sensor& wrapped, int n_lut = 200, uint16_t* lut = nullptr);

    ~CalibratedSensor();

    /*
    Override the update function
    */
    virtual void update() override;

    /**
    * Calibrate method computes the LUT for the correction
    */
    virtual void calibrate(FOCMotor& motor, int settle_time_ms = 30);

    /**
     * Print the LUT to the provided Print instance
     * @param motor the FOCMotor instance (for direction info)
     * @param printer the Print instance to print to
     * 
     * Print in the form of C++ code array for easy copy-pasting
     */
    void printLUT(FOCMotor& motor, Print &printer);

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
    uint16_t* calibrationLut;
    // pre-computed inverse LUT resolution for faster lookups: n_lut / (2*PI)
    float lut_resolution_inv { 0.0f };
    float lut_resolution { 0.0f };
    
    // Helper functions for quantization
    inline uint16_t encodeOffsetU16(float offset) { return (uint16_t)(offset * LUT_SCALE + LUT_OFFSET); }
    inline float decodeOffsetU16(uint16_t encoded) { return (encoded - LUT_OFFSET) / LUT_SCALE; }
};

#endif
