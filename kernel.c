#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "./vga/vga.h"
#include "./gdt/gdt.h"
#include "./idt/idt.h"
#include "./pit/pit.h"

void kernel_main(void) {
	terminal_init();
	gdt_init();
	idt_init();
	pit_init();
	for(;;);
}