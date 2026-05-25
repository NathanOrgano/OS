[BITS 32]
global start ;la fonction start est globale
global idt_load ;la fonction idt_load est globale
extern main ;fonction main du kernel C

start:
    call main ;on saute dans la fonction main du kernel C
    hlt ;si on sort du kernel on fige le processeur
    jmp $

idt_load: ;chargement de l'interrupt descriptor table
    mov eax, [esp+4] ;on récupère le premier argument passé dans l'appel de la fonction depuis idt.c (pointeur de l'adresse du descriptor de l'idt)
    lidt [eax] ;on charge l'idt depuis cette adresse
    ret