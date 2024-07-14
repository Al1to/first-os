#include "../../stdafx.h"

void syscall_handler(struct int_regs* regs) {
    uint32_t num = regs->eax;
    // vga_printf("syscall %x\n", num); // из-за этой строки выходит page fault, я хз почему

    switch (num) {
        case 1:
            sc_wait(regs->ebx);
            break;
        case 2:
            sc_fork();
            break;
        case 3:
            sc_read(regs->ebx, regs->ecx, regs->edx);
            break;
        case 4:
            sc_write(regs->ebx, regs->ecx, regs->edx);
            break;
        case 5:
            sc_open(regs->ebx, regs->ecx);
            break;
        case 6:
            sc_close(regs->ebx);
            break;
        case 11:
            sc_execve(regs->ebx, regs->ecx, regs->edx);
            break;
        case 19:
            sc_lseek(regs->ebx, regs->ecx, regs->edx);
            break;
        case 20:
            sc_getpid();
            break;
        case 37:
            sc_kill(regs->ebx, regs->ecx);
            break;
        case 39:
            sc_getppid();
            break;
        case 195:
            sc_stat(regs->ebx, regs->ecx);
            break;
        default:
            vga_print("unknown syscall");
            regs->eax = -1;
            break;
  }
}