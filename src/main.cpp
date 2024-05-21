#include "TouchSensor.hpp"
#include "Fader.hpp"

#include <USB-MIDI.h>
USBMIDI_CREATE_DEFAULT_INSTANCE();

#define FADERS_SIZE (sizeof(faders) / sizeof(Fader))

Fader faders[] = {
  Fader(A1, 7, 6, A2),
  Fader(A0, 9, 8, A3)
};
uint8_t CURRENT_BANK = 1;

struct BankOffset {
  uint8_t bank;
  uint8_t offset;

  BankOffset(uint8_t channel) {
    bank = (channel - 1) / FADERS_SIZE + 1;
    offset = (channel - 1) % FADERS_SIZE + 1;
  }
};

uint8_t extractChannel(uint8_t bank, uint8_t offset) {
  return (bank - 1) * FADERS_SIZE + offset;
}

void handlePitchBend(byte channel, int bend) {
  BankOffset bankOffset(channel);

  if (bankOffset.bank == CURRENT_BANK) {
    faders[bankOffset.offset - 1].setTarget(bend);
  }
}

void handlePositionUpdate(uint8_t channel, int16_t mapped) {
  MIDI.sendPitchBend(mapped, extractChannel(CURRENT_BANK, channel));
}

void setup() {
  Serial.begin(9600);
  MIDI.begin(0);
  MIDI.setHandlePitchBend(handlePitchBend);
  for (uint8_t i = 0; i < FADERS_SIZE; i++) {
    faders[i].calibrate();
    faders[i].setUpdatePositionCallback(handlePositionUpdate);
  }
}

uint32_t previousMillis = 0;
uint32_t currentMillis = 0;
void cycleBanksDemo() {
  currentMillis = millis();
  if (currentMillis - previousMillis >= 5000) {
    CURRENT_BANK++;
    if (CURRENT_BANK > 4) {
      CURRENT_BANK = 1;
    }
    
    // when cycling through internal banks we have to reselect the current bank of 8 channels to get the updated positions
    MIDI.sendNoteOn(47, 127, 1);                                    // next fader bank
    MIDI.sendNoteOn(46, 127, 1);                                    // previous fader bank
    MIDI.sendNoteOn(24 + extractChannel(CURRENT_BANK, 0), 127, 1);  // select first fader in bank
    MIDI.read();

    previousMillis = currentMillis;
  }
}

void loop() {
  cycleBanksDemo();
  for (uint8_t i = 1; i <= FADERS_SIZE; i++) {
    faders[i - 1].update(i);
  }
  MIDI.read();
}