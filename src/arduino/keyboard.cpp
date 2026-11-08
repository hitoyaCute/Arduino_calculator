#include <Arduino.h>

// header

// vcc[0] -> sense[0] = D
// vcc[3] -> sense[3] = 1
// vcc[0] -> sense[3] = *
// vcc[3] -> sense[0] = A
typedef struct keypad4x4_pins_t {
    // voltage source
    int vcc[4];
    // voltage sense
    int sense[4];
} keypad4x4_pins_t;

void keyboard4x4_read(const keypad4x4_pins_t pins) {

}
