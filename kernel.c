/*
 * SacramentuOS v0.0.4 final (без калькулятора)
 * GitHub: https://github.com/tikzek1337/SacramentuOS
 * Telegram: t.me/SacramentuOS
 */

/* ---------- Hardware ports ---------- */
static inline unsigned char inb(unsigned short port) {
    unsigned char r;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(r) : "Nd"(port));
    return r;
}
static inline void outb(unsigned short port, unsigned char data) {
    __asm__ __volatile__ ("outb %0, %1" : : "a"(data), "Nd"(port));
}
static inline void outw(unsigned short port, unsigned short data) {
    __asm__ __volatile__ ("outw %0, %1" : : "a"(data), "Nd"(port));
}

/* ---------- 64-bit division helpers (нужны для ull_to_str) ---------- */
unsigned long long __udivdi3(unsigned long long n, unsigned long long d) {
    if (d == 0) return 0;
    unsigned long long q = 0;
    while (n >= d) {
        unsigned long long t = d, m = 1;
        while ((t << 1) <= n) { t <<= 1; m <<= 1; }
        q += m;
        n -= t;
    }
    return q;
}
unsigned long long __umoddi3(unsigned long long n, unsigned long long d) {
    if (d == 0) return 0;
    while (n >= d) {
        unsigned long long t = d;
        while ((t << 1) <= n) t <<= 1;
        n -= t;
    }
    return n;
}

/* ---------- VGA text mode ---------- */
#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH   80
#define VGA_HEIGHT  25
static unsigned int cursor = 0;
static unsigned char color = 0x0F;   // white on black by default

void clear_screen() {
    volatile char *vid = (volatile char*) VGA_ADDRESS;
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT * 2; i += 2) {
        vid[i] = ' ';
        vid[i+1] = color;
    }
    cursor = 0;
}
void scroll_screen() {
    volatile char *vid = (volatile char*) VGA_ADDRESS;
    for (int i = 0; i < (VGA_HEIGHT-1)*VGA_WIDTH*2; i++)
        vid[i] = vid[i + VGA_WIDTH*2];
    int last = (VGA_HEIGHT-1)*VGA_WIDTH*2;
    for (int i = 0; i < VGA_WIDTH*2; i += 2) {
        vid[last+i] = ' ';
        vid[last+i+1] = color;
    }
    cursor -= VGA_WIDTH*2;
}
void putchar(char c) {
    volatile char *vid = (volatile char*) VGA_ADDRESS;
    if (c == '\n') {
        cursor = (cursor / (VGA_WIDTH*2) + 1) * (VGA_WIDTH*2);
    } else if (c == '\b') {
        if (cursor > 0) {
            cursor -= 2;
            vid[cursor] = ' ';
            vid[cursor+1] = color;
        }
    } else {
        vid[cursor] = c;
        vid[cursor+1] = color;
        cursor += 2;
    }
    if (cursor >= VGA_WIDTH*VGA_HEIGHT*2) scroll_screen();
}
void print(const char *str) { while (*str) putchar(*str++); }

/* set color – only text color, background stays black (0) */
void set_color(unsigned char fg) {
    color = fg & 0x0F;          // low nibble = foreground
}

/* ---------- Keyboard ---------- */
#define KEYBOARD_DATA_PORT   0x60
#define KEYBOARD_STATUS_PORT 0x64

static char scancode_to_ascii[128] = {
    0,0,'1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/',0,
    '*',0,' ',0
};

int getchar() {
    unsigned char sc;
    while (1) {
        while (!(inb(KEYBOARD_STATUS_PORT) & 0x01));
        sc = inb(KEYBOARD_DATA_PORT);
        if (sc & 0x80) continue;
        if (sc == 0x01) return 0x1B;  // Esc
        if (sc >= 0x3B && sc <= 0x44) // F1-F10
            return (int)(sc - 0x3B) + 0x80;
        if (sc == 0xE0) {
            while (!(inb(KEYBOARD_STATUS_PORT) & 0x01));
            sc = inb(KEYBOARD_DATA_PORT);
            switch(sc) {
                case 0x48: return 0x90; // up
                case 0x50: return 0x91; // down
                case 0x4B: return 0x92; // left
                case 0x4D: return 0x93; // right
            }
        }
        if (sc < 128) {
            char ascii = scancode_to_ascii[sc];
            if (ascii != 0) return (unsigned char)ascii;
        }
    }
}

void readline(char *buf, int max) {
    int i = 0;
    while (i < max-1) {
        int c = getchar();
        if (c == '\n') { buf[i] = '\0'; putchar('\n'); return; }
        if (c == '\b') {
            if (i > 0) { i--; putchar('\b'); }
        } else if (c >= ' ' && c <= '~') {
            buf[i++] = (char)c;
            putchar((char)c);
        }
    }
    buf[i] = '\0';
    putchar('\n');
}

/* ---------- IDT / PIC / Timer ---------- */
#define IDT_SIZE 256
typedef struct {
    unsigned short base_low;
    unsigned short sel;
    unsigned char zero;
    unsigned char flags;
    unsigned short base_high;
} __attribute__((packed)) idt_entry_t;

typedef struct {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed)) idt_ptr_t;

static idt_entry_t idt[IDT_SIZE];
static idt_ptr_t idt_ptr;

extern void idt_load(idt_ptr_t*);
extern void irq0_stub();

void idt_set_gate(int n, unsigned int handler, unsigned short sel, unsigned char flags) {
    idt[n].base_low = handler & 0xFFFF;
    idt[n].base_high = (handler >> 16) & 0xFFFF;
    idt[n].sel = sel;
    idt[n].zero = 0;
    idt[n].flags = flags | 0x60;
}

void init_pic() {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0xFF);   // mask all IRQs
    outb(0xA1, 0xFF);
}
void enable_irq(int irq) {
    unsigned short port = 0x21;
    if (irq >= 8) { port = 0xA1; irq -= 8; }
    outb(port, inb(port) & ~(1 << irq));
}

static volatile unsigned long ticks = 0;
void irq0_handler() { ticks++; }

void pit_init(unsigned int freq) {
    unsigned int div = 1193180 / freq;
    outb(0x43, 0x36);
    outb(0x40, div & 0xFF);
    outb(0x40, (div >> 8) & 0xFF);
    enable_irq(0);
}
unsigned long get_ticks() { return ticks; }

/* ---------- Memory ---------- */
typedef struct {
    unsigned int base_low, base_high;
    unsigned int length_low, length_high;
    unsigned int type;
} mem_entry_t;

static unsigned int total_mem = 0;
static unsigned int used_mem = 0;   // system usage in MB (approximate)

void memory_init(mem_entry_t *map, int entries) {
    unsigned long max_len = 0;
    for (int i = 0; i < entries; i++) {
        if (map[i].type == 1) {
            unsigned long len = ((unsigned long)map[i].length_high << 16) | map[i].length_low;
            if (len > max_len) max_len = len;
        }
    }
    total_mem = max_len / (1024*1024);
    used_mem = 2;                   // kernel + stack ≈ 2 MB
}

/* ---------- String utilities ---------- */
int strcmp(const char *a, const char *b) {
    while (*a && *a == *b) { a++; b++; }
    return *(unsigned char*)a - *(unsigned char*)b;
}
int strncmp(const char *a, const char *b, int n) {
    for (int i=0; i<n; i++) {
        if (a[i] != b[i]) return a[i] - b[i];
        if (a[i] == '\0') return 0;
    }
    return 0;
}
void int_to_str(unsigned int n, char *out) {
    int idx = 0;
    if (n == 0) { out[idx++] = '0'; out[idx] = 0; return; }
    char tmp[12]; int cnt = 0;
    while (n > 0) { tmp[cnt++] = '0' + (n % 10); n /= 10; }
    while (cnt > 0) out[idx++] = tmp[--cnt];
    out[idx] = '\0';
}
void ull_to_str(unsigned long long n, char *out) {
    int idx = 0;
    if (n == 0) { out[idx++] = '0'; out[idx] = 0; return; }
    char tmp[21]; int cnt = 0;
    while (n > 0) { tmp[cnt++] = '0' + (n % 10); n /= 10; }
    while (cnt > 0) out[idx++] = tmp[--cnt];
    out[idx] = '\0';
}

/* ---------- History ---------- */
#define HISTORY_MAX 32
static char history[HISTORY_MAX][128];
static unsigned long history_start[HISTORY_MAX];
static unsigned long history_dur[HISTORY_MAX];
static int history_count = 0;

void add_history(const char *cmd, unsigned long start, unsigned long dur) {
    if (history_count < HISTORY_MAX) {
        int i=0;
        while (cmd[i] && i<127) { history[history_count][i] = cmd[i]; i++; }
        history[history_count][i] = '\0';
        history_start[history_count] = start;
        history_dur[history_count] = dur;
        history_count++;
    }
}
void print_history() {
    for (int i=0; i<history_count; i++) {
        char num[4]; int_to_str(i+1, num);
        print(num); print("  ");
        unsigned long s = history_start[i]/1000;
        int h = s/3600, m = (s%3600)/60, sec = s%60;
        char t[9];
        t[0] = '0'+h/10; t[1]='0'+h%10; t[2]=':';
        t[3] = '0'+m/10; t[4]='0'+m%10; t[5]=':';
        t[6] = '0'+sec/10; t[7]='0'+sec%10; t[8]='\0';
        print(t); print(" ");
        print(history[i]); print(" ");
        unsigned long dur = history_dur[i];
        if (dur > 1000) {
            print("(");
            char buf[8];
            int_to_str(dur/1000, buf); print(buf); print(".");
            int_to_str((dur%1000)/10, buf); print(buf); print("s)");
        } else {
            print("(0)");
        }
        print("\n");
    }
}

/* ---------- Rainbow text ---------- */
void print_rainbow(const char *str) {
    unsigned char rain[] = {4,6,2,3,1,5,14}; // red, brown, green, cyan, blue, magenta, yellow
    int i = 0;
    while (*str) {
        set_color(rain[i % 7]);
        putchar(*str);
        str++;
        i++;
    }
    set_color(15); // back to white
}

/* ---------- Bookmarks (files) ---------- */
#define BOOK_MAX 16
static char bookmark[BOOK_MAX][64];
static char bookmark_text[BOOK_MAX][1024];
static int bookmark_len[BOOK_MAX];
static int bookmark_count = 0;

int find_bookmark(const char *name) {
    for (int i=0; i<bookmark_count; i++)
        if (strcmp(bookmark[i], name) == 0) return i;
    return -1;
}
void cmd_bookmark_add(char *name) {
    if (bookmark_count >= BOOK_MAX) { print("No free slots.\n"); return; }
    if (find_bookmark(name) >= 0) { print("Already exists.\n"); return; }
    int i=0;
    while (name[i] && i<63) { bookmark[bookmark_count][i] = name[i]; i++; }
    bookmark[bookmark_count][i] = '\0';
    bookmark_text[bookmark_count][0] = '\0';
    bookmark_len[bookmark_count] = 0;
    bookmark_count++;
    print("Bookmark added.\n");
}

void ls() {
    if (bookmark_count == 0) {
        print("No files.\n");
        return;
    }
    int total_size = 0;
    for (int i=0; i<bookmark_count; i++) {
        print(bookmark[i]); print(".ram  ");
        char sz[8]; int_to_str(bookmark_len[i], sz); print(sz); print(" bytes\n");
        total_size += bookmark_len[i];
    }
    print("-----------------------------\n");
    print("Total files: ");
    char cnt[6]; int_to_str(bookmark_count, cnt); print(cnt); print("\n");
    print("Total file size: ");
    char tsz[8]; int_to_str(total_size, tsz); print(tsz); print(" bytes\n");
    print("System memory used: ");
    char mus[8]; int_to_str(used_mem, mus); print(mus); print(" MB\n");
    print("Free memory: ");
    char mfree[8]; int_to_str(total_mem - used_mem, mfree); print(mfree); print(" MB\n");
}

/* ---------- Text Editor (F2 save, Esc exit) ---------- */
#define EDITOR_COLS 78
#define EDITOR_ROWS 20
static char editor_buf[EDITOR_ROWS][EDITOR_COLS+1];

void text_editor(char *name) {
    int bm_idx = find_bookmark(name);
    if (bm_idx < 0) {
        cmd_bookmark_add(name);
        bm_idx = find_bookmark(name);
    }

    int line_count = 0;
    char *src = bookmark_text[bm_idx];
    int src_len = bookmark_len[bm_idx];
    int pos = 0;
    for (int l=0; l<EDITOR_ROWS; l++) {
        int col=0;
        while (col<EDITOR_COLS && pos<src_len && src[pos]!='\n') {
            editor_buf[l][col] = src[pos];
            col++; pos++;
        }
        if (src[pos]=='\n') pos++;
        editor_buf[l][col] = '\0';
        if (col>0 || src[pos]!='\0') line_count = l+1;
        else if (l==0 && col==0) line_count = 1;
    }

    int cy = 0, cx = 0;

    while (1) {
        volatile char *vid = (volatile char*) VGA_ADDRESS;
        for (int r=0; r<EDITOR_ROWS; r++) {
            for (int c=0; c<EDITOR_COLS; c++) {
                unsigned idx = (r+2)*VGA_WIDTH*2 + c*2;
                vid[idx] = ' ';
                vid[idx+1] = color;
            }
        }
        for (int r=0; r<line_count; r++) {
            unsigned idx = (r+2)*VGA_WIDTH*2;
            int c=0;
            while (editor_buf[r][c]) {
                vid[idx + c*2] = editor_buf[r][c];
                vid[idx + c*2 +1] = color;
                c++;
            }
        }
        unsigned cidx = ((cy+2)*VGA_WIDTH + cx)*2;
        vid[cidx+1] = (color & 0xF0) | 0x0E;

        const char *help = "F2 Save   Esc Exit   Arrows";
        int hx = (VGA_WIDTH - 28)/2;
        for (int i=0; help[i]; i++) {
            vid[(24*VGA_WIDTH + hx+i)*2] = help[i];
            vid[(24*VGA_WIDTH + hx+i)*2+1] = 0x07;
        }

        int c = getchar();
        if (c == 0x81) { // F2 -> Save
            int total=0;
            for (int r=0; r<line_count; r++) {
                int len=0;
                while (editor_buf[r][len]) len++;
                for (int i=0; i<len; i++) {
                    if (total<1023) bookmark_text[bm_idx][total++] = editor_buf[r][i];
                }
                if (r < line_count-1 && total<1023) bookmark_text[bm_idx][total++] = '\n';
            }
            bookmark_text[bm_idx][total] = '\0';
            bookmark_len[bm_idx] = total;
            break;
        } else if (c == 0x1B) { // Esc -> Exit
            break;
        } else if (c == '\n') {
            if (line_count < EDITOR_ROWS) {
                for (int r=line_count; r>cy; r--)
                    for (int cc=0; cc<=EDITOR_COLS; cc++) editor_buf[r][cc] = editor_buf[r-1][cc];
                int move = 0;
                for (int cc=cx; cc<EDITOR_COLS; cc++) {
                    editor_buf[cy+1][move] = editor_buf[cy][cc+1];
                    move++;
                }
                editor_buf[cy+1][move] = '\0';
                editor_buf[cy][cx] = '\0';
                line_count++;
                cy++;
                cx = 0;
            }
        } else if (c == '\b') {
            if (cx > 0) {
                for (int cc=cx; cc<=EDITOR_COLS; cc++) editor_buf[cy][cc-1] = editor_buf[cy][cc];
                cx--;
            } else if (cy > 0) {
                int len1=0; while (editor_buf[cy-1][len1]) len1++;
                int len2=0; while (editor_buf[cy][len2]) len2++;
                for (int cc=0; cc<len2; cc++) editor_buf[cy-1][len1+cc] = editor_buf[cy][cc];
                editor_buf[cy-1][len1+len2] = '\0';
                for (int r=cy; r<line_count-1; r++)
                    for (int cc=0; cc<=EDITOR_COLS; cc++) editor_buf[r][cc] = editor_buf[r+1][cc];
                line_count--;
                cy--;
                cx = len1;
            }
        } else if (c >= ' ' && c <= '~') {
            if (cx < EDITOR_COLS) {
                int len = 0; while (editor_buf[cy][len]) len++;
                for (int cc=len; cc>=cx; cc--) editor_buf[cy][cc+1] = editor_buf[cy][cc];
                editor_buf[cy][cx] = (char)c;
                cx++;
            }
        } else if (c == 0x90) { if (cy > 0) cy--;
        } else if (c == 0x91) { if (cy < line_count-1) cy++;
        } else if (c == 0x92) { if (cx > 0) cx--;
        } else if (c == 0x93) { if (editor_buf[cy][cx] && cx<EDITOR_COLS) cx++;
        }
    }
    clear_screen();
}

/* ---------- Version and Drivers ---------- */
void command_version() {
    print("SacramentuOS v0.0.4\n");
    print("Applications: Text Editor v1.0\n");
    print("Drivers: VGA, Keyboard, PIT, Memory, IDT/PIC\n");
}

void command_drivers() {
    print("Loaded drivers:\n");
    print("  VGA text mode\n");
    print("  Keyboard (polling)\n");
    print("  PIT timer (1000 Hz)\n");
    print("  Memory manager\n");
    print("  IDT/PIC\n");
}

/* ---------- Other commands ---------- */
static char username[32] = "user";
static char hostname[32] = "sacramentuos";

void command_fetch() {
    print("SacramentuOS v0.0.4\n");
    print("   SSSSS  \n");
    print("  S     S \n");
    print("  S       \n");
    print("   SSSSS  \n");
    print("        S \n");
    print("  S     S \n");
    print("   SSSSS  \n");
    print("----------------------------\n");
    print("Hostname: "); print(hostname); print("\n");
    print("User: "); print(username); print("\n");
    print("Kernel: 32-bit protected mode\n");
    unsigned long secs = ticks / 1000;
    int hrs = secs/3600, mins = (secs%3600)/60, sec = secs%60;
    char t[9];
    t[0] = '0'+hrs/10; t[1]='0'+hrs%10; t[2]=':';
    t[3] = '0'+mins/10; t[4]='0'+mins%10; t[5]=':';
    t[6] = '0'+sec/10; t[7]='0'+sec%10; t[8]='\0';
    print("Uptime: "); print(t); print("\n");
    print("RAM: "); char buf[8];
    int_to_str(total_mem, buf); print(buf); print(" MB total, ");
    int_to_str(total_mem - used_mem, buf); print(buf); print(" MB free\n");
}

void command_uptime() {
    unsigned long secs = ticks/1000;
    int hrs = secs/3600, mins = (secs%3600)/60, sec = secs%60;
    char t[9];
    t[0] = '0'+hrs/10; t[1]='0'+hrs%10; t[2]=':';
    t[3] = '0'+mins/10; t[4]='0'+mins%10; t[5]=':';
    t[6] = '0'+sec/10; t[7]='0'+sec%10; t[8]='\0';
    print("Uptime: "); print(t); print("\n");
}

void command_ram() {
    print("Total RAM: "); char buf[8];
    int_to_str(total_mem, buf); print(buf); print(" MB\n");
    print("Used by system: "); int_to_str(used_mem, buf); print(buf); print(" MB\n");
    print("Free: "); int_to_str(total_mem - used_mem, buf); print(buf); print(" MB\n");
}

void command_docs() {
    print("SacramentuOS v0.0.4\n");
    print("GitHub: https://github.com/tikzek1337/SacramentuOS\n");
    print("Telegram: t.me/SacramentuOS\n\n");
    print("Commands:\n");
    print("  fetch, clear, color <1..5>, uptime, whoami\n");
    print("  hostname <name>, name <user>, ram, history\n");
    print("  rainbow <text>, banner\n");
    print("  bookmark add <name>, bookmark edit <name>\n");
    print("  ls, version, drivers, off/shutdown, reboot\n");
}

void command_shutdown() {
    print("Shutting down...\n");
    outw(0x604, 0x2000);
    while(1) __asm__("hlt");
}
void command_reboot() {
    print("Rebooting...\n");
    unsigned char s;
    do { s = inb(0x64); } while (s & 0x02);
    outb(0x64, 0xFE);
    while(1) __asm__("hlt");
}

/* ---------- Main shell ---------- */
void shell() {
    clear_screen();
    print("Welcome to SacramentuOS!\n");
    print("Enter your username: ");
    readline(username, 32);
    print("Enter hostname: ");
    readline(hostname, 32);
    clear_screen();
    print("Hello, ");
    print(username);
    print("@");
    print(hostname);
    print("!\n\n");

    while (1) {
        print("> ");
        char input[256];
        readline(input, 256);
        if (input[0] == '\0') continue;

        unsigned long t0 = ticks;
        add_history(input, t0, 0);

        char *arg = input;
        while (*arg && *arg != ' ') arg++;
        if (*arg == ' ') { *arg = '\0'; arg++; } else arg = "";

        if (strcmp(input, "fetch") == 0) command_fetch();
        else if (strcmp(input, "clear") == 0) clear_screen();
        else if (strcmp(input, "color") == 0) {
            if (arg[0] == '1') set_color(15);      // white
            else if (arg[0] == '2') set_color(10);  // green
            else if (arg[0] == '3') set_color(9);   // blue
            else if (arg[0] == '4') set_color(14);  // yellow
            else if (arg[0] == '5') set_color(13);  // magenta
            else {
                print("Usage: color 1|2|3|4|5\n");
                print("1=white, 2=green, 3=blue, 4=yellow, 5=magenta\n");
            }
        } else if (strcmp(input, "uptime") == 0) command_uptime();
        else if (strcmp(input, "whoami") == 0) { print(username); print("\n"); }
        else if (strcmp(input, "hostname") == 0) {
            if (arg[0]) {
                int i=0;
                while (arg[i] && i<31) { hostname[i] = arg[i]; i++; }
                hostname[i] = 0;
            }
            print("Hostname: "); print(hostname); print("\n");
        } else if (strcmp(input, "name") == 0) {
            if (arg[0]) {
                int i=0;
                while (arg[i] && i<31) { username[i] = arg[i]; i++; }
                username[i] = 0;
            }
            print("Username: "); print(username); print("\n");
        } else if (strcmp(input, "ram") == 0) command_ram();
        else if (strcmp(input, "history") == 0) print_history();
        else if (strcmp(input, "banner") == 0) {
            print("   SSSSS  \n  S     S \n  S       \n   SSSSS  \n        S \n  S     S \n   SSSSS  \n");
        } else if (strcmp(input, "rainbow") == 0) {
            print_rainbow(arg);
            print("\n");
        } else if (strcmp(input, "bookmark") == 0) {
            if (strncmp(arg, "add ", 4) == 0) cmd_bookmark_add(arg+4);
            else if (strncmp(arg, "edit ", 5) == 0) text_editor(arg+5);
            else print("Usage: bookmark add <name> | bookmark edit <name>\n");
        } else if (strcmp(input, "ls") == 0) ls();
        else if (strcmp(input, "version") == 0) command_version();
        else if (strcmp(input, "drivers") == 0) command_drivers();
        else if (strcmp(input, "binary") == 0) {
            // простой перевод целого числа в двоичный вид
            unsigned int n = 0;
            char *p = arg;
            while (*p >= '0' && *p <= '9') { n = n*10 + (*p - '0'); p++; }
            char bin[33];
            bin[32] = '\0';
            for (int i=31; i>=0; i--) { bin[i] = (n&1)?'1':'0'; n>>=1; }
            print("Binary: "); print(bin); print("\n");
        } else if (strcmp(input, "off") == 0) command_shutdown();
        else if (strcmp(input, "shutdown") == 0) command_shutdown();
        else if (strcmp(input, "reboot") == 0) command_reboot();
        else if (strcmp(input, "docs") == 0) command_docs();
        else print("Unknown command. Type 'docs'.\n");

        unsigned long dt = ticks - t0;
        if (history_count > 0) history_dur[history_count-1] = dt;
        if (dt > 3000) {
            print("Command took ");
            char buf[8];
            int_to_str(dt/1000, buf); print(buf); print(".");
            int_to_str((dt%1000)/10, buf); print(buf); print("s\n");
        }
    }
}

void kmain() {
    idt_ptr.limit = sizeof(idt)-1;
    idt_ptr.base = (unsigned int)idt;
    idt_set_gate(0x20, (unsigned int)irq0_stub, 0x08, 0x8E);
    idt_load(&idt_ptr);
    init_pic();

    mem_entry_t *map = (mem_entry_t*)(0x5000 + 2);
    int count = *(unsigned short*)0x5000;
    memory_init(map, count);

    pit_init(1000);
    __asm__ __volatile__("sti");

    shell();
}