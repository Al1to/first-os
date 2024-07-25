#include "stdafx.h"

void kernel_main(uint32_t magic, uint32_t boot_info) {
	vga_init();
	gdt_init();
	idt_init();
	pit_init();
	keyboard_init();
	paging_init((struct multibut_info_t *)boot_info);
	ahci_init();

	pci_scan();
	
	vga_print("\nEl psy congroo.\n");
	terminal_init("$ ");
}
