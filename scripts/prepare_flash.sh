#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
make uefi-prebuilt
make uefi-flash
printf '\nReady: ./flash\nCopy the CONTENTS of ./flash to a FAT32 USB flash drive.\n'
