global isr0
extern isr_division_by_zero

isr0:
    cli
    pushad
    call isr_division_by_zero
    popad
    iret
