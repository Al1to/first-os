#include "./string.h"

unsigned int get_digits(unsigned long number, const unsigned int base) {
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

    unsigned long num;
    if (base == 10 && number < 0) {
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