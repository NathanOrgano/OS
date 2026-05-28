#include "screen.h"
#include "idt.h"
#include "io.h"
#include "string.h"
#include "keyboard.h"
#include "timer.h"

//fonction de test de l'isr 0 (division par 0)
void crash_division_by_zero(){
    volatile int numerateur = 42;
    volatile int denominateur = 0;
    volatile int resultat = numerateur / denominateur;
}

//affichage du message d'entrée
void welcome(){
    /*char *message = 
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
        " OOOOO   SSSSS\n";*/

    char *message = 
        "_________________________________________________________________\n"
        "___________________/ / / /__/   |__/ __ \\__/ ____/_/ __ \\__  /\n"
        "__________________/ /_/ /__/ /| |_/ /_/ /_/ __/___/ /_/ /_/ /\n"
        "_________________/ __  / _/ ___ |/ ____/_/ /_____/ _, _/_/ /__\n"
        "                /_/ /_/  /_/  |_/_/     /_____/ /_/ |_| /____/\n"
        "\n"
        "_______________________________\n"
        "__________________/ __ \\/  ___/\n"
        "_________________/ / / /\\___ \\\n"
        "                / /_/ /____/ /\n"
        "                \\____//_____/\n";
        
    clear_screen();
    print_string(message, GREEN_ON_BLACK);

    for(int i=0; i < 13; i++){ //On saute 13 lignes
        print_string("\n", GREEN_ON_BLACK);
    }

    print_string("Appuyez sur entrer pour continuer.", GREEN_ON_BLACK);
    read_line();
    clear_screen();
}

//Entrée dans le "shell"
void shell(){
    while(1){ //on boucle infiniment
        print_string("User@HaperzOS# ", WHITE_ON_BLACK);
        read_line();
        print_string("\n", WHITE_ON_BLACK);
        print_string(">>", WHITE_ON_BLACK);

        //clear
        if(strcmp(key_buffer, "clear")){ //appel de la fonction de comparaison de str
            clear_screen();
        }
        
        //reboot
        else if(strcmp(key_buffer, "reboot")){
            outb(0x64, 0xFE);
        }

        //crash 
        else if(strcmp(key_buffer, "crash")){
            crash_division_by_zero();
        }

        //temps depuis l'allumage 
        else if(strcmp(key_buffer, "uptime")){
            uint32_t uptime = get_ticks()/1000;
            print_string("Allume depuis ", WHITE_ON_BLACK);
            print_string(itoa(uptime), WHITE_ON_BLACK);
            print_string(" secondes\n", WHITE_ON_BLACK);
        }

        //commande inconnue
        else{
            print_string("Commande inconnue\n", WHITE_ON_BLACK);
        }
    }
}