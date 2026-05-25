[BITS 32]
global start
global idt_load
extern main
extern init_idt

start:
    call init_idt
    sti
    call main
    hlt 
    jmp $

idt_load:
    mov eax, [esp+4]
    lidt [eax]
    ret