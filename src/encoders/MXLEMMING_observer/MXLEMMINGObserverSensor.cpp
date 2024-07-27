#include "MXLEMMINGObserverSensor.h"
#include "common/foc_utils.h"
#include "common/time_utils.h"


MXLEMMINGObserverSensor::MXLEMMINGObserverSensor(const FOCMotor& m) : _motor(m)
{
  // Derive Flux linkage from KV_rating and pole_pairs
  if (_isset(_motor.pole_pairs) && _isset(_motor.KV_rating)){
    flux_linkage = 60 / ( _SQRT3 * _PI * _motor.KV_rating * _motor.pole_pairs * 2);
  }
}


void MXLEMMINGObserverSensor::update() {
  // Current sense is required for the observer
  if (!_motor.current_sense) return;
  
  // Exit if one of the parameter needed for the flux observer is 0
  if ((_motor.phase_inductance == 0) ||
      (_motor.phase_resistance == 0) ||
      (flux_linkage == 0)) return;

  // Update sensor, with optional downsampling of update rate
  if (sensor_cnt++ < sensor_downsample) return;

  sensor_cnt = 0;

  // read current phase currents
  PhaseCurrent_s current = _motor.current_sense->getPhaseCurrents();

  // calculate clarke transform
  ABCurrent_s ABcurrent = _motor.current_sense->getABCurrents(current);

  // get current timestamp
  long now_us = _micros();
  // calculate the sample time from last call
  float dt = (now_us - angle_prev_ts) * 1e-6f;
  // quick fix for strange cases (micros overflow + timestamp not defined)
  if(dt <= 0 || dt > 0.5f) dt = 1e-3f;

  // This work deviates slightly from the BSD 3 clause licence.
  // The work here is entirely original to the MESC FOC project, and not based
  // on any appnotes, or borrowed from another project. This work is free to
  // use, as granted in BSD 3 clause, with the exception that this note must
  // be included in where this code is implemented/modified to use your
  // variable names, structures containing variables or other minor
  // rearrangements in place of the original names I have chosen, and credit
  // to David Molony as the original author must be noted.
  
  // MXLEMMING Flux Observer
  float resistive_term_a =  _motor.phase_resistance * ABcurrent.alpha;
  float resistive_term_b =  _motor.phase_resistance * ABcurrent.beta;
  float inductive_term_a = _motor.phase_inductance * (ABcurrent.alpha - i_alpha_prev);
  float inductive_term_b = _motor.phase_inductance * (ABcurrent.beta  - i_beta_prev);

  flux_alpha = _constrain( flux_alpha + (_motor.Ualpha - resistive_term_a) * dt - inductive_term_a ,-flux_linkage, flux_linkage);
  flux_beta  = _constrain( flux_beta  + (_motor.Ubeta  - resistive_term_b) * dt - inductive_term_b ,-flux_linkage, flux_linkage);
  
  // Calculate electrical angle
  electrical_angle = _normalizeAngle(_atan2(flux_beta,flux_alpha));

  // Electrical angle difference
  float d_electrical_angle = electrical_angle - electrical_angle_prev;
  if(abs(d_electrical_angle) > _2PI * 0.8 ){ //change the  factor based on sample rate can also just use _PI for simplicity 
    if (d_electrical_angle > 0){
      d_electrical_angle -= _2PI;
    }else{
      d_electrical_angle += _2PI;
    }
  }
  angle_track += d_electrical_angle;

  // Mechanical angle and full_rotations
  float full_rotation = _2PI * _motor.pole_pairs;
  if(abs(angle_track) > full_rotation){
    if (angle_track>0){
      full_rotations += 1;
      angle_track -= full_rotation;
    }else{
      full_rotations -= 1;
      angle_track += full_rotation;
    }
  }
  angle_prev = angle_track /_motor.pole_pairs;
  
  // Store Previous values
  i_alpha_prev = ABcurrent.alpha;
  i_beta_prev = ABcurrent.beta;
  angle_prev_ts = now_us;
  electrical_angle_prev = electrical_angle;

}

void MXLEMMINGObserverSensor::init(){
  this->Sensor::init(); // call base class
}

/*
	Shaft angle calculation
*/
float MXLEMMINGObserverSensor::getSensorAngle(){
  return 0;
}