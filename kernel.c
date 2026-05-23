#include "screen.h"
#include "idt.h"

#define WHITE_ON_BLACK 0x0F
#define GREEN_ON_BLACK 0x0A
#define RED_ON_BLACK   0x0C

void main(){
    init_idt();

    /*volatile int numerateur = 42;
    volatile int denominateur = 0;
    volatile int resultat = numerateur / denominateur;*/

    char* message = 
        "HH   HH   AAA   PPPPPP  EEEEEEE RRRRRR  ZZZZZ\n"
        "HH   HH  AAAAA  PP   PP EE      RR   RR    ZZ\n"
        "HHHHHHH AA   AA PPPPPP  EEEEE   RRRRRR    ZZ\n"
        "HH   HH AAAAAAA PP      EE      RR  RR   ZZ\n"
        "HH   HH AA   AA PP      EEEEEEE RR   RR ZZZZZ\n"
        " \n"
        " OOOOO   SSSSS\n"
        "OO   OO SS\n"
        "OO   OO  SSSSS\n"
        "OO   OO      SS\n"
        " OOOOO   SSSSS\n";
        
    clear_screen();
    print_string(message, GREEN_ON_BLACK);

    for(int i=0; i < 13; i++){
        print_string("\n", GREEN_ON_BLACK);
    }

    print_string("Appuyez sur entrer pour continuer.", GREEN_ON_BLACK);
}