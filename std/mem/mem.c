#include "./mem.h"

typedef struct alloc_t alloc_t;

struct alloc_t {
	size_t size;
	bool used;
	alloc_t *next;
	alloc_t *prev;
};

heap_t cur_heap;

heap_t heap_create(uint32_t pages) {
	vga_print("heap_create start\n");

	heap_t ret;
	void *mem = alloc_page(pages);

	vga_print("heap_create alloc_page\n");
	
	ret.heap_start = mem;
	ret.last_alloc_loc = ret.heap_start;
	ret.heap_end = ret.heap_start;
	ret.heap_size = pages;

	vga_print("heap_create before memset\n");
	
	memset((char*)ret.heap_start, 0, ret.heap_end-ret.heap_start);
	alloc_t *root_alloc = ret.heap_start;

	vga_print("heap_create after memset\n");
	
	root_alloc->used = false;            // вот где-то тут все крашится нахер
	root_alloc->size = pages * 0x1000;
	root_alloc->next = NULL;
	root_alloc->prev = NULL;

	vga_print("heap_create no problems\n");

	return ret;
}

void heap_free(heap_t heap) {
	free_page((void *)heap.heap_start,heap.heap_size);
}

void set_current_heap(heap_t heap) {
	vga_print("heap_create start\n");
	cur_heap = heap;
	vga_print("heap_create no problems\n");
}

heap_t get_current_heap() {
	return cur_heap;
}

static bool grow_heap() {
	cur_heap.heap_end += 0x1000;
	if (((uintptr_t)(cur_heap.heap_end - cur_heap.heap_start)) < cur_heap.heap_size) {
		return true;
	}
	return false;
}

void *malloc(size_t size) {
	alloc_t *cur_alloc = cur_heap.heap_start;
	while(true) {
		while (cur_alloc->used) {
			cur_alloc = cur_alloc->next;
		}
		if (cur_alloc->size == size) {
			if (cur_alloc == cur_heap.last_alloc_loc) {
				if (!grow_heap()) {
					return 0;
				}
				cur_alloc->size += 0x1000;
			} else {
				cur_alloc->used = true;
				return (void *)(cur_alloc) + sizeof(alloc_t);
			}
		}
		if (cur_alloc->size > size) {
			if (cur_alloc->size < size + sizeof(alloc_t)) {
				if (cur_alloc == cur_heap.last_alloc_loc) {
					grow_heap();
					cur_alloc->size += 0x1000;
				} else {
					cur_alloc->used = true;
					return (void *)(cur_alloc) + sizeof(alloc_t);
				}
			}
			alloc_t * new_alloc = (void *)(cur_alloc) + sizeof(alloc_t);

			new_alloc->used = false;
			new_alloc->size = cur_alloc->size - (sizeof(alloc_t));
			new_alloc->next = cur_alloc->next;
			new_alloc->prev = cur_alloc;

			if (new_alloc->next == NULL) {
				cur_heap.last_alloc_loc = new_alloc;
			}

			cur_alloc->used = true;
			cur_alloc->size = size;
			cur_alloc->next = new_alloc;

			return (void *)(cur_alloc) + sizeof(alloc_t);
		}
		if (cur_alloc == cur_heap.last_alloc_loc && cur_alloc->size < size) {
			while (true) {
				if (!grow_heap()) {
					return NULL;
				}
				cur_alloc->size += 0x1000;
				if (cur_alloc->size >= size) {
					break;
				}
			}
		}

	}
}

void free(void *ptr) {
	alloc_t *cur_alloc = ptr - sizeof(alloc_t);
	cur_alloc->used = false;

	alloc_t *next_alloc = cur_alloc->next;
	alloc_t *prev_alloc = cur_alloc->prev;

	if (next_alloc && !next_alloc->used) {
		cur_alloc->next = next_alloc->next;
		cur_alloc->size += next_alloc->size + sizeof(alloc_t);

		if (cur_alloc->next == NULL) {
			cur_heap.last_alloc_loc = cur_alloc;
		}
	}

	if (prev_alloc && !prev_alloc->used) {
		prev_alloc->next = cur_alloc->next;
		prev_alloc->size += cur_alloc->size + sizeof(alloc_t);

		if (prev_alloc->next == NULL) {
			cur_heap.last_alloc_loc = prev_alloc;
		}
	}
}