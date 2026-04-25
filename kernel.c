/*
 * kernel.c - SacramentuOS v0.0.2
 * Создатель: tikzek
 * GitHub: https://github.com/tikzek1337/SacramentuOS
 * Telegram: t.me/SacramentuOS
 */

/* ======================== Порты ввода-вывода ======================== */
static inline unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}
static inline void outb(unsigned short port, unsigned char data) {
    __asm__ __volatile__ ("outb %0, %1" : : "a"(data), "Nd"(port));
}
static inline void outw(unsigned short port, unsigned short data) {
    __asm__ __volatile__ ("outw %0, %1" : : "a"(data), "Nd"(port));
}

/* ======================== VGA ======================== */
#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH   80
#define VGA_HEIGHT  25
static unsigned int cursor = 0;
static unsigned char current_color = 0x0F;  // белый на чёрном

void clear_screen() {
    volatile char *vid = (volatile char*) VGA_ADDRESS;
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT * 2; i += 2) {
        vid[i]     = ' ';
        vid[i + 1] = current_color;
    }
    cursor = 0;
}

void scroll_screen() {
    volatile char *vid = (volatile char*) VGA_ADDRESS;
    for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH * 2; i++)
        vid[i] = vid[i + VGA_WIDTH * 2];
    int last = (VGA_HEIGHT - 1) * VGA_WIDTH * 2;
    for (int i = 0; i < VGA_WIDTH * 2; i += 2) {
        vid[last + i]     = ' ';
        vid[last + i + 1] = current_color;
    }
    cursor -= VGA_WIDTH * 2;
}

void putchar(char c) {
    volatile char *vid = (volatile char*) VGA_ADDRESS;
    if (c == '\n') {
        cursor = (cursor / (VGA_WIDTH * 2) + 1) * (VGA_WIDTH * 2);
    } else if (c == '\b') {
        if (cursor > 0) {
            cursor -= 2;
            vid[cursor]     = ' ';
            vid[cursor + 1] = current_color;
        }
    } else {
        vid[cursor]     = c;
        vid[cursor + 1] = current_color;
        cursor += 2;
    }
    if (cursor >= VGA_WIDTH * VGA_HEIGHT * 2) scroll_screen();
}

void print(const char *str) { while (*str) putchar(*str++); }

void set_color(unsigned char fg, unsigned char bg) {
    current_color = (bg << 4) | (fg & 0x0F);
}

/* ======================== Клавиатура ======================== */
#define KEYBOARD_DATA_PORT   0x60
#define KEYBOARD_STATUS_PORT 0x64

static char scancode_to_ascii[128] = {
    0, 0, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0, 'a','s','d','f','g','h','j','k','l',';','\'','`',
    0, '\\','z','x','c','v','b','n','m',',','.','/', 0,
    '*', 0, ' ', 0
};

char getchar() {
    unsigned char sc;
    while (1) {
        while (!(inb(KEYBOARD_STATUS_PORT) & 0x01));
        sc = inb(KEYBOARD_DATA_PORT);
        if (sc & 0x80) continue;
        if (sc < 128) {
            char ascii = scancode_to_ascii[sc];
            if (ascii != 0) return ascii;
        }
    }
}

void readline(char *buffer, int max_len) {
    int i = 0;
    while (i < max_len - 1) {
        char c = getchar();
        if (c == '\n') { buffer[i] = '\0'; putchar('\n'); return; }
        if (c == '\b') { if (i > 0) { i--; putchar('\b'); } }
        else { buffer[i++] = c; putchar(c); }
    }
    buffer[i] = '\0'; putchar('\n');
}

/* ======================== PIT (таймер) ======================== */
static volatile unsigned long ticks = 0;

void pit_init(unsigned int freq) {
    unsigned int divisor = 1193180 / freq;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

unsigned long get_ticks() { return ticks; }

void timer_handler() { ticks++; }

/* ======================== Память ======================== */
typedef struct {
    unsigned int base_low, base_high;
    unsigned int length_low, length_high;
    unsigned int type;
} mem_entry_t;

static unsigned int total_mem = 0;
static unsigned int free_pages = 0;

void memory_init(mem_entry_t *map, int entries) {
    unsigned long max_len = 0;
    for (int i = 0; i < entries; i++) {
        if (map[i].type == 1) {
            unsigned long len = ((unsigned long)map[i].length_high << 16) | (map[i].length_low & 0xFFFF);
            if (len > max_len) max_len = len;
        }
    }
    total_mem = max_len / 1024 / 1024;
    free_pages = max_len / 4096;
}

/* ======================== Строковые функции ======================== */
int strcmp(const char *a, const char *b) {
    while (*a && *a == *b) { a++; b++; }
    return *(unsigned char*)a - *(unsigned char*)b;
}

static char username[32] = "user";

/* ======================== Команды ======================== */
void command_fetch() {
    print("SacramentuOS v0.0.2\n");
    print("   SSSSS  \n");
    print("  S     S \n");
    print("  S       \n");
    print("   SSSSS  \n");
    print("        S \n");
    print("  S     S \n");
    print("   SSSSS  \n");
    print("----------------------------\n");
    print("Creator: tikzek\n");
    print("Kernel: 32-bit protected mode\n");
    print("Current color: fg=");
    char fg = current_color & 0x0F;
    char bg = (current_color >> 4) & 0x0F;
    char buf[8];
    buf[0] = '0' + fg;
    buf[1] = ' ';
    buf[2] = 'b';
    buf[3] = 'g';
    buf[4] = '=';
    buf[5] = '0' + bg;
    buf[6] = '\0';
    print(buf);
    print("\nShell: MyShell v0.0.2\n");
}

void command_clear() { clear_screen(); }

void command_color(char *arg) {
    if (strcmp(arg, "list") == 0) {
        print("Available colors (0-15):\n");
        print("0 Black  4 Red       8 Dark Gray   12 Light Red\n");
        print("1 Blue   5 Magenta   9 Light Blue 13 Light Magenta\n");
        print("2 Green  6 Brown    10 Light Green 14 Yellow\n");
        print("3 Cyan   7 Gray     11 Light Cyan  15 White\n");
        print("Usage: color <fg> <bg>  (e.g., color 2 0)\n");
        return;
    }
    if (arg[0] == '\0') { print("Usage: color <fg> <bg> | color list\n"); return; }
    char fg_str[3] = {0}, bg_str[3] = {0};
    int i = 0;
    while (arg[i] && arg[i] != ' ') { fg_str[i] = arg[i]; i++; }
    if (arg[i] == ' ') i++;
    int j = 0;
    while (arg[i] && j < 2) { bg_str[j] = arg[i]; i++; j++; }
    unsigned char fg = fg_str[0] - '0';
    unsigned char bg = bg_str[0] - '0';
    set_color(fg, bg);
    clear_screen();
    print("Color changed.\n");
}

void command_uptime() {
    unsigned long secs = ticks / 1000;
    print("Uptime: ");
    int hrs = secs / 3600, mins = (secs % 3600) / 60, sec = secs % 60;
    char t[9];
    t[0] = '0' + hrs / 10; t[1] = '0' + hrs % 10; t[2] = ':';
    t[3] = '0' + mins / 10; t[4] = '0' + mins % 10; t[5] = ':';
    t[6] = '0' + sec / 10; t[7] = '0' + sec % 10; t[8] = '\0';
    print(t);
    print("\n");
}

void command_whoami() { print(username); print("\n"); }

void command_ram() {
    print("Total RAM: ~");
    if (total_mem == 0) { print("?\n"); return; }
    char buf[16];
    int n = total_mem;
    int idx = 0;
    int div = 1000000000;
    while (div > 1 && n / div == 0) div /= 10;
    while (div > 0) {
        buf[idx++] = '0' + (n / div) % 10;
        div /= 10;
    }
    buf[idx] = '\0';
    print(buf);
    print(" MB\nFree: ~");
    n = free_pages * 4 / 1024;
    div = 1000000000;
    while (div > 1 && n / div == 0) div /= 10;
    idx = 0;
    while (div > 0) {
        buf[idx++] = '0' + (n / div) % 10;
        div /= 10;
    }
    buf[idx] = '\0';
    print(buf);
    print(" MB\n");
}

void command_docs() {
    print("SacramentuOS Documentation (v0.0.2)\n");
    print("GitHub: https://github.com/tikzek1337/SacramentuOS\n");
    print("Telegram: t.me/SacramentuOS\n");
    print("Commands:\n");
    print("  fetch  - system information\n");
    print("  clear  - clear screen\n");
    print("  color  - color <fg> <bg> | color list\n");
    print("  uptime - time since boot\n");
    print("  whoami - current user\n");
    print("  ram    - memory info\n");
    print("  docs   - this help\n");
    print("  reboot - restart\n");
    print("  shutdown - power off\n");
}

void command_shutdown() {
    print("Shutting down...\n");
    outw(0x604, 0x2000);
    while (1) __asm__("hlt");
}

void command_reboot() {
    print("Rebooting...\n");
    unsigned char s;
    do { s = inb(0x64); } while (s & 0x02);
    outb(0x64, 0xFE);
    while (1) __asm__("hlt");
}

/* ======================== Оболочка ======================== */
void shell() {
    clear_screen();
    print("Welcome to SacramentuOS!\n");
    print("Please enter your username: ");
    readline(username, 32);
    print("Hello, ");
    print(username);
    print("!\n\n");

    while (1) {
        print("> ");
        char input[256];
        readline(input, 256);
        if (input[0] == '\0') continue;

        char *arg = input;
        while (*arg && *arg != ' ') arg++;
        if (*arg == ' ') { *arg = '\0'; arg++; } else arg = "";

        if (strcmp(input, "fetch") == 0) command_fetch();
        else if (strcmp(input, "clear") == 0) command_clear();
        else if (strcmp(input, "color") == 0) command_color(arg);
        else if (strcmp(input, "uptime") == 0) command_uptime();
        else if (strcmp(input, "whoami") == 0) command_whoami();
        else if (strcmp(input, "ram") == 0) command_ram();
        else if (strcmp(input, "docs") == 0) command_docs();
        else if (strcmp(input, "reboot") == 0) command_reboot();
        else if (strcmp(input, "shutdown") == 0) command_shutdown();
        else { print("Unknown command. Type 'docs'.\n"); }
    }
}

/* ======================== Точка входа ======================== */
void kmain() {
    mem_entry_t *map = (mem_entry_t*)(0x5000 + 2);
    int count = *(unsigned short*)0x5000;
    memory_init(map, count);
    pit_init(1000);
    shell();
}