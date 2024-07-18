#include "../stdafx.h"

#ifndef MEMORY_H
#define MEMORY_H

#define KERNEL_START 0xC0000000
#define KERNEL_MALLOC 0xD000000
#define REC_PAGE_DIR ((uint32_t*)0xFFFFF000)
#define REC_PAGE_TABLE(i) ((uint32_t*)(0xFFC00000 + ((i) << 12))) 
#define PAGE_FLAG_PRESENT (1 << 0)
#define PAGE_FLAG_WRITE (1 << 1)
#define PAGE_FLAG_OWNER (1 << 9)

extern uint32_t page_dir_init[1024];

void pmm_init(uint32_t mem_low, uint32_t mem_high); 
uint32_t pmm_alloc_page_frame();

void mem_map_page(uint32_t vaddr, uint32_t paddr, uint32_t flags);
uint32_t* mem_get_cur_page_dir();
void mem_change_page_dir(uint32_t* pd);
void sync_page_dirs();
void mem_map_page(uint32_t vaddr, uint32_t paddr, uint32_t flags);

void memory_init(uint32_t mem_high, uint32_t phys_alloc_start); 
void invalidate(uint32_t vaddr);

#endif