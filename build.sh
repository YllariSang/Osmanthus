#!/bin/bash
set -e # Terminate instantly if any internal pipeline step returns a failure code

echo "=== 1. PURGING WORKSPACE CACHE ==="
# Clean out stale binaries, object files, and workspace trees cleanly
rm -rf *.o isodir myos.iso myos.bin 2>/dev/null || true

# Recreate the target GRUB directory structures natively before compiling
mkdir -p isodir/boot/grub

echo "=== 2. ASSEMBLING ASSEMBLY STUBS ==="
# Assemble every low-level core CPU descriptor stub file cleanly
nasm -f elf32 boot.asm -o boot.o
nasm -f elf32 gdt_flush.asm -o gdt_flush.o
nasm -f elf32 interrupt.asm -o interrupt.o

echo "=== 3. COMPILING CORE C MODULES ==="
# Compile with strict bare-metal optimizations, disabling implicit vectorization loops (SSE)
FLAGS="-m32 -c -std=gnu99 -ffreestanding -O2 -Wall -Wextra -fno-stack-protector -fno-pie -nostdlib -fno-builtin -mno-sse -mno-sse2 -mno-mmx"

gcc $FLAGS kernel.c -o kernel.o
gcc $FLAGS gdt.c -o gdt.o
gcc $FLAGS idt.c -o idt.o
gcc $FLAGS shell.c -o shell.o
gcc $FLAGS graphics.c -o graphics.o
gcc $FLAGS malloc.c -o malloc.o
gcc $FLAGS keyboard.c -o keyboard.o

echo "=== 4. LINKING UNIFIED KERNEL BINARY ==="
# Bind all compiled artifacts sequentially, locking boot.o strictly at the head front
ld -m elf_i386 -T linker.ld -o isodir/boot/myos.bin boot.o interrupt.o kernel.o gdt.o gdt_flush.o idt.o shell.o graphics.o malloc.o keyboard.o

echo "=== 5. BUILDING ISO FILESYSTEM TREE ==="
if [ -f grub.cfg ]; then
    cp grub.cfg isodir/boot/grub/
else
    # Automatically generate a generic configuration fallback array if missing from disk root
    cat << 'EOF' > isodir/boot/grub/grub.cfg
menuentry "Osmanthus OS" {
    multiboot /boot/myos.bin
    boot
}
EOF
fi

echo "=== 6. GENERATING BOOTABLE ISO MEDIA ==="
# Package raw files inside the GRUB image framework wrapper layout
grub-mkrescue -o myos.iso isodir

echo "=== 7. BOOTING TARGET ENGINE ==="
# Kill off frozen background processes securely to clear out network port assignments
killall -9 qemu-system-i386 novnc_proxy websockify 2>/dev/null || true

# Initialize QEMU background virtualization linking hardware graphics arrays along standard serial lines
qemu-system-i386 -cdrom myos.iso -vga std -vnc :0 -serial mon:stdio &
QEMU_PID=$!

# Give the virtual hardware a brief second to claim memory regions securely
sleep 1

# Fire up the native noVNC local desktop socket routing agent on port 6080
./novnc_desktop/utils/novnc_proxy --vnc localhost:5900 --listen 6080 &
PROXY_PID=$!

sleep 1

# Auto-launch Windows browser page container targeted at our local pipeline endpoint frame
powershell.exe Start-Process '"http://127.0.0.1:6080/vnc.html?autoconnect=true&resize=scale"'

echo "=== KERNEL DEPLOYMENT RUNNING ==="
echo "Watch this terminal window for live microkernel text diagnostic reports."
echo "Press [Ctrl + C] inside this terminal to cleanly terminate the kernel execution loop."

# Maintain focus on target process thread container logs
wait $QEMU_PID

# Auto-cleanup network listeners cleanly upon termination signal drops
kill $PROXY_PID 2>/dev/null || true
