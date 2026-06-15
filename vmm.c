#include "vmm.h"
#include "pmm.h"
#include "string.h"

static page_directory_t* kernel_directory = NULL;

extern void load_page_directory(uint32_t* pd_addr);
extern void enable_paging(void);

void vmm_init(void) {
    // Allocate a page for the kernel directory
    kernel_directory = (page_directory_t*)pmm_alloc_block();
    memset(kernel_directory, 0, sizeof(page_directory_t));

    // Identity map the first 8MB
    // 8MB / 4KB = 2048 pages
    // This requires 2 page tables (each covers 4MB)
    for (uint32_t i = 0; i < 0x800000; i += 4096) {
        vmm_map_page((void*)i, (void*)i, VMM_PAGE_PRESENT | VMM_PAGE_WRITE);
    }

    // Load and enable paging
    vmm_switch_directory(kernel_directory);
    enable_paging();
}

void vmm_map_page(void* phys, void* virt, uint32_t flags) {
    uint32_t pd_index = (uint32_t)virt >> 22;
    uint32_t pt_index = ((uint32_t)virt >> 12) & 0x3FF;

    if (!(kernel_directory->entries[pd_index] & VMM_PAGE_PRESENT)) {
        // Allocate a new page table
        page_table_t* pt = (page_table_t*)pmm_alloc_block();
        memset(pt, 0, sizeof(page_table_t));
        kernel_directory->entries[pd_index] = (uint32_t)pt | flags | VMM_PAGE_PRESENT;
    }

    page_table_t* pt = (page_table_t*)(kernel_directory->entries[pd_index] & ~0xFFF);
    pt->entries[pt_index] = (uint32_t)phys | flags | VMM_PAGE_PRESENT;
}

void vmm_switch_directory(page_directory_t* pd) {
    load_page_directory((uint32_t*)pd);
}
