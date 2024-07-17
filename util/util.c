#include "./util.h"

void memset(void *dest, char val, uint32_t count) {
    char *temp = (char*) dest;
    for (; count != 0; count--) *temp++ = val;
}

unsigned long pow(unsigned long number, int degree) {
    if (number == 0 || degree == 0) return 1;
    // vga_print("pow");
    return number * pow(number, degree-1);
} 

unsigned int get_digits(unsigned long number, const unsigned int base) {
    unsigned int digits = 0;
    // vga_put_char('3');
    for (number; number > 0; number /= base) ++digits;
    // vga_put_char('4');
    return digits;
}

void itoa(unsigned long number, char *str, const unsigned int base) {
    int cur = 0;

    // vga_put_char('0');

    if (base < 2) {
        str[cur] = '\0';
        return;
    }

    if (number == 0) {
        // vga_print("\nnumber = 0\n");
        str[cur++] = '0';
        str[cur] = '\0';
        return;
    }

    // vga_put_char('1');

    unsigned long num;
    if (base == 10 && number < 0) {
        str[cur++] = '-';
        num = -number;
    } else if (base == 16) {
        // str[cur++] = '0';
        // str[cur++] = 'x';
        num = number;
    } else { 
        num = number; 
    }

    // vga_put_char('2');

    int digits = get_digits(num, base);

    // vga_put_char('5');

    for (unsigned long digit; digits > 0; --digits) { 
        digit = num / pow(base, digits - 1);
        if (digit < 10) {                  
            str[cur++] = digit + '0';
        } else {
            str[cur++] = digit + 'A' - 10;
        }
        num %= pow(base, digits - 1);
    }

    // vga_put_char('6');

    str[cur] = '\0';
}

void reverse_srting(char* str, int len) {
    int i = 0;
    int j = len - 1;

    while (i < j) {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j++;
    }
}

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
