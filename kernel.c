#include "stdafx.h"

void kernel_main(uint32_t, struct multiboot_info* boot_info) {
	vga_init();
	gdt_init();
	idt_init();
	pit_init();
	keyboard_init();
	memory_init(boot_info);
	vga_print("\nel psy congroo.\n$ ");
	while(1);
}