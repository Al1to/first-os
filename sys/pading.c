// #include "../stdafx.h"

// page_dir_entry kernel_page_dir[1024] __attribute__((aligned(4096)));
// page_table_entry new_base_page_table[1024] __attribute__((aligned(4096)));
// page_table_entry full_kernel_table_storage[1024] __attribute__((aligned(4096)));

// page_table_entry *kernel_tables = (page_table_entry *)0xC0400000;

//     // какой нах массив на 131072 байта еще и aligned(4096)
//     // что ето блять ↓ такое
// uint8_t pmem_used[131072] __attribute__((aligned(4096)));
// const char* mem_types[] = {"ERROR", "Available", "Reserved", "ACPI Reclaimable", "NVS", "Bad RAM"};

// #define pagedir_addr 768

// multiboot_memory_map_t * mmap;

// _Bool check_phys_page(void *addr) {
//     uint32_t page = (uint32_t)addr;
//     page /= 4096;
//     _Bool r = (_Bool)(pmem_used[page/8] & (1 << (page % 8)));
//     return r;
// }

// bool warn_claimed = true;

// void claim_phys_page(void *addr) {
//     if (check_phys_page(addr) && warn_claimed) {
//         vga_printf("[WARN] Attempting to claim already claimed page: %h\n", addr);
//     }
//     uint32_t page = (uint32_t)addr;
//     page /= 4096;
//     pmem_used[page/8] = 1 << (page % 8);
// }
//                                                         это все уже есть, ну кроме мб пары полезных функций, которы енадо еще написать
// void release_phys_page(void* addr) {                    если быть конкретнее, то в memory.c надо сделать что-то вроде аллокатора
//     uint32_t page = (uint32_t)addr;
//     page /= 4096;
//     pmem_used[page/8]& = ~(1 << (page % 8));
// }

// uint8_t check_page_cluster(void *addr) {
//     uint32_t page = (uint32_t)addr;
//     page /= 4096;
//     return pmem_used[page/8];
// }

// inline page_table_entry get_table_vaddr(void *vaddr) {
//     return kernel_tables[(uint32_t)vaddr/4096];
// }

// void init_pading(multiboot_info_t *mbi) {
//     kernel_page_dir[pagedir_addr].present = 1;
//     kernel_page_dir[pagedir_addr].readwrite = 1;
//     uint32_t nbpt_addr = (uint32_t)&new_base_page_table[0];
//     nbpt_addr >>= 12;
//     kernel_page_dir[pagedir_addr].address = nbpt_addr;

//     for (uint16_t i = 0; i < 1023; ++i) {
//         if ((i * 4096) >= 0x100000) {
//             new_base_page_table[i].present = 1;
//             new_base_page_table[i].readwrite = 1;
//             new_base_page_table[i].address = i;
//         }
//     }
//     new_base_page_table[1023].present = 1;
//     new_base_page_table[1023].readwrite = 1;
//     new_base_page_table[1023].address = 0x000B8;

//     uint32_t new_addr = (uint32_t)&kernel_page_dir[0];
//     new_addr -= 0xC0000000;
//     asm volatile("mov %0, %%cr3" :: "r"(new_assr));
    
// }

// void* alloc_page(size_t pages) {
//     if (!pages) {
//         return 0;
//     }

//     // First find consecutive
//     for (uint32_t i = 786432; i < 108576; ++i) {
//         if (!kernel_tables[i]) {}
//     }
// }