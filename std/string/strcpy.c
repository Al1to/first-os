#include "./string.h"

char* strcpy(char *destptr, char *scrptr) {
    if (strlen(destptr) < strlen(scrptr)) {
        return NULL;
    }
    char* ptr = destptr;
    while (*scrptr) {
        *destptr = *scrptr++;
    }

    return ptr;
}