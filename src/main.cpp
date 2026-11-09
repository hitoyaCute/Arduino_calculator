#define CALC_IMPL

#define ERROR(fmt, ...)                                                        \
    do {                                                                       \
        char b_[48];                                                           \
        snprintf_P(b_, sizeof(b_), PSTR(fmt), ##__VA_ARGS__);                  \
        Serial.print(F("!! "));                                                \
        Serial.print(b_);                                                      \
    } while (0)
#include "main.hpp"
#include "arduino/keyboard.hpp"
#include "key_handler.hpp"
#include "libs/calc.h"
#include "libs/str_acc.hpp"

#include <Arduino.h>
#include <HardwareSerial.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

const keypad4x4_pins_t key_pins = {{9, 8, 7, 6}, {5, 4, 3, 2}};
str_acc_t<255> acc;
// to know if key are pressed (each of them)
keypad4x4_state_t keypad_states = {};
// to tell if key press is handled
keypad4x4_state_t keypad_handled = {};

void blink_init() {
    Serial.begin(115200);
    keypad4x4_init(key_pins);
    u8g2.begin();
    Serial.println(F("ready"));
}

void blink_task() {
    keyboard4x4_read(key_pins, &keypad_states);
    float res;
    process_key_events(&acc, keypad_states, &keypad_handled, &res);

    Serial.print(F(">>> "));
    Serial.println(acc.view());
    Serial.print(F("res: "));
    Serial.println(res);

    u8g2.firstPage();
    do {
        u8g2.drawFrame(0, 0, 128, 32); // border
        // u8g2.drawLine(0, 0, 127, 31);      // diagonal line
        // u8g2.drawHLine(10, 28, 40);        // horizontal line
        // u8g2.drawPixel(64, 16);            // single pixel
        // x,y,    bitmap_w, bitmap_h, bitmap
        // u8g2.drawXBM(x, 12, 8, 8, smiley); // sprite at (x, 12)
    } while (u8g2.nextPage());
}
