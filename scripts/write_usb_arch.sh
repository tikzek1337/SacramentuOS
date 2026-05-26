#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 1 ]]; then
  echo "Usage: $0 /dev/sdX"
  echo "Example: $0 /dev/sdb"
  echo "WARNING: the selected device will be overwritten. Do not pass a partition like /dev/sdb1."
  exit 1
fi

DEV="$1"
ISO="build/SacramentuOS.iso"

if [[ ! -f "$ISO" ]]; then
  echo "$ISO not found. Run: make iso"
  exit 1
fi

read -r -p "This will overwrite $DEV. Type SACRAMENTUOS to continue: " CONFIRM
if [[ "$CONFIRM" != "SACRAMENTUOS" ]]; then
  echo "Cancelled."
  exit 1
fi

sudo dd if="$ISO" of="$DEV" bs=4M status=progress oflag=sync
sync
echo "Done. Reboot and choose the USB device in your boot menu."
