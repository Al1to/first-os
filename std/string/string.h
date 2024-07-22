#include "../../stdafx.h"

#ifndef STRING_H
#define STRING_H

// ↓ заполняет первые count байтов области памяти, на которую указывает dest, байтом val
void memset(void *dest, char val, uint32_t count);

// ↓ переводит из числа в строку
void itoa(unsigned long number, char *str, const unsigned int base);

// ↓ сравнивает строки на идентичность
bool strcmp(const char *str1, const char *str2);

// ↓ подсчитывает и возращает количество символов в строке
size_t strlen(const char *string);

// | копирует содержимое scrptr в destptr и возвращает ссылку на 1 элемент destptr,
// ↓ при условии что длина destptr > длины scrptr, иначе возвращает NULL
char* strcpy(char *destptr, char *scrptr); 

#endif