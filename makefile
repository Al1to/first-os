AS:=nasm -f elf32
CC:=gcc -m32

CFLAGS:=-ffreestanding -O2 -Wall -Wextra -nostdlib
LIBS:=-lgcc

OBJS:=\
boot.o \
kernel.o \
vga/vga.o \
gdt/gdt.o \
gdt/gdt_s.o \
util/util.o \
idt/idt.o \
idt/idt_s.o \
pit/pit.o \
drivers/keyboard/keyboard.o \
memory/memory.o \

all: myos.bin

.PHONEY: all clean iso run-qemu

myos.bin: $(OBJS) linker.ld
	$(CC) -T linker.ld -fno-pic -Wl,--build-id=none -o $@ $(CFLAGS) $(OBJS) $(LIBS)

%.o: %.c
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
	qemu-system-i386 -cdrom myos.iso
