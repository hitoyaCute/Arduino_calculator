


#include "main.hpp"
#include "arduino/keyboard.hpp"

#define CALC_IMPL
#include "libs/calc.h"

template <int CAP>
class str_acc {
    char str[CAP];
    char len;
public:
    
};

const keypad4x4_pins_t key_pins = {{9,8,7,6},{5,4,3,2}};
keypad4x4_state_t keypad_states = {};

void blink_init() {
    Serial.begin(115200);
    Serial.print("starting");

    keypad4x4_init(key_pins);
}

void blink_task() {
    keyboard4x4_read(key_pins, &keypad_states);
    printKeypad(keypad_states.states);
    delay(50);
}
