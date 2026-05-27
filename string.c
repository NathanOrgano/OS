#include "string.h"

//fonction de comparaison de chaînes de caractères
int strcmp(char *str_1, char *str_2){ //on utilise les adresses mémoires des deux str
    while(*str_1 && (*str_1 == *str_2)){ //tant que la chaîne 1 n'arrive pas à \0 et que le caractère de str_1 est identique à celui de str_2
        str_1 ++; //on passe aux adresses mémoire suivantes
        str_2 ++;
    }
    int result = *(const unsigned char *)str_1 - *(const unsigned char *)str_2; //si les chaînes sont identiques, le résultat est 0
    return !result; //si result == 0 on retourne 1 donc la condition est validée dans le if
}