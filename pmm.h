#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stddef.h>

#define PMM_BLOCK_SIZE 0x1000 //4096
#define PMM_BLOCKS_PER_CELL 0x20 //32

void pmm_init(uint32_t bitmap_start_addr, uint32_t mem_size);
void pmm_init_region(uint32_t base_addr, uint32_t size);
void pmm_deinit_region(uint32_t base_addr, uint32_t size);
void *memory_allocation();
void memory_free(void *ptr);

uint32_t get_max_memory();
uint32_t get_used_memory();

#endif