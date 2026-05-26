# UEFI-флешка

1. Отформатируй флешку в FAT32.
2. Выполни:

```bash
make uefi-prebuilt UEFI_CC=clang
make uefi-flash
```

3. Скопируй содержимое папки `flash/` в корень флешки.
4. Проверь путь: `EFI/BOOT/BOOTX64.EFI`.
5. Загружайся с флешки в UEFI mode.

`BOOTX64.EFI` содержит native GOP pixel desktop. Для мыши используются firmware pointer protocols; на обычном UEFI это обычно работает с USB-мышью через прошивку.


## Исправление захвата мыши

Добавлен raw PS/2 polling fallback для UEFI и legacy BIOS, расширен поиск UEFI pointer protocols через handle enumeration. QEMU-запуск переведён на PS/2 mouse path с `-display gtk,grab-on-hover=on`. Для ручного захвата мыши в QEMU используй `Ctrl+Alt+G`; повторное нажатие освобождает мышь. USB-мышь на реальном UEFI-железе зависит от того, отдаёт ли прошивка `SimplePointer`/`AbsolutePointer` приложению.
