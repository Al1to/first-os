#include "stdafx.h"

void kernel_main(void) {
	vga_init();
	gdt_init();
	idt_init();
	pit_init();
	keyboard_init();

	vga_print("el psy congroo.\n$ ");
	while(1);
}