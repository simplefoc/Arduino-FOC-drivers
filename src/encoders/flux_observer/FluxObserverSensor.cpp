#include "FluxObserverSensor.h"
#include "common/foc_utils.h"
#include "common/time_utils.h"


FluxObserverSensor::FluxObserverSensor(const FOCMotor& m) : _motor(m)
{
}


void FluxObserverSensor::update() {
  // Current sense is required for the observer
  if (!_motor.current_sense) return;
  
  // Update sensor, with optional downsampling of update rate
  if(sensor_cnt++ < sensor_downsample) return;
  
  sensor_cnt = 0;

  // read current phase currents
  PhaseCurrent_s current = _motor.current_sense->getPhaseCurrents();

  // calculate clarke transform
  float i_alpha, i_beta;
  if(!current.c){
      // if only two measured currents
      i_alpha = current.a;  
      i_beta = _1_SQRT3 * current.a + _2_SQRT3 * current.b;
  }if(!current.a){
      // if only two measured currents
      float a = -current.c - current.b;
      i_alpha = a;  
      i_beta = _1_SQRT3 * a + _2_SQRT3 * current.b;
  }if(!current.b){
      // if only two measured currents
      float b = -current.a - current.c;
      i_alpha = current.a;  
      i_beta = _1_SQRT3 * current.a + _2_SQRT3 * b;
  } else {
      // signal filtering using identity a + b + c = 0. Assumes measurement error is normally distributed.
      float mid = (1.f/3) * (current.a + current.b + current.c);
      float a = current.a - mid;
      float b = current.b - mid;
      i_alpha = a;
      i_beta = _1_SQRT3 * a + _2_SQRT3 * b;
  }

  // Flux linkage observer    
  float now = _micros();
  float Ts = ( now - angle_prev_ts) * 1e-6f; 
  flux_a = _constrain( flux_a + (_motor.ABVoltage.alpha - _motor.phase_resistance * i_alpha) * Ts -
        _motor.phase_inductance * (i_alpha - i_alpha_prev),-_motor.flux_linkage, _motor.flux_linkage);
  flux_b = _constrain( flux_b + (_motor.ABVoltage.beta  - _motor.phase_resistance * i_beta)  * Ts -
        _motor.phase_inductance * (i_beta  - i_beta_prev) ,-_motor.flux_linkage, _motor.flux_linkage);
  
  // Calculate angle
  float electrical_angle = _normalizeAngle(_atan2(flux_b,flux_a));

  // Electrical angle difference
  float d_electrical_angle = 0;
  if (first){
    first = 0;
    d_electrical_angle = electrical_angle;
  }else{
    d_electrical_angle = electrical_angle - electrical_angle_prev;
    if(abs(d_electrical_angle) > _2PI * 0.8 ){ //change the  factor based on sample rate can also just use _PI for simplicity 
      if (d_electrical_angle > 0){
        d_electrical_angle -= _2PI;
      }else{
        d_electrical_angle += _2PI;
      }
    }
  }
  angle_track += d_electrical_angle;

  // Mechanical angles
  if(angle_track > _2PI * _motor.pole_pairs){
    full_rotations += 1;
    angle_track -= _2PI * _motor.pole_pairs;
  }else if (angle_track < 0){
    full_rotations -= 1;
    angle_track += _2PI * _motor.pole_pairs;
  }
  angle_prev = angle_track /_motor.pole_pairs;
  
  // Store Previous values
  i_alpha_prev = i_alpha;
  i_beta_prev = i_beta;
  angle_prev_ts = now;
  electrical_angle_prev = electrical_angle;

}

void FluxObserverSensor::init(){
  this->Sensor::init(); // call base class
}

/*
	Shaft angle calculation
*/
float FluxObserverSensor::getSensorAngle(){
  return 0;//return this->Sensor::getSensorAngle(); // call base class 
}