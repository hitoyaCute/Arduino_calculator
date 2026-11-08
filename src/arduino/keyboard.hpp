#ifndef INCLUDE_ARDUINO_KEYBOARD_H_
#define INCLUDE_ARDUINO_KEYBOARD_H_

///////////////////////////////////////////////////////////////
/// struct that hols keypad pins
///
/// vcc[0] -> sense[0] = 1
/// vcc[3] -> sense[0] = *
/// vcc[0] -> sense[3] = A
/// vcc[3] -> sense[3] = D
///
//////////////////////////////////////////////////////////////

typedef struct keypad4x4_pins_t {
    // voltage source
    int vcc[4];
    // voltage sense
    int sense[4];
} keypad4x4_pins_t;

typedef struct keypad4x4_state_t {
    bool states[4][4];
} keypad4x4_state_t;

static const char keypad4x4_chars[4][4] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}};

// debugging tool
#include <Arduino.h>
static inline void printKeypad(const bool grid[4][4]) {
    Serial.println("\n\n\n\n\n\n\n\n\n\n");
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            if (grid[row][col]) {
                Serial.print('[');
                Serial.print(keypad4x4_chars[row][col]);
                Serial.print(']');
            } else {
                Serial.print(' ');
                Serial.print(keypad4x4_chars[row][col]);
                Serial.print(' ');
            }
        }
        Serial.println();
    }
}

void keyboard4x4_read(const keypad4x4_pins_t pins, keypad4x4_state_t* state);
void keypad4x4_init(const keypad4x4_pins_t pins);


#endif  // INCLUDE_ARDUINO_KEYBOARD_H_
