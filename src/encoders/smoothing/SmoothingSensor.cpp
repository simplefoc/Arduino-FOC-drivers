#include "SmoothingSensor.h"
#include "common/foc_utils.h"
#include "common/time_utils.h"


SmoothingSensor::SmoothingSensor(Sensor& s, const FOCMotor& m) : _wrapped(s), _motor(m)
{
}


void SmoothingSensor::update() {
  // Update sensor, with optional downsampling of update rate
  if(sensor_cnt++ >= sensor_downsample) {
    sensor_cnt = 0;
    _wrapped.update();
  }

  // Copy state variables from the sensor
  angle_prev = _wrapped.angle_prev;
  angle_prev_ts = _wrapped.angle_prev_ts;
  full_rotations = _wrapped.full_rotations;

  // Perform angle prediction, using low-pass filtered velocity. But don't advance more than
  // pi/3 (equivalent to one step of block commutation) from the last true angle reading.
  float dt = (_micros() - angle_prev_ts) * 1e-6f;
  angle_prev += _motor.sensor_direction * _constrain(_motor.shaft_velocity * dt, -_PI_3 / _motor.pole_pairs, _PI_3 / _motor.pole_pairs);

  // Apply phase correction if needed
  if (phase_correction != 0) {
    if (_motor.shaft_velocity < -0) angle_prev -= _motor.sensor_direction * phase_correction / _motor.pole_pairs;
    else if (_motor.shaft_velocity > 0) angle_prev += _motor.sensor_direction * phase_correction / _motor.pole_pairs;
  }

  // Handle wraparound of the projected angle
  if (angle_prev < 0) full_rotations -= 1, angle_prev += _2PI;
  else if (angle_prev >= _2PI) full_rotations += 1, angle_prev -= _2PI;
}


float SmoothingSensor::getVelocity() {
  return _wrapped.getVelocity();
}


int SmoothingSensor::needsSearch() {
  return _wrapped.needsSearch();
}


float SmoothingSensor::getSensorAngle() {
  return _wrapped.getSensorAngle();
}


void SmoothingSensor::init() {
  _wrapped.init();
}
