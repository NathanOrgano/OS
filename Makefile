# Outils de compilation
CC = gcc
ASM = nasm
LD = ld

# Options de compilation 32 bits freestanding
CFLAGS = -m32 -ffreestanding -fno-pic -fno-pie -fno-stack-protector -Wall -Wextra -c
BUILD_DIR = build


# Liste des fichiers objets du noyau (PMM inclus)
OBJ = $(BUILD_DIR)/kernel_entry.o \
      $(BUILD_DIR)/kernel.o \
      $(BUILD_DIR)/pmm.o \
      $(BUILD_DIR)/shell.o \
      $(BUILD_DIR)/screen.o \
      $(BUILD_DIR)/idt.o \
      $(BUILD_DIR)/interrupt.o \
      $(BUILD_DIR)/io.o \
      $(BUILD_DIR)/exceptions.o \
      $(BUILD_DIR)/timer.o \
      $(BUILD_DIR)/keyboard.o \
      $(BUILD_DIR)/string.o


# Règle par défaut
all: $(BUILD_DIR)/os_image.img

# Génération de l'image disque finale (1440 Ko)
$(BUILD_DIR)/os_image.img: $(BUILD_DIR)/bootloader.bin $(BUILD_DIR)/kernel.bin
	@mkdir -p $(BUILD_DIR)
	cat $(BUILD_DIR)/bootloader.bin $(BUILD_DIR)/kernel.bin > $(BUILD_DIR)/os_image.img
	truncate -s 1440k $(BUILD_DIR)/os_image.img

# Édition de liens du noyau binaire brut
$(BUILD_DIR)/kernel.bin: linker.ld $(OBJ)
	@mkdir -p $(BUILD_DIR)
	$(LD) -m elf_i386 -T linker.ld $(OBJ) -o $(BUILD_DIR)/kernel.bin --oformat binary


# Secteur d'amorçage (16 bits)
$(BUILD_DIR)/bootloader.bin: bootloader.asm
	@mkdir -p $(BUILD_DIR)
	$(ASM) -f bin bootloader.asm -o $(BUILD_DIR)/bootloader.bin

# Point d'entrée du noyau (32 bits)
$(BUILD_DIR)/kernel_entry.o: kernel_entry.asm
	@mkdir -p $(BUILD_DIR)
	$(ASM) -f elf32 kernel_entry.asm -o $(BUILD_DIR)/kernel_entry.o

# Wrappers d'interruptions matérielles
$(BUILD_DIR)/interrupt.o: interrupt.asm
	@mkdir -p $(BUILD_DIR)
	$(ASM) -f elf32 interrupt.asm -o $(BUILD_DIR)/interrupt.o

# Fonctions d'entrées/sorties ports
$(BUILD_DIR)/io.o: io.asm
	@mkdir -p $(BUILD_DIR)
	$(ASM) -f elf32 io.asm -o $(BUILD_DIR)/io.o


# Noyau principal (dépendance ajoutée vers pmm.h)
$(BUILD_DIR)/kernel.o: kernel.c idt.h pmm.h
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) kernel.c -o $(BUILD_DIR)/kernel.o

# Gestionnaire de Mémoire Physique
$(BUILD_DIR)/pmm.o: pmm.c pmm.h
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) pmm.c -o $(BUILD_DIR)/pmm.o

# Shell interactif
$(BUILD_DIR)/shell.o: shell.c idt.h shell.h io.h
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) shell.c -o $(BUILD_DIR)/shell.o

# Pilote d'affichage écran VGA
$(BUILD_DIR)/screen.o: screen.c screen.h io.h
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) screen.c -o $(BUILD_DIR)/screen.o

# Table des descripteurs d'interruptions
$(BUILD_DIR)/idt.o: idt.c idt.h io.h
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) idt.c -o $(BUILD_DIR)/idt.o

# Gestion des exceptions du processeur
$(BUILD_DIR)/exceptions.o: exceptions.c exceptions.h screen.h
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) exceptions.c -o $(BUILD_DIR)/exceptions.o

# Pilote du timer (PIT 8253)
$(BUILD_DIR)/timer.o: timer.c timer.h
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) timer.c -o $(BUILD_DIR)/timer.o

# Pilote du clavier
$(BUILD_DIR)/keyboard.o: keyboard.c keyboard.h screen.h
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) keyboard.c -o $(BUILD_DIR)/keyboard.o

# Fonctions de manipulation de chaînes
$(BUILD_DIR)/string.o: string.c string.h
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) string.c -o $(BUILD_DIR)/string.o


# Nettoyage des fichiers générés
clean:
	rm -rf $(BUILD_DIR)