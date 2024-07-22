#include "./math.h"

unsigned long pow(unsigned long number, int degree) {
    if (number == 0 || degree == 0) return 1;
    return number * pow(number, degree-1);
} 