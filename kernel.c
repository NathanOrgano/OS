#include"idt.h"
#include "shell.h"
#include "keyboard.h"
#include "pmm.h"

#include <stdint.h>
#include <stddef.h>

//fonction d'entrée
void main(){
    init_idt(); //initialisation de l'idt avec toutes les interruptions programmées
    __asm__ volatile("sti"); //réactivation via l'asm inline des interruptions
    uint16_t *ext_memory_size_kb = (uint16_t*)0x0500; //On récupère la taille de la mémoire étendue en kilo octets
    uint32_t total_memory_size = (1024*1024) + ((uint32_t)(*ext_memory_size_kb)*1024); //On calcule la taille totale de la mémoire en octets
    pmm_init(0x20000, total_memory_size); //On initialise toute la mémoire (en bloquant tout). Chargement de la bitmap à l'adresse 0x20000 
    uint32_t ext_memory_size = (uint32_t)(*ext_memory_size_kb)*1024; //Calcule de la taille de la mémoire étendue en octet
    pmm_init_region(0x100000, ext_memory_size); //on libère toute la mémoire étendue
    writing_perm = 0; //on ne donne pas la permission d'écrire à l'écran
    welcome(); //affichage du message d'accueil
    writing_perm = 1; //on donne la permission d'écrire
    shell(); //on saute dans le shell
}