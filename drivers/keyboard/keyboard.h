#include "../../stdafx.h"

#ifndef KEYBOARD_H
#define KEYBOARD_H

void keyboard_init(void);

void keyboard_handler(struct int_regs *regs);

#endif