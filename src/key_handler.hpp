#ifndef INCLUDE_SRC_KEY_HANDLER_HPP_
#define INCLUDE_SRC_KEY_HANDLER_HPP_

#include "arduino/keyboard.hpp"
#include "libs/calc.h"
#include "libs/str_acc.hpp"
#include "main.hpp"

template <unsigned char CAP>
inline static void handle_event(bool Fkey_held, str_acc_t<CAP> *acc,
                                const keypad4x4_state_t keypad_states, int i,
                                float *res) {
    if (keypad_states.view('=')) {
        evaluate(acc->view(), res);
        acc->clear();
    } else if (keypad_states.view('R')) {
        for (int i = 0; i < ROOT_LEN; ++i)
            acc->append(ROOT_SYM[i]);
    } else if (Fkey_held) {
        if (keypad_states.view('D') && keypad_states.view('F'))
            acc->pop();
        else
            acc->append(keypad_chars_F[i / 4][i % 4]);
    } else
        acc->append(keypad_chars_normal[i / 4][i % 4]);
}

template <unsigned char CAP>
void process_key_events(str_acc_t<CAP> *acc,
                        const keypad4x4_state_t keypad_states,
                        keypad4x4_state_t *keypad_handled, float *res) {
    // handle keys to make it detect low to high
    //
    // flag for each key
    const bool Fkey_held = keypad_states.view(3, 0);

    if (Fkey_held)
        printKeypad(keypad_states, keypad_chars_F);
    else
        printKeypad(keypad_states, keypad_chars_normal);

    for (int i = 0; i < 4 * 4; ++i) {
        if (i / 4 == 3 && i % 4 == 0)
            continue;
        if (keypad_states.view(i / 4, i % 4)) {
            if (!keypad_handled->view(i / 4, i % 4))
                handle_event(Fkey_held, acc, keypad_states, i, res);
            keypad_handled->set(i / 4, i % 4, 1);
        } else {
            keypad_handled->set(i / 4, i % 4, 0);
        }
    }
}

#endif // INCLUDE_SRC_KEY_HANDLER_HPP_
