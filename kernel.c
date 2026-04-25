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

#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_ATTR    0x0F

#define KEYBOARD_DATA_PORT   0x60
#define KEYBOARD_STATUS_PORT 0x64

static unsigned int cursor = 0;

void clear_screen() {
    volatile char *vid = (volatile char*) VGA_ADDRESS;
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT * 2; i += 2) {
        vid[i]     = ' ';
        vid[i + 1] = VGA_ATTR;
    }
    cursor = 0;
}

void scroll_screen() {
    volatile char *vid = (volatile char*) VGA_ADDRESS;
    for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH * 2; i++) {
        vid[i] = vid[i + VGA_WIDTH * 2];
    }
    int last = (VGA_HEIGHT - 1) * VGA_WIDTH * 2;
    for (int i = 0; i < VGA_WIDTH * 2; i += 2) {
        vid[last + i]     = ' ';
        vid[last + i + 1] = VGA_ATTR;
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
            vid[cursor + 1] = VGA_ATTR;
        }
    } else {
        vid[cursor]     = c;
        vid[cursor + 1] = VGA_ATTR;
        cursor += 2;
    }
    if (cursor >= VGA_WIDTH * VGA_HEIGHT * 2) scroll_screen();
}

void print(const char *str) {
    while (*str) putchar(*str++);
}

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
    buffer[i] = '\0';
    putchar('\n');
}

int strcmp(const char *a, const char *b) {
    while (*a && *a == *b) { a++; b++; }
    return *(unsigned char*)a - *(unsigned char*)b;
}

void command_fetch() {
    print("SacramentuOS v0.0.1 (x86)\n");
    print("  @@@   @@@ @@@   @@@\n");
    print("  @@@@  @@@ @@@@  @@@\n");
    print("  @@ @@ @@@ @@ @@ @@@\n");
    print("  @@  @@@@@ @@  @@@@@\n");
    print("  @@   @@@@ @@   @@@@\n");
    print("  @@    @@@ @@    @@@\n");
    print("----------------------------\n");
    print("Kernel: 32-bit protected mode\n");
    print("Memory: 4 GB (flat model)\n");
    print("Shell:  SacramentuShell v0.0.1\n");
}

void command_apps() {
    print("Installed applications:\n");
    print("  - calculator     (Simple calculator)\n");
    print("  - texteditor     (Plain text editor)\n");
    print("  - filemanager    (File browser)\n");
    print("Type 'packages <app>' to see dependencies.\n");
}

void command_packages(const char *arg) {
    if (arg[0] == '\0') { print("Usage: packages <app>\n"); return; }
    if (strcmp(arg, "calculator") == 0) {
        print("calculator dependencies: libmath, libgui\n");
    } else if (strcmp(arg, "texteditor") == 0) {
        print("texteditor dependencies: libtext, libio\n");
    } else if (strcmp(arg, "filemanager") == 0) {
        print("filemanager dependencies: libfs, libgui\n");
    } else {
        print("No such application.\n");
    }
}

void command_docs() {
    print("MyOS Documentation:\n");
    print("Commands: fetch, apps, packages, docs, reboot, shutdown.\n");
    print("See README.md for detailed guide.\n");
}

void command_shutdown() {
    print("Shutting down...\n");
    outw(0x604, 0x2000);
    while (1) { __asm__("hlt"); }
}

void command_reboot() {
    print("Rebooting...\n");
    unsigned char s;
    do { s = inb(0x64); } while (s & 0x02);
    outb(0x64, 0xFE);
    while (1) { __asm__("hlt"); }
}

void shell() {
    char input[128];
    print("Welcome to MyOS!\n\n");
    while (1) {
        print("> ");
        readline(input, 128);
        if (input[0] == '\0') continue;

        char *arg = input;
        while (*arg && *arg != ' ') arg++;
        if (*arg == ' ') { *arg = '\0'; arg++; } else arg = "";

        if (strcmp(input, "fetch") == 0)         command_fetch();
        else if (strcmp(input, "apps") == 0)     command_apps();
        else if (strcmp(input, "packages") == 0) command_packages(arg);
        else if (strcmp(input, "docs") == 0)     command_docs();
        else if (strcmp(input, "reboot") == 0)   command_reboot();
        else if (strcmp(input, "shutdown") == 0) command_shutdown();
        else { print("Unknown command. Type 'docs'.\n"); }
    }
}

void kmain() {
    clear_screen();
    shell();
}