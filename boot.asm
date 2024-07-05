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

section .boot
global _start
_start:              ; ← сюда мультибут передаст управление, если секция multiboot верно заполнена
	
	; | page_dir_init - это адрес таблицы страниц (внизу этого файла написана)
	; | вычитаем 0xC0000000 (физ адрес начала ядра), т.к. нужно смещение от начала ядра ? 
	; ↓ ну и кладем в cr3, это спец регистр для этого смещения к таблице страниц  
	mov ecx, (page_dir_init - 0xC0000000)
	mov cr3, ecx

	; ↓ 
	mov ecx, cr4
	or ecx, 0x10
	mov cr4, ecx

	; ↓ 
	mov ecx, cr0
	or ecx, 0x80000000
	mov cr0, ecx

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
global page_dir_init
page_dir_init:
	dd 10000011b
	times 768-1 dd 0
	
	dd (0 << 22) | 10000011b
	dd (1 << 22) | 10000011b
	dd (2 << 22) | 10000011b
	dd (3 << 22) | 10000011b
	times 256-4 dd 0