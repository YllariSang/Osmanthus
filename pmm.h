#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stddef.h>

#define PMM_BLOCK_SIZE 4096
#define PMM_BLOCKS_PER_BYTE 8

void pmm_init(uint32_t start_addr, uint32_t size);
void pmm_mark_used(uint32_t addr);
void pmm_mark_free(uint32_t addr);
void* pmm_alloc_block(void);
void pmm_free_block(void* addr);
uint32_t pmm_get_free_block_count(void);
uint32_t pmm_get_used_block_count(void);

#endif
