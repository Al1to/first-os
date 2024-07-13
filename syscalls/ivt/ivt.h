#include "../../stdafx.h"

#ifndef IVT_H
#define IVT_H

uint32_t syscall_search(uint32_t num);

uint32_t sc_open(uint32_t path, uint32_t flags);
uint32_t sc_close(uint32_t fd);
uint32_t sc_read(uint32_t fd, uint32_t buffer, uint32_t count);
uint32_t sc_write(uint32_t fd, uint32_t buffer, uint32_t count);
uint32_t sc_lseek(uint32_t fd, uint32_t offset, uint32_t from); // from - это сомнительное название, в этом аргументе должно храниться то, откуда идет смещение (offset) 
uint32_t sc_stat(uint32_t path, uint32_t stat_struct_address); 

uint32_t sc_fork(); 
uint32_t sc_execve(uint32_t elf_file_name, uint32_t args_array, uint32_t envp); 
uint32_t sc_wait(uint32_t status_var_address); 
uint32_t sc_kill(uint32_t pid, uint32_t signal);
uint32_t sc_getppid(); 
uint32_t sc_getpid(); 

#endif