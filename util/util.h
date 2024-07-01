#include "../stdafx.h"

#ifndef UTIL_H
#define UTIL_H

// Заполняет первые count байтов области памяти, на которую указывает dest, байтом val
void memset(void *dest, char val, uint32_t count);

// Переводит из числа в строку
void itoa(unsigned long number, char *str, const unsigned int base); 

void out_port_b(uint16_t port, uint8_t value);
char in_port_b(uint16_t port);

struct int_regs {
    uint32_t cr2;
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, csm, eflags, useresp, ss;
};

#endif