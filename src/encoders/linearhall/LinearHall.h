#ifndef LINEAR_HALL_SENSOR_LIB_H
#define LINEAR_HALL_SENSOR_LIB_H

#include <SimpleFOC.h>

class LinearHall: public Sensor{
  public:
    LinearHall(int hallA, int hallB, int centerA, int centerB, int pp);

    // initialize the sensor hardware
    void init();

    // Get current shaft angle from the sensor hardware, and 
    // return it as a float in radians, in the range 0 to 2PI.
    //  - This method is pure virtual and must be implemented in subclasses.
    //    Calling this method directly does not update the base-class internal fields.
    //    Use update() when calling from outside code.
    float getSensorAngle() override;
  
  private:
    int pinA;
    int pinB;
    int pp;
    int electrical_rev;
    float prev_reading;

    int centerA;
    int centerB;
};

#endif
