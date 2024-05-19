#include "TouchSensor.hpp"
#include "Fader.hpp"

#include <USB-MIDI.h>
USBMIDI_CREATE_DEFAULT_INSTANCE();

#define FADERS_SIZE (sizeof(faders) / sizeof(Fader))

Fader faders[] = {
  Fader(A1, 7, 6, A2),
  Fader(A0, 9, 8, A3)
};

void handlePitchBend(byte channel, int bend) {
  if (channel <= FADERS_SIZE) {
    faders[channel - 1].setTarget(bend);
  }
}

void handlePositionUpdate(int16_t mapped, uint8_t channel) {
  MIDI.sendPitchBend(mapped, channel);
}

void setup() {
  for (uint8_t i = 0; i < FADERS_SIZE; i++) {
    faders[i].calibrate();
    faders[i].setUpdatePositionCallback(handlePositionUpdate);
  }
  Serial.begin(31250);
  MIDI.begin(0);
  MIDI.setHandlePitchBend(handlePitchBend);
}

void loop() {
  for (uint8_t i = 0; i < FADERS_SIZE; i++) {
    faders[i].update(i + 1);
  }
  MIDI.read();
}