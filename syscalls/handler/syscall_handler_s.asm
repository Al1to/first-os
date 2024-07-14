extern syscall_handler
global syscall_common_stub
syscall_common_stub:
    cli
    push long 0
    push long 0x80

    pusha
    mov eax, ds
    push eax
    mov eax, cr2
    push eax

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
    call syscall_handler

    add esp, 8
    pop ebx
    mov ds, bx
    mov es, bx
    mov fs, bx 
    mov gs, bx

    popa
    add esp, 8
    sti
    iret