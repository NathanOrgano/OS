#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

void clear_screen();

void print_char(char character, uint8_t color);

void print_string(char* string, uint8_t color);

void clear_char();

#endif