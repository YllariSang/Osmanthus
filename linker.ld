ENTRY(_start)

SECTIONS
{
    . = 0x100000;

    .text : AT(ADDR(.text))
    {
        *(.multiboot)
        *(.text)
    }

    .rodata : AT(ADDR(.rodata))
    {
        *(.rodata)
    }

    .data : AT(ADDR(.data))
    {
        *(.data)
    }

    .bss : AT(ADDR(.bss))
    {
        *(COMMON)
        *(.bss)
        *(.bootstrap_stack)
    }
    
    end = .;
}
