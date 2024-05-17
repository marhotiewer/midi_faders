#include <Arduino.h>

#include <USB-MIDI.h>
#include <MagicPot.h>

USBMIDI_CREATE_DEFAULT_INSTANCE();
#define FADERS_SIZE (sizeof(faders) / sizeof(Fader))

struct Fader {
private:
    const uint16_t FOUT;
    const uint16_t BOUT;
    boolean locked;
    uint16_t target_pos;
    uint16_t position;
    MagicPot FADER;

public:
    Fader(uint16_t fader_pin, uint16_t fout, uint16_t bout) : FOUT(fout), BOUT(bout), FADER(MagicPot(fader_pin)) {
        locked = false;
        target_pos = 512;
        position = 0;

        pinMode(FOUT, OUTPUT);
        pinMode(BOUT, OUTPUT);

        digitalWrite(FOUT, LOW);
        digitalWrite(BOUT, LOW);
    }

    void setTarget(int16_t position) {
        target_pos = map(position, 8176, -8192, 0, 1023);
    }

    bool setLock(bool state) {
        bool original = locked;
        locked = state;
        return original;
    }

    void update() {
        FADER.read();
        position = FADER.getValue();

        if (abs(position - target_pos) > 16) {
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
};

Fader faders[] = {
    Fader(A1, 7, 6),
    Fader(A0, 9, 8)
};

void handlePitchBend(byte channel, int bend) {
    if (channel <= FADERS_SIZE) {
        faders[channel - 1].setTarget(bend);
    }
}

void setup() {
    Serial.begin(31250);
    MIDI.begin(0);
    MIDI.setHandlePitchBend(handlePitchBend);
}

void loop() {
    for (uint16_t i = 0; i < FADERS_SIZE; i++) {
        faders[i].update();
    } MIDI.read();
}
