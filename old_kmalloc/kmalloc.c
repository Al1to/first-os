#include "./kmalloc.h"

// static uint32_t heap_start;
// static uint32_t heap_size;
// static uint32_t threshold;
// static bool kmalloc_initalized = false;

// void kmalloc_init(uint32_t heap_size_init) {
//     heap_start = KERNEL_START;
//     heap_size = 0;
//     threshold = 0;
//     kmalloc_initalized = true;

//     change_heap_size(heap_size_init);
// }

// void change_heap_size(new_size) {
//     int old_page_top = CEIL_DIV(heap_size, 0x1000);
//     int new_page_top = CEIL_DIV(new_size, 0x1000);

//     int diff = new_page_top - old_page_top;

//     for (int i = 0;  i < diff; ++i) {
//         uint32_t phys = pmm_alloc_page_frame();
//         mem_map_page(KERNEL_MALLOC + old_page_top * 0x1000 + i * 0x1000, phys, PAGE_FLAG_WRITE);
//     }
// }