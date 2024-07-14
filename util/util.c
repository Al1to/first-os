#include "./util.h"

void memset(void *dest, char val, uint32_t count) {
    char *temp = (char*) dest;
    for (; count != 0; count--)
    {
        *temp++ = val;
    }
}

uint64_t pow(uint64_t number, uint8_t degree) {
    if (number == 0)
        return 1;

    if (degree == 0)
        return 1;

    return number * pow(number, degree-1);
} 

unsigned int get_digits(uint64_t number, const int base) {
    unsigned int digits = 0;
    for (number; number > 0; number /= base) ++digits;
    return digits;
}

void itoa(unsigned long number, char *str, const unsigned int base) {
    int cur = 0;

    if (base < 2) {
        str[cur] = '\0';
        return;
    }

    if (number == 0) {
        str[cur++] = '0';
        str[cur] = '\0';
        return;
    }

    unsigned long long num;
    if (base == 10 && num < 0) {
        str[cur++] = '-';
        num = -number;
    } else if (base == 16) {
        // str[cur++] = '0';
        // str[cur++] = 'x';
        num = number;
    } else { 
        num = number; 
    }

    int digits = get_digits(num, base);

    for (unsigned long digit; digits > 0; --digits) { 
        digit = num / pow(base, digits - 1);
        if (digit < 10) {                  
            str[cur++] = digit + '0';
        } else {
            str[cur++] = digit + 'A' - 10;
        }
        num %= pow(base, digits - 1);
    }
    str[cur] = '\0';
}

void out_port_b(uint16_t port, uint8_t value) {
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

char in_port_b(uint16_t port) {
    char rv;
    asm volatile ("inb %1, %0": "=a"(rv):"dN"(port));
    return rv;
}

void syscall_dbg() {
  asm volatile ("mov $0x4, %%eax" : : : "eax"); 
  asm volatile ("int $0x80" : : : );
}
