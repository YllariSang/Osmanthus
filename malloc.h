#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>
#include <stdint.h>

/* The metadata chunk attached to the front of every memory allocation */
typedef struct kmalloc_header {
    size_t size;                  /* Size of the usable memory block following this header */
    uint8_t is_free;              /* 1 if the block is free, 0 if it's allocated */
    struct kmalloc_header* next;  /* Link to the next physical block in the heap */
} kmalloc_header_t;

/* --- MEMORY OPERATIONS --- */
void kmalloc_init(uint32_t start_address, size_t initial_size);
void* kmalloc(size_t size);
void kfree(void* ptr);

#endif
