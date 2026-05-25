#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

//io.asm
extern void outb(uint16_t port, uint8_t data);
extern uint8_t inb(uint16_t port);

void clear_screen();

void print_char(char character, uint8_t color);

void print_string(char *string, uint8_t color);

void clear_char();

#endif