#include "./memory.h"

void memory_init(struct multiboot_info* boot_info) {
    for (int i = 0; i < boot_info->mmap_length; i += sizeof(struct multiboot_mmap_entry)) {
        struct multiboot_mmap_entry* mmmt = (struct multiboot_mmap_entry*)(boot_info->mmap_addr + i);
        vga_printf("low addr: %x | ", mmmt->addr_low);
        vga_printf("high addr: %x | ", mmmt->addr_high);
        vga_printf("low length: %x | ", mmmt->len_low);
        vga_printf("hight length: %x | ", mmmt->len_high);
        vga_printf("size: %x | ", mmmt->size);
        vga_printf("type: %x \n", mmmt->type);
    }
}