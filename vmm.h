#ifndef VMM_H
#define VMM_H

#include <stdint.h>
#include <stddef.h>

#define VMM_PAGE_PRESENT   0x1
#define VMM_PAGE_WRITE     0x2
#define VMM_PAGE_USER      0x4

typedef uint32_t page_directory_entry_t;
typedef uint32_t page_table_entry_t;

typedef struct page_directory {
    page_directory_entry_t entries[1024];
} __attribute__((aligned(4096))) page_directory_t;

typedef struct page_table {
    page_table_entry_t entries[1024];
} __attribute__((aligned(4096))) page_table_t;

void vmm_init(void);
void vmm_map_page(void* phys, void* virt, uint32_t flags);
void vmm_switch_directory(page_directory_t* pd);

#endif
