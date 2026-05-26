# SacramentuOS 1.0.1 Pixel Desktop

SacramentuOS 1.0.1 теперь содержит пиксельный графический интерфейс: рабочий стол, панель задач, Start-меню, окна, проводник, консоль, настройки, About/Help, bitmap-шрифт и software renderer для прямоугольников, линий, текста и курсора.

## Что добавлено

- BIOS/GRUB путь: Multiboot v1 + запрос VBE linear framebuffer 1024x768x32.
- UEFI путь: native `BOOTX64.EFI` + Graphics Output Protocol framebuffer.
- PS/2 mouse driver для legacy BIOS/QEMU: IRQ12 + polling fallback, 3-byte packet parser, left/right buttons, software cursor.
- UEFI pointer support: `EFI_SIMPLE_POINTER_PROTOCOL` и `EFI_ABSOLUTE_POINTER_PROTOCOL`, плюс fallback через прямой polling raw PS/2 mouse-пакетов. В QEMU теперь по умолчанию используется PS/2 mouse path с `-display gtk,grab-on-hover=on`, чтобы окно реально захватывало мышь.
- Bitmap font 5x7 с масштабированием, pixel primitives: pixel, fill rect, rect, line, text.
- Оконный shell: drag titlebar мышью, click Start/icons/windows, close button, taskbar apps.
- Apps: Explorer, Console, Settings, About, Help.

## Важное ограничение

Raw USB host stack для xHCI/EHCI/OHCI в legacy kernel пока не реализован. Для QEMU и PS/2-совместимого ввода добавлен прямой raw PS/2 polling fallback. USB-мышь на реальном UEFI-железе работает через firmware pointer protocols, если прошивка отдаёт `SimplePointer`/`AbsolutePointer` приложению.

## Быстрый запуск

Legacy BIOS kernel напрямую:

```bash
make run-kernel-built CC=gcc
```

Прямой `-kernel` может не передать VBE framebuffer. Для пиксельного BIOS-режима лучше грузить через GRUB ISO:

```bash
make iso CC=gcc
make run
```

Native UEFI:

```bash
make uefi-prebuilt UEFI_CC=clang
make uefi-flash
make run-uefi-prebuilt
```

Для реальной UEFI-флешки скопируй содержимое папки `flash/` в корень FAT32-накопителя. Главный файл: `EFI/BOOT/BOOTX64.EFI`.

## Управление

- Мышь: клик по Start, иконкам, окнам; перетаскивание окна за заголовок.
- `F1` — Start menu.
- `F2` — Explorer.
- `F3` — Console.
- `F4` — About.
- `F5` — Settings.
- `Tab` — переключение окон.
- `Esc` — закрыть активное окно.

Команды консоли: `help`, `ver`, `fetch`, `dir`, `ls`, `cat`, `clear`, `explorer`, `settings`, `about`, `reboot`, `shutdown`.

## Исправление мерцания и мыши

В этом build добавлен software backbuffer для BIOS/VBE и UEFI/GOP. Интерфейс больше не рисуется прямо в видимый framebuffer по частям: сначала кадр собирается в памяти, затем копируется на экран. Это уменьшает мерцание и разрывы кадра в QEMU/OVMF.

Для UEFI-запуска Makefile теперь использует PS/2 mouse path и GTK grab-on-hover:

```bash
make run-uefi-prebuilt
```

Если мышь в окне QEMU не двигается, наведи курсор на окно или нажми `Ctrl+Alt+G`, чтобы QEMU захватил мышь. Освободить мышь обратно в систему — снова `Ctrl+Alt+G`. Если твоя сборка QEMU не поддерживает `-display gtk,grab-on-hover=on`, удали этот параметр из Makefile/scripts и используй ручной grab через `Ctrl+Alt+G`.
