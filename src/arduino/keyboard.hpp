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

#include <Arduino.h>
typedef struct keypad4x4_pins_t {
    // voltage source
    int vcc[4];
    // voltage sense
    int sense[4];
} keypad4x4_pins_t;

typedef struct keypad4x4_state_t {
    uint16_t states;
    void set(int x, int y, bool set) {
        if (set)
            this->states |= 1 << (y * 4 + x);
        else
            this->states &= ~(1 << (y * 4 + x));
    }

    bool view(int x, int y) const { return (this->states >> (y * 4 + x)) & 1; }

    bool view(char c) const {
        switch (c) {
        case '1':
            return this->view(0, 0);
        case '2':
            return this->view(0, 1);
        case '3':
            return this->view(0, 2);
        case '4':
            return this->view(1, 0);
        case '5':
        case 'D':
            return this->view(1, 1);
        case '6':
            return this->view(1, 2);
        case '7':
            return this->view(2, 0);
        case '8':
            return this->view(2, 1);
        case '9':
            return this->view(2, 2);
        case '0':
            return this->view(3, 1);
        case 'F':
            return this->view(3, 0);
        case '.':
            return this->view(3, 3);
        case '+':
        case '/':
            return this->view(0, 3);
        case '-':
        case '^':
            return this->view(1, 3);
        case '*':
        case 'R':
            return this->view(2, 3);
        case '=':
            return this->view(3, 3);
        default:
            0;
        }
    }
} keypad4x4_state_t;

static const char keypad4x4_chars[4][4] = {{'1', '2', '3', 'A'},
                                           {'4', '5', '6', 'B'},
                                           {'7', '8', '9', 'C'},
                                           {'*', '0', '#', 'D'}};

// debugging tool
#include <Arduino.h>
static inline void
printKeypad(const keypad4x4_state_t grid,
            const char keypad4x4chars[4][4] = keypad4x4_chars) {
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            if (grid.view(row, col)) {
                Serial.print('[');
                Serial.print(keypad4x4chars[row][col]);
                Serial.print(']');
            } else {
                Serial.print(' ');
                Serial.print(keypad4x4chars[row][col]);
                Serial.print(' ');
            }
        }
        Serial.println();
    }
}

void keyboard4x4_read(const keypad4x4_pins_t pins, keypad4x4_state_t *state);
void keypad4x4_init(const keypad4x4_pins_t pins);

#endif // INCLUDE_ARDUINO_KEYBOARD_H_
