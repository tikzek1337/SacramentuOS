#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
qemu-system-i386 -machine pc -m 256M -serial stdio -no-reboot -no-shutdown -kernel prebuilt/sacramentuos.kernel
