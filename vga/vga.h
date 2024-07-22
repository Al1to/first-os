#include "../stdafx.h"

#ifndef VGA_H
#define VGA_H

/*/
 *   vga - это просто массив в оперативке, который железобетонно начинается со смещения 0xB8000
 *   его размеры 80*25*2 = 4000 байт (ширина в символах на высоту в символах на кол-во байтов в символе)
 *  
 *   записывать в него можно символы по 2 байта (первый байт - это ascii код символа, а второй - это цвет самого символа и фона)
 *   касаемо второго байта, там по 4 бита на цвет, т.е. 16 разных цветов и для символа, и для фона
 *   BLACK     = 0000 (0),   BLUE          = 0001 (1),   GREEN       = 0010 (2),   CYAN       = 0011 (3),
 *   RED       = 0100 (4),   MAGENTA       = 0101 (5),   BROWN       = 0110 (6),   LIGHT_GREY = 0111 (7),
 *   DARK_GREY = 1000 (8),   LIGHT_BLUE    = 1001 (9),   LIGHT_GREEN = 1010 (10),  LIGHT_CYAN = 1011 (11),
 *   LIGHT_RED = 1100 (12),  LIGHT_MAGENTA = 1101 (13),  LIGHT_BROWN = 1110 (14),  WHITE      = 1111 (15)
 * 
 *   P.S. смещение от ядра, которое начинается с 0xC0000000, так что сам vga с 0xC00B8000 (мб где-то неверно выразился, но числа верные)
/*/

// | заполняет весь массив vga символами пробела с черным фоном
// ↓ а еще ставит позицию курсора на (0; 0)
void vga_init(void);

// ↓ просто обновляет положение курсора, эту функцию надо вызывать как только что-то напечатали
void vga_update_cursor();

// ↓ перерисовывает весь экран без верхней строки и создает внизу новую, заполненную пробелами
void vga_scroll_up(void);

// ↓ переносит курсор, если это была последняя строка, то вызывает vga_scroll_up
void vga_new_line(void);

// ↓ все функции - это различные методы вывода на экран символов
void vga_put_char(char ch);
void vga_put_charс(uint8_t color, char ch);
void vga_print(const char* str);
void vga_printc(uint8_t color, const char* str);
void vga_printf(const char* str, int d);

// | принимает цвет символа и цвет фона
// ↓ возвращает байт цвета для vga
static inline uint8_t vga_entry_color(uint8_t fg, uint8_t bg) {
	return fg | bg << 4;
}

// | принимает байт символа и байт цвета
// ↓ возвращает 2 слитых байта
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}

typedef struct {
	size_t row;
	size_t col;
} position;

position vga_get_pos();

#endif