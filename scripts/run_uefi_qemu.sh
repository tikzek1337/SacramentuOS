#!/usr/bin/env bash
set -e
OVMF=${OVMF:-/usr/share/OVMF/OVMF_CODE.fd}
qemu-system-x86_64 \
  -machine pc \
  -m 256M \
  -vga std \
  -display gtk,grab-on-hover=on \
  -serial stdio \
  -no-reboot \
  -no-shutdown \
  -bios "$OVMF" \
  -drive format=raw,file=fat:rw:flash
