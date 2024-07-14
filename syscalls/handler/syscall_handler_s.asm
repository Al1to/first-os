extern syscall_handler
global syscall_common_stub
syscall_common_stub:
    cli

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

    add esp, 8 ; вот тут, или дальше, general protection fault 
    pop ebx
    mov ds, bx
    mov es, bx
    mov fs, bx 
    mov gs, bx

    popa
    add esp, 8
    sti
    iret