#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
make iso-prebuilt
qemu-system-i386 -machine pc -m 256M -serial stdio -no-reboot -no-shutdown -boot d -cdrom build/SacramentuOS.iso
