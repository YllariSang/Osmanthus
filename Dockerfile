# Osmanthus build environment.
#
# This image only contains the toolchain (compiler, assembler, GRUB/ISO
# tools, QEMU) — not the kernel source itself. Mount the repo as a volume
# and build inside the container, the same way you'd use `nix develop`.
#
# Build:
#   docker build -t osmanthus-dev .
#
# Use (from the repo root):
#   docker run --rm -it -v "$(pwd)":/workspace osmanthus-dev
#   (inside the container) ./build.sh --build-only

FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
        gcc \
        gcc-multilib \
        nasm \
        grub-pc-bin \
        grub-common \
        grub-efi-amd64-bin \
        xorriso \
        mtools \
        qemu-system-x86 \
        ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

CMD ["/bin/bash"]
