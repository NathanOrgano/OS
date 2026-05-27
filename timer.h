#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

uint32_t get_ticks(); //récupère le nombre de ticks depuis l'allumage du processeur à une fréquence de 1 kHz
void sleep(uint32_t duration); //fonction d'attent processeur

#endif