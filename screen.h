#ifndef SCREEN_H //on vérifie si le fichier est déjà intégré par le compilateur (si oui on ignore jusqu'au endif)
#define SCREEN_H //sinon on définit SCREEN_H

#include <stdint.h> //on inclue la bibliothèque stdint pour pouvoir gérer précisément la taille de nos variables

//définition des couleurs d'affichage des caractères
#define WHITE_ON_BLACK 0x0F
#define GREEN_ON_BLACK 0x0A
#define RED_ON_BLACK   0x0C

//défintions des données pour l'utilisation de l'écran
#define VIDEO_ADDRESS 0xB8000
#define MAX_ROWS 25
#define MAX_COLS 80

//définitions des fonctions utilisables par un autre fichier dans screen.c
void clear_screen();
void print_char(char character, uint8_t color);
void print_string(char *string, uint8_t color);
void clear_char();

#endif