#include <stdint.h>

struct gdt_entry_struct {
    uint16_t limit_low;           // The lower 16 bits of the limit
    uint16_t base_low;            // The lower 16 bits of the base
    uint8_t  base_middle;         // The next 8 bits of the base
    uint8_t  access;              // Access flags, determine what ring this segment can be used in
    uint8_t  granularity;
    uint8_t  base_high;           // The last 8 bits of the base
} __attribute__((packed));

struct gdt_ptr_struct {
    uint16_t limit;               // The upper 16 bits of all selector limits
    uint32_t base;                // The address of the first gdt_entry_struct
} __attribute__((packed));

struct gdt_entry_struct gdt_entries[3];
struct gdt_ptr_struct   gdt_ptr;

extern void gdt_flush(uint32_t);

static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access      = access;
}

void init_gdt(void) {
    gdt_ptr.limit = (sizeof(struct gdt_entry_struct) * 3) - 1;
    gdt_ptr.base  = (uint32_t)&gdt_entries;

    // Null descriptor
    gdt_set_gate(0, 0, 0, 0, 0);                
    // Code segment: Base=0, Limit=4GB, Access=0x9A, Granularity=0xCF
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); 
    // Data segment: Base=0, Limit=4GB, Access=0x92, Granularity=0xCF
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); 

    gdt_flush((uint32_t)&gdt_ptr);
}
