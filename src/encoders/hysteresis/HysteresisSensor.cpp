
#include "./HysteresisSensor.h"


HysteresisSensor::HysteresisSensor(Sensor& wrapped, float amount) : _amount(amount), _wrapped(wrapped) {
    // empty
};


void HysteresisSensor::init() {
    _wrapped.update();
    _window = _wrapped.getMechanicalAngle();
    this->Sensor::init();
};

float HysteresisSensor::getSensorAngle() {
    _wrapped.update();
    float raw = _wrapped.getMechanicalAngle();

    float d_angle = raw - _window;
    if(abs(d_angle) > (0.8f*_2PI) ) {
        if (d_angle > 0) {
            if (raw < (_2PI - _amount + _window)) {
                _window = _normalizeAngle(raw + _amount);
                return raw;
            }
        } else {
            if (raw > (_amount - (_2PI - _window))) {
                _window = _normalizeAngle(raw - _amount);
                return raw;
            }
        }
    }
    else  {
        if (raw > (_window + _amount)) {
            _window = _normalizeAngle(raw - _amount);
            return raw;
        }
        if (raw < (_window - _amount)) {
            _window = _normalizeAngle(raw + _amount);
            return raw;
        }
    }
    return angle_prev; // no change
};

