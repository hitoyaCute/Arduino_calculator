#include "display.hpp"
#include "../libs/calc.h"
#include <U8g2lib.h>

/* TOP------ABC-------s
 * x+ 1 - 32 (56 * 7)
 * result
 */

/* F-------------------
 * x+ 1 - 32 (56 * 7)
 * result
 */

void draw_UI(const char *equation, const char *res,
             U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C *u8g2) {
    u8g2->firstPage();
    do {
        u8g2->drawRBox(0, 0, 128, 16, 4);
        u8g2->setBitmapMode(1);
        u8g2->setDrawColor(0);
        int x = 4;
        // draw the equations
        for (const char *c = equation; *c != 0; ++c) {
            if (*c == ' ')
                x += 12;
            else if (is_numeric(*c)) {
                u8g2->drawXBMP(x, 1, 10, 14, display_digits[*c - '0']);
                x += 12;
            } else if (is_alphaSmal(*c)) {
                u8g2->drawXBMP(x, 1, 10, 16, display_alpha_lower[*c - 'a']);
                x += 12;
            } else if (is_alphaCap(*c)) {
                u8g2->drawXBMP(x, 1, 10, 14, display_alpha_upper[*c - 'A']);
                x += 12;
            } else {
                char op = display_operator_chars(*c);
                u8g2->drawXBMP(x, 1, 10, 14, display_operator[op]);
                if (*c == '.')
                    x += 10;
                else
                    x += 12;
            }
        }
        char op = display_operator_chars('_');
        u8g2->drawXBMP(x, 1, 10, 14, display_operator[op]);
        u8g2->setDrawColor(1);

        x = 4;
        for (int i = 0; i < 10; ++i) {
            if (res[i] == 0 || res[i] == ' ') {
                x += 12;
                continue;
            } else if (res[i] == '.') {
                u8g2->drawXBMP(x, 17, 10, 14, display_operator[display_operator_chars('.')]);
                x += 10;
            } else if (res[i] == '-') {
                u8g2->drawXBMP(x, 17, 10, 14, display_operator[display_operator_chars('-')]);
                x += 12;
            } else if (is_numeric(res[i])) {
                u8g2->drawXBMP(x, 17, 10, 14, display_digits[res[i] - '0']);
                x += 12;
            } else {
                x += 12;
            }
        }

    } while (u8g2->nextPage());
}
