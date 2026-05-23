#include "idt.h"
#include "screen.h"

idt_entry_t idt[256];
idt_ptr_t idt_ptr;

void set_idt_gate(uint8_t n, uint32_t handler_adress){
    idt[n].low_offset = (uint16_t)(handler_adress & 0xFFFF);
    idt[n].sel = 0x08;
    idt[n].always0 = 0x00;
    idt[n].flags = 0x8E;
    idt[n].high_offset = (uint16_t)((handler_adress >> 16) & 0xFFFF);
}

void init_idt(){
    idt_ptr.limit = (sizeof(idt_entry_t) * 256) - 1;
    idt_ptr.base = (uint32_t)&idt;

    for (int i = 0; i < 256; i++){
        set_idt_gate(i, 0);
    }

    set_idt_gate(0, (uint32_t)isr0);

    idt_load((uint32_t)&idt_ptr);
}

void isr_division_by_zero(){
    char* msg_erreur = "Erreur : Division par zero ! Systeme fige.";

    clear_screen();
    print_string(msg_erreur, RED_ON_BLACK);
    while(1);
}