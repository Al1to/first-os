#include "../stdafx.h"

#ifndef GDT_H
#define GDT_H

/*/
 *   gdt - ...
 * 
/*/

// ↓ сам дескриптор
struct gdt_enrty {
    uint16_t limit;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t limit_flags;
    uint8_t base_high;
} __attribute__((packed));

// ↓ указатель на дескриптор
struct gdt_ptr {
    uint16_t limit;
    unsigned int base;
}__attribute__((packed));

// ↓ ...
struct tss_entry {
    uint32_t prev_tss;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t ebx;
    uint32_t edx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint32_t trap;
    uint32_t iomap_base;
}__attribute__((packed));

void gdt_init();
void set_gdt_gate(uint32_t num, uint32_t base, uint32_t limit,
                uint8_t access, uint8_t gran);
void write_tss(uint32_t num, uint16_t ss0, uint32_t esp0);

#endif