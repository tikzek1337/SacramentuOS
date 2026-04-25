; ========================================================
; bootloader.asm
; Загрузчик операционной системы (BIOS, Legacy Boot)
; Собирается в 512-байтный первый сектор (MBR)
; Загружает ядро с диска, переключает в защищённый режим
; ========================================================
[org 0x7c00]              ; BIOS загружает нас по адресу 0x7C00
[bits 16]                 ; Начинаем в 16-битном реальном режиме

KERNEL_OFFSET equ 0x1000  ; Адрес, куда загрузится ядро
SECTORS_TO_LOAD equ 16    ; Количество секторов ядра (8 КБ)

start:
    ; Сохраняем номер загрузочного диска (BIOS передаёт в DL)
    mov [BOOT_DRIVE], dl

    ; Настройка сегментов и стека
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Загружаем ядро
    mov bx, KERNEL_OFFSET   ; ES:BX = буфер для чтения
    mov ah, 0x02            ; Функция BIOS: чтение секторов
    mov al, SECTORS_TO_LOAD ; Сколько читать
    mov ch, 0x00            ; Цилиндр 0
    mov cl, 0x02            ; Сектор 2 (сразу после загрузчика)
    mov dh, 0x00            ; Головка 0
    mov dl, [BOOT_DRIVE]    ; Диск
    int 0x13                ; Вызов BIOS
    jc disk_error           ; Если ошибка – выводим сообщение

    ; Загружаем таблицу дескрипторов (GDT) и переходим в защищённый режим
    cli                     ; Запрещаем прерывания
    lgdt [gdt_descriptor]   ; Загружаем указатель на GDT
    mov eax, cr0
    or eax, 0x1             ; Устанавливаем бит PE (Protection Enable)
    mov cr0, eax
    ; Теперь мы в защищённом режиме, но ещё выполняем 16-битный код
    jmp CODE_SEG:init_pm    ; Дальний прыжок сбрасывает конвейер

disk_error:
    mov si, DISK_ERROR_MSG
    call print_string_rm
    jmp $

print_string_rm:            ; Вывод строки в реальном режиме (BIOS)
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print_string_rm
.done:
    ret

[bits 32]
init_pm:
    ; Настройка сегментных регистров для защищённого режима
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov esp, 0x90000        ; Стек ядра
    ; Прыгаем на точку входа в ядро
    call KERNEL_OFFSET
    jmp $                   ; На случай возврата

; Данные в пределах загрузочного сектора
BOOT_DRIVE:      db 0
DISK_ERROR_MSG:  db "Disk read error!",0

; ==================== GDT (Глобальная Таблица Дескрипторов) ====================
; Плоская модель: сегмент кода и данных от 0 до 4 ГБ
gdt_start:
gdt_null:       ; Обязательный нулевой дескриптор
    dd 0x0
    dd 0x0
gdt_code:       ; Сегмент кода: base=0, limit=4ГБ, исполняемый, читаемый
    dw 0xFFFF       ; limit (0-15)
    dw 0x0000       ; base (0-15)
    db 0x00         ; base (16-23)
    db 10011010b    ; флаги доступа (present, ring0, code, executable, readable)
    db 11001111b    ; флаги (granularity=4K, 32-bit) + limit (16-19)
    db 0x00         ; base (24-31)
gdt_data:       ; Сегмент данных: base=0, limit=4ГБ, записываемый
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b    ; data, writable
    db 11001111b
    db 0x00
gdt_end:

gdt_descriptor:         ; Структура для команды lgdt
    dw gdt_end - gdt_start - 1   ; размер
    dd gdt_start                 ; адрес

; Константы селекторов (индекс в GDT)
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; Заполняем нулями до 510 байт и добавляем сигнатуру загрузочного сектора
times 510-($-$$) db 0
dw 0xAA55