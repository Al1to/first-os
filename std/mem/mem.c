#include "./mem.h"

typedef struct {
	size_t size;
	bool used;
	alloc_t *next;
	alloc_t *prev;
} alloc_t;

heap_t cur_heap;

heap_t heap_create(uint32_t pages) {
	heap_t ret;
	void *mem = alloc_page(pages);
	ret.heap_start = mem;
	ret.last_alloc_loc = ret.heap_start;
	ret.heap_end = ret.heap_start;
	ret.heap_size = pages;
	memset((char*)ret.heap_start, 0, ret.heap_end-ret.heap_start);
	alloc_t *root_alloc = ret.heap_start;
	root_alloc->used = false;
	root_alloc->size = pages*4096;
	root_alloc->next = NULL;
	root_alloc->prev = NULL;
	return ret;
}