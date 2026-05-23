ENTRY(_start)

SECTIONS
{
    /* Force the kernel image to start precisely at the 1 Megabyte physical address boundary mark */
    . = 0x00100000;

    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)
        *(.text)
    }

    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }

    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }

    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(.COMMON)
        *(.bss)
    }

    /* Set our global heap placement boundary tracking markers safely past our code blocks */
    _end = .;
    end = .;
}
