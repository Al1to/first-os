#include <stdint.h>

#ifndef UTIL_H
#define UTIL_H

// Заполняет первые count байтов области памяти, на которую указывает dest, байтом val
void memset(void *dest, char val, uint32_t count);

void out_port_b(uint16_t port, uint8_t value);

struct int_regs {
    uint32_t cr2;
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, csm, eflags, useresp, ss;
};

#endif