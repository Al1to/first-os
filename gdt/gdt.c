#include "gdt.h"

extern void gdt_flush(uint32_t); 
extern void tss_flush(uint32_t); 

struct gdt_enrty gdt_entries[6];
struct gdt_ptr gdt_ptr;
struct tss_entry tss_entry;

void gdt_init() {
    gdt_ptr.limit = (sizeof(struct gdt_enrty) * 6) - 1;
    gdt_ptr.base = (uint32_t)&gdt_entries;

    set_gdt_gate(0, 0, 0, 0, 0);
    set_gdt_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    set_gdt_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    set_gdt_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    set_gdt_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    write_tss(5, 0x10, 0x0);

    gdt_flush((uint32_t)&gdt_ptr);
    tss_flush((uint32_t)&gdt_ptr);
}

void set_gdt_gate(uint32_t num, uint32_t base, uint32_t limit,
                uint8_t access, uint8_t gran) {
    gdt_entries[num].base_low = (base & 0xFFFF);
    gdt_entries[num].base_mid = (base >> 16) & 0xFF;
    gdt_entries[num].base_high = (base >> 24) & 0xFF;

    gdt_entries[num].limit = (limit & 0xFFFF);
    gdt_entries[num].limit_flags = (limit >> 16) & 0x0F;
    gdt_entries[num].limit_flags |= (gran & 0xF0);
    
    gdt_entries[num].access = access;
}

void write_tss(uint32_t num, uint16_t ss0, uint32_t esp0) {
    uint32_t base = (uint32_t) &tss_entry;
    uint32_t limit = base + sizeof(tss_entry);

    set_gdt_gate(num, base, limit, 0xE9, 0x00);
    memset(&tss_entry, 0, sizeof(tss_entry));

    tss_entry.ss0 = ss0;
    tss_entry.esp0 = esp0;

    tss_entry.cs = 0x08 | 0x3;
    tss_entry.ss = tss_entry.ds = 
    tss_entry.es = tss_entry.fs = 
    tss_entry.gs = 0x10 | 0x3;
}