#include "idt.h" //inclusion de idt.h
#include "screen.h" //inclusion de screen.h
#include "io.h" //gestion des entrées et sorties via ports matériels

idt_entry_t idt[256]; //crée un tableau de 256 entrées qui copie la structure de idt_entry_struct
idt_ptr_t idt_ptr; //crée une variable du type idt_ptr_t (structure du pointeur/descriptor de l'idt)

void remap_pic() {
    // ========================================================================
    // 1. ICW1 : Reprogrammation des puces
    // ========================================================================
    // On envoie 0x11 pour dire aux deux PICs de s'initialiser et d'attendre
    // les 3 prochains octets de configuration (ICW2, ICW3 et ICW4).
    outb(0x20, 0x11); // PIC Maître (Master)
    outb(0xA0, 0x11); // PIC Esclave (Slave)

    // ========================================================================
    // 2. ICW2 : Déplacement des vecteurs d'interruption (Le Remappage)
    // ========================================================================
    // On décale les interruptions matérielles pour éviter le conflit avec 
    // les exceptions d'Intel (0 à 31).
    outb(0x21, 0x20); // Master gère désormais les interruptions 0x20 à 0x27 (L'IRQ 1 Clavier devient 0x21)
    outb(0xA1, 0x28); // Slave gère désormais les interruptions 0x28 à 0x2F

    // ========================================================================
    // 3. ICW3 : Chaînage entre le Maître et l'Esclave
    // ========================================================================
    // Le Slave étant branché sur l'IRQ 2 du Master, on configure leur liaison.
    outb(0x21, 0x04); // Au Master : "Tu as un esclave connecté sur ton IRQ 2" (0x04 = binaire 00000100)
    outb(0xA1, 0x02); // Au Slave : "Tu es connecté sur l'IRQ 2 de ton Maître"

    // ========================================================================
    // 4. ICW4 : Mode de fonctionnement
    // ========================================================================
    // On configure les deux puces pour fonctionner en mode architectural x86 (8086).
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    // ========================================================================
    // 5. MASQUAGE SÉCURISÉ (Pour éviter le crash sur VirtualBox)
    // ========================================================================
    // Rappel : Un bit à 0 = ACTIVÉ, un bit à 1 = MASQUÉ/BLOQUÉ.
    //
    // Pour le Master, on veut 0xF9 (soit 11111000 en binaire) :
    // - Bit 0 (IRQ 0 - Horloge)       : 0 -> ACTIVÉ !
    // - Bit 1 (IRQ 1 - Clavier)       : 0 -> ACTIVÉ !
    // - Bit 2 (IRQ 2 - Liaison Slave) : 0 -> ACTIVÉ (Obligatoire pour que le Slave fonctionne)
    // - Bits 3 à 7 (Autres IRQ)       : 1 -> Bloqués
    outb(0x21, 0b11111000); 
    
    // Pour le Slave, on bloque absolument tout pour l'instant (0xFF = 11111111)
    outb(0xA1, 0xFF); 
}

//création d'une entrée dans l'idt
void set_idt_gate(uint8_t n, uint32_t handler_adress){ //handler_adress = adresse de la fonction asm d'appel de l'interruption
    idt[n].low_offset = (uint16_t)(handler_adress & 0xFFFF); //partie basse de handler_adress
    idt[n].sel = 0x08; //attribution du segment de code au selecteur
    idt[n].always0 = 0x00;
    idt[n].flags = 0x8E; //paramètres 
    idt[n].high_offset = (uint16_t)((handler_adress >> 16) & 0xFFFF); //partie haute de handler_adress
}

void init_idt(){
    idt_ptr.limit = (sizeof(idt_entry_t) * 256) - 1; //initialisation de la limte de l'idt à 16 384 - 1
    idt_ptr.base = (uint32_t)&idt; //adresse de base du tableau de structures de l'idt (ensemble des entrées)

    for (int i = 0; i < 256; i++){
        set_idt_gate(i, 0); //initialisation des interruptions à 0 (fonction d'appel asm)
    }

    remap_pic();

    set_idt_gate(0x00, (uint32_t)isr0); //création d'une entrée dans l'idt qui gère l'interruption de division par 0
    outb(0x43, 0b00110110); //envoi de l'octet de configuration du PIT (Programmable Interval Timer)
    outb(0x40, (uint8_t)0x4A9); //envoi de la partie basse de la fréquence sur le port de commande (diviseur = 1193, fréquence = 1000 Hz)
    outb(0x40, 0x4A9 >> 8); //envoi de la partie haute de la fréquence sur le port de commande (diviseur = 1193, fréquence = 1000 Hz)
    set_idt_gate(0x20, (uint32_t)isr1); //création d'une entrée dans l'idt qui gère le timer
    set_idt_gate(0x21, (uint32_t)isr2); //création d'une entrée dans l'idt qui gère l'interruption clavier

    idt_load((uint32_t)&idt_ptr); //chargement de l'idt (via fonction asm dans kernel_entry.asm)
}