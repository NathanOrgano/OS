[BITS 32]

;fonctions utilisable par les autres fichiers
global isr0 
global isr1
global isr2 

;fonctions de gestion de l'interruption en c (idt.c)
extern isr_division_by_zero 
extern isr_timer_handler
extern isr_keyboard_handler

isr0:
    cli ;désactivation de toutes les interruptions
    pushad ;empilement de tous les registres 32 bits
    call isr_division_by_zero ;appel de la fonction de gestion de l'interruption (exceptions.c)
    popad ;désempilement des registres 32 bits
    iretd ; retour avec réactivation des interruptions (32 bits)

isr1:
    cli
    pushad
    call isr_timer_handler ;timer.c
    popad
    iretd

isr2:
    cli
    pushad
    call isr_keyboard_handler ;idt.c
    popad
    iretd
