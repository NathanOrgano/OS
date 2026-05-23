[BITS 32]

global outb
global inb

outb:
    mov edx, [esp + 4] ;premier argument = port(16 bits)
    mov eax, [esp + 8] ;second argument = la donnée
    out dx, al ;envoie la donnée al sur le port dx
    ret

inb:
    mov edx, [esp + 4] ;port d'entrée
    xor eax, eax
    in al, dx ; lit le port dx et me le résultat dans al
    ret