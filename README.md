# SacramentuOS 0.3.5

## Русский раздел

SacramentuOS — учебная консольная система. Основной вариант для современного ПК — нативный UEFI-запуск через `flash/EFI/BOOT/BOOTX64.EFI`. ISO для UEFI-режима не нужен: файл `BOOTX64.EFI` содержит саму UEFI-версию SacramentuOS с shell, документацией, RAM-файлами и встроенным редактором.

### Структура проекта

```text
SacramentuOS/
├─ src/                 исходный код
│  └─ uefi/             исходник чистой UEFI-версии
├─ include/             заголовочные файлы Legacy BIOS-ядра
├─ boot/grub/           конфиг GRUB для Legacy ISO
├─ prebuilt/            готовые бинарники
│  ├─ BOOTX64.EFI       основная UEFI-система
│  └─ sacramentuos.kernel запасное Legacy BIOS-ядро
├─ flash/               готовая структура для копирования на FAT32-флешку
├─ docs/                краткая документация
├─ scripts/             скрипты запуска/подготовки
├─ Makefile
└─ README.md
```

### Перенос на флешку

Отформатируй флешку в `FAT32`. Затем скопируй **содержимое папки `flash/`** в корень флешки. В корне должно получиться так:

```text
ФЛЕШКА:\
├─ EFI\BOOT\BOOTX64.EFI
├─ SacramentuOS\SYSTEM\VERSION.txt
├─ SacramentuOS\SYSTEM\TEST_COMMANDS.txt
└─ README_FIRST_RU.txt
```

В UEFI/BIOS рекомендуется выставить:

```text
Secure Boot: Disabled
Fast Boot: Disabled
Boot Mode: UEFI
CSM/Legacy: можно Disabled
```

В Boot Menu выбирай пункт вида `UEFI: <название флешки>`.

### Команды основной UEFI-системы

| Команда | Что делает |
|---|---|
| `help` | Показывает компактную встроенную справку по основным командам. |
| `docs` | Алиас для `help`. Открывает ту же встроенную документацию. |
| `commands` | Показывает короткий список доступных команд. |
| `clear` | Очищает экран консоли. |
| `clean` | Алиас для `clear`. Заменяет старый `cls`. |
| `ver` | Показывает название ОС, версию, дату последней сборки и дату последнего обновления. |
| `fetch` | Показывает компактную карточку системы: версия, сборка, пользователь, режим консоли, способ загрузки, ввод и редактор. |
| `whoami` | Показывает имя пользователя, выбранное при старте системы. |
| `time` | Показывает текущую дату и время через UEFI Runtime Services. |
| `date` | Алиас для `time`. Показывает дату и время. |
| `mode` | Показывает текущий безопасный текстовый режим или активный framebuffer-режим. |
| `mode list` | Показывает текущий безопасный режим. Опасный перебор UEFI-видеорежимов отключён, чтобы система не зависала на реальном железе. |
| `mode auto` | Включает большой framebuffer-режим через текущий UEFI GOP-режим без опасного QueryMode/SetMode-перебора. |
| `mode large` | Включает большую консоль framebuffer по текущему разрешению монитора, если GOP доступен. |
| `mode safe` | Возвращает безопасную firmware text console. |
| `color` | Показывает подсказку по изменению цвета. |
| `color list` | Показывает список цветовых кодов `0..15`. |
| `color <число>` | Меняет цвет интерфейса по номеру, например `color 10`. |
| `color <имя>` | Меняет цвет интерфейса по имени, например `color green`, `color amber`, `color white`, `color cyan`, `color red`, `color blue`. |
| `edit <файл>` | Открывает встроенный RAM-редактор кода. Файл хранится в памяти до перезагрузки. |
| `files` | Показывает RAM-файлы, созданные в редакторе. |
| `show <файл>` | Показывает содержимое RAM-файла с номерами строк. |
| `cat <файл>` | Алиас для `show <файл>`. |
| `rm <файл>` | Удаляет RAM-файл. |
| `reboot` | Перезагружает компьютер через UEFI Runtime Services. |
| `shutdown` | Пытается выключить компьютер через UEFI Runtime Services. Работа зависит от прошивки. |
| `exit` | Выходит из UEFI-приложения обратно в прошивку, если прошивка поддерживает такой возврат. |

### Встроенный редактор UEFI-версии

Открытие редактора:

```text
edit main.c
```

Управление:

| Клавиша | Действие |
|---|---|
| `Стрелки` | Перемещение курсора по тексту. |
| `Home` | Переход в начало строки. |
| `End` | Переход в конец строки. |
| `Enter` | Создать новую строку. |
| `Backspace` | Удалить символ слева от курсора. |
| `Delete` | Удалить символ под курсором. |
| `F7` | Сохранить файл и остаться в редакторе. |
| `F8` | Сохранить файл и выйти. |
| `F9` | Выйти без сохранения. |
| `Ctrl+C` | Отмена/выход без сохранения из редактора. |

### Быстрая проверка UEFI-системы

```text
ver
fetch
commands
color list
color 10
color green
mode
mode list
mode large
edit test.c
files
show test.c
cat test.c
rm test.c
clean
```

### Команды Legacy BIOS-ядра

Legacy BIOS-ядро оставлено как запасной вариант для QEMU/CSM. Оно запускается через `prebuilt/sacramentuos.kernel`, `make run-kernel` или `make run-kernel-built`. В Legacy-режиме удалены лишние команды `set`, `get`, `env`, `unset`, `grep`, `head`, `wc`, `sleep`.

| Команда | Что делает |
|---|---|
| `help` | Показывает документацию или справку по конкретной команде: `help calc`. |
| `docs` | Показывает компактную встроенную документацию. |
| `commands` | Показывает список команд сеткой. |
| `find <слово>` | Ищет команду по названию или описанию. |
| `clear` | Очищает экран. |
| `clean` | Алиас для `clear`. Заменяет старый `cls`. |
| `fetch` | Показывает системную информацию Legacy-ядра. |
| `sysinfo` | Алиас для `fetch`. |
| `about` | Кратко описывает систему. |
| `echo <текст>` | Печатает текст обратно в консоль. |
| `uname` | Показывает имя ОС, версию и архитектуру. |
| `whoami` | Показывает текущего пользователя. |
| `mem` | Показывает память, полученную от Multiboot. |
| `uptime` | Показывает время работы системы. |
| `ticks` | Показывает сырые тики PIT-таймера. |
| `date` | Показывает дату RTC. |
| `time` | Показывает время RTC. |
| `calc <a> <оператор> <b>` | Выполняет простые вычисления: `calc 40 + 2`. |
| `color` | Показывает подсказку по цветам. |
| `color list` | Показывает цветовые коды VGA `0..15`. |
| `color <число>` | Меняет цвет по номеру, например `color 10`. |
| `color <имя>` | Меняет цвет по имени, например `color green`, `color amber`, `color cyan`. |
| `theme <тема>` | Быстро выбирает тему: `theme green`, `theme amber`, `theme white`, `theme cyan`. |
| `history` | Показывает историю введённых команд. |
| `len <текст>` | Считает символы в тексте. |
| `upper <текст>` | Переводит текст в верхний регистр. |
| `lower <текст>` | Переводит текст в нижний регистр. |
| `reverse <текст>` | Печатает текст наоборот. |
| `repeat <число> <текст>` | Повторяет текст заданное число раз. |
| `ascii` | Показывает таблицу печатных ASCII-символов. |
| `hexdump <текст>` | Показывает байты текста в шестнадцатеричном виде. |
| `ls` | Показывает виртуальные файлы и RAM-файлы. |
| `cat <путь>` | Показывает виртуальный или RAM-файл, например `cat /etc/os-release`. |
| `pwd` | Показывает текущий виртуальный путь. |
| `ver` | Показывает название ОС, версию, дату последней сборки и дату последнего обновления. |
| `note add <текст>` | Добавляет RAM-заметку. |
| `note list` | Показывает RAM-заметки. |
| `note clear` | Очищает RAM-заметки. |
| `notes` | Алиас для просмотра заметок. |
| `noteclr` | Алиас для очистки заметок. |
| `todo add <текст>` | Добавляет задачу в RAM-список. |
| `todo list` | Показывает список задач. |
| `todo clear` | Очищает список задач. |
| `done <номер>` | Помечает задачу выполненной. |
| `todoclr` | Алиас для очистки задач. |
| `base <число>` | Показывает число в десятичном, шестнадцатеричном и двоичном виде. |
| `hash <текст>` | Считает FNV-1a checksum текста. |
| `rot13 <текст>` | Применяет ROT13 к тексту. |
| `count <текст>` | Считает символы и слова. |
| `edit <файл>` | Открывает полноэкранный RAM-редактор. |
| `files` | Показывает RAM-файлы редактора. |
| `show <файл>` | Показывает RAM-файл. |
| `rm <файл>` | Удаляет RAM-файл. |
| `ps` | Показывает демонстрационную таблицу задач ядра. |
| `top` | Показывает краткий системный снимок. |
| `kernel` | Показывает диагностическую информацию ядра. |
| `log` | Показывает shell-log. |
| `reboot` | Перезагружает машину. |
| `halt` | Останавливает CPU. |
| `shutdown` | Алиас для `halt`. |

## English section

SacramentuOS is an educational console system. The main build for modern PCs is the native UEFI boot path through `flash/EFI/BOOT/BOOTX64.EFI`. A UEFI ISO is not required: `BOOTX64.EFI` contains the SacramentuOS UEFI system, shell, documentation, RAM files and built-in editor.

### Project layout

```text
SacramentuOS/
├─ src/                 source code
│  └─ uefi/             pure UEFI source
├─ include/             Legacy BIOS kernel headers
├─ boot/grub/           GRUB config for Legacy ISO
├─ prebuilt/            ready binaries
│  ├─ BOOTX64.EFI       main UEFI system
│  └─ sacramentuos.kernel fallback Legacy BIOS kernel
├─ flash/               ready FAT32 USB tree
├─ docs/                short documentation
├─ scripts/             helper scripts
├─ Makefile
└─ README.md
```

### Copying to a USB drive

Format the USB drive as `FAT32`. Then copy the **contents of `flash/`** to the USB root. The USB root should look like this:

```text
USB:\
├─ EFI\BOOT\BOOTX64.EFI
├─ SacramentuOS\SYSTEM\VERSION.txt
├─ SacramentuOS\SYSTEM\TEST_COMMANDS.txt
└─ README_FIRST_RU.txt
```

Recommended firmware settings:

```text
Secure Boot: Disabled
Fast Boot: Disabled
Boot Mode: UEFI
CSM/Legacy: may stay Disabled
```

In the boot menu, select `UEFI: <USB drive name>`.

### Main UEFI system commands

| Command | Description |
|---|---|
| `help` | Shows compact built-in help. |
| `docs` | Alias for `help`. Opens the same built-in documentation. |
| `commands` | Shows a short list of available commands. |
| `clear` | Clears the console screen. |
| `clean` | Alias for `clear`. Replaces the old `cls`. |
| `ver` | Shows the OS name, version, last build date and last update date. |
| `fetch` | Shows a compact system card: version, build, user, console mode, boot path, input and editor. |
| `whoami` | Shows the username selected at startup. |
| `time` | Shows current date and time through UEFI Runtime Services. |
| `date` | Alias for `time`. Shows date and time. |
| `mode` | Shows the current safe text console or active framebuffer console. |
| `mode list` | Shows the current safe mode. Unsafe UEFI video-mode probing is disabled to prevent freezes on real firmware. |
| `mode auto` | Enables the large framebuffer console through the current UEFI GOP mode without unsafe QueryMode/SetMode probing. |
| `mode large` | Enables a large framebuffer console based on the current monitor resolution if GOP is available. |
| `mode safe` | Returns to the safe firmware text console. |
| `color` | Shows color usage. |
| `color list` | Lists color codes `0..15`. |
| `color <number>` | Changes UI color by number, for example `color 10`. |
| `color <name>` | Changes UI color by name, for example `color green`, `color amber`, `color white`, `color cyan`, `color red`, `color blue`. |
| `edit <file>` | Opens the built-in RAM code editor. The file exists in memory until reboot. |
| `files` | Lists RAM files created in the editor. |
| `show <file>` | Shows a RAM file with line numbers. |
| `cat <file>` | Alias for `show <file>`. |
| `rm <file>` | Deletes a RAM file. |
| `reboot` | Reboots through UEFI Runtime Services. |
| `shutdown` | Attempts to power off through UEFI Runtime Services. Firmware support may vary. |
| `exit` | Exits the UEFI application back to firmware if supported. |

### Built-in UEFI editor

Open the editor:

```text
edit main.c
```

Controls:

| Key | Action |
|---|---|
| `Arrow keys` | Move through the text. |
| `Home` | Move to the start of the line. |
| `End` | Move to the end of the line. |
| `Enter` | Create a new line. |
| `Backspace` | Delete the character to the left. |
| `Delete` | Delete the character under the cursor. |
| `F7` | Save and stay in the editor. |
| `F8` | Save and exit. |
| `F9` | Exit without saving. |
| `Ctrl+C` | Cancel/exit without saving. |

### Quick UEFI system test

```text
ver
fetch
commands
color list
color 10
color green
mode
mode list
mode large
edit test.c
files
show test.c
cat test.c
rm test.c
clean
```

### Legacy BIOS kernel commands

The Legacy BIOS kernel is kept as a fallback for QEMU/CSM. It can be launched through `prebuilt/sacramentuos.kernel`, `make run-kernel` or `make run-kernel-built`. The unnecessary commands `set`, `get`, `env`, `unset`, `grep`, `head`, `wc`, and `sleep` were removed from Legacy mode too.

| Command | Description |
|---|---|
| `help` | Shows documentation or focused help: `help calc`. |
| `docs` | Shows compact built-in documentation. |
| `commands` | Shows the command list as a grid. |
| `find <word>` | Searches command names and descriptions. |
| `clear` | Clears the screen. |
| `clean` | Alias for `clear`. Replaces the old `cls`. |
| `fetch` | Shows Legacy kernel system information. |
| `sysinfo` | Alias for `fetch`. |
| `about` | Shows a short system description. |
| `echo <text>` | Prints text back to the console. |
| `uname` | Shows OS name, version and architecture. |
| `whoami` | Shows current user. |
| `mem` | Shows memory reported by Multiboot. |
| `uptime` | Shows system uptime. |
| `ticks` | Shows raw PIT ticks. |
| `date` | Shows RTC date. |
| `time` | Shows RTC time. |
| `calc <a> <operator> <b>` | Performs simple arithmetic: `calc 40 + 2`. |
| `color` | Shows color usage. |
| `color list` | Lists VGA color codes `0..15`. |
| `color <number>` | Changes color by number, for example `color 10`. |
| `color <name>` | Changes color by name, for example `color green`, `color amber`, `color cyan`. |
| `theme <theme>` | Selects a quick theme: `theme green`, `theme amber`, `theme white`, `theme cyan`. |
| `history` | Shows typed command history. |
| `len <text>` | Counts characters. |
| `upper <text>` | Converts text to uppercase. |
| `lower <text>` | Converts text to lowercase. |
| `reverse <text>` | Prints text in reverse. |
| `repeat <count> <text>` | Repeats text. |
| `ascii` | Shows printable ASCII characters. |
| `hexdump <text>` | Shows text bytes in hexadecimal. |
| `ls` | Lists virtual files and RAM files. |
| `cat <path>` | Displays a virtual or RAM file, for example `cat /etc/os-release`. |
| `pwd` | Shows current virtual path. |
| `ver` | Shows OS name, version, last build date and last update date. |
| `note add <text>` | Adds a RAM note. |
| `note list` | Lists RAM notes. |
| `note clear` | Clears RAM notes. |
| `notes` | Alias for listing notes. |
| `noteclr` | Alias for clearing notes. |
| `todo add <text>` | Adds a RAM todo item. |
| `todo list` | Lists todo items. |
| `todo clear` | Clears todo items. |
| `done <number>` | Marks a todo item as done. |
| `todoclr` | Alias for clearing todo items. |
| `base <number>` | Shows a number in decimal, hexadecimal and binary. |
| `hash <text>` | Calculates an FNV-1a checksum. |
| `rot13 <text>` | Applies ROT13. |
| `count <text>` | Counts characters and words. |
| `edit <file>` | Opens the full-screen RAM editor. |
| `files` | Lists editor RAM files. |
| `show <file>` | Shows a RAM file. |
| `rm <file>` | Deletes a RAM file. |
| `ps` | Shows a demonstration kernel task table. |
| `top` | Shows a compact system snapshot. |
| `kernel` | Shows kernel diagnostics. |
| `log` | Shows the shell log. |
| `reboot` | Reboots the machine. |
| `halt` | Halts the CPU. |
| `shutdown` | Alias for `halt`. |
