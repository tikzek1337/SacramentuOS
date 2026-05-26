# Сборка SacramentuOS 1.0.1 Pixel Desktop

## Legacy BIOS kernel

```bash
make kernel CC=gcc
make prebuilt CC=gcc
```

Для настоящего VBE framebuffer лучше запускать через GRUB ISO, потому что прямой QEMU `-kernel` не всегда передаёт VBE mode info:

```bash
make iso CC=gcc
make run
```

## UEFI BOOTX64.EFI

```bash
make uefi UEFI_CC=clang
make uefi-prebuilt UEFI_CC=clang
make uefi-flash
```

Для QEMU/OVMF:

```bash
make run-uefi-prebuilt
```

Makefile добавляет `-usb -device usb-tablet`, чтобы OVMF отдавал pointer device через UEFI protocols.


## Исправление захвата мыши

Добавлен raw PS/2 polling fallback для UEFI и legacy BIOS, расширен поиск UEFI pointer protocols через handle enumeration. QEMU-запуск переведён на PS/2 mouse path с `-display gtk,grab-on-hover=on`. Для ручного захвата мыши в QEMU используй `Ctrl+Alt+G`; повторное нажатие освобождает мышь. USB-мышь на реальном UEFI-железе зависит от того, отдаёт ли прошивка `SimplePointer`/`AbsolutePointer` приложению.
