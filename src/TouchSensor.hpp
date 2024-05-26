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

  unsigned long previousMillis = 0;
  unsigned long currentMillis = 0;

public:
  TouchSensor(int pin, int threshold = 40) : PIN(pin), threshold(threshold){};

  void calibrate(int samples = 500) {
    refValue = ADCTouch.read(PIN, samples);
  }

  void update(int samples = 100) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= 50) {
      previousMillis = currentMillis;
      rawValue = ADCTouch.read(PIN, samples) - refValue;
    }
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