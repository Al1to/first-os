#ifndef STDAFX_H
#define STDAFX_H
 
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "./pci/pci.h"
#include "./vga/vga.h"
#include "./gdt/gdt.h"
#include "./idt/idt.h"
#include "./drivers/pit/pit.h"
#include "./util/util.h"
#include "./drivers/keyboard/keyboard.h"
#include "./multiboot.h"
// #include "./memory/memory.h"
#include "./syscalls/ivt/ivt.h"
// #include "./drivers/disk/disk.h"
// #include "./drivers/disk/ahci/ahci.h"
#include "./terminal/terminal.h"
// #include "./std/math/math.h"
// #include "./std/string/string.h"
// #include "./std/mem/mem.h"

#endif