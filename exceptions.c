#include "exceptions.h"
#include "screen.h"
#include "timer.h"
#include "io.h"

//interruption 0 : Division par zero
void isr_division_by_zero(){
    clear_screen();
    print_string("Erreur : Division par zero, votre ordinateur va redemarrer.", RED_ON_BLACK);
    __asm__ volatile ("sti");
    sleep(3000);
    outb(0x64, 0xFE);
}