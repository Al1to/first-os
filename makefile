AS:=nasm -f elf32
CC:=gcc -m32

CFLAGS:=-ffreestanding -O2 -Wall -Wextra -nostdlib
LIBS:=-lgcc

OBJS:=\
boot.o \
kernel.o \
pci/pci.o \
vga/vga.o \
gdt/gdt.o \
gdt/gdt_s.o \
util/util.o \
idt/idt.o \
idt/idt_s.o \
drivers/pit/pit.o \
drivers/keyboard/keyboard.o \
memory/memory.o \
drivers/disk/disk.o \
drivers/disk/ahci/ahci.o \
syscalls/handler/syscall_handler.o \
syscalls/handler/syscall_handler_s.o \
syscalls/ivt/ivt.o \
terminal/terminal.o \
std/math/pow.o \
std/mem/mem.o \
std/string/itoa.o \
std/string/memcpy.o \
std/string/memset.o \
std/string/strlen.o \
std/string/strcmp.o \
std/string/strcpy.o \

all: myos.bin

.PHONEY: all clean iso run-qemu

myos.bin: $(OBJS) linker.ld
	$(CC) -T linker.ld -fno-pic -Wl,--build-id=none -o $@ $(CFLAGS) $(OBJS) $(LIBS)

%.o: %.c
	# флаг -g для отладочной информации в объектниках
	$(CC) -c $< -o $@ -std=gnu99 $(CFLAGS) -fno-pic

%.o: %.asm
	$(AS) $< -o $@

clean:
	rm -rf isodir
	rm -f myos.bin myos.iso $(OBJS)

iso: myos.iso

isodir isodir/boot isodir/boot/grub:
	mkdir -p $@

isodir/boot/myos.bin: myos.bin isodir/boot
	cp $< $@

isodir/boot/grub/grub.cfg: grub.cfg isodir/boot/grub
	cp $< $@

myos.iso: isodir/boot/myos.bin isodir/boot/grub/grub.cfg
	grub-mkrescue -o $@ isodir

run-qemu: myos.iso
	# dd if=/dev/zero of=my_SATA_disk.img bs=1M count=32   # 32 мб диск
	qemu-system-i386 -m 512M -cdrom myos.iso -device ahci,id=ahci -drive file=my_SATA_disk.img,id=disk,if=none,format=raw -device ide-hd,drive=disk,bus=ahci.0

run-qemu-dbg: myos.iso
	# dd if=/dev/zero of=my_SATA_disk.img bs=1M count=32   # 32 мб диск
	qemu-system-i386 -m 512M -s -S -cdrom myos.iso -device ahci,id=ahci -drive file=my_SATA_disk.img,id=disk,if=none,format=raw -device ide-hd,drive=disk,bus=ahci.0
