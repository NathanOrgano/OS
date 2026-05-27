[BITS 32]

global isr0 ;fonction utilisable par les autres fichiers
global isr1 ;fonction utilisable par les autres fichiers
extern isr_division_by_zero ;fonction de gestion de l'interruption en c (idt.c)
extern isr_keyboard_handler ;fonction de gestion de l'interruption en c (idt.c)

isr0:
    cli ;désactivation de toutes les interruptions
    pushad ;empilement de tous les registres 32 bits
    call isr_division_by_zero ;appel de la fonction de gestion de l'interruption (idt.c)
    popad ;désempilement des registres 32 bits
    iretd ; retour avec réactivation des interruptions (32 bits)

isr1:
    cli
    pushad
    call isr_keyboard_handler ;idt.c
    popad
    iretd
