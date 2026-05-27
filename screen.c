#include "screen.h" //inclusion du fichier d'en tête
#include "io.h" //gestion des entrées et sorties via ports matériels

char *video_memory = (char*) VIDEO_ADDRESS; //On définit un pointeur à l'adresse de débute de la VRAM
int cursor_x = 0; //position x
int cursor_y = 0; //position y

//scrolling vertical
void scrolling(uint8_t color){ //fonction de scrolling (on passe quand même la couleur en attribut pour définir la couleur du curseur.)
    //on décale tous les octets de la mémoire vidéo de 160 octets en arrière
    for(int i=0; i < 3840; i++){ 
        video_memory[i] = video_memory[i+160]; 
    }

    //On remplie la dernière ligne d'espaces
    for(int i=3840; i < 4000; i+=2){
        video_memory[i] = ' ';
        video_memory[i+1] = color;
    }

    cursor_y = 24; //on bloque le curseur d'impression à la dernière ligne de l'écran
}

void update_cursor_position(){
    uint16_t position = (cursor_y * MAX_COLS) + cursor_x; //on cherche la position du curseur clignotant à l'écran (entre 0 et 1999)

    /*on modifie la position du curseur grâce au contrôleur CRTC sur le port 0x3D4, on envoie le registre
    dans lequel on veut placer la donnée et sur le port 0x3D5, on envoie la donnée */
    outb(0x3D4, 0x0E); //on place la partie haute dans le registre 14
    outb(0x3D5, (uint8_t)(position >> 8)); //envoie de la partie haute de la position

    outb(0x3D4, 0x0F); //on place la partie basse dans le registre 15
    outb(0x3D5, (uint8_t)position); //envoie de la partie basse de la position
}

//Effacement de l'écran
void clear_screen(){
    for (int i=0; i < 4000; i += 2){ //on remplie la VRAM d'espaces blancs
        video_memory[i] = ' ';
        video_memory[i+1] = WHITE_ON_BLACK; 
    }

    //remise à 0 de la position d'impression
    cursor_x = 0;
    cursor_y = 0;
}

//affichage d'un caractère
void print_char(char character, uint8_t color){
    if(cursor_y >= MAX_ROWS){ //si on dépasse la ligne 24, on déclenche le scrolling
        scrolling(color);
    }

    int position = (cursor_y * MAX_COLS + cursor_x)*2; //calcul de la position mémoire

    //affichage du caractère
    video_memory[position] = character;
    video_memory[position + 1] = color;
    cursor_x++; //incrémentation de la position x

    //changement de ligne (initialisation des nouvelles coordonnées)
    if(cursor_x >= MAX_COLS){
        cursor_x = 0; //début de ligne (position x = 0)
        cursor_y ++; //numéro de ligne
    }

    //on update la position du curseur clignotant
    update_cursor_position();
}

//affichage d'une chaîne de caractères
void print_string(char *string, uint8_t color){
    int index_string = 0; //position dans la chaîne

    while (string[index_string] != '\0'){ //tant que la chaîne n'est pas terminée
        if (string[index_string] == '\n'){ //saut de ligne
            cursor_x = 0;
            cursor_y++;

            update_cursor_position();
        }

        else{ 
            print_char(string[index_string], color); //on affiche le caractère à l'index index_string
        }

        index_string ++; //on passe au carcatère suivant
    }
}

void clear_char(){
    //si on est au début de l'écran on return
    if (cursor_x == 0 && cursor_y == 0) {
        return;
    }

    //si on est au début d'une ligne
    if (cursor_x == 0) {
        cursor_y--; //on revient une ligne en arrière
        cursor_x = MAX_COLS - 1; //on va au dernier caractère de cette ligne
    } else {
        cursor_x--; //sinon on recule d'une position en x
    }

    int position = (cursor_y * MAX_COLS + cursor_x) * 2; //calcul de la nouvelle position
    video_memory[position] = ' '; //on écrit un espace
    video_memory[position + 1] = WHITE_ON_BLACK; //en blanc sur noir

    update_cursor_position(); //on met à jour la position du curseur clignotant
}
