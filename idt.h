#ifndef IDT_H //vérifie si le fichier est déjà inclus par le compilateur
#define IDT_H

#include <stdint.h> //inclusion d'une bibiloithèque pour gérer la taille des variables

//idt.c
void init_idt(); //initialisation de l'idt (descriptor et gates), (appellée dans la fonction main du kernel C)
uint32_t get_ticks(); //récupère le nombre de ticks depuis l'allumage du processeur à une fréquence de 1 kHz
void reset_buffer(); //réinitialisation du buffer clavier (appellée par la fonction main du kernel C)
extern volatile int line_ready; //déclare si une ligne est prête (appuie sur la touche entrer)
extern char key_buffer[256]; //buffer clavier
extern int writing_perm; //permission d'écriture

//kernel_entry.asm
extern void idt_load(uint32_t idt_ptr_adress); //Chargement du descriptor de l'idt en asm (appellée dans la fonction init_idt de idt.c)

//interrupt.asm
extern void isr0(void); //fonctions asm d'appel des interruptions (division par zero)
extern void isr1(void); //fonctions asm d'appel des interruptions (timer)
extern void isr2(void); //fonctions asm d'appel des interruptions (clavier)

//définition de la structure d'une entrée de l'idt
struct idt_entry_struct
{
    uint16_t low_offset; //base basse de l'adress de la fonction d'appel asm
    uint16_t sel; //segment mémoire utilisé (GDT)
    uint8_t always0; 
    uint8_t flags; //paramètres de l'entrée
    uint16_t high_offset; //base haute de l'adress de la fonction d'appel asm
} __attribute__((packed)); //packed = reproduction exacte de cette structure en mémoire (pas d'alignement par 4 octets des données)

typedef struct idt_entry_struct idt_entry_t; //alias

struct idt_ptr_struct //définition de la structure du descriptor /pointeur
{
    uint16_t limit; //taille de l'idt (256 entrées, 64 bits par entrée : 256*64 = 16 384 bits)
    uint32_t base; //base
} __attribute__((packed)); //packed = reproduction exacte de cette structure en mémoire (pas d'alignement par 4 octets des données)

typedef struct idt_ptr_struct idt_ptr_t; //alias

#endif