/*
 * PreciseMagneticSensorAS5048A.cpp
 *
 *  Created on: 1 May 2021
 *      Author: runger
 */

#include <encoders/as5048a/PreciseMagneticSensorAS5048A.h>
#include "common/foc_utils.h"
#include "common/time_utils.h"

PreciseMagneticSensorAS5048A::PreciseMagneticSensorAS5048A(int nCS, bool fastMode, SPISettings settings) : AS5048A(settings, nCS), fastMode(fastMode) { }

PreciseMagneticSensorAS5048A::~PreciseMagneticSensorAS5048A() { }


void PreciseMagneticSensorAS5048A::init(SPIClass* _spi) {
    this->AS5048A::init(_spi);
	// velocity calculation init
    current_ts = _micros();
    /*uint16_t angle_data =*/ readRawAngle();
    current_angle = PreciseAngle(readRawAngle(), 0);
    getAngle();
}



float PreciseMagneticSensorAS5048A::getSensorAngle() {
    previous_ts = current_ts;
    previous_angle = current_angle;
    uint16_t angle_data = readRawAngle();
    if (!fastMode) // read again to ensure current value
    	angle_data = readRawAngle();
    current_ts = _micros();
    current_angle.update(angle_data);
    return current_angle.asFloat();
}



/*
unlike the normal MagneticSensorSPI implementation, this one uses the angle previously read by the last call to getAngle to do its
calculation, and does not directly poll any data from the sensor.
This is an optimisation for speed, based on the assumption that loopFOC() (which calls getAngle()) is invoked at least as often as
move() (which calls getVelocity()). If this is the case, getVelocity() should always have a sufficiently "fresh" value to work with.
If using this function in a different context, simply call getAngle() first to be sure of a fresh angle value.
*/
float PreciseMagneticSensorAS5048A::getVelocity() {
	return current_angle.velocity(previous_angle, (current_ts-previous_ts));
}
