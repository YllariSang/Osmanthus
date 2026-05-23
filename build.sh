#!/bin/bash
set -e

echo "=== 1. PURGING WORKSPACE CACHE ==="
rm -f *.o myos.bin myos.iso
rm -rf isodir

echo "=== 2. ASSEMBLING ASSEMBLY STUBS ==="
nasm -f elf32 boot.asm -o boot.o
nasm -f elf32 gdt_flush.asm -o gdt_flush.o
nasm -f elf32 interrupt.asm -o interrupt.o

echo "=== 3. COMPILING CORE C MODULES ==="
gcc -m32 -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -fno-stack-protector
gcc -m32 -c gdt.c -o gdt.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -fno-stack-protector
gcc -m32 -c idt.c -o idt.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -fno-stack-protector
gcc -m32 -c keyboard.c -o keyboard.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -fno-stack-protector
gcc -m32 -c shell.c -o shell.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -fno-stack-protector

echo "=== 4. LINKING UNIFIED KERNEL BINARY ==="
# CRITICAL: shell.o MUST be appended to the end of this object list!
ld -m elf_i386 -T linker.ld -o myos.bin boot.o gdt_flush.o interrupt.o kernel.o gdt.o idt.o keyboard.o shell.o

echo "=== 5. BUILDING ISO FILESYSTEM TREE ==="
mkdir -p isodir/boot/grub
cp myos.bin isodir/boot/myos.bin
cp grub.cfg isodir/boot/grub/grub.cfg

echo "=== 6. GENERATING BOOTABLE ISO MEDIA ==="
grub-mkrescue -o myos.iso isodir

echo "=== 7. BOOTING TARGET ENGINE ==="
qemu-system-i386 -cdrom myos.iso -vga std -vnc :0 -k en-us
