#ifndef MXLEMMING_OBSERVER_SENSOR_H
#define MXLEMMING_OBSERVER_SENSOR_H

#include "Arduino.h"
#include "common/base_classes/FOCMotor.h"
#include "common/base_classes/Sensor.h"

/**
  
*/

class MXLEMMINGObserverSensor : public Sensor
{
  public:
    /**
    MXLEMMINGObserverSensor class constructor
    @param m  Motor that the MXLEMMINGObserverSensor will be linked to
    */
    MXLEMMINGObserverSensor(const FOCMotor& m);
    void update() override;
 
    void init() override;

    // Abstract functions of the Sensor class implementation
    /** get current angle (rad) */
    float getSensorAngle() override;

    
    // For sensors with slow communication, use these to poll less often
    unsigned int sensor_downsample = 0; // parameter defining the ratio of downsampling for sensor update
    unsigned int sensor_cnt = 0; // counting variable for downsampling
    float flux_alpha   = 0; // Flux Alpha 
    float flux_beta    = 0; // Flux Beta
    float flux_linkage = 0; // Flux linkage, calculated based on KV and pole number
    float i_alpha_prev = 0; // Previous Alpha current
    float i_beta_prev  = 0; // Previous Beta current
    float electrical_angle = 0; // Electrical angle
    float electrical_angle_prev = 0; // Previous electrical angle
    float angle_track = 0; // Total Electrical angle
    
  protected:    
    const FOCMotor& _motor;

};

#endif