#include "src/main.hpp"
#include <Arduino.h>
#include <Wire.h>

void setup() {
    blink_init();
}

void loop() {
    blink_task();
}
