#ifndef FADER_HPP
#define FADER_HPP

#include <Arduino.h>
#include "TouchSensor.hpp"

#define  SMOOTHING_SAMPLE_SIZE  300

struct Fader {
private:
  const uint8_t FADER;
  const uint8_t FOUT;
  const uint8_t BOUT;
  TouchSensor sensor;

  uint16_t target_pos;
  uint16_t position;

  uint32_t previousMillis = 0;
  uint32_t currentMillis = 0;

  void (*updatePositionCallback)(int16_t, uint8_t);

  void debug(int channel, String status = "") {
    Serial.print("(Fader ");
    Serial.print(channel);
    Serial.print(") ");
    Serial.println(status);
  }

public:
  Fader(uint8_t fader, uint8_t fout, uint8_t bout, uint8_t touch) : FADER(fader), FOUT(fout), BOUT(bout), sensor(touch, 100) {
    target_pos = 512;
    position = 0;

    pinMode(FOUT, OUTPUT);
    pinMode(BOUT, OUTPUT);

    digitalWrite(FOUT, LOW);
    digitalWrite(BOUT, LOW);
  }

  void setUpdatePositionCallback(void (*callback)(int16_t, uint8_t)) {
    updatePositionCallback = callback;
  }

  void setTarget(int16_t position) {
    target_pos = map(position, 8176, -8192, 0, 1023);
  }

  void calibrate() {
    sensor.calibrate();
  }

  void update(int channel) {
    position = analogRead(FADER);
    currentMillis = millis();
    if (currentMillis - previousMillis >= 100) {
      previousMillis = currentMillis;
      sensor.update();
      if (channel == 1) {
        debug(channel, (String)"pos: " + position + " target: " + target_pos + " touch: " + sensor.isTouching());
      }  
    }

    if (sensor.wasReleased()) {
      debug(channel, "was released");
      target_pos = position;
    }
    else if (sensor.wasTouched()) {
      digitalWrite(FOUT, LOW);
      digitalWrite(BOUT, LOW);
      debug(channel, "was touched");
    }

    if (!sensor.isTouching()) {
      if (abs(position - target_pos) > 8) {
        if (position > target_pos) {
          digitalWrite(FOUT, HIGH);
          digitalWrite(BOUT, LOW);
        }
        if (position < target_pos) {
          digitalWrite(FOUT, LOW);
          digitalWrite(BOUT, HIGH);
        }
      }
      else {
        digitalWrite(FOUT, LOW);
        digitalWrite(BOUT, LOW);
      }
    }
    else {
      if (updatePositionCallback != nullptr) {
        int mapped = map(position, 0, 1023, 8176, -8192);
        updatePositionCallback(mapped, channel);
      }
    }
  }
};

#endif