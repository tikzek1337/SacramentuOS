; ========================================================
; bootloader.asm
; Загрузчик SacramentuOS v0.0.2
; ========================================================
[org 0x7c00]
[bits 16]

KERNEL_OFFSET equ 0x1000
SECTORS_TO_LOAD equ 32

MEM_MAP_ENTRIES equ 0x5000

start:
    mov [BOOT_DRIVE], dl
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Получаем карту памяти (E820)
    mov di, MEM_MAP_ENTRIES + 2
    xor ebx, ebx
    mov edx, 0x534D4150
    mov eax, 0xE820
    mov ecx, 24
    mov word [MEM_MAP_ENTRIES], 0
.next_entry:
    int 0x15
    jc .e820_done
    cmp eax, 0x534D4150
    jne .e820_done
    test ebx, ebx
    jz .e820_done
    add di, 24
    inc word [MEM_MAP_ENTRIES]
    jmp .next_entry
.e820_done:
    cmp word [MEM_MAP_ENTRIES], 0
    jne .load_kernel
    ; Заглушка
    mov di, MEM_MAP_ENTRIES + 2
    mov dword [di], 0
    mov dword [di+4], 0
    mov dword [di+8], 0xFFFFFFFF
    mov dword [di+12], 0
    mov dword [di+16], 1
    mov word [MEM_MAP_ENTRIES], 1

.load_kernel:
    mov bx, KERNEL_OFFSET
    mov ah, 0x02
    mov al, SECTORS_TO_LOAD
    mov ch, 0x00
    mov cl, 0x02
    mov dh, 0x00
    mov dl, [BOOT_DRIVE]
    int 0x13
    jc disk_error

    ; Переход в защищённый режим
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:init_pm

disk_error:
    mov si, DISK_ERROR_MSG
    call print_string_rm
    jmp $

print_string_rm:
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
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov esp, 0x90000
    call KERNEL_OFFSET
    jmp $

BOOT_DRIVE      db 0
DISK_ERROR_MSG  db "Disk read error!",0

; GDT
gdt_start:
gdt_null:
    dd 0x0
    dd 0x0
gdt_code:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0x00
gdt_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; Таблица разделов
times 0x1BE - ($ - $$) db 0
partition_table:
    db 0x80
    db 0x00, 0x01, 0x00
    db 0x0C
    db 0xFE, 0xFF, 0xFF
    dd 1
    dd 0xFFFFFFFF
    times 16 db 0
    times 16 db 0
    times 16 db 0

dw 0xAA55