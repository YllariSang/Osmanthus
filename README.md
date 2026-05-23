# Custom x86 Arch Microkernel

A minimalistic, bare-metal 32-bit x86 operating system microkernel built from scratch. This project demonstrates low-level systems programming, hardware initialization, and interrupt handling without relying on standard C runtime environments.

## 🚀 Features
- **Multiboot Compliant:** Boots cleanly via GRUB.
- **Custom Global Descriptor Table (GDT):** Sets up flat-mode memory segmentation.
- **Interrupt Descriptor Table (IDT):** Handles core CPU exceptions and hardware lines.
- **Dual 8259 PIC Remapping:** Prevents hardware interrupt collisions with CPU exceptions.
- **PS/2 Keyboard Driver:** Decodes raw hardware scancodes via I/O ports into real ASCII characters.
- **Custom VGA Terminal:** Handles raw video memory writing ($0xB8000$) with scroll support.

## 🛠️ Prerequisites
To build and run this kernel, you need an x86 cross-compiler or a Linux environment with:
- `gcc` (with 32-bit support: `gcc-multilib`)
- `nasm` (Assembler)
- `grub-mkrescue` & `xorriso` (For bootable ISO generation)
- `qemu-system-i386` (Emulator)

## 📦 How to Build and Run
Simply execute the automated build pipeline script:
```bash
chmod +x build.sh
./build.sh
