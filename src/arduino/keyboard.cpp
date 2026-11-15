#include "keyboard.hpp"
#include <Arduino.h>

void keyboard4x4_read(const keypad4x4_pins_t pins, keypad4x4_state_t* state) {
    for (int vcc = 0; vcc < 4; ++vcc) {
        pinMode(pins.vcc[vcc], OUTPUT);
        digitalWrite(pins.vcc[vcc], LOW);
        delayMicroseconds(50);

        for (int sense = 0; sense < 4; ++sense)
            state->set(vcc, sense, digitalRead(pins.sense[sense]) == LOW);

        digitalWrite(pins.vcc[vcc], HIGH);      // actively recharge the lines
        delayMicroseconds(10);
        pinMode(pins.vcc[vcc], INPUT);          // then release to high-Z
    }
}

void keypad4x4_init(const keypad4x4_pins_t pins) {
    for (const auto pin: pins.vcc)
        pinMode(pin, INPUT);
    for (const auto pin: pins.sense)
        pinMode(pin, INPUT_PULLUP);
}
