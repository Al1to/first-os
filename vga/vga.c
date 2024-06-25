#include "vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t vga_row;
size_t vga_column;
uint8_t vga_color;
uint16_t* vga_buffer; 

void vga_init(void) {
	vga_row = 0;
	vga_column = 0;
	vga_color = vga_entry_color(15, 0);
	vga_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			vga_buffer[index] = vga_entry(' ', vga_color);
		}
	}
}

void vga_scroll_up(void) {
	for (uint16_t y = 0; y < VGA_HEIGHT; ++y) {
		for (uint16_t x = 0; x < VGA_WIDTH; ++x) {
			vga_buffer[(y-1) * VGA_WIDTH + x] = vga_buffer[y * VGA_WIDTH + x];
		}
	}

	for (uint16_t x = 0; x < VGA_WIDTH; ++x) {
		vga_buffer[(VGA_HEIGHT-1) * VGA_WIDTH + x] = vga_entry(' ', vga_color);
	}
}

void vga_new_line(void) {
	if (vga_row < VGA_HEIGHT - 1) ++vga_row;
	else vga_scroll_up();
	vga_column = 0;
}

void vga_print(const char* str) {
	while (*str) {
		switch (*str) {
			case '\n':
				vga_new_line();
				break;
			default:
				if (vga_column == VGA_WIDTH) vga_new_line();
				vga_buffer[vga_row * VGA_WIDTH + (vga_column++)] = vga_entry(*str, vga_color);
				break;
		}
		++str;
	}	
}

void vga_printf(const char* str, size_t d) {
	while (*str) {
		switch (*str) {
			case '%':
				uint16_t start = str;
				++str;
				if (*str == 'd') {
					// TODO:
				}
				break;
			default:
				break;
		}
		++str;
	}
}
