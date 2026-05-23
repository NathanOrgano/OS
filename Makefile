# ==============================================================================
#                 MAKEFILE - SYSTÈME D'EXPLOITATION (NATHAN)
# ==============================================================================

# Outils de compilation
CC = gcc
ASM = nasm
LD = ld

# Options de compilation pour le mode 32 bits freestanding
CFLAGS = -m32 -ffreestanding -fno-pic -fno-pie -fno-stack-protector -Wall -Wextra -c

# Dossier de destination des fichiers compilés
BUILD_DIR = build

# Liste de tous les fichiers objets nécessaires pour le noyau (dans le dossier build)
OBJ = $(BUILD_DIR)/kernel_entry.o $(BUILD_DIR)/kernel.o $(BUILD_DIR)/screen.o $(BUILD_DIR)/idt.o $(BUILD_DIR)/interrupt.o $(BUILD_DIR)/io.o

# Règle par défaut : génère l'image finale du système (.img) dans build/
all: $(BUILD_DIR)/os_image.img

# Assemblage du bootloader et du noyau pour créer la disquette virtuelle
$(BUILD_DIR)/os_image.img: $(BUILD_DIR)/bootloader.bin $(BUILD_DIR)/kernel.bin
	@mkdir -p $(BUILD_DIR)
	cat $(BUILD_DIR)/bootloader.bin $(BUILD_DIR)/kernel.bin > $(BUILD_DIR)/os_image.img
	truncate -s 1440k $(BUILD_DIR)/os_image.img

# Édition de liens pour fusionner tous les fichiers objets C et ASM du noyau
$(BUILD_DIR)/kernel.bin: linker.ld $(OBJ)
	@mkdir -p $(BUILD_DIR)
	$(LD) -m elf_i386 -T linker.ld $(OBJ) -o $(BUILD_DIR)/kernel.bin --oformat binary

# Compilation du secteur d'amorçage (Bootloader 16 bits -> binaire brut)
$(BUILD_DIR)/bootloader.bin: bootloader.asm
	@mkdir -p $(BUILD_DIR)
	$(ASM) -f bin bootloader.asm -o $(BUILD_DIR)/bootloader.bin

# Compilation du point d'entrée en assembleur du noyau (NASM -> ELF32)
$(BUILD_DIR)/kernel_entry.o: kernel_entry.asm
	@mkdir -p $(BUILD_DIR)
	$(ASM) -f elf32 kernel_entry.asm -o $(BUILD_DIR)/kernel_entry.o

# Compilation du wrapper d'interruptions en assembleur (NASM -> ELF32)
$(BUILD_DIR)/interrupt.o: interrupt.asm
	@mkdir -p $(BUILD_DIR)
	$(ASM) -f elf32 interrupt.asm -o $(BUILD_DIR)/interrupt.o
	
$(BUILD_DIR)/io.o: io.asm
	@mkdir -p $(BUILD_DIR)
	$(ASM) -f elf32 io.asm -o $(BUILD_DIR)/io.o

# Compilation des fichiers sources C
$(BUILD_DIR)/kernel.o: kernel.c idt.h screen.h
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) kernel.c -o $(BUILD_DIR)/kernel.o

$(BUILD_DIR)/screen.o: screen.c screen.h
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) screen.c -o $(BUILD_DIR)/screen.o

$(BUILD_DIR)/idt.o: idt.c idt.h
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) idt.c -o $(BUILD_DIR)/idt.o

# Nettoyage complet du dossier build
clean:
	rm -rf $(BUILD_DIR)