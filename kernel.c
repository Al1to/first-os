#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "./vga/vga.h"
#include "./gdt/gdt.h"

void kernel_main(void) {
	terminal_init();
	gdt_init();
	
	terminal_writestring("gdt is done\n");
}
