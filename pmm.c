#include "pmm.h"
#include "string.h"

static uint8_t* pmm_bitmap = NULL;
static uint32_t pmm_total_blocks = 0;
static uint32_t pmm_used_blocks = 0;

void pmm_init(uint32_t bitmap_addr, uint32_t total_memory_size) {
    pmm_bitmap = (uint8_t*)bitmap_addr;
    pmm_total_blocks = total_memory_size / PMM_BLOCK_SIZE;
    pmm_used_blocks = 0;

    // Initially mark everything as free
    memset(pmm_bitmap, 0x00, pmm_total_blocks / PMM_BLOCKS_PER_BYTE);
}

void pmm_mark_used(uint32_t addr) {
    uint32_t block = addr / PMM_BLOCK_SIZE;
    if (block >= pmm_total_blocks) return;
    
    if (!(pmm_bitmap[block / 8] & (1 << (block % 8)))) {
        pmm_used_blocks++;
    }
    pmm_bitmap[block / 8] |= (1 << (block % 8));
}

void pmm_mark_free(uint32_t addr) {
    uint32_t block = addr / PMM_BLOCK_SIZE;
    if (block >= pmm_total_blocks) return;

    if (pmm_bitmap[block / 8] & (1 << (block % 8))) {
        pmm_used_blocks--;
    }
    pmm_bitmap[block / 8] &= ~(1 << (block % 8));
}

void* pmm_alloc_block(void) {
    for (uint32_t i = 0; i < pmm_total_blocks / 8; i++) {
        if (pmm_bitmap[i] != 0xFF) {
            for (int j = 0; j < 8; j++) {
                if (!(pmm_bitmap[i] & (1 << j))) {
                    uint32_t block = i * 8 + j;
                    pmm_mark_used(block * PMM_BLOCK_SIZE);
                    return (void*)(block * PMM_BLOCK_SIZE);
                }
            }
        }
    }
    return NULL; // Out of memory
}

void pmm_free_block(void* addr) {
    pmm_mark_free((uint32_t)addr);
}

uint32_t pmm_get_free_block_count(void) {
    return pmm_total_blocks - pmm_used_blocks;
}

uint32_t pmm_get_used_block_count(void) {
    return pmm_used_blocks;
}
