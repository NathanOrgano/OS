#include "screen.h"

#define VIDEO_ADDRESS 0xB8000
#define MAX_ROWS 25
#define MAX_COLS 80

#define WHITE_ON_BLACK 0x0F
#define GREEN_ON_BLACK 0x0A
#define RED_ON_BLACK   0x0C

char* video_memory = (char*) VIDEO_ADDRESS;
int position_memory = 0;
int cursor_x = 0;
int cursor_y = 0;

void scrolling(uint8_t color){
    for(int i=0; i < 4000; i++){
        video_memory[i] = video_memory[i+160];
    }

    for(int i=3840; i < 4000; i+=2){
        video_memory[i] = ' ';
        video_memory[i+1] = color;
    }

    cursor_y = 24;
}

void clear_screen(){
    for (int i=0; i < 4000; i += 2){
        video_memory[i] = ' ';
        video_memory[i+1] = GREEN_ON_BLACK; 
    }
    cursor_x = 0;
    cursor_y = 0;
}

void print_char(char character, uint8_t color){
    if(cursor_y >= MAX_ROWS){
        scrolling(color);
    }

    int position = (cursor_y * MAX_COLS + cursor_x)*2;

    video_memory[position] = character;
    video_memory[position + 1] = color;
    cursor_x++;

    if(cursor_x >= MAX_COLS){
        cursor_x = 0;
        cursor_y ++;
    }
}

void print_string(char* string, uint8_t color){
    int index_string = 0;

    while (string[index_string] != '\0'){
        if (string[index_string] == '\n'){
            cursor_x = 0;
            cursor_y++;
        }

        else{
            print_char(string[index_string], color);
        }

        index_string ++;
    }
}

void clear_char(){
    if (cursor_x == 0 && cursor_y == 0) {
        return;
    }

    if (cursor_x == 0) {
        cursor_y--;
        cursor_x = MAX_COLS - 1;
    } else {
        cursor_x--;
    }

    int position = (cursor_y * MAX_COLS + cursor_x) * 2;
    video_memory[position] = ' ';
    video_memory[position + 1] = WHITE_ON_BLACK;
}
