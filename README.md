# SacramentuOS 0.2.3 Compact Help IDE Keys

Учебная 32-bit x86 ОС на C/GNU Assembly: Multiboot-ядро, VGA-консоль, PIT-таймер, PS/2-клавиатура и shell.

## Что изменено в 0.2.3

- `help` и `docs` теперь показывают команды компактно: меньше пустых строк, меньше вложенных подкатегорий, больше полезного текста на одном экране.
- `commands` теперь выводит команды плотной сеткой по 8 команд в строке.
- Редактор `edit` заменён на полноэкранный RAM-редактор.
- В редакторе работают стрелки: перемещение по строкам и символам.
- В редакторе работают `Enter`, `Backspace`, `Delete`, `Home`, `End`.
- Горячие клавиши редактора: `F7` — сохранить, `F8` — сохранить и выйти, `F9` — выйти без сохранения.
- Добавлена обработка `Ctrl+C`: можно отменять ввод в shell, выходить из редактора, отменять `sleep`.
- Лимиты редактора увеличены: 8 RAM-файлов, 64 строки на файл, 119 символов на строку.

## Быстрый запуск на Windows 11 через WSL

```bash
sudo apt update
sudo apt install -y build-essential gcc-multilib make qemu-system-x86
cd /mnt/c/Users/<ТВОЙ_ПОЛЬЗОВАТЕЛЬ>/Downloads/SacramentuOS
make run-kernel
```

Полная пересборка из исходников:

```bash
make clean
make CC=gcc run-kernel-built
```

Если ISO зависает на `Booting from DVD/CD...`, запускай напрямую ядро:

```bash
make run-kernel
```

## Проверка внутри ОС

```text
help
docs
commands
sleep 10000
# нажать Ctrl+C
edit main.c
# написать код, подвигаться стрелками, F7 сохранить, F8 сохранить и выйти
files
show main.c
grep return main.c
```

## Документация

- `docs/TEST_WINDOWS_11_RU.txt` — подробная проверка на Windows 11.
- `docs/FIX_BOOT_WINDOWS_11_RU.txt` — исправление зависания DVD/CD.
- `docs/BUILD_RU.txt` — сборка проекта.
- `docs/CHANGELOG_RU.txt` — список изменений.
