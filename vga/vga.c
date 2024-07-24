#include "./vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t vga_row;
size_t vga_column;
uint8_t vga_color;
uint16_t* vga_buffer; 

void print_pci_vga_data(pci_t pci, uint8_t i, uint8_t j, uint8_t k) {
	// (void)i;
	// (void)j;
	// (void)k;
	// if (pci.vendorID != 0xFFFF && pci.class == 1 && pci.subclass == 6) {
	// 	/*if (k==0) {
	// 	  printf("Detected SATA Host on port %X:%X\n", i,j);
	// 	  dprintf("Detected SATA Host on port %X:%X\n", i,j);
	// 	} else {
	// 	  printf("Detected SATA Host on port %X:%X.%u\n",i,j,k);
	// 	  dprintf("Detected SATA Host on port %X:%X.%u\n",i,j,k);
	// 	}*/
	// 	// identity_map((void *)pci.BAR5);
	// 	// initialize_abar((HBAData *)pci.BAR5);
	// }
	vga_print("Detected VGA on port ");
	vga_printf("%x", i);
	vga_print(":");
	vga_printf("%x", j); // itoa не работает
	vga_print("\n");
}

void vga_init(void) {
	vga_row = 0;
	vga_column = 0;
	vga_color = vga_entry_color(15, 0);
	vga_buffer = (uint16_t*) 0xC00B8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			vga_buffer[index] = vga_entry(' ', vga_color); 
		}
	}
	vga_update_cursor();
	pci_register_driver(print_pci_vga_data, 3, 0);
}

void vga_update_cursor() {
	uint16_t pos = vga_row * VGA_WIDTH + vga_column;

	out_port_b(0x3D4, 0x0F);
	out_port_b(0x3D5, (uint8_t) (pos & 0xFF));
	out_port_b(0x3D4, 0x0E);
	out_port_b(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
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
	if (vga_row < VGA_HEIGHT - 1) {
		++vga_row;
	} else {
		vga_scroll_up();
	}
	vga_column = 0;
	vga_update_cursor();
}

void vga_put_char(char ch) {
	switch (ch) {
		case '\n':
			vga_new_line();
			break;
		case '\r':
			vga_column = 0;
			vga_update_cursor();
			break;
		case '\b':
			if (vga_column == 0 && vga_row != 0) {
				--vga_row;
				vga_column = VGA_WIDTH;
			}
			vga_buffer[vga_row * VGA_WIDTH + (--vga_column)] = vga_entry(' ', vga_color);
			vga_update_cursor();
			break;
		case '\t':
			uint16_t tab_len = 4 - (vga_column % 4);
			while (tab_len != 0) {
				vga_buffer[vga_row * VGA_WIDTH + (vga_column++)] = vga_entry(' ', vga_color);
			}
			vga_update_cursor();
			break;
		default:
			if (vga_column == VGA_WIDTH) vga_new_line();
			vga_buffer[vga_row * VGA_WIDTH + (vga_column++)] = vga_entry(ch, vga_color);
			vga_update_cursor();
			break;
	}
}

void vga_put_charс(uint8_t color, char ch) {
	switch (ch) {
		case '\n':
			vga_new_line();
			break;
		case '\r':
			vga_column = 0;
			vga_update_cursor();
			break;
		case '\b':
			if (vga_column == 0 && vga_row != 0)
			{
				--vga_row;
				vga_column = VGA_WIDTH;
			}
			vga_buffer[vga_row * VGA_WIDTH + (--vga_column)] = vga_entry(' ', color);
			vga_update_cursor();
			break;
		case '\t':
			uint16_t tab_len = 4 - (vga_column % 4);
			while (tab_len != 0) {
				vga_buffer[vga_row * VGA_WIDTH + (vga_column++)] = vga_entry(' ', color);
			}
			vga_update_cursor();
			break;
		default:
			if (vga_column == VGA_WIDTH) vga_new_line();
			vga_buffer[vga_row * VGA_WIDTH + (vga_column++)] = vga_entry(ch, color);
			vga_update_cursor();
			break;
	}
}

void vga_print(const char* str) {
	while (*str) {
		vga_put_char(*str++);
	}
}	

void vga_printc(uint8_t color, const char* str) {
	uint8_t vga_printc_color = vga_entry_color(color, 0);
	while (*str) {
		vga_put_charс(vga_printc_color, *str++);
	}
}

void vga_printf(const char* str, int n) {
	while (*str) 
	{
		switch (*str) {
			case '%':
				++str;
				if (*str == 'd') {
					char* num;
					itoa(n, num, 10);
					vga_print(num);
				} else if (*str == 'x') {
					char* num;
					itoa(n, num, 16);
					vga_print(num);
				} else if(*str == 'c') {
					vga_put_char(n);
				} else {
					--str;
				}
				break;
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
	vga_update_cursor();
}

position vga_get_pos() {
	position pos;
	pos.row = vga_row;
	pos.col = vga_column;
	return pos;
}

void vga_set_pos(position pos) {
	vga_row = pos.row;
	vga_column = pos.col;
	vga_update_cursor();
}