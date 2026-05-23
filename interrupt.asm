[BITS 32]

global isr0
global isr1
extern isr_division_by_zero
extern isr_keyboard_handler

isr0:
    cli
    pushad
    call isr_division_by_zero
    popad
    iret

isr1:
    cli
    pushad
    call isr_keyboard_handler
    popad
    iret
