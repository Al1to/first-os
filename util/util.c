#include "./util.h"

void memset(void *dest, char val, uint32_t count) {
    char *temp = (char*) dest;
    for (; count != 0; count--)
    {
        *temp++ = val;
    }
}

char *itoa(size_t num, char *str, int base) {
    size_t i = 0;
    bool isNegative = false;

    if (num < 0) {
        isNegative = true;
        num = -num;
    }

    do {
        int remainder = num % base;
        str[i++] = (remainder < 10) ? (remainder + '0') : (remainder - 10 + 'A');
        num /= base;
    } while (num);

    if (isNegative) {
        str[i++] = '-';
    }

    str[i] = '\0';
    for (int j = 0, k = i - 1; j < k; ++j, --k) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }

    return str;
}

void out_port_b(uint16_t port, uint8_t value) {
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}