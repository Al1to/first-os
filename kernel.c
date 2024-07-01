#include "stdafx.h"

void kernel_main(uint32_t magic, struct multiboot_info* boot_info) {
	vga_init();
	gdt_init();
	idt_init();
	pit_init();
	keyboard_init();

	uint32_t mod1 = *(uint32_t*)(boot_info->mods_addr + 4);
	uint32_t phys_alloc_start = (mod1 + 0xFFF) & ~0xFFF;
	memory_init(boot_info->mem_upper * 1024, phys_alloc_start);
	kmalloc_init(0x1000);

	vga_print("el psy congroo.\n");
	while(1);
}