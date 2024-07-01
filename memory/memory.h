#include "../stdafx.h"

#ifndef MEMORY_H
#define MEMORY_H

#define KERNEL_START 0xC0000000
#define PAGE_FLAG_PRESENT (1 << 0)
#define PAGE_FLAG_WRITE (1 << 1)

extern uint32_t page_dir_init[1024];
void pmm_init(uint32_t mem_low,uint32_t mem_high); 
void memory_init(uint32_t mem_high, uint32_t phys_alloc_start); 
void invalidate(uint32_t vaddr);

#endif