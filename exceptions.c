#include "exceptions.h"
#include "screen.h"

//interruption 0 : Division par zero
void isr_division_by_zero(){
    char* msg_erreur = "Erreur : Division par zero";

    clear_screen();
    print_string(msg_erreur, RED_ON_BLACK);
    while(1); //bloquage processeur
}