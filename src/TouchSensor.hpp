#ifndef TOUCH_SENSOR_HPP
#define TOUCH_SENSOR_HPP

#include <ADCTouch.h>

struct TouchSensor {
private:
  uint8_t PIN;
  const uint16_t threshold;
  
  uint16_t refValue;
  uint16_t rawValue;

public:
  TouchSensor(uint8_t pin, uint16_t threshold = 40) : PIN(pin), threshold(threshold){};

  void calibrate() {
    refValue = ADCTouch.read(PIN, 500);
  }

  void update() {
    rawValue = ADCTouch.read(PIN) - refValue;
  }

  bool isTouching() {
    return rawValue > threshold;
  }
};

#endif