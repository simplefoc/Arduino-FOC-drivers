#include "LinearHall.h"

LinearHall::LinearHall(int _hallA, int _hallB, int _centerA, int _centerB, int _pp){
  pinA = _hallA;
  pinB = _hallB;
  centerA = _centerA;
  centerB = _centerB;
  pp = _pp;
}

float LinearHall::getSensorAngle() {
  float rawA = analogRead(pinA);
  float rawB = analogRead(pinB);

  //offset readings using center values, then compute angle
  float a = rawA - centerA;
  float b = rawB - centerB;
  float reading = atan2f(a, b);

  //handle rollover logic between each electrical revolution of the motor
  if (reading > prev_reading) {
    if (reading - prev_reading >= PI) {
      if (electrical_rev - 1 < 0) {
        electrical_rev = pp - 1;
      } else {
        electrical_rev = electrical_rev - 1;
      }
    }
  } else if (reading < prev_reading) {
    if (prev_reading - reading >= PI) {
      if (electrical_rev + 1 >= pp) {
        electrical_rev = 0;
      } else {
        electrical_rev = electrical_rev + 1;
      }
    }
  }

  //convert result from electrical angle and electrical revolution count to shaft angle in radians
  float result = (reading + PI) / _2PI;
  result = _2PI * (result + electrical_rev) / pp;

  //update previous reading for rollover handling
  prev_reading = reading;
  return result;
}

void LinearHall::init () {
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);

  //establish initial reading for rollover handling
  electrical_rev = 0;
  prev_reading = atan2f(analogRead(pinA) - centerA, analogRead(pinB) - centerB);
}
