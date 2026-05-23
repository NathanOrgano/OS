[BITS 16]
[ORG 0x7C00]

init:
    cli 
    xor ax, ax
    mov es, ax
    mov ds, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

chargement_kernel:
    mov ax, 0x1000
    mov es, ax
    xor bx, bx

    mov ah, 0x02
    mov al, 0x0A
    mov ch, 0x00
    mov cl, 0x02
    mov dh, 0x00

    int 0x13

mode_protégé:
    cli
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 0x01
    mov cr0, eax

    jmp CODE_SEG:init_32bits

[BITS 32]
init_32bits:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov fs, ax

    mov esp, 0X9000

    jmp 0x10000

gdt_start:
    .gdt_null:
        dd 0x00
        dd 0x00
    
    .gdt_code:
        dw 0xFFFF
        dw 0X0000
        db 0x00
        db 10011010b
        db 11001111b
        db 0x00

    .gdt_data:
        dw 0xFFFF
        dw 0X0000
        db 0x00
        db 10010010b
        db 11001111b
        db 0x00
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_start.gdt_code - gdt_start
DATA_SEG equ gdt_start.gdt_data - gdt_start

times 510 - ($-$$) db 0
dw 0xAA55