#include "../stdafx.h"

#ifndef VGA_H
#define VGA_H

void vga_init(void);

void vga_scroll_up(void);
void vga_new_line(void);

void vga_print(const char* str);
void vga_printf(const char* str, int d);

static inline uint8_t vga_entry_color(uint8_t fg, uint8_t bg) {
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}

#endif