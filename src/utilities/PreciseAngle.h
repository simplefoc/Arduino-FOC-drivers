/*
 * PreciseAngle.h
 *
 *  Created on: 1 May 2021
 *      Author: runger
 */

#ifndef LIBRARIES_ARDUNIO_FOC_DRIVERS_SRC_UTILITIES_PRECISEANGLE_H_
#define LIBRARIES_ARDUNIO_FOC_DRIVERS_SRC_UTILITIES_PRECISEANGLE_H_

#include <stdint.h>

class PreciseAngle {
public:
	PreciseAngle();
	virtual ~PreciseAngle();
	PreciseAngle(const PreciseAngle &other);
	PreciseAngle(uint16_t count, int32_t rotations);
	PreciseAngle(float radians);
	PreciseAngle(double radians);
	//PreciseAngle& operator=(const PreciseAngle &other);

	// returns the angle mod 2PI, i.e. the angle of the motor shaft, in radians
	float getShaftAngle();
	// number of complete motor rotations
	int32_t getRotations();
	uint16_t getShaftTicks();

	// returns the total angle, including rotations, in radians
	float asFloat();
	// returns the total angle, including rotations, in radians, as double precision
	double asDouble();
	// returns the total angle, including rotations, in ticks, as 64 bit integer
	int64_t asTicks();


	PreciseAngle operator+(const PreciseAngle &other);
	PreciseAngle operator-(const PreciseAngle &other);

	// velocity in rad/s
	float velocity(const PreciseAngle &previous, uint32_t microseconds);

	// update the angle with a new ticks value. this will increment the rotations
	// if necessary, using a simple algorithm to detect overflows.
	// if the motor turns too much between calls to update, the overflow could be missed
	// and the rotations counted incorrectly.
	void update(uint16_t current_angle);


	const static uint16_t cpr = 16384;
	const static uint16_t cpr_overflow_check = 13107; //cpr*0.8;
protected:
	uint16_t angle;
	int32_t rotations;
};

#endif /* LIBRARIES_ARDUNIO_FOC_DRIVERS_SRC_UTILITIES_PRECISEANGLE_H_ */
