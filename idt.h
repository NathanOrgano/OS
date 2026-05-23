#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#define WHITE_ON_BLACK 0x0F
#define GREEN_ON_BLACK 0x0A
#define RED_ON_BLACK   0x0C

//kernel_entry.asm
extern void idt_load(uint32_t idt_ptr_adress);

//interrupt.asm
extern void isr0(void);
extern void isr1(void);

//io.asm
extern void outb(uint16_t port, uint8_t data);
extern uint8_t inb(uint16_t port);

struct idt_entry_struct
{
    uint16_t low_offset;
    uint16_t sel;
    uint8_t always0;
    uint8_t flags;
    uint16_t high_offset;
} __attribute__((packed));

typedef struct idt_entry_struct idt_entry_t;

struct idt_ptr_struct
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

typedef struct idt_ptr_struct idt_ptr_t;

void init_idt();
void set_idt_gate(uint8_t n, uint32_t handler_adress);

#endif