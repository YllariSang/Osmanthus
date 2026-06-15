#!/bin/bash
set -e

echo "=== 1. PURGING WORKSPACE CACHE ==="
rm -rf *.o isodir myos.iso myos.bin 2>/dev/null || true

mkdir -p isodir/boot/grub

echo "=== 2. ASSEMBLING ASSEMBLY STUBS ==="
nasm -f elf32 boot.asm -o boot.o
nasm -f elf32 gdt_flush.asm -o gdt_flush.o
nasm -f elf32 interrupt.asm -o interrupt.o
nasm -f elf32 paging_asm.asm -o paging_asm.o
nasm -f elf32 switch.asm -o switch.o

echo "=== 3. COMPILING CORE C MODULES ==="
FLAGS="-m32 -c -std=gnu99 -ffreestanding -O2 -Wall -Wextra -fno-stack-protector -fno-pie -nostdlib -fno-builtin -mno-sse -mno-sse2 -mno-mmx"

gcc $FLAGS kernel.c -o kernel.o
gcc $FLAGS gdt.c -o gdt.o
gcc $FLAGS idt.c -o idt.o
gcc $FLAGS shell.c -o shell.o
gcc $FLAGS graphics.c -o graphics.o
gcc $FLAGS malloc.c -o malloc.o
gcc $FLAGS keyboard.c -o keyboard.o
gcc $FLAGS vga.c -o vga.o
gcc $FLAGS timer.c -o timer.o
gcc $FLAGS string.c -o string.o
gcc $FLAGS stdio.c -o stdio.o
gcc $FLAGS pmm.c -o pmm.o
gcc $FLAGS vmm.c -o vmm.o
gcc $FLAGS task.c -o task.o
gcc $FLAGS scheduler.c -o scheduler.o

echo "=== 4. LINKING UNIFIED KERNEL BINARY ==="
ld -m elf_i386 -T linker.ld -o isodir/boot/myos.bin boot.o interrupt.o paging_asm.o switch.o kernel.o gdt.o gdt_flush.o idt.o shell.o graphics.o malloc.o pmm.o vmm.o task.o scheduler.o keyboard.o vga.o timer.o string.o stdio.o

echo "=== 5. BUILDING ISO FILESYSTEM TREE ==="
if [ -f grub.cfg ]; then
    cp grub.cfg isodir/boot/grub/
else
    cat << 'GRUBEOF' > isodir/boot/grub/grub.cfg
menuentry "Osmanthus OS" {
    multiboot /boot/myos.bin
    boot
}
GRUBEOF
fi

echo "=== 6. GENERATING BOOTABLE ISO MEDIA ==="
grub-mkrescue -o myos.iso isodir

echo "=== BUILD SUCCESSFUL ==="
echo "Kernel binary: isodir/boot/myos.bin"
echo "ISO image: myos.iso"
