# Сборка

Для сборки UEFI-версии в WSL/Ubuntu:

```bash
sudo apt update
sudo apt install -y clang lld make
make clean
make uefi-prebuilt
make uefi-flash
```

Готовая структура для флешки будет в папке:

```text
flash/
```

Для проверки файла:

```bash
make check-uefi
```

Для QEMU/OVMF, если установлен QEMU и OVMF:

```bash
sudo apt install -y qemu-system-x86 ovmf
make run-uefi-prebuilt
```

Legacy BIOS-режим оставлен как запасной:

```bash
sudo apt install -y build-essential gcc-multilib qemu-system-x86
make CC=gcc run-kernel-built
```
