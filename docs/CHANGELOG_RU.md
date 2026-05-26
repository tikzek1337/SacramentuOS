# Changelog

## 1.0.1 Pixel Desktop

- Добавлен VBE framebuffer для legacy BIOS/GRUB загрузки.
- Добавлен GOP framebuffer для native UEFI `BOOTX64.EFI`.
- Добавлен PS/2 mouse driver: IRQ12, packet parser, кнопки, координаты, bounds.
- Добавлена UEFI mouse/pointer поддержка через Simple Pointer и Absolute Pointer protocols.
- Добавлен software renderer: пиксели, прямоугольники, линии, bitmap-текст, software cursor.
- Добавлен Windows-like desktop shell: Start menu, taskbar, draggable windows, close button.
- Добавлены приложения Explorer, Console, Settings, About, Help.
- Обновлены QEMU flags: `-vga std`, для UEFI добавлен `-usb -device usb-tablet`.

Ограничение: raw USB host stack для legacy kernel не входит в эту версию; USB pointer поддерживается через UEFI firmware protocols.

## 1.0.1 framebuffer fix build

Fixed first pixel desktop test issues:
- added software backbuffer for BIOS/VBE renderer;
- added software backbuffer for UEFI/GOP renderer;
- framebuffer is now presented after a completed frame instead of drawing visible UI directly line-by-line;
- reduced visible flicker/tearing in QEMU/OVMF;
- enlarged the window close button hit-zone;
- improved UEFI SimplePointer movement scaling so small deltas are not rounded to zero;
- improved UEFI AbsolutePointer button handling;
- changed QEMU UEFI launch flags to use xHCI + usb-tablet.


## Исправление захвата мыши

Добавлен raw PS/2 polling fallback для UEFI и legacy BIOS, расширен поиск UEFI pointer protocols через handle enumeration. QEMU-запуск переведён на PS/2 mouse path с `-display gtk,grab-on-hover=on`. Для ручного захвата мыши в QEMU используй `Ctrl+Alt+G`; повторное нажатие освобождает мышь. USB-мышь на реальном UEFI-железе зависит от того, отдаёт ли прошивка `SimplePointer`/`AbsolutePointer` приложению.
