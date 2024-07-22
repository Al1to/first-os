#include "../../stdafx.h"

#ifndef KEYBOARD_H
#define KEYBOARD_H

#define MAX_INPUT_LENGTH 128

void keyboard_init(void);

void keyboard_handler(struct int_regs *regs);

char* wait_keyboard_input(void);

#endif