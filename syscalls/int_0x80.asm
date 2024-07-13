global syscall_handler
syscall_handler:
    push eax
    extern syscall_search
    call syscall_search
    
    call eax  ; вроде все верно, наверное?
    pop eax

    iret
