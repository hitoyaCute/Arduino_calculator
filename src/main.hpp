#ifndef INCLUDE_SRC_MAIN_HPP_
#define INCLUDE_SRC_MAIN_HPP_


#include <math.h>
#include <Arduino.h>
#include <HardwareSerial.h>
#include "libs/calc.h"

/* 1 2 3 +
 * 4 5 6 -
 * 7 8 9 *
 * F 0 . =*/
const char keypad_chars_normal[][4] = {
    {'1','2','3','+'},
    {'4','5','6','-'},
    {'7','8','9','*'},
    {'F','0','.','='}
};
/* _ _ _ /
 * _ x _ ^
 * _ _ _ √
 * F _ _ =*/
const char keypad_chars_F[][4] = {
    {' ',' ','(','/'},
    {' ','D',' ','^'},
    {' ',' ',')','R'},
    {'F',' ','.','='}
};

inline static void evaluate(const char *t, float *res) {
    parser_t parser = {};                       // was uninitialised
    expr_index root = parse(&parser, t);
    if (root == EXPR_INDEX_EOF)
        ERROR("empty expression");
    else if (root != EXPR_INDEX_ERROR)          // parse() already reported errors
        *res = expression_evaluate(&parser, t, parser.expression_pool[root]);
    parser_free(&parser);
}

void blink_task();
void blink_init();

#endif  // INCLUDE_SRC_MAIN_HPP_
