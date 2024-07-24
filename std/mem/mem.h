#include "../../stdafx.h"

#ifndef MEM_H
#define MEM_H

typedef struct {
	void *heap_start;
	void *heap_end;
	void *last_alloc_loc;
	size_t heap_size;
} heap_t;

heap_t heap_create(uint32_t pages);
void   heap_free(heap_t heap);

void   set_current_heap(heap_t heap);
heap_t get_current_heap();

void* malloc(size_t size);
void  free(void *ptr);

#endif