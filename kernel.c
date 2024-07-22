#include "stdafx.h"

/*/
 * kernel_main - это главная функция ядра ос, ее вызывает бутлоадер (boot.asm)
 * она принимает 2 аргумента:
 * magic - это просто число, которое говорит о том, успешно ли прошел этап бутлоадера (я хз, вроде так)
 * boot_info - это предоставленная нам вся необходимая инфа (структура в multiboot.h)
/*/

void kernel_main(uint32_t magic, struct multiboot_info* boot_info) {

	// | это 3 базовых инита + для cli графики (vga) и ввода (keyboard), порядок именно такой
	// ↓ их описания в соответствующих файлах
	vga_init();
	gdt_init();
	idt_init();
	pit_init();
	keyboard_init();

	// | mods_addr - это адрес старта какого-то там модуля (сам хз пока что)
	// | +4 т.к. там первые 4 байта хранят че-то ненужное нам
	// | в итоге (boot_info->mods_addr + 4) это какой-то адрес и мы его приводим к указателю на uint32_t, а потом распаковываем
	// ↓ P.S. если приводить тупо к uint32_t, то мы получим переменную типа uint32_t, которая хранит в себе какой-то адрес
	uint32_t mod1 = *(uint32_t*)(boot_info->mods_addr + 4);
	
	// | по сути из 0x1000 делает 0x1000, а из 0x1234 делает 0x2000
	// ↓ из 0x2345 делает 0x3000, думаю ты понял (выравнивает по 4кб, (0x1000 - это 4кб))
	uint32_t phys_alloc_start = (mod1 + 0xFFF) & ~0xFFF;
	
	// | что делает этот инит, описано в соответствующих файлах
	// | тут, mem_upper - это кол-во доступной физической памяти выше 1мб, выраженное в кб, поэтому *1024
	// ↓ P.S. в этом 1мб хранятся всякие BIOS и т.д., т.к. там 16-бит режим, где адресация максимум 1мб (здесь, в ядре, уже 32-бит режим)
	memory_init(boot_info->mem_upper * 1024, phys_alloc_start);

	// | что делает этот инит, описано в соответствующих файлах
	// ↓ 0x1000 это 4096 (4кб), т.е. размер страницы, которою выделит kmalloc для ядра
	kmalloc_init(0x1000);

	// syscall_dbg();

	// ahci_init();

	pci_scan();

	vga_print("\nEl psy congroo.\n");

	terminal_init("$ ");
}
