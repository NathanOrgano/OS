[BITS 16]
[ORG 0x7C00]

init:
    cli ;on coupe les interruptions
    xor ax, ax ;on met ax à 0
    mov es, ax
    mov ds, ax
    mov ss, ax
    mov sp, 0x7C00 ;on place le sommet de la pile à 0x7C00
    sti ;on réactive les interruptions

chargement_kernel:
    mov ax, 0x1000 ;on déclare le chargement de notre kernel au segment 0x1000
    mov es, ax
    xor bx, bx ;et à l'offset 0x0000 (0x1000:0x0000 = 0x10000)

    mov ah, 0x02 ;lecture de disque
    mov al, 0x0A ;lecture sur 10 secteurs
    mov ch, 0x00 ;sur cylindre 0
    mov cl, 0x02 ;début de lecture au secteur 2
    mov dh, 0x00 ;tête de lecture 0

    int 0x13

mode_protégé:
    cli ;on coupe les interruptions
    lgdt [gdt_descriptor] ;chargement de la global descriptor table

    mov eax, cr0 ;on place le contenu de cr0 dans eax
    or eax, 0x01 ;ou logique sur le bit 0 (PE) du CR0 qui autorise le mode protégé
    mov cr0, eax ;passage en mode protégé

    jmp CODE_SEG:init_32bits ;far jump 32 bits en ustilisant le segment de code pour l'entrée dans le kernel C

[BITS 32]
init_32bits:
    mov ax, DATA_SEG ;les segments de données utilisent le DATA_SEG de la GDT
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov fs, ax

    mov esp, 0X9000 ;on déplace le sommet de la pile à 0x90000 (en dessous du kernel)

    jmp 0x10000 ;on saute dans le kernel_entry.asm

gdt_start: ;début de la GDT (flat model sur 4 GO)
    .gdt_null: ;null descriptor (64 bits)
        dd 0x00
        dd 0x00
    
    .gdt_code: ;code descriptor
        dw 0xFFFF ;limite du segment (4 Go) 0-15
        dw 0X0000 ;base du segment partie basse (0x0000) 16-31
        db 0x00 ;base du segment partie moyenne(0x00) 32-39
        db 10011010b ;octet d'accès
        ;bit 7 (à gauche, poids fort) : 1 = valide et présent en mémoire (utilisable)
        ;bits 6-5 : ring 00 (mode kernel, full accès)
        ;bit 4 descriptor type : 1 = segment de code/donnée
        ;bit 3 executable : 1 = code executable 0 = données
        ;bit 2 : 1 = le code ne peut être executé que par un programme du même ring (ring 00)
        ;bit 1 : 1 = segment lisible
        ; bit 0 accessed : laissé à 0, le processeur le passe à 1 quand il exécute une intruction de ce segment 
        db 11001111b ;flags
        ;quartet de poids fort (4 bits à gauche) :
        ;bit 7 : 1 = multiplie la limite par 4096, les 4 Go sont adressables sinon c'est 1 Mo
        ;bit 6 : 1 = segment de code 32 bits (utilisation des registres extended)
        ;bit 5 (long mode) : 1 = 64 bits, on laisse à 0 ici car on reste en 32 bits
        ;bit 4 : bit libre (on peut y strocker un booléen dans certains cas)
        ;quarte de poids faible = fin de la limite (limite sur 20 bits)
        db 0x00 ; base du segment partie haute

    .gdt_data:
        dw 0xFFFF ;limite du segment (4 Go) 0-15
        dw 0X0000 ;base du segment partie basse (0x0000) 16-31
        db 0x00 ;base du segment partie moyenne(0x00) 32-39
        db 10010010b ;base du segment partie moyenne(0x00) 32-39
        ;bit 7 (à gauche, poids fort) : 1 = valide et présent en mémoire (utilisable)
        ;bits 6-5 : ring 00 (mode kernel, full accès)
        ;bit 4 descriptor type : 1 = segment de code/donnée
        ;bit 3 executable : 1 = code executable 0 = données
        ;bit 2 : 1 = le code ne peut être executé que par un programme du même ring (ring 00)
        ;bit 1 : 1 = segment lisible
        ; bit 0 accessed : laissé à 0, le processeur le passe à 1 quand il exécute une intruction de ce segment 
        db 11001111b ;flags
        ;quartet de poids fort (4 bits à gauche) :
        ;bit 7 : 1 = multiplie la limite par 4096, les 4 Go sont adressables sinon c'est 1 Mo
        ;bit 6 : 1 = segment de code 32 bits (utilisation des registres extended)
        ;bit 5 (long mode) : 1 = 64 bits, on laisse à 0 ici car on reste en 32 bits
        ;bit 4 : bit libre (on peut y strocker un booléen dans certains cas)
        ;quarte de poids faible = fin de la limite (limite sur 20 bits)
        db 0x00 ; base du segment partie haute
gdt_end: ;fin de la segmentation de la GDT

gdt_descriptor:
    dw gdt_end - gdt_start - 1 ;limite de la GDT - 1
    dd gdt_start ;base de la GDT

CODE_SEG equ gdt_start.gdt_code - gdt_start ;0x08
DATA_SEG equ gdt_start.gdt_data - gdt_start ;0x10

times 510 - ($-$$) db 0 
dw 0xAA55 ;signature bootloader