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

//fonction de conversion integer to ascii
char *itoa(int32_t integer){ //pointeur mémoire vers itoa (integer signé)
    static char string_buffer[32]; //création d'un buffer pour la création de la chaîne de caractère

    uint8_t index = 0; //index dans le buffer
    uint8_t is_negative = 0; //passe à 1 si le nombre est négatif

    if(integer == 0){ //gestion du zero
        string_buffer[0] = '0'; //écriture de 0 dans le buffer
        string_buffer[1] = '\0'; //fin de la chaîne 
        return string_buffer; //retour du buffer
    }

    if(integer < 0){ //vérifie si le nombre est négatif
        is_negative = 1; //nb négatif
        integer = -integer; //prend la valeur opposée
    }

    while(integer > 0){ //tant qu'on a pas traité tous les chiffres de l'integer (on commence par traiter le chiffre des unités et on remonte vers la gauche)
        string_buffer[index++] = (integer % 10) + '0'; /*On prend le modulo par 10 de l'integer (reste de la division euclidienne par 10), on obtient ainsi la
        valeur du dernier chiffre. On ajoute à cette valeur, la valeur ascii du zero (0x30). L'addition des deux donne le caractère ascii correspondant au chiffre.*/
        integer /= 10; //On divise le nombre par 10 pour passer au chiffre suivant (on utilise un type integer, donc la partie décimale est supprimée)
    }

    if(is_negative){
        string_buffer[index++] = '-'; //si le nombre est négatif, on ajoute le caractère - à la fin du string_buffer.
    }

    string_buffer[index] = '\0'; //on termine la chaîne de caractère


    //Notre nombre est écrit à l'envers dans le buffer, il faut donc le retourner.
    uint8_t start_index = 0; //notre chaîne (inversée) commence à l'index 0 du string_buffer
    uint8_t end_index = index - 1; //On ne prend pas en compte le dernier caractère entré (\0) car il signe la fin de la chaîne

    //inversion du buffer
    while(start_index < end_index){ //tant que toutes les paires de caractère n'ont pas été inversée
        char temp = string_buffer[start_index]; //on sauvegarde temporairement la valeur de string_buffer[start_index] car elle va être modifier
        string_buffer[start_index] = string_buffer[end_index]; //on place le dernier caractère du buffer à l'index de start_index
        string_buffer[end_index] = temp; //on met la valeur à l'index start_index dans string_buffer[end_index]
        end_index--; //on recule l'index de fin d'un pas
        start_index++; //on avance l'index de départ d'un pas
        //on passe à la paire de caractères suivante
    }

    return string_buffer; //on retourne le buffer 
}