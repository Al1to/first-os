#include "../stdafx.h"

#ifndef UTIL_H
#define UTIL_H

// ↓ заполняет первые count байтов области памяти, на которую указывает dest, байтом val
void memset(void *dest, char val, uint32_t count);

// ↓ переводит из числа в строку
void itoa(unsigned long number, char *str, const unsigned int base);

// ↓ эти функции - это просто сишные обертки для асм инструкций out и in
void out_port_l(uint16_t port, uint32_t value);
void out_port_w(uint16_t port, uint16_t value);
void out_port_b(uint16_t port, uint8_t  value);
uint32_t in_port_l(uint16_t port);
uint16_t in_port_w(uint16_t port);
uint8_t  in_port_b(uint16_t port);

void syscall_dbg();

// ↓ деление с округлением вверх, например, a=10 и b=3 получится 4
#define CEIL_DIV(a, b) ((a + b - 1) / b)

// ↓ все значения, которые должны сохраняться при прерывании (int)
struct int_regs {
    uint32_t cr2;
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, csm, eflags, useresp, ss;
};

#endif