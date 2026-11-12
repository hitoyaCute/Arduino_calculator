#include "arduino/display.hpp"
#define CALC_IMPL

#define ERROR(fmt, ...)                                                        \
    do {                                                                       \
        char b_[32];                                                           \
        snprintf_P(b_, sizeof(b_), PSTR(fmt), ##__VA_ARGS__);                  \
        Serial.print(F("!! "));                                                \
        Serial.print(b_);                                                      \
    } while (0)
#include "arduino/keyboard.hpp"
#include "key_handler.hpp"
#include "libs/calc.h"
#include "libs/str_acc.hpp"
#include "main.hpp"

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
float res = 0;

void blink_init() {
    Serial.begin(115200);
    keypad4x4_init(key_pins);
    u8g2.begin();
    Serial.println(F("ready"));
}

void blink_task() {
    keyboard4x4_read(key_pins, &keypad_states);
    process_key_events(&acc, keypad_states, &keypad_handled, &res);

    Serial.print(F(">>> "));
    Serial.print(acc.view());
    Serial.println("_");

    char buf[11] = {0};
    // dtostrf(value, width, precision, output_buffer)
    dtostrf(res, 10, 2, buf);
    Serial.print("res: '");
    Serial.print(buf);
    Serial.println("'");
    // buf[9] = '0';

    //
    const unsigned char len = acc.get_len();
    if (len >= 9)
        draw_UI(acc.view() + (len - 9), buf, &u8g2);
    else
        draw_UI(acc.view(), buf, &u8g2);
}
