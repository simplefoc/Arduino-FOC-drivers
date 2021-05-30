
#include "./MagneticSensorAS5047.h"
#include "common/foc_utils.h"
#include "common/time_utils.h"

MagneticSensorAS5047::MagneticSensorAS5047(int nCS, bool fastMode, SPISettings settings) : AS5047(settings, nCS), fastMode(fastMode) {

}

MagneticSensorAS5047::~MagneticSensorAS5047(){ 

}
void MagneticSensorAS5047::init(SPIClass* _spi) {
    this->AS5047::init(_spi);
	// velocity calculation init
	angle_prev = 0;
	velocity_calc_timestamp = _micros();
	// full rotations tracking number
	full_rotation_offset = 0;
    readRawAngle();
    angle_data_prev = readRawAngle();
    getAngle();
}

float MagneticSensorAS5047::getAngle() {
    float angle_data = readRawAngle();
    if (!fastMode) // read again to ensure current value
        angle_data = readRawAngle();
    angle_curr_ts = _micros();

    // TODO check for and handle sensor errors

    // tracking the number of rotations 
    // in order to expand angle range form [0,2PI] 
    // to basically infinity
    float d_angle = angle_data - angle_data_prev;
    // if overflow happened track it as full rotation
    if(abs(d_angle) > (0.8*AS5047_CPR) ) full_rotation_offset += d_angle > 0 ? -_2PI : _2PI; 
    // save the current angle value for the next steps
    // in order to know if overflow happened
    angle_data_prev = angle_data;
    angle_curr = full_rotation_offset + ( angle_data / (float)AS5047_CPR) * _2PI;

    // return the full angle 
    // (number of full rotations)*2PI + current sensor angle 
    return angle_curr;
}



/* 
unlike the normal MagneticSensorSPI implementation, this one uses the angle previously read by the last call to getAngle to do its
calculation, and does not directly poll any data from the sensor.
This is an optimisation for speed, based on the assumption that loopFOC() (which calls getAngle()) is invoked at least as often as
move() (which calls getVelocity()). If this is the case, getVelocity() should always have a sufficiently "fresh" value to work with.
If using this function in a different context, simply call getAngle() first to be sure of a fresh angle value.
*/
float MagneticSensorAS5047::getVelocity() {
    // calculate sample time
    float Ts = (angle_curr_ts - velocity_calc_timestamp)*1e-6;
    // quick fix for strange cases (micros overflow)
    if(Ts <= 0 || Ts > 0.5) Ts = 1e-3; 

    // velocity calculation
    float vel = (angle_curr - angle_prev)/Ts;

    // save variables for future pass
    angle_prev = angle_curr;
    velocity_calc_timestamp = angle_curr_ts;
    return vel;
}
