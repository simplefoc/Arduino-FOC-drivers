#ifndef SMOOTHING_SENSOR_H
#define SMOOTHING_SENSOR_H

#include "Arduino.h"
#include "common/base_classes/FOCMotor.h"
#include "common/base_classes/Sensor.h"

/**
  SmoothingSensor is a wrapper class which is inserted inbetween a sensor and motor to provide better
  quality angle readings from sensors that are low resolution or are slow to communicate. It provides
  no improvement for sensors that are high resolution and quick to update.
  It uses the timestamp of the last angle reading from the sensor and the low-pass filtered velocity
  from the motor to predict what the angle should be at the current time.
*/

class SmoothingSensor : public Sensor
{
  public:
    /**
    SmoothingSensor class constructor
    @param s  Wrapped sensor
    @param m  Motor that the SmoothingSensor will be linked to
    */
    SmoothingSensor(Sensor& s, const FOCMotor& m);

    void update() override;
    float getVelocity() override;
    int needsSearch() override;

    // For sensors with slow communication, use these to poll less often
    unsigned int sensor_downsample = 0; // parameter defining the ratio of downsampling for sensor update
    unsigned int sensor_cnt = 0; // counting variable for downsampling

    // For hall sensors, the predicted angle is always 0 to 60 electrical degrees ahead of where it would be without
    // smoothing, so offset it backward by 30 degrees (set this to -PI_6) to get the average in phase with the rotor
    float phase_correction = 0;

  protected:
    float getSensorAngle() override;
    void init() override;

    Sensor& _wrapped;
    const FOCMotor& _motor;
};

#endif
