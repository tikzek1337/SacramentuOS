# Запуск SacramentuOS с флешки в UEFI

1. Отформатируй флешку в FAT32.
2. Скопируй содержимое папки `flash/` в корень флешки.
3. Проверь путь:

```text
ФЛЕШКА:\EFI\BOOT\BOOTX64.EFI
```

4. В UEFI/BIOS выставь:

```text
Secure Boot: Disabled
Fast Boot: Disabled
Boot Mode: UEFI
```

5. В Boot Menu выбери:

```text
UEFI: название_флешки
```

Проверочные команды после запуска:

```text
ver
fetch
commands
color list
color 10
mode list
edit test.c
files
show test.c
clean
```
