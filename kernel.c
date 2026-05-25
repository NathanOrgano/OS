#include "screen.h"
#include "idt.h"

//fonction de test de l'isr 0 (division par 0)
void crash_division_by_zero(){
    volatile int numerateur = 42;
    volatile int denominateur = 0;
    volatile int resultat = numerateur / denominateur;
}

//fonction d'entrée dans la lecture de ligne (<=> input)
void read_line(){
    reset_buffer();
    while(!line_ready){
        __asm__ volatile("hlt");
    }
}

//affichage du message d'entrée
void welcome(){
    char *message = 
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

    for(int i=0; i < 13; i++){ //On saute 13 lignes
        print_string("\n", GREEN_ON_BLACK);
    }

    print_string("Appuyez sur entrer pour continuer.", GREEN_ON_BLACK);
    read_line();
    clear_screen();
}

//fonction de comparaison de chaînes de caractères
int strcmp(char *str_1, char *str_2){ //on utilise les adresses mémoires des deux str
    while(*str_1 && (*str_1 == *str_2)){ //tant que la chaîne 1 n'arrive pas à \0 et que le caractère de str_1 est identique à celui de str_2
        str_1 ++; //on passe aux adresses mémoire suivantes
        str_2 ++;
    }
    int result = *(const unsigned char *)str_1 - *(const unsigned char *)str_2; //si les chaînes sont identiques, le résultat est 0
    return !result; //si result == 0 on retourne 1 donc la condition est validée dans le if
}

//Entrée dans le "shell"
void shell(){
    while(1){ //on boucle infiniment
        print_string("User@HaperzOS# ", WHITE_ON_BLACK);
        read_line();
        print_string("\n", WHITE_ON_BLACK);

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

        //commande inconnue
        else{
            print_string("Commande inconnue\n", WHITE_ON_BLACK);
        }
    }
}

//fonction d'entrée
void main(){
    init_idt(); //initialisation de l'idt avec toutes les interruptions programmées
    __asm__ volatile("sti"); //réactivation via l'asm inline des interruptions
    writing_perm = 0; //on ne donne pas la permission d'écrire à l'écran
    welcome(); //affichage du message d'accueil
    writing_perm = 1; //on donne la permission d'écrire
    shell(); //on saute dans le shell
}