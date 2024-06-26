#include "./util.h"

void memset(void *dest, char val, uint32_t count) {
    char *temp = (char*) dest;
    for (; count != 0; count--)
    {
        *temp++ = val;
    }
}

void itoa(int num, char *str, int base) {
    int i = 0;
    bool is_negative = false;
    if (base == 10) {
        is_negative = num < 0;
        if (is_negative) {
            num = -num;
        }
    }

    do {
        int digit = num % base;
        if (digit < 10) {
            str[i++] = digit + '0';
        } else {
            str[i++] = digit - 10 + 'A';
        }
        num /= base;
    } while (num);

    if (is_negative) {
        str[i++] = '-';
    }

    str[i] = '\0';

    for (int j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}

void out_port_b(uint16_t port, uint8_t value) {
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

char in_port_b(uint16_t port) {
    char rv;
    asm volatile ("inb %1, %0": "=a"(rv):"dN"(port));
    return rv;
}