#include "./util.h"

// void reverse_srting(char* str, int len) {
//     int i = 0;
//     int j = len - 1;
//     while (i < j) {
//         char temp = str[i];
//         str[i] = str[j];
//         str[j] = temp;
//         i++; j++;
//     }
// }

void out_port_b(uint16_t port, uint8_t value) {
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

void out_port_w(uint16_t port, uint16_t value) {
	asm volatile("outw %%ax, %%dx": :"d" (port), "a" (value));
}

void out_port_l(uint16_t port, uint32_t value) {
	asm volatile("outl %%eax, %%dx": :"d" (port), "a" (value));
}

uint8_t in_port_b(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0": "=a"(ret):"dN"(port));
    return ret;
}

uint16_t in_port_w(uint16_t port) {
	uint16_t ret;
	asm volatile("inw %%dx, %%ax":"=a"(ret):"d"(port));
	return ret;
}

uint32_t in_port_l(uint16_t port) {
	uint32_t ret;
	asm volatile("inl %%dx, %%eax":"=a"(ret):"d"(port));
	return ret;
}

void syscall_dbg() {
  asm volatile ("mov $0x4, %%eax" : : : "eax"); // 0x4 просто для теста, пофиг какой сисколл
  asm volatile ("int $0x80" : : : );
}
