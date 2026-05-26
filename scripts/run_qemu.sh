#!/usr/bin/env bash
set -euo pipefail
make iso
qemu-system-i386 -cdrom build/SacramentuOS.iso -m 256M -serial stdio
