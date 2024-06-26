#include "./util.h"

void memset(void *dest, char val, uint32_t count) {
    char *temp = (char*) dest;
    for (; count != 0; count--)
    {
        *temp++ = val;
    }
}

char *itoa(int num, char *str) {
    size_t i = 0;
    bool isNegative = false;

    if (num < 0) {
        isNegative = true;
        num = -num;
    }

    do {
        int remainder = num % 10;
        str[i++] = (remainder + '0');
        num /= 10;
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

    return str; // + '\0'
}

void out_port_b(uint16_t port, uint8_t value) {
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

char in_port_b(uint16_t port) {
    char rv;
    asm volatile ("inb %1, %0": "=a"(rv):"dN"(port));
    return rv;
}