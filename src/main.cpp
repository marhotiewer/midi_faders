#include "TouchSensor.hpp"
#include "Fader.hpp"
#include <USB-MIDI.h>

USBMIDI_CREATE_DEFAULT_INSTANCE();

#define NUM_FADERS (sizeof(faders) / sizeof(Fader))
#define NUM_BANKS ((8 / NUM_FADERS) - 1)

uint32_t previousMillis = 0;
uint32_t currentMillis = 0;
uint8_t activeBank = 0;
int16_t positions[8] = { 0 };
Fader faders[] = {
  Fader(A1, 7, 6, A2),
  Fader(A0, 9, 8, A3)
};

struct BankOffset {
  uint8_t bank;
  uint8_t offset;
  uint8_t channel;

  BankOffset(uint8_t channel) : channel(channel) {
    bank = (channel - 1) / NUM_FADERS;
    offset = (channel - 1) % NUM_FADERS;
  }

  BankOffset(uint8_t bank, uint8_t offset) : bank(bank), offset(offset) {
    channel = (bank * NUM_FADERS) + offset;
  }
};

void selectBank(uint8_t bank) {
  for (uint8_t i = 0; i < NUM_FADERS; i++) {
    faders[i].setTarget(positions[(bank * NUM_FADERS) + i]);
  }
  MIDI.sendNoteOn(24 + BankOffset(bank, 0).channel, 127, 1);
  MIDI.read();
}

void cycleBanksDemo() {
  currentMillis = millis();
  if (currentMillis - previousMillis >= 5000) {
    if (++activeBank > NUM_BANKS) {
      activeBank = 0;
    } selectBank(activeBank);
    previousMillis = currentMillis;
  }
}

void handleExternalPosUpdate(byte channel, int bend) {
  positions[channel - 1] = bend;

  BankOffset bankOffset(channel);
  if (bankOffset.bank == activeBank) {
    faders[bankOffset.offset].setTarget(bend);
  }
}

void handleInternalPosUpdate(uint8_t channel, int16_t mapped) {
  MIDI.sendPitchBend(mapped, BankOffset(activeBank, channel).channel);
}

void setup() {
  Serial.begin(9600);
  MIDI.begin(0);
  MIDI.setHandlePitchBend(handleExternalPosUpdate);
  for (uint8_t i = 0; i < NUM_FADERS; i++) {
    faders[i].calibrate();
    faders[i].setUpdatePositionCallback(handleInternalPosUpdate);
  }
}

void loop() {
  cycleBanksDemo();
  for (uint8_t i = 1; i <= NUM_FADERS; i++) faders[i - 1].update(i);
  MIDI.read();
}