#!/usr/bin/env bash
set -e
qemu-system-i386 \
  -machine pc \
  -m 256M \
  -vga std \
  -display gtk,grab-on-hover=on \
  -serial stdio \
  -no-reboot \
  -no-shutdown \
  -kernel prebuilt/sacramentuos.kernel
