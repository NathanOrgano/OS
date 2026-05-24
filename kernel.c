#include "screen.h"
#include "idt.h"

#define WHITE_ON_BLACK 0x0F
#define GREEN_ON_BLACK 0x0A
#define RED_ON_BLACK   0x0C

void main(){
    init_idt();
    writing_perm = 0;
    welcome();
    writing_perm = 1;
    shell();
}

void crash_division_by_zero(){
    volatile int numerateur = 42;
    volatile int denominateur = 0;
    volatile int resultat = numerateur / denominateur;
}

void welcome(){

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
    read_line();
    clear_screen();
}

void read_line(){
    reset_buffer();
    while(!line_ready){
        __asm__ volatile("hlt");
    }
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

void shell(){
    while(1){
        print_string("User@HaperzOS# ", WHITE_ON_BLACK);
        read_line();
        print_string("\n", WHITE_ON_BLACK);
    }
}