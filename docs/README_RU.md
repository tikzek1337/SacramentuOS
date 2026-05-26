# SacramentuOS 1.0.1 Pixel Desktop

Это сборка SacramentuOS с пиксельным графическим интерфейсом. Вместо старой текстовой оболочки добавлен framebuffer UI: рабочий стол, окна, Start-меню, taskbar, проводник, консоль и настройки.

## Графика

BIOS-ветка использует Multiboot v1 и VBE linear framebuffer. В `src/boot.S` ядро запрашивает 1024x768x32, а `boot/grub/grub.cfg` включает `gfxpayload=keep`. После загрузки `src/fb.c` берёт VBE mode info из multiboot-структуры и пишет напрямую в framebuffer.

UEFI-ветка использует `EFI_GRAPHICS_OUTPUT_PROTOCOL`. Файл `src/uefi/uefi_main.c` сам выбирает GOP-режим, получает `FrameBufferBase`, рисует окна, текст и курсор без VGA text mode.

## Ввод

BIOS/QEMU: добавлен PS/2 mouse driver в `src/mouse.c`: IRQ12, enable auxiliary device, command byte, packet parser, кнопки и движение курсора.

UEFI/QEMU/флешка: используются `EFI_SIMPLE_POINTER_PROTOCOL` и `EFI_ABSOLUTE_POINTER_PROTOCOL`. Это покрывает USB-мышь/USB-tablet через прошивку. Raw USB xHCI/EHCI/OHCI host stack пока не написан.

## Рендерер

`src/fb.c` содержит pixel primitives: `put_pixel`, `fill_rect`, `rect`, `line`, `draw_text`. Текст рисуется 5x7 bitmap-шрифтом, масштаб 1/2.

## Fix build: framebuffer backbuffer

Добавлен software backbuffer для устранения сильного мерцания при полной перерисовке рабочего стола. Кнопка закрытия окна получила расширенную область клика. Для UEFI-мыши улучшена обработка SimplePointer/AbsolutePointer.


## Исправление захвата мыши

Добавлен raw PS/2 polling fallback для UEFI и legacy BIOS, расширен поиск UEFI pointer protocols через handle enumeration. QEMU-запуск переведён на PS/2 mouse path с `-display gtk,grab-on-hover=on`. Для ручного захвата мыши в QEMU используй `Ctrl+Alt+G`; повторное нажатие освобождает мышь. USB-мышь на реальном UEFI-железе зависит от того, отдаёт ли прошивка `SimplePointer`/`AbsolutePointer` приложению.
