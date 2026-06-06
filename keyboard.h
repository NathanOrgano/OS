#ifndef KEYBOARD_H
#define KEYBOARD_H

void reset_buffer(void);
void read_line(void);

extern volatile int line_ready; //déclare si une ligne est prête (appuie sur la touche entrer)
extern char key_buffer[256]; //buffer clavier
extern int writing_perm; //permission d'écriture

#endif