#include <stdint.h>
#include "../vga/vga.h"
#include "../idt/idt.h"
#include "../util/util.h"
#include "./pit.h"

uint64_t ticks;
const uint32_t freq = 100;

void on_irq0(struct int_regs *regs) {
    ticks += 1;
    terminal_print("tick\n");
}

void pit_init() {// 
    ticks = 0;
    irq_install_handler(0, &on_irq0);

    uint32_t divisor = 1193180 / freq;

    out_port_b(0x43, 0x36);
    out_port_b(0x40, (uint8_t)(divisor & 0xFF));
    out_port_b(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}
