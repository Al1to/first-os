#include "./ivt.h"

uint32_t syscall_search(uint32_t num) {
    uint32_t addr = 0;
    if      (num == 0x001) addr = sc_wait;
    else if (num == 0x002) addr = sc_fork;
    else if (num == 0x003) addr = sc_read;
    else if (num == 0x004) addr = sc_write;
    else if (num == 0x005) addr = sc_open;
    else if (num == 0x006) addr = sc_close;
    else if (num == 0x011) addr = sc_execve;
    else if (num == 0x019) addr = sc_lseek;
    else if (num == 0x020) addr = sc_getpid;
    else if (num == 0x037) addr = sc_kill;
    else if (num == 0x039) addr = sc_getppid;
    else if (num == 0x195) addr = sc_stat;
    return addr;
}

uint32_t sc_open(uint32_t path, uint32_t flags) {

}

uint32_t sc_close(uint32_t fd) {

}

uint32_t sc_read(uint32_t fd, uint32_t buffer, uint32_t count) {

}

uint32_t sc_write(uint32_t fd, uint32_t buffer, uint32_t count) {
    vga_print("write syscall");
}

uint32_t sc_lseek(uint32_t fd, uint32_t offset, uint32_t from) {

}

uint32_t sc_stat(uint32_t path, uint32_t stat_struct_address) {

} 


uint32_t sc_fork() {

} 

uint32_t sc_execve(uint32_t elf_file_name, uint32_t args_array, uint32_t envp) {

} 

uint32_t sc_wait(uint32_t status_var_address) {

} 

uint32_t sc_kill(uint32_t pid, uint32_t signal) {

}

uint32_t sc_getppid() {

} 

uint32_t sc_getpid() {

} 
