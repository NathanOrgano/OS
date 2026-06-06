#include "pmm.h"

static uint32_t *pmm_bitmap = NULL; //pointeur contenant l'adresse de départ de la bitmap
static uint32_t pmm_max_blocks = 0; // contient la valeur maximale de blocs de 4096 octets pouvant être contenus dans la mémoire
static uint32_t pmm_used_blocks = 0; //contient la valeur du nombre de blocs utilisés

//Initialise la mémoire
void pmm_init(uint32_t bitmap_start_addr, uint32_t mem_size){ //Prend l'adresse de base de la bitmap en entrée et la taille de la mémoire en octet (récupérée dans le bootloader)
    pmm_bitmap = (uint32_t*)bitmap_start_addr; //initialise le tableau de la bitmap à l'adresse passée en argument
    pmm_max_blocks = mem_size / PMM_BLOCK_SIZE; //calcule le nombre maximum de blocs pouvant être contenus dans la ram
    pmm_used_blocks = pmm_max_blocks; //à l'initialisation on bloque toute la mémoire pour éviter des conflits
    
    uint32_t total_cells = pmm_max_blocks / PMM_BLOCKS_PER_CELL; //définit le nombre total de case du tableau bitmap (1 case = uint32_t = 32 blocs de 4ko dans une case)

    //on traite toute la mémoire
    for(uint32_t i=0; i < total_cells; i++){
        pmm_bitmap[i] = 0xFFFFFFFF; //on initalise toute la bitmap à 1 (1 = bloc occupé, 0 = bloc libre)
    }
}

//Libère toute une zone de mémoire
void pmm_init_region(uint32_t base_addr, uint32_t size){ //prend l'adresse de départ de la zone et sa taille en argument
    uint32_t start_block = base_addr/PMM_BLOCK_SIZE; //Calcule l'index du bloc de départ de la zone à libérer
    uint32_t total_blocks = size/PMM_BLOCK_SIZE; //Calcule le nombre total de bloc de cette zone


    //Traite toute la zone mémoire concernée
    for(uint32_t i = 0; i < total_blocks; i++){
        uint32_t current_block = start_block + i; //Calcule l'index du bloc en cours de traitement

        uint32_t cell_index = current_block / PMM_BLOCKS_PER_CELL; //Calcule l'index dans le tableau de la bitmap
        uint32_t bit_position = current_block % PMM_BLOCKS_PER_CELL; //Calcule la position du bit dans l'entier 32 bits stocké à l'index cell_index

        pmm_bitmap[cell_index] &= ~(1 << bit_position); 
        /*On se rend à l'index cell_index dans la bitmap. Notre but est de libérer un bloc mémoire (passer un bit de 1 à 0). Pour ça, on vient calculer la position
        du bloc mémoire dans l'entier 32 bits en décalant 1 de bit position vers la gauche. (Si on veut modifier le 3e bit, On place le 1 au bit 3) Ensuit, on inverse
        tous les bits du masque avec un not (~) ainsi le bit à libérer passe à 0 alors que les autres passent à 1. Ensuite on effectue un et logique sur la bitmap pour
        passer un seul bit à 0 dans la case de la bitmap*/

        if(pmm_used_blocks > 0) pmm_used_blocks--; //On décrémente le nomre de blocs utilisés.
    }
}

//bloque toute une zone de mémoire
void pmm_deinit_region(uint32_t base_addr, uint32_t size){ //prend l'adresse de départ de la zone et sa taille en argument
    uint32_t start_block = base_addr/PMM_BLOCK_SIZE; //Calcule l'index du bloc de départ de la zone à bloquer
    uint32_t total_blocks = size/PMM_BLOCK_SIZE; //Calcule le nombre total de bloc de cette zone


    //Traite toute la zone mémoire concernée
    for(uint32_t i = 0; i < total_blocks; i++){
        uint32_t current_block = start_block + i; //Calcule l'index du bloc en cours de traitement

        uint32_t cell_index = current_block / PMM_BLOCKS_PER_CELL; //Calcule l'index dans le tableau de la bitmap
        uint32_t bit_position = current_block % PMM_BLOCKS_PER_CELL; //Calcule la position du bit dans l'entier 32 bits stocké à l'index cell_index

        pmm_bitmap[cell_index] |= (1 << bit_position); //effectue un ou logique cette fois.
        pmm_used_blocks++; //On incrémente le nomre de blocs utilisés.
    }
}

//on cherche le premier bloc de mémoire libre (fonction privée)
static int32_t pmm_firts_free_block(){
    uint32_t total_cells = pmm_max_blocks/PMM_BLOCKS_PER_CELL; //On récupère le nombre total de cases de notre pmm_bitmap
    
    //on traite toute la bitmap
    for(uint32_t i = 0; i < total_cells; i++){ //on traite les cases une par une
        if(pmm_bitmap[i] != 0xFFFFFFFF){ //si la case est déjà entièrement occuppée on ignore ce qui vient et on reboucle dans le for en passant à la case suivante
            for(uint32_t j = 0; j < PMM_BLOCKS_PER_CELL; j++){ //On traite tous les bits (32) de cette case
                uint32_t bit_mask = (1 << j); //on crée le masque de comparaison pour savoir quel est le premier bit à 0 dans l'entier 32 bits (non signé)
                
                if((pmm_bitmap[i] & bit_mask) == 0){ //on applique un et logique entre la case de la bitmap à l'index i et le bit mask. Si le résultat est 0 alors on a trouvé le premier bloc mémoire non occupé
                    return (i * PMM_BLOCKS_PER_CELL) + j; //on retourne la position absolue du bloc mémoire
                }
            }
        }
    } 
    return -1; //on retourne -1 si toute la mémoire est occuppée
}

//alloue UN bloc de mémoire (4ko)
void *memory_allocation(){
    int32_t block_index = pmm_firts_free_block(); //on récupère l'adresse du tout premier bloc libre

    if(block_index == -1) return NULL; //si toute la mémoire est occupée on retourne une fonction nulle
    
    uint32_t cell_index = block_index / PMM_BLOCKS_PER_CELL; //on récupère l'index de la case 
    uint32_t bit_position = block_index % PMM_BLOCKS_PER_CELL; //on récupère la position du bit 

    pmm_bitmap[cell_index] |= (1 << bit_position); //on applique un ou binaire pour bloquer un bloc mémoire
    pmm_used_blocks++; //on incrémente le nombre de bloc utilisés

    return (void*)(block_index * PMM_BLOCK_SIZE); //on retourne l'adresse absolue du bloc mémoire
}

//on libère UN bloc mémoire 
void memory_free(void *ptr){ //on a besoin de l'adresse de base de ce bloc
    if(ptr == NULL) return; //probablement s'il n'y a plus de place en mémoire

    uint32_t paddr = (uint32_t)(ptr); //on cast l'adresse du pointeur
    uint32_t block_index = paddr / PMM_BLOCK_SIZE; //on cherche l'index du bloc mémoire
    uint32_t cell_index = block_index / PMM_BLOCKS_PER_CELL; //Calcule de la case dans la bitmap
    uint32_t bit_position = block_index % PMM_BLOCKS_PER_CELL; //Calcule de la position du bit

    pmm_bitmap[cell_index] &= ~(1 << bit_position); //on vient libérer la zone (comme dans init_region)
    if(pmm_used_blocks > 0) pmm_used_blocks--; //on décrémente le nombre de blocs utilisés
}

uint32_t get_max_memory(){
    return pmm_max_blocks * 4096;
}

uint32_t get_used_memory(){
    return pmm_used_blocks * 4096;
}