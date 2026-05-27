#include "timer.h"
#include "io.h"

//irq0 : timer
volatile uint32_t system_ticks = 0;

void isr_timer_handler(){
    system_ticks ++;
    outb(0x20, 0x20);

    //tps en s = system_ticks / fréquence = system_ticks / 1000
}
//récupère le nombre de ticks depuis l'allumage du processeur
uint32_t get_ticks(){
    return system_ticks;
}

//fonction d'attente processeur
void sleep(uint32_t duration){
    uint32_t target_ticks = get_ticks() + duration;

    while(get_ticks() < target_ticks){
        __asm__ volatile ("hlt");
    }
}