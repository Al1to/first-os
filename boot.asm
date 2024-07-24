; | по сути просто 1, говорит о том, что нужно выравнивание по странице 
; ↓ если написать 0, то оно не нужно
MBOOT_PAGE_ALIGN equ 1 << 0  

; | по сути просто 2 (10), говорит о том, 
; | что загрузочный сектор должен предоставить информацию о физической памяти
; ↓ если написать сдвиг 0, то инфа не предоставляется
MBOOT_MEM_INFO   equ 1 << 1  

; | говорит о том, используется графика, или нет (vga не в счет)
; ↓ 0 - нет, 1 - да
MBOOT_USE_GFX    equ 0

; ↓ константы и флаги для мультибута (в нашем случае GRUB)
MBOOT_MAGIC      equ 0x1BADB002
MBOOT_FLAGS		 equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO | MBOOT_USE_GFX
MBOOT_CHECKSUM   equ -(MBOOT_MAGIC + MBOOT_FLAGS)

section .multiboot
align 4
    ; ↓ самые первые 12 байт бутлоадера, по ним мультибут и найдет его
	dd MBOOT_MAGIC
	dd MBOOT_FLAGS
	dd MBOOT_CHECKSUM

	; ↓ 20 байт нулей для соблюдения спецификации
	dd 0, 0, 0, 0, 0   

	dd 0    ; ← хз   надо
	dd 800  ; ← хз   читать
	dd 600  ; ← хз   доки
	dd 32   ; ← хз   мультибута

section .bss
align 16
stack_bottom:
	resb 16384 * 8   ; 16кб стек
stack_top:

; Preallocate pages used for paging. Don't hard-code addresses and assume they
; are available, as the bootloader might have loaded its multiboot structures or
; modules there. This lets the bootloader know it must avoid the addresses.
; section .bss
; align 4096
; boot_page_directory:
; 	resb 4096
; boot_page_table1:
; 	resb 4096
; Further page tables may be required if the kernel grows beyond 3 MiB.

section .boot
global _start
_start:              ; ← сюда мультибут передаст управление, если секция multiboot верно заполнена

;	; Physical address of boot_page_table1.
; 	mov edi, boot_page_table1
; 	; First address to map is address 0.
; 	mov esi, 0
; 	; Map 1023 pages. The 1024th will be the VGA text buffer.
; 	mov ecx, 1023

; 	; Loop for mapping pages.
; map_loop:
; 	; Only map the kernel.
; 	cmp esi, _kernel_start
; 	jl map_kernel
; 	cmp esi, _kernel_end
; 	jge map_vga

; 	; Map physical address as "present, writable". Note that this maps
; 	; .text and .rodata as writable. Mind security and map them as non-writable.
; 	mov edx, esi
; 	or edx, 0x003
; 	mov [edi], edx

; map_kernel:
; 	; Size of page is 4096 bytes.
; 	add esi, 4096
; 	; Size of entries in boot_page_table1 is 4 bytes.
; 	add edi, 4
; 	; Loop to the next entry if we haven't finished.
; 	loop map_loop

; map_vga:
; 	; Map VGA video memory to 0xC03FF000 as "present, writable".
; 	mov eax, 0x000B8000  ; VGA video memory address
; 	or eax, 0x003         ; Set present and writable flags
; 	mov [boot_page_table1 + 1023*4], eax

; 	; The page table is used at both page directory entry 0 (virtually from 0x0
; 	; to 0x3FFFFF) (thus identity mapping the kernel) and page directory entry
; 	; 768 (virtually from 0xC0000000 to 0xC03FFFFF) (thus mapping it in the
; 	; higher half). The kernel is identity mapped because enabling paging does
; 	; not change the next instruction, which continues to be physical. The CPU
; 	; would instead page fault if there was no identity mapping.

; 	; Map the page table to both virtual addresses 0x00000000 and 0xC0000000.
; 	mov eax, boot_page_table1
; 	or eax, 0x003      ; Set present and writable flags
; 	mov [boot_page_directory], eax
; 	mov eax, boot_page_table1
; 	or eax, 0x003      ; Set present and writable flags
; 	mov [boot_page_directory + 768*4], eax


	; Set cr3 to the address of the boot_page_directory.
	mov ecx, (boot_page_directory - 0xC0000000)
	mov cr3, ecx

	; ↓ 
	mov ecx, cr4
	or ecx, 0x10
	mov cr4, ecx

	; ↓ 
	mov ecx, cr0
	or ecx, 0x80000000
	mov cr0, ecx

	; Unmap the identity mapping as it is now unnecessary.
	; mov [boot_page_directory], 0

	; Reload cr3 to force a TLB flush so the changes take effect.
	; mov ecx, cr3
	; mov cr3, ecx 

	jmp higher_half

section .text
higher_half:
	mov esp, stack_top
	push ebx
	push eax
	xor ebp, ebp

	extern kernel_main    ; из kernel.c
	call kernel_main

halt:
	hlt
	jmp halt

section .data
align 4096
global boot_page_directory
boot_page_directory:
	dd 10000011b
	times 768-1 dd 0
	
	dd (0 << 22) | 10000011b
	dd (1 << 22) | 10000011b
	dd (2 << 22) | 10000011b
	dd (3 << 22) | 10000011b
	times 256-4 dd 0