#include "stdafx.h"

void kernel_main(void) {
	vga_init();
	vga_print("VGA is done!\n");

	gdt_init();
	vga_print("GDT is done!\n");
	
	idt_init();
	vga_print("IDT is done!\n");

	pit_init();
	vga_print("PIT is done!\n");

	vga_print("el psy congru.\n");

	while(1);
}