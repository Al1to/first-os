#ifndef STDAFX_H
#define STDAFX_H
 
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "./vga/vga.h"
#include "./gdt/gdt.h"
#include "./idt/idt.h"
#include "./pit/pit.h"
#include "./util/util.h"
#include "./drivers/keyboard/keyboard.h"
#include "./multiboot.h"
#include "./memory/memory.h"
#include "./kmalloc/kmalloc.h"

#endif