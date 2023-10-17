#ifndef FLUX_OBSERVER_SENSOR_H
#define FLUX_OBSERVER_SENSOR_H

#include "Arduino.h"
#include "common/base_classes/FOCMotor.h"
#include "common/base_classes/Sensor.h"

/**
  
*/

class FluxObserverSensor : public Sensor
{
  public:
    /**
    FluxObserverSensor class constructor
    @param m  Motor that the FluxObserverSensor will be linked to
    */
    FluxObserverSensor(const FOCMotor& m);
    void update() override;
 
    void init() override;

    // Abstract functions of the Sensor class implementation
    /** get current angle (rad) */
    float getSensorAngle() override;

    
    // For sensors with slow communication, use these to poll less often
    unsigned int sensor_downsample = 0; // parameter defining the ratio of downsampling for sensor update
    unsigned int sensor_cnt = 0; // counting variable for downsampling
    float flux_a = 0;
    float flux_b = 0;
    float i_alpha_prev = 0;
    float i_beta_prev = 0;
    float electrical_angle_prev = 0;
    float full_electrical_rotations = 0;

  protected:    
    const FOCMotor& _motor;

};

#endif
