#include "../stdafx.h"

#ifndef KMALLOC_H
#define KMALLOC_H

void kmalloc_init(uint32_t heap_size_init);
void change_heap_size(new_size);

#endif