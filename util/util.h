#include <stdint.h>

#ifndef UTIL_H
#define UTIL_H

// Заполняет первые count байтов области памяти, на которую указывает dest, константным байтом val
void memset(void *dest, char val, uint32_t count);

#endif