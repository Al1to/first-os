#include "../../stdafx.h"

void syscall_handler(struct int_regs* regs) {
    uint32_t num = regs->eax;
    // vga_printf("syscall %x", num); // из-за этой строки выходит page fault

    switch (num) {
        case 0x004:
            sc_write(regs->ebx, regs->ecx, regs->edx);
            break;
        default:
            vga_print("syscall err");
            regs->eax = -1;
            break;
  }
}