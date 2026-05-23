#include "malloc.h"

static kmalloc_header_t* heap_start = NULL;

/* Initialize our heap space parameters */
void kmalloc_init(uint32_t start_address, size_t initial_size) {
    heap_start = (kmalloc_header_t*)start_address;
    heap_start->size = initial_size - sizeof(kmalloc_header_t);
    heap_start->is_free = 1;
    heap_start->next = NULL;
}

/* First-Fit Memory Allocation Engine */
void* kmalloc(size_t size) {
    if (size == 0) return NULL;

    /* Align allocations to 4-byte boundaries for 32-bit CPU efficiency */
    size = (size + 3) & ~3;

    kmalloc_header_t* current = heap_start;
    while (current != NULL) {
        if (current->is_free && current->size >= size) {
            /* Check if we can split this block into a used chunk and a new free chunk */
            if (current->size >= size + sizeof(kmalloc_header_t) + 4) {
                kmalloc_header_t* new_block = (kmalloc_header_t*)((uint32_t)current + sizeof(kmalloc_header_t) + size);
                new_block->size = current->size - size - sizeof(kmalloc_header_t);
                new_block->is_free = 1;
                new_block->next = current->next;

                current->size = size;
                current->next = new_block;
            }

            current->is_free = 0;
            /* Return a pointer to the usable memory area immediately AFTER the header */
            return (void*)((uint32_t)current + sizeof(kmalloc_header_t));
        }
        current = current->next;
    }

    return NULL; /* Out of memory! */
}

/* Memory Deallocation and Coalescing Engine */
void kfree(void* ptr) {
    if (ptr == NULL) return;

    /* Step back in memory to find the block's true header entry */
    kmalloc_header_t* header = (kmalloc_header_t*)((uint32_t)ptr - sizeof(kmalloc_header_t));
    header->is_free = 1;

    /* Coalesce (merge) continuous free blocks together to prevent fragmentation */
    kmalloc_header_t* current = heap_start;
    while (current != NULL && current->next != NULL) {
        if (current->is_free && current->next->is_free) {
            current->size += sizeof(kmalloc_header_t) + current->next->size;
            current->next = current->next->next;
            /* Retest the current block against the newly pulled ahead node index */
            continue; 
        }
        current = current->next;
    }
}
