// io.h
#ifndef IO_H
#define IO_H

#include <stdint.h> //inclusion d'une bibiloithèque pour gérer la taille des variables

extern void outb(uint16_t port, uint8_t data); //envoi d'un mot sur un port
extern void outw(uint16_t port, uint16_t data); //envoi d'un octet sur un port
extern uint8_t inb(uint16_t port); //lecture d'un port (16 bits)

#endif