#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "./vga/vga.h"
#include "./gdt/gdt.h"
#include "./idt/idt.h"

void kernel_main(void) {
	terminal_init();
	gdt_init();
	idt_init();
	
	terminal_print("everything is working\n");
}
