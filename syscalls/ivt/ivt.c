#include "./ivt.h"

uint32_t sc_open(uint32_t path, uint32_t flags) {
    vga_print("open syscall\n");
}

uint32_t sc_close(uint32_t fd) {
    vga_print("close syscall\n");
}

uint32_t sc_read(uint32_t fd, uint32_t buffer, uint32_t count) {
    vga_print("read syscall\n");
}

uint32_t sc_write(uint32_t fd, uint32_t buffer, uint32_t count) {
    vga_print("write syscall\n");
}

uint32_t sc_lseek(uint32_t fd, uint32_t offset, uint32_t from) {
    vga_print("lseek syscall\n");
}

uint32_t sc_stat(uint32_t path, uint32_t stat_struct_address) {
    vga_print("stat syscall\n");
} 


uint32_t sc_fork() {
    vga_print("fork syscall\n");
} 

uint32_t sc_execve(uint32_t elf_file_name, uint32_t args_array, uint32_t envp) {
    vga_print("execve syscall\n");
} 

uint32_t sc_wait(uint32_t status_var_address) {
    vga_print("wait syscall\n");
} 

uint32_t sc_kill(uint32_t pid, uint32_t signal) {
    vga_print("kill syscall\n");
}

uint32_t sc_getppid() {
    vga_print("getppid syscall\n");
} 

uint32_t sc_getpid() {
    vga_print("getpid syscall\n");
} 
