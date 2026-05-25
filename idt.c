#include "idt.h"
#include "screen.h"

idt_entry_t idt[256];
idt_ptr_t idt_ptr;

// À mettre dans idt.c (ou dans un fichier keyboard.c si tu veux séparer plus tard)
unsigned char kbd_azerty[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', ')', '=', '\b', /* Backspace */
  '\t', 'a', 'z', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '^', '$', '\n', /* Enter */
    0, /* Control */
  'q', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', '%', '*', 
    0, /* Left Shift */
  '*', 'w', 'x', 'c', 'v', 'b', 'n', ',', ';', ':', '!', 0, /* Right Shift */
  '*', 0, /* Alt */
  ' ', /* Space bar */
    0, /* Caps lock */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* F1 à F10 */
    0, /* Num lock */
    0, /* Scroll lock */
    0, /* Home key */
    0, /* Up Arrow */
    0, /* Page Up */
  '-',
    0, /* Left Arrow */
    0,
    0, /* Right Arrow */
  '+',
    0, /* End key */
    0, /* Down Arrow */
    0, /* Page Down */
    0, /* Insert Key */
    0, /* Delete Key */
    0, 0, 0,
    0, /* F11 Key */
    0, /* F12 Key */
    0, /* Tout le reste est à 0 */
};

void set_idt_gate(uint8_t n, uint32_t handler_adress){
    idt[n].low_offset = (uint16_t)(handler_adress & 0xFFFF);
    idt[n].sel = 0x08;
    idt[n].always0 = 0x00;
    idt[n].flags = 0x8E;
    idt[n].high_offset = (uint16_t)((handler_adress >> 16) & 0xFFFF);
}

void remap_pic() {
    // ========================================================================
    // 1. ICW1 : Initialisation des puces
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
    // Pour le Master, on veut 0xF9 (soit 11111001 en binaire) :
    // - Bit 0 (IRQ 0 - Horloge)       : 1 -> Bloqué (évite l'interruption fantôme 0x20)
    // - Bit 1 (IRQ 1 - Clavier)       : 0 -> ACTIVÉ !
    // - Bit 2 (IRQ 2 - Liaison Slave) : 0 -> ACTIVÉ (Obligatoire pour que le Slave fonctionne)
    // - Bits 3 à 7 (Autres IRQ)       : 1 -> Bloqués
    outb(0x21, 0xF9); 
    
    // Pour le Slave, on bloque absolument tout pour l'instant (0xFF = 11111111)
    outb(0xA1, 0xFF); 
}

void init_idt(){
    idt_ptr.limit = (sizeof(idt_entry_t) * 256) - 1;
    idt_ptr.base = (uint32_t)&idt;

    for (int i = 0; i < 256; i++){
        set_idt_gate(i, 0);
    }

    remap_pic();

    set_idt_gate(0x00, (uint32_t)isr0);
    set_idt_gate(0x21, (uint32_t)isr1);

    idt_load((uint32_t)&idt_ptr);
}

void isr_division_by_zero(){
    char* msg_erreur = "Erreur : Division par zero";

    clear_screen();
    print_string(msg_erreur, RED_ON_BLACK);
    while(1);
}

char key_buffer[256];
int buffer_idx = 0;
volatile int line_ready = 0;
int writing_perm = 0;

void isr_keyboard_handler(){
    uint8_t scancode = inb(0x60);

    if(!(scancode & 0x80)){

        char lettre = kbd_azerty[scancode];
        
        if(lettre == '\n'){
            key_buffer[buffer_idx] = '\0';
            line_ready = 1;
        }

        else if(lettre == '\b' && buffer_idx > 0){
            buffer_idx--;
            key_buffer[buffer_idx] = 0x00;
            clear_char();
        }

        else if (lettre >=32 && buffer_idx < 255)
        {
            if (writing_perm != 0){
                key_buffer[buffer_idx++] = lettre;

                char str[2] = {lettre, '\0'};
                print_string(str, WHITE_ON_BLACK);
            }
        }
        
    }

    outb(0x20, 0x20);
}

void reset_buffer(){
    buffer_idx = 0;
    line_ready = 0;
    key_buffer[0] = '\0';
}