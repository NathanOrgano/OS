#include "screen.h"
#include "io.h"

//Table de correspondance scancode --> ASCII
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

//irq1 : clavier
char key_buffer[256]; //partie mémoire réservée qui contiendra les touches entrées par l'utilisateur lors de la saisie d'une commande
int buffer_index = 0; //index du buffer
volatile int line_ready = 0; //flag passant à 1 lorsque la touche entrer est pressée (volatile car la variable peut changer dans l'interruption)
int writing_perm = 0; //définit la permission d'afficher à l'écran les caractères saisis par l'utilisateur

void isr_keyboard_handler(){
    uint8_t scancode = inb(0x60); //lecture du port clavier (0x60)

    if(!(scancode & 0x80)){ //si la touche est pressée (pas relachée)

        char lettre = kbd_azerty[scancode]; //on cherche la correspondance du scancode en ascii
        
        if(lettre == '\n'){ //saut de ligne (touche entrer)
            key_buffer[buffer_index] = '\0';
            line_ready = 1; //ligne prête
        }

        else if(lettre == '\b' && buffer_index > 0){ //touche supprimer
            buffer_index--;
            key_buffer[buffer_index] = 0x00; //suppression du dernier caractère dans le buffer
            clear_char();
        }

        else if (lettre >=32 && buffer_index < 255) //si la lettre est un caractère ascii et si le buffer clavier n'est pas rempli
        {
            if (writing_perm != 0){ //si on a la permission d'écrire
                key_buffer[buffer_index++] = lettre; //déplacement de la lettre dans le key buffer

                char str[2] = {lettre, '\0'};
                print_string(str, WHITE_ON_BLACK); //affichage du caractère à l'écran
            }
        }
        
    }

    outb(0x20, 0x20); // fin de l'interrutpion (EOI) sur le maître
}

void reset_buffer(){
    buffer_index = 0; //remise à 0 du buffer index
    line_ready = 0;
    key_buffer[0] = '\0'; //écriture du null caracter au début du buffer (pas d'écrasement du buffer, mais pas de lecture après le null caracter)
}

//fonction d'entrée dans la lecture de ligne (<=> input)
void read_line(){
    reset_buffer();
    while(!line_ready){
        __asm__ volatile("hlt");
    }
}