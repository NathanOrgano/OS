#include"idt.h"
#include "shell.h"

//fonction d'entrée
void main(){
    init_idt(); //initialisation de l'idt avec toutes les interruptions programmées
    __asm__ volatile("sti"); //réactivation via l'asm inline des interruptions
    writing_perm = 0; //on ne donne pas la permission d'écrire à l'écran
    welcome(); //affichage du message d'accueil
    writing_perm = 1; //on donne la permission d'écrire
    shell(); //on saute dans le shell
}