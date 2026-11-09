#include "keyboard.hpp"
#include <Arduino.h>

void keyboard4x4_read(const keypad4x4_pins_t pins, keypad4x4_state_t* state) {
    for (int vcc = 0; vcc < 4; ++vcc) {
        digitalWrite(pins.vcc[vcc], LOW);
        // delayMicroseconds(50); // let the line settle

        for (int sense = 0; sense < 4; ++sense)
            state->set(vcc,sense,digitalRead(pins.sense[sense]) == LOW);
        digitalWrite(pins.vcc[vcc], HIGH);
    }
}

void keypad4x4_init(const keypad4x4_pins_t pins) {
    for (const auto pin: pins.vcc) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, HIGH);
    }
    for (const auto pin: pins.sense)
        pinMode(pin, INPUT_PULLUP);
}
