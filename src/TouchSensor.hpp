#ifndef TOUCH_SENSOR_HPP
#define TOUCH_SENSOR_HPP

#include <ADCTouch.h>

struct TouchSensor {
private:
  const int PIN;
  const int threshold;
  
  int refValue;
  int rawValue;
  
  bool _state = false;

public:
  TouchSensor(int pin, int threshold = 40) : PIN(pin), threshold(threshold){};

  void calibrate(int samples = 500) {
    refValue = ADCTouch.read(PIN, samples);
  }

  void update(int samples = 100) {
    rawValue = ADCTouch.read(PIN, samples) - refValue;
  }

  bool isTouching() {
    return rawValue > threshold;
  }

  bool wasTouched() {
      bool v = isTouching();
      if (v && !_state) {
          _state = v;
          return true;
      }
      return false;
  }

  bool wasReleased() {
    bool v = isTouching();
    if (_state && !v) {
      _state = v;
      return true;
    }
    return false;  
  }
};

#endif