/*
 * PreciseAngle.cpp
 *
 *  Created on: 1 May 2021
 *      Author: runger
 */

#include <utilities/PreciseAngle.h>
#include <common/foc_utils.h>

PreciseAngle::PreciseAngle() : angle(0), rotations(0)  {}

PreciseAngle::~PreciseAngle() {}

PreciseAngle::PreciseAngle(const PreciseAngle &other) : angle(other.angle), rotations(other.rotations) {}


PreciseAngle::PreciseAngle(uint16_t count, int32_t rotations) : angle(count), rotations(rotations) {}


PreciseAngle::PreciseAngle(float radians) {
	rotations = radians/_2PI;
	angle = (radians - (rotations * _2PI)) * cpr / _2PI;
}

PreciseAngle::PreciseAngle(double radians) {
	rotations = radians/_2PI;
	angle = (radians - (rotations * _2PI)) * cpr / _2PI;
}

// returns the angle mod 2PI, i.e. the angle of the motor shaft, in radians
float PreciseAngle::getShaftAngle() {
	return angle * _2PI / cpr;
}


// returns the angle mod 2PI, i.e. the angle of the motor shaft, in radians
int32_t PreciseAngle::getRotations() {
	return rotations;
}

// returns the angle mod 2PI, i.e. the angle of the motor shaft, in radians
uint16_t PreciseAngle::getShaftTicks() {
	return angle;
}


// returns the total angle, including rotations, in radians
float PreciseAngle::asFloat() {
	return (angle * _2PI / (float)cpr) + ((float)rotations * _2PI);
}


// returns the total angle, including rotations, in radians, as double precision
double PreciseAngle::asDouble() {
	return (double)angle * _2PI / cpr + (double)rotations * _2PI;
}


// returns the total angle, including rotations, in ticks, as 64 bit integer
int64_t PreciseAngle::asTicks() {
	return (int64_t)angle + (int64_t)rotations * cpr;
}




PreciseAngle PreciseAngle::operator+(const PreciseAngle &other) {
	PreciseAngle result;
	result.rotations = rotations + other.rotations;
	uint32_t temp = angle + other.angle;
	if (temp>=cpr) {
		result.angle = temp-cpr;
		result.rotations+=1;
	}
	else
		result.angle = temp;
	return result;
}


PreciseAngle PreciseAngle::operator-(const PreciseAngle &other) {
	PreciseAngle result;
	result.rotations = rotations - other.rotations;
	int32_t temp = angle - other.angle;
	if (temp<0) {
		result.angle = temp+cpr;
		result.rotations-=1;
	}
	else
		result.angle = temp;
	return result;
}


// velocity in rad/s
float PreciseAngle::velocity(const PreciseAngle &previous, uint32_t microseconds) {
	PreciseAngle diff = (*this - previous);
	return diff.asFloat() * (1e6/(float)microseconds);
}



void PreciseAngle::update(uint16_t current_angle){
	int32_t diff = current_angle - angle;
	if (abs(diff)>cpr_overflow_check){
		rotations += diff>0?-1:1;
	}
	angle = current_angle;
}



