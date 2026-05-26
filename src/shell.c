#include "shell.h"
#include "keyboard.h"
#include "io.h"
#include "kprintf.h"
#include "rtc.h"
#include "string.h"
#include "system.h"
#include "timer.h"
#include "vga.h"

#define LINE_MAX 160
#define HISTORY_MAX 12
#define PROMPT_MAX 48
#define USERNAME_MAX 16

static char history[HISTORY_MAX][LINE_MAX];
static int history_count = 0;
static char username[USERNAME_MAX] = "user";
static char prompt[PROMPT_MAX] = "user@sacramentuOS> ";
static vga_color_t shell_accent = VGA_COLOR_LIGHT_GREEN;

#define ENV_MAX 8
#define ENV_KEY_MAX 16
#define ENV_VALUE_MAX 64
#define NOTE_MAX 8
#define NOTE_LEN 120
#define TODO_MAX 8
#define TODO_LEN 96
#define SYSLOG_MAX 16
#define SYSLOG_LEN 80
#define EDIT_FILE_MAX 8
#define EDIT_NAME_MAX 24
#define EDIT_LINES_MAX 64
#define EDIT_LINE_MAX 120
#define EDIT_VIEW_ROWS 20
#define EDIT_SCREEN_PREFIX 5
#define EDIT_VISIBLE_COLS (VGA_WIDTH - EDIT_SCREEN_PREFIX)

static char env_keys[ENV_MAX][ENV_KEY_MAX] = {"USER", "SHELL"};
static char env_values[ENV_MAX][ENV_VALUE_MAX] = {"user", "/bin/sacramentu-sh"};
static int env_count = 2;

static char notes[NOTE_MAX][NOTE_LEN];
static int note_count = 0;

static char todos[TODO_MAX][TODO_LEN];
static uint8_t todo_done_flags[TODO_MAX];
static int todo_count = 0;

static char syslog_buf[SYSLOG_MAX][SYSLOG_LEN];
static int syslog_count = 0;

static char edit_names[EDIT_FILE_MAX][EDIT_NAME_MAX];
static char edit_lines[EDIT_FILE_MAX][EDIT_LINES_MAX][EDIT_LINE_MAX];
static int edit_line_counts[EDIT_FILE_MAX];
static int edit_file_count = 0;

typedef void (*command_handler_t)(char* args);

typedef struct command {
    const char* name;
    const char* help;
    command_handler_t handler;
} command_t;

static void cmd_help(char* args);
static void cmd_docs(char* args);
static void cmd_commands(char* args);
static void cmd_find(char* args);
static void cmd_clear(char* args);
static void cmd_cls(char* args);
static void cmd_fetch(char* args);
static void cmd_about(char* args);
static void cmd_echo(char* args);
static void cmd_uname(char* args);
static void cmd_whoami(char* args);
static void cmd_mem(char* args);
static void cmd_uptime(char* args);
static void cmd_ticks(char* args);
static void cmd_date(char* args);
static void cmd_time(char* args);
static void cmd_calc(char* args);
static void cmd_color(char* args);
static void cmd_colors(char* args);
static void cmd_theme(char* args);
static void cmd_history(char* args);
static void cmd_len(char* args);
static void cmd_upper(char* args);
static void cmd_lower(char* args);
static void cmd_reverse(char* args);
static void cmd_repeat(char* args);
static void cmd_sleep(char* args);
static void cmd_ascii(char* args);
static void cmd_hexdump(char* args);
static void cmd_ls(char* args);
static void cmd_cat(char* args);
static void cmd_pwd(char* args);
static void cmd_ver(char* args);
static void cmd_env(char* args);
static void cmd_set(char* args);
static void cmd_get(char* args);
static void cmd_unset(char* args);
static void cmd_note(char* args);
static void cmd_notes(char* args);
static void cmd_noteclr(char* args);
static void cmd_todo(char* args);
static void cmd_done(char* args);
static void cmd_todoclr(char* args);
static void cmd_base(char* args);
static void cmd_hash(char* args);
static void cmd_rot13(char* args);
static void cmd_count(char* args);
static void cmd_grep(char* args);
static void cmd_head(char* args);
static void cmd_wc(char* args);
static void cmd_ps(char* args);
static void cmd_top(char* args);
static void cmd_kernel(char* args);
static void cmd_log(char* args);
static void cmd_edit(char* args);
static void cmd_files(char* args);
static void cmd_show(char* args);
static void cmd_rm(char* args);
static void cmd_shutdown(char* args);
static void cmd_reboot(char* args);
static void cmd_halt(char* args);

static command_t commands[] = {
    {"help",    "open documentation or focused help: help calc", cmd_help},
    {"docs",    "open built-in documentation", cmd_docs},
    {"commands","compact command index", cmd_commands},
    {"find",    "search command names/help text: find time", cmd_find},
    {"clear",   "clear the screen", cmd_clear},
    {"cls",     "alias for clear", cmd_cls},
    {"fetch",   "show full SacramentuOS system information", cmd_fetch},
    {"sysinfo", "alias for fetch", cmd_fetch},
    {"about",   "short description of the OS", cmd_about},
    {"echo",    "print text: echo hello", cmd_echo},
    {"uname",   "print kernel name and architecture", cmd_uname},
    {"whoami",  "print current user", cmd_whoami},
    {"mem",     "show memory reported by Multiboot", cmd_mem},
    {"uptime",  "show uptime", cmd_uptime},
    {"ticks",   "show raw PIT ticks", cmd_ticks},
    {"date",    "show RTC date", cmd_date},
    {"time",    "show RTC time", cmd_time},
    {"calc",    "simple math: calc 40 + 2", cmd_calc},
    {"color",   "set prompt/logo color: color green or color 10", cmd_color},
    {"colors",  "list VGA color codes", cmd_colors},
    {"theme",   "set theme: theme green|amber|white|cyan", cmd_theme},
    {"history", "show typed command history", cmd_history},
    {"len",     "count characters: len text", cmd_len},
    {"upper",   "convert text to uppercase", cmd_upper},
    {"lower",   "convert text to lowercase", cmd_lower},
    {"reverse", "reverse text", cmd_reverse},
    {"repeat",  "repeat text: repeat 3 hi", cmd_repeat},
    {"sleep",   "pause in milliseconds: sleep 500", cmd_sleep},
    {"ascii",   "show printable ASCII table", cmd_ascii},
    {"hexdump", "hex-dump text: hexdump hello", cmd_hexdump},
    {"ls",      "list virtual files", cmd_ls},
    {"cat",     "show virtual/RAM file: cat /etc/os-release", cmd_cat},
    {"pwd",     "show current virtual path", cmd_pwd},
    {"ver",     "show version", cmd_ver},
    {"env",     "list RAM environment variables", cmd_env},
    {"set",     "set RAM variable: set USER Fernando", cmd_set},
    {"get",     "read RAM variable: get USER", cmd_get},
    {"unset",   "remove RAM variable: unset name", cmd_unset},
    {"note",    "RAM notes: note add|list|clear", cmd_note},
    {"notes",   "list RAM notes", cmd_notes},
    {"noteclr", "clear RAM notes", cmd_noteclr},
    {"todo",    "RAM todo: todo add|list|clear", cmd_todo},
    {"done",    "mark todo done: done 1", cmd_done},
    {"todoclr", "clear todo list", cmd_todoclr},
    {"base",    "convert number to dec/hex/bin: base 42", cmd_base},
    {"hash",    "FNV-1a checksum for text", cmd_hash},
    {"rot13",   "ROT13-transform text", cmd_rot13},
    {"count",   "count characters and words", cmd_count},
    {"grep",    "search virtual file: grep OS /etc/os-release", cmd_grep},
    {"head",    "print first lines of virtual/RAM file", cmd_head},
    {"wc",      "count lines/words/chars in virtual/RAM file", cmd_wc},
    {"edit",    "full-screen RAM code editor: edit main.c", cmd_edit},
    {"files",   "list RAM editor files", cmd_files},
    {"show",    "show RAM/virtual file: show main.c", cmd_show},
    {"rm",      "delete RAM editor file: rm main.c", cmd_rm},
    {"ps",      "show kernel task table", cmd_ps},
    {"top",     "show live system snapshot", cmd_top},
    {"kernel",  "show kernel diagnostics", cmd_kernel},
    {"log",     "show shell system log", cmd_log},
    {"reboot",  "reboot the machine", cmd_reboot},
    {"halt",    "halt the CPU", cmd_halt},
    {"shutdown","alias for halt", cmd_shutdown},
};
static const int command_count = (int)(sizeof(commands) / sizeof(commands[0]));

static void print_padded(const char* text, int width) {
    int len = (int)strlen(text);
    terminal_writestring(text);
    for (int i = len; i < width; i++) terminal_putchar(' ');
}

static void shell_set_accent(vga_color_t fg) {
    shell_accent = fg;
    terminal_setfg(shell_accent);
}

static void update_prompt(void) {
    prompt[0] = '\0';
    strcpy(prompt, username);
    strcat(prompt, "@sacramentuOS> ");
}

static void set_username(const char* raw) {
    int out = 0;
    if (!raw) raw = "";
    while (is_space(*raw)) raw++;
    for (int i = 0; raw[i] && out < USERNAME_MAX - 1; i++) {
        char c = raw[i];
        if (is_space(c)) break;
        if (is_alpha(c) || is_digit(c) || c == '_' || c == '-') username[out++] = c;
    }
    if (out == 0) {
        strcpy(username, "user");
    } else {
        username[out] = '\0';
    }
    update_prompt();
    int idx = -1;
    for (int i = 0; i < env_count; i++) {
        if (strcmp(env_keys[i], "USER") == 0) idx = i;
    }
    if (idx < 0 && env_count < ENV_MAX) {
        idx = env_count++;
        strcpy(env_keys[idx], "USER");
    }
    if (idx >= 0) {
        strncpy(env_values[idx], username, ENV_VALUE_MAX - 1);
        env_values[idx][ENV_VALUE_MAX - 1] = '\0';
    }
}

static int color_from_arg(char* args, vga_color_t* out) {
    args = trim_left(args);
    if (!args || !args[0]) return 0;
    if (is_digit(args[0])) {
        int c = atoi(args);
        if (c < 0 || c > 15) return 0;
        *out = (vga_color_t)c;
        return 1;
    }
    if (strcmp(args, "black") == 0) *out = VGA_COLOR_BLACK;
    else if (strcmp(args, "blue") == 0) *out = VGA_COLOR_LIGHT_BLUE;
    else if (strcmp(args, "green") == 0) *out = VGA_COLOR_LIGHT_GREEN;
    else if (strcmp(args, "cyan") == 0) *out = VGA_COLOR_LIGHT_CYAN;
    else if (strcmp(args, "red") == 0) *out = VGA_COLOR_LIGHT_RED;
    else if (strcmp(args, "magenta") == 0) *out = VGA_COLOR_LIGHT_MAGENTA;
    else if (strcmp(args, "amber") == 0 || strcmp(args, "yellow") == 0) *out = VGA_COLOR_LIGHT_BROWN;
    else if (strcmp(args, "white") == 0) *out = VGA_COLOR_WHITE;
    else if (strcmp(args, "grey") == 0 || strcmp(args, "gray") == 0) *out = VGA_COLOR_LIGHT_GREY;
    else return 0;
    return 1;
}

static int contains_text(const char* haystack, const char* needle) {
    if (!needle || !needle[0]) return 1;
    if (!haystack) return 0;
    size_t nlen = strlen(needle);
    for (size_t i = 0; haystack[i]; i++) {
        size_t j = 0;
        while (j < nlen && haystack[i + j] && to_lower(haystack[i + j]) == to_lower(needle[j])) j++;
        if (j == nlen) return 1;
    }
    return 0;
}

static void log_event(const char* text) {
    if (!text || !text[0]) return;
    if (syslog_count < SYSLOG_MAX) {
        strncpy(syslog_buf[syslog_count], text, SYSLOG_LEN - 1);
        syslog_buf[syslog_count][SYSLOG_LEN - 1] = '\0';
        syslog_count++;
        return;
    }
    for (int i = 1; i < SYSLOG_MAX; i++) strcpy(syslog_buf[i - 1], syslog_buf[i]);
    strncpy(syslog_buf[SYSLOG_MAX - 1], text, SYSLOG_LEN - 1);
    syslog_buf[SYSLOG_MAX - 1][SYSLOG_LEN - 1] = '\0';
}

static int env_find(const char* key) {
    for (int i = 0; i < env_count; i++) {
        if (strcmp(env_keys[i], key) == 0) return i;
    }
    return -1;
}

static uint32_t parse_uint_any(const char* s) {
    uint32_t value = 0;
    int base = 10;
    while (is_space(*s)) s++;
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) { base = 16; s += 2; }
    else if (s[0] == '0' && (s[1] == 'b' || s[1] == 'B')) { base = 2; s += 2; }
    while (*s) {
        int d = -1;
        if (*s >= '0' && *s <= '9') d = *s - '0';
        else if (*s >= 'a' && *s <= 'f') d = *s - 'a' + 10;
        else if (*s >= 'A' && *s <= 'F') d = *s - 'A' + 10;
        else break;
        if (d < 0 || d >= base) break;
        value = value * (uint32_t)base + (uint32_t)d;
        s++;
    }
    return value;
}

static void print_binary32(uint32_t value) {
    int started = 0;
    for (int bit = 31; bit >= 0; bit--) {
        uint32_t mask = 1U << bit;
        if (value & mask) started = 1;
        if (started || bit == 0) terminal_putchar((value & mask) ? '1' : '0');
    }
}

static const char* virtual_file_content(const char* path) {
    static const char os_release[] =
        "NAME=" OS_NAME "\n"
        "VERSION=" OS_VERSION "\n"
        "CODENAME=" OS_CODENAME "\n"
        "ARCH=" OS_ARCH "\n";
    static const char motd[] = "Build small. Boot fast. Use docs, fetch, top, edit, notes and todo.\n";
    static const char help_file[] =
        "SacramentuOS docs file\n"
        "help/docs: compact command guide; commands: command index; find <word>: search\n"
        "editor: edit <file>; arrows move; F7 save; F8 save+exit; F9 discard; Ctrl+C abort\n"
        "system: fetch top kernel ps mem uptime ticks date time uname whoami ver\n"
        "files: ls cat grep head wc files show rm; ram: env set get unset note todo done\n";
    static const char cpuinfo[] =
        "processor: 0\n"
        "arch: i386 protected mode\n"
        "features: multiboot v1, idt, pic, pit, ps2-keyboard, vga-text\n";
    static const char profile[] =
        "USER=<runtime-login>\n"
        "HOME=/\n"
        "SHELL=/bin/sacramentu-sh\n"
        "PATH=/bin:/sbin:/usr/bin\n";

    if (strcmp(path, "/etc/os-release") == 0 || strcmp(path, "etc/os-release") == 0) return os_release;
    if (strcmp(path, "/motd") == 0 || strcmp(path, "motd") == 0) return motd;
    if (strcmp(path, "/help") == 0 || strcmp(path, "help") == 0) return help_file;
    if (strcmp(path, "/docs") == 0 || strcmp(path, "docs") == 0) return help_file;
    if (strcmp(path, "/proc/cpuinfo") == 0 || strcmp(path, "proc/cpuinfo") == 0) return cpuinfo;
    if (strcmp(path, "/etc/profile") == 0 || strcmp(path, "etc/profile") == 0) return profile;
    return 0;
}

static void add_history(const char* line) {
    if (!line || !line[0]) return;
    if (history_count < HISTORY_MAX) {
        strncpy(history[history_count], line, LINE_MAX - 1);
        history[history_count][LINE_MAX - 1] = '\0';
        history_count++;
        return;
    }
    for (int i = 1; i < HISTORY_MAX; i++) {
        strcpy(history[i - 1], history[i]);
    }
    strncpy(history[HISTORY_MAX - 1], line, LINE_MAX - 1);
    history[HISTORY_MAX - 1][LINE_MAX - 1] = '\0';
}

static char* next_token(char** cursor) {
    char* s = trim_left(*cursor);
    if (!*s) {
        *cursor = s;
        return 0;
    }
    char* start = s;
    while (*s && !is_space(*s)) s++;
    if (*s) {
        *s = '\0';
        s++;
    }
    *cursor = s;
    return start;
}

static int read_line(char* line, size_t max) {
    size_t len = 0;
    keyboard_clear_cancel();
    while (1) {
        int key = keyboard_getkey();
        if (key == KEY_CTRL_C) {
            line[0] = '\0';
            terminal_writestring("^C\n");
            return 0;
        }
        if (key == '\n') {
            terminal_putchar('\n');
            line[len] = '\0';
            return 1;
        }
        if (key == '\b') {
            if (len > 0) {
                len--;
                terminal_backspace();
            }
            continue;
        }
        if (key >= 32 && key <= 126 && len + 1 < max) {
            line[len++] = (char)key;
            terminal_putchar((char)key);
        }
    }
}

static command_t* find_command(const char* name) {
    for (int i = 0; i < command_count; i++) {
        if (strcmp(commands[i].name, name) == 0) return &commands[i];
    }
    return 0;
}

static void print_logo_lines(void) {
    terminal_setfg(shell_accent);
    kprintf("  ____                                      _        ___  ____  \n");
    kprintf(" / ___|  __ _  ___ _ __ __ _ _ __ ___   ___| |_ _   / _ \\/ ___| \n");
    kprintf(" \\___ \\ / _` |/ __| '__/ _` | '_ ` _ \\ / _ \\ __| | | | \\___ \\ \n");
    kprintf("  ___) | (_| | (__| | | (_| | | | | | |  __/ |_| |_| |___) |\n");
    kprintf(" |____/ \\__,_|\\___|_|  \\__,_|_| |_| |_|\\___|\\__|\\___/|____/ \n");
    terminal_setfg(VGA_COLOR_LIGHT_GREY);
}

void shell_boot_animation(void) {
    terminal_clear();
    shell_set_accent(VGA_COLOR_LIGHT_GREEN);
    terminal_draw_rule('=');
    terminal_center("SacramentuOS boot sequence");
    terminal_draw_rule('=');
    terminal_setfg(VGA_COLOR_LIGHT_GREY);

    const char* steps[] = {
        "Loading kernel core",
        "Installing interrupt table",
        "Starting PIT timer",
        "Connecting keyboard driver",
        "Reading Multiboot information",
        "Opening console shell"
    };

    for (int i = 0; i < 6; i++) {
        kprintf("[ ");
        terminal_setfg(VGA_COLOR_LIGHT_BROWN);
        kprintf("....");
        terminal_setfg(VGA_COLOR_LIGHT_GREY);
        kprintf(" ] %s", steps[i]);
        for (int j = 0; j < 3; j++) {
            kprintf(".");
            timer_sleep(90);
        }
        kprintf("\r[ ");
        terminal_setfg(VGA_COLOR_LIGHT_GREEN);
        kprintf(" OK ");
        terminal_setfg(VGA_COLOR_LIGHT_GREY);
        kprintf(" ] %s      \n", steps[i]);
    }

    timer_sleep(250);
    terminal_clear();
    print_logo_lines();
    kprintf("\n");
    terminal_setfg(VGA_COLOR_LIGHT_GREY);
    kprintf("Choose username: ");
    char namebuf[LINE_MAX];
    if (!read_line(namebuf, sizeof(namebuf))) namebuf[0] = '\0';
    trim_right(namebuf);
    set_username(namebuf);
    terminal_setfg(shell_accent);
    kprintf("Welcome, %s. ", username);
    terminal_setfg(VGA_COLOR_LIGHT_GREY);
    kprintf("Type 'help' or 'docs' to open documentation.\n\n");
    log_event("boot: shell started");
}

void shell_start(void) {
    char line[LINE_MAX];
    while (1) {
        terminal_setfg(shell_accent);
        terminal_writestring(prompt);
        terminal_setfg(VGA_COLOR_LIGHT_GREY);

        if (!read_line(line, sizeof(line))) continue;
        trim_right(line);
        char* input = trim_left(line);
        if (!input[0]) continue;

        add_history(input);
        char* args = input;
        char* name = next_token(&args);
        args = trim_left(args);

        command_t* cmd = find_command(name);
        if (!cmd) {
            terminal_setfg(VGA_COLOR_LIGHT_RED);
            kprintf("Unknown command: %s\n", name);
            terminal_setfg(VGA_COLOR_LIGHT_GREY);
            kprintf("Type 'help' for the command list.\n");
            continue;
        }
        log_event(input);
        cmd->handler(args);
    }
}

static void cmd_help(char* args) {
    args = trim_left(args);
    if (!args[0]) {
        cmd_docs(args);
        return;
    }

    command_t* cmd = find_command(args);
    if (!cmd) {
        kprintf("No help: %s. Use docs | commands | find <word>\n", args);
        return;
    }
    terminal_setfg(shell_accent);
    kprintf("%s", cmd->name);
    terminal_setfg(VGA_COLOR_LIGHT_GREY);
    kprintf(" - %s\n", cmd->help);
}

static void cmd_docs(char* args) {
    (void)args;
    terminal_setfg(shell_accent);
    kprintf("SacramentuOS docs  |  help <cmd>  commands  find <word>  Ctrl+C=cancel\n");
    terminal_setfg(VGA_COLOR_LIGHT_GREY);
    terminal_draw_rule('-');
    kprintf("system : fetch top kernel ps mem uptime ticks date time uname whoami ver\n");
    kprintf("files  : ls cat <file> grep <term> <file> head <file> wc <file> files show rm\n");
    kprintf("editor : edit <file>  arrows move  Enter newline  Backspace/Delete edit\n");
    kprintf("         F7 save  F8 save+exit  F9 exit without saving  Ctrl+C abort\n");
    kprintf("ram    : env set/get/unset  note add/list/clear  todo add/list/clear  done N\n");
    kprintf("text   : calc len count upper lower reverse repeat base hash rot13 hexdump ascii\n");
    kprintf("look   : color <name|0..15>  colors  theme green|amber|white|cyan  clear cls\n");
    kprintf("power  : reboot halt shutdown\n");
    kprintf("examples: edit main.c | color green | set USER admin | grep OS /etc/os-release\n");
}

static void cmd_commands(char* args) {
    (void)args;
    for (int i = 0; i < command_count; i++) {
        print_padded(commands[i].name, 10);
        if ((i + 1) % 8 == 0) terminal_putchar('\n');
    }
    terminal_putchar('\n');
}

static void cmd_find(char* args) {
    args = trim_left(args);
    if (!args[0]) {
        kprintf("usage: find <word>\n");
        return;
    }
    int matches = 0;
    for (int i = 0; i < command_count; i++) {
        if (contains_text(commands[i].name, args) || contains_text(commands[i].help, args)) {
            print_padded(commands[i].name, 12);
            kprintf("- %s\n", commands[i].help);
            matches++;
        }
    }
    if (!matches) kprintf("nothing matched: %s\n", args);
}

static void cmd_clear(char* args) {
    (void)args;
    terminal_clear();
}

static void cmd_cls(char* args) {
    cmd_clear(args);
}

static void cmd_fetch(char* args) {
    (void)args;
    uint32_t total_kb = g_mem_lower_kb + g_mem_upper_kb;
    uint32_t seconds = timer_seconds();
    rtc_time_t t = rtc_read_time();

    print_logo_lines();
    terminal_setfg(shell_accent);
    kprintf("+--------------------------------------------------------------+\n");
    kprintf("| SacramentuOS system fetch                                    |\n");
    kprintf("+--------------------------------------------------------------+\n");
    terminal_setfg(VGA_COLOR_LIGHT_GREY);
    kprintf(" OS name       : %s\n", OS_NAME);
    kprintf(" Version       : %s (%s)\n", OS_VERSION, OS_CODENAME);
    kprintf(" Architecture  : %s / 32-bit protected mode\n", OS_ARCH);
    kprintf(" Build style   : %s\n", OS_BUILD_TYPE);
    kprintf(" Boot protocol : Multiboot v1, magic=0x%x\n", g_multiboot_magic);
    kprintf(" Bootloader    : %s\n", g_bootloader_name);
    kprintf(" Console       : VGA text mode %dx%d\n", VGA_WIDTH, VGA_HEIGHT);
    kprintf(" Timer         : PIT %d Hz, ticks=%u\n", TIMER_HZ, timer_ticks());
    kprintf(" Uptime        : %u sec\n", seconds);
    kprintf(" RTC           : %d-%d-%d %d:%d:%d\n", t.year, t.month, t.day, t.hour, t.minute, t.second);
    kprintf(" Memory lower  : %u KB\n", g_mem_lower_kb);
    kprintf(" Memory upper  : %u KB\n", g_mem_upper_kb);
    kprintf(" Memory total  : %u KB (%u MB)\n", total_kb, total_kb / 1024);
    kprintf(" Commands      : %d built-ins\n", command_count);
}

static void cmd_about(char* args) {
    (void)args;
    kprintf("%s is a tiny educational console OS written in C and GNU Assembly.\n", OS_NAME);
    kprintf("It boots through GRUB/Multiboot, uses VGA text mode, IRQ keyboard input and PIT timer ticks.\n");
}

static void cmd_echo(char* args) {
    kprintf("%s\n", args ? args : "");
}

static void cmd_uname(char* args) {
    (void)args;
    kprintf("%s %s %s %s\n", OS_NAME, OS_VERSION, OS_ARCH, OS_CODENAME);
}

static void cmd_whoami(char* args) {
    (void)args;
    kprintf("%s\n", username);
}

static void cmd_mem(char* args) {
    (void)args;
    uint32_t total = g_mem_lower_kb + g_mem_upper_kb;
    kprintf("lower=%u KB upper=%u KB total=%u KB (%u MB)\n", g_mem_lower_kb, g_mem_upper_kb, total, total / 1024);
}

static void cmd_uptime(char* args) {
    (void)args;
    uint32_t sec = timer_seconds();
    kprintf("uptime: %u seconds (%u minutes, %u hours)\n", sec, sec / 60, sec / 3600);
}

static void cmd_ticks(char* args) {
    (void)args;
    kprintf("ticks: %u at %d Hz\n", timer_ticks(), TIMER_HZ);
}

static void cmd_date(char* args) {
    (void)args;
    rtc_time_t t = rtc_read_time();
    kprintf("%d-%d-%d\n", t.year, t.month, t.day);
}

static void cmd_time(char* args) {
    (void)args;
    rtc_time_t t = rtc_read_time();
    kprintf("%d:%d:%d\n", t.hour, t.minute, t.second);
}

static void cmd_calc(char* args) {
    char* cursor = args;
    char* a = next_token(&cursor);
    char* op = next_token(&cursor);
    char* b = next_token(&cursor);
    if (!a || !op || !b) {
        kprintf("usage: calc <number> <+|-|*|/|%%> <number>\n");
        return;
    }
    int x = atoi(a);
    int y = atoi(b);
    int result = 0;
    if (op[0] == '+') result = x + y;
    else if (op[0] == '-') result = x - y;
    else if (op[0] == '*') result = x * y;
    else if (op[0] == '/') {
        if (y == 0) { kprintf("division by zero\n"); return; }
        result = x / y;
    } else if (op[0] == '%') {
        if (y == 0) { kprintf("modulo by zero\n"); return; }
        result = x % y;
    } else {
        kprintf("unknown operator: %s\n", op);
        return;
    }
    kprintf("%d\n", result);
}

static void cmd_color(char* args) {
    vga_color_t c;
    if (!color_from_arg(args, &c)) {
        kprintf("usage: color <0..15|green|amber|white|cyan|red|blue|magenta|grey>\n");
        return;
    }
    shell_set_accent(c);
    kprintf("accent color changed. Prompt/logo/docs now use this color.\n");
    terminal_setfg(VGA_COLOR_LIGHT_GREY);
}

static void cmd_colors(char* args) {
    (void)args;
    static const char* names[] = {
        "black", "blue", "green", "cyan", "red", "magenta", "brown", "light-grey",
        "dark-grey", "light-blue", "light-green", "light-cyan", "light-red", "light-magenta", "amber", "white"
    };
    for (int i = 0; i < 16; i++) {
        terminal_setfg((vga_color_t)i);
        kprintf("%2d ", i);
        terminal_setfg(VGA_COLOR_LIGHT_GREY);
        kprintf("%s\n", names[i]);
    }
}

static void cmd_theme(char* args) {
    args = trim_left(args);
    if (strcmp(args, "green") == 0) shell_set_accent(VGA_COLOR_LIGHT_GREEN);
    else if (strcmp(args, "amber") == 0) shell_set_accent(VGA_COLOR_LIGHT_BROWN);
    else if (strcmp(args, "white") == 0) shell_set_accent(VGA_COLOR_WHITE);
    else if (strcmp(args, "cyan") == 0) shell_set_accent(VGA_COLOR_LIGHT_CYAN);
    else {
        kprintf("usage: theme green|amber|white|cyan\n");
        return;
    }
    kprintf("theme applied: %s\n", args);
    terminal_setfg(VGA_COLOR_LIGHT_GREY);
}

static void cmd_history(char* args) {
    (void)args;
    for (int i = 0; i < history_count; i++) {
        kprintf("%d  %s\n", i + 1, history[i]);
    }
}

static void cmd_len(char* args) {
    kprintf("%u\n", (uint32_t)strlen(args));
}

static void cmd_upper(char* args) {
    for (size_t i = 0; args[i]; i++) terminal_putchar((char)to_upper(args[i]));
    terminal_putchar('\n');
}

static void cmd_lower(char* args) {
    for (size_t i = 0; args[i]; i++) terminal_putchar((char)to_lower(args[i]));
    terminal_putchar('\n');
}

static void cmd_reverse(char* args) {
    size_t len = strlen(args);
    while (len > 0) terminal_putchar(args[--len]);
    terminal_putchar('\n');
}

static void cmd_repeat(char* args) {
    char* cursor = args;
    char* count_s = next_token(&cursor);
    if (!count_s) {
        kprintf("usage: repeat <count> <text>\n");
        return;
    }
    int count = atoi(count_s);
    if (count < 1) count = 1;
    if (count > 20) count = 20;
    cursor = trim_left(cursor);
    for (int i = 0; i < count; i++) kprintf("%s\n", cursor);
}

static void cmd_sleep(char* args) {
    int ms = atoi(args);
    if (ms < 1) ms = 1;
    if (ms > 30000) ms = 30000;
    uint32_t ticks = ((uint32_t)ms * TIMER_HZ) / 1000;
    if (ticks == 0) ticks = 1;
    uint32_t end = timer_ticks() + ticks;
    keyboard_clear_cancel();
    kprintf("sleeping %d ms... Ctrl+C cancels\n", ms);
    while (timer_ticks() < end) {
        if (keyboard_cancel_requested()) {
            keyboard_flush();
            kprintf("cancelled by Ctrl+C\n");
            return;
        }
        cpu_halt();
    }
    kprintf("awake\n");
}

static void cmd_ascii(char* args) {
    (void)args;
    for (int i = 32; i < 127; i++) {
        kprintf("%d='%c'  ", i, (char)i);
        if ((i - 31) % 5 == 0) terminal_putchar('\n');
    }
    terminal_putchar('\n');
}

static void print_hex2(uint8_t v) {
    const char* h = "0123456789ABCDEF";
    terminal_putchar(h[(v >> 4) & 0xF]);
    terminal_putchar(h[v & 0xF]);
}

static void cmd_hexdump(char* args) {
    if (!args || !args[0]) {
        kprintf("usage: hexdump <text>\n");
        return;
    }
    for (size_t i = 0; args[i]; i++) {
        print_hex2((uint8_t)args[i]);
        terminal_putchar(' ');
    }
    terminal_putchar('\n');
}

static int ram_file_find(const char* name) {
    if (!name || !name[0]) return -1;
    for (int i = 0; i < edit_file_count; i++) {
        if (strcmp(edit_names[i], name) == 0) return i;
    }
    return -1;
}

static int ram_file_alloc(const char* name) {
    int idx = ram_file_find(name);
    if (idx >= 0) return idx;
    if (edit_file_count >= EDIT_FILE_MAX) return -1;
    idx = edit_file_count++;
    strncpy(edit_names[idx], name, EDIT_NAME_MAX - 1);
    edit_names[idx][EDIT_NAME_MAX - 1] = '\0';
    edit_line_counts[idx] = 0;
    for (int i = 0; i < EDIT_LINES_MAX; i++) edit_lines[idx][i][0] = '\0';
    return idx;
}

static void ram_file_print(int idx) {
    if (idx < 0 || idx >= edit_file_count) return;
    for (int i = 0; i < edit_line_counts[idx]; i++) kprintf("%s\n", edit_lines[idx][i]);
}

static void ram_file_delete_index(int idx) {
    if (idx < 0 || idx >= edit_file_count) return;
    for (int f = idx + 1; f < edit_file_count; f++) {
        strcpy(edit_names[f - 1], edit_names[f]);
        edit_line_counts[f - 1] = edit_line_counts[f];
        for (int l = 0; l < EDIT_LINES_MAX; l++) strcpy(edit_lines[f - 1][l], edit_lines[f][l]);
    }
    edit_file_count--;
}

static int editor_line_len(const char* line) {
    return (int)strlen(line);
}

static void editor_clamp_cursor(char lines[EDIT_LINES_MAX][EDIT_LINE_MAX], int count, int* cy, int* cx) {
    if (count < 1) count = 1;
    if (*cy < 0) *cy = 0;
    if (*cy >= count) *cy = count - 1;
    int len = editor_line_len(lines[*cy]);
    if (*cx < 0) *cx = 0;
    if (*cx > len) *cx = len;
}

static void editor_adjust_view(int cy, int cx, int* top, int* coloff) {
    if (cy < *top) *top = cy;
    if (cy >= *top + EDIT_VIEW_ROWS) *top = cy - EDIT_VIEW_ROWS + 1;
    if (*top < 0) *top = 0;

    if (cx < *coloff) *coloff = cx;
    if (cx >= *coloff + EDIT_VISIBLE_COLS) *coloff = cx - EDIT_VISIBLE_COLS + 1;
    if (*coloff < 0) *coloff = 0;
}

static void editor_render(const char* name,
                          char lines[EDIT_LINES_MAX][EDIT_LINE_MAX],
                          int count,
                          int cy,
                          int cx,
                          int top,
                          int coloff,
                          int dirty,
                          const char* message) {
    terminal_clear();
    terminal_setfg(shell_accent);
    kprintf("SacramentuOS editor  file:%s  %s\n", name, dirty ? "modified" : "saved");
    terminal_setfg(VGA_COLOR_LIGHT_GREY);
    kprintf("Arrows move | Enter newline | Backspace/Delete edit | F7 save | F8 save+exit | F9 discard | Ctrl+C abort\n");
    terminal_draw_rule('-');

    for (int row = 0; row < EDIT_VIEW_ROWS; row++) {
        int line_index = top + row;
        if (line_index < count) {
            kprintf("%2d | ", line_index + 1);
            int len = editor_line_len(lines[line_index]);
            for (int col = coloff; col < len && col < coloff + EDIT_VISIBLE_COLS; col++) {
                char ch = lines[line_index][col];
                if (ch == '\t') ch = ' ';
                terminal_putchar(ch);
            }
            terminal_putchar('\n');
        } else {
            kprintf("   |\n");
        }
    }

    terminal_setfg(shell_accent);
    kprintf("Ln %d/%d  Col %d  limit:%d files %d lines %d chars/line\n",
            cy + 1, count, cx + 1, EDIT_FILE_MAX, EDIT_LINES_MAX, EDIT_LINE_MAX - 1);
    terminal_setfg(VGA_COLOR_LIGHT_GREY);
    if (message && message[0]) terminal_writestring(message);

    int screen_row = 3 + (cy - top);
    int screen_col = EDIT_SCREEN_PREFIX + (cx - coloff);
    if (screen_row < 3) screen_row = 3;
    if (screen_row >= 3 + EDIT_VIEW_ROWS) screen_row = 3 + EDIT_VIEW_ROWS - 1;
    if (screen_col < EDIT_SCREEN_PREFIX) screen_col = EDIT_SCREEN_PREFIX;
    if (screen_col >= VGA_WIDTH) screen_col = VGA_WIDTH - 1;
    terminal_setcursor((size_t)screen_row, (size_t)screen_col);
}

static int editor_save(const char* name, char lines[EDIT_LINES_MAX][EDIT_LINE_MAX], int count) {
    int idx = ram_file_alloc(name);
    if (idx < 0) return 0;
    edit_line_counts[idx] = count;
    for (int i = 0; i < EDIT_LINES_MAX; i++) {
        if (i < count) strncpy(edit_lines[idx][i], lines[i], EDIT_LINE_MAX - 1);
        else edit_lines[idx][i][0] = '\0';
        edit_lines[idx][i][EDIT_LINE_MAX - 1] = '\0';
    }
    return 1;
}

static void editor_delete_char_at(char* line, int pos) {
    int len = editor_line_len(line);
    if (pos < 0 || pos >= len) return;
    for (int i = pos; i < len; i++) line[i] = line[i + 1];
}

static void editor_insert_char_at(char* line, int pos, char c) {
    int len = editor_line_len(line);
    if (len >= EDIT_LINE_MAX - 1) return;
    if (pos < 0) pos = 0;
    if (pos > len) pos = len;
    for (int i = len; i >= pos; i--) line[i + 1] = line[i];
    line[pos] = c;
}

static void cmd_files(char* args) {
    (void)args;
    kprintf("static: /etc/os-release /etc/profile /docs /motd /proc/cpuinfo /proc/meminfo /proc/uptime /proc/ticks\n");
    kprintf("ram   : ");
    if (edit_file_count == 0) { kprintf("<none>\n"); return; }
    for (int i = 0; i < edit_file_count; i++) {
        kprintf("%s(%d)%s", edit_names[i], edit_line_counts[i], (i + 1 == edit_file_count) ? "" : "  ");
    }
    terminal_putchar('\n');
}

static void cmd_show(char* args) {
    args = trim_left(args);
    if (!args[0]) { kprintf("usage: show <file>\n"); return; }
    int idx = ram_file_find(args);
    if (idx >= 0) { ram_file_print(idx); return; }
    cmd_cat(args);
}

static void cmd_rm(char* args) {
    args = trim_left(args);
    if (!args[0]) { kprintf("usage: rm <RAM-file>\n"); return; }
    int idx = ram_file_find(args);
    if (idx < 0) { kprintf("RAM file not found: %s\n", args); return; }
    ram_file_delete_index(idx);
    kprintf("removed RAM file: %s\n", args);
}

static void cmd_edit(char* args) {
    args = trim_left(args);
    char name[EDIT_NAME_MAX];
    if (!args[0]) strcpy(name, "main.c");
    else {
        char* cursor = args;
        char* token = next_token(&cursor);
        if (!token) strcpy(name, "main.c");
        else {
            strncpy(name, token, EDIT_NAME_MAX - 1);
            name[EDIT_NAME_MAX - 1] = '\0';
        }
    }

    char buffer[EDIT_LINES_MAX][EDIT_LINE_MAX];
    int count = 1;
    for (int i = 0; i < EDIT_LINES_MAX; i++) buffer[i][0] = '\0';

    int existing = ram_file_find(name);
    if (existing >= 0) {
        count = edit_line_counts[existing];
        if (count < 1) count = 1;
        for (int i = 0; i < count; i++) strcpy(buffer[i], edit_lines[existing][i]);
    }

    int cy = 0;
    int cx = 0;
    int top = 0;
    int coloff = 0;
    int dirty = 0;
    char message[80];
    if (existing >= 0) kprintf("opened RAM file %s (%d lines)\n", name, count);
    else kprintf("new RAM file %s\n", name);
    strcpy(message, "Ready. F7 saves, F8 saves and exits, F9 discards.");
    timer_sleep(250);
    keyboard_clear_cancel();

    while (1) {
        editor_clamp_cursor(buffer, count, &cy, &cx);
        editor_adjust_view(cy, cx, &top, &coloff);
        editor_render(name, buffer, count, cy, cx, top, coloff, dirty, message);

        int key = keyboard_getkey();
        message[0] = '\0';

        if (key == KEY_CTRL_C || key == KEY_F9) {
            terminal_clear();
            kprintf("editor closed; unsaved changes discarded\n");
            return;
        }

        if (key == KEY_F7) {
            if (editor_save(name, buffer, count)) {
                dirty = 0;
                strcpy(message, "Saved to RAM file.");
            } else {
                strcpy(message, "Save failed: editor storage is full. Use rm <file>.");
            }
            continue;
        }

        if (key == KEY_F8) {
            terminal_clear();
            if (editor_save(name, buffer, count)) {
                kprintf("saved %s (%d lines), editor closed\n", name, count);
            } else {
                kprintf("save failed: editor storage is full. Editor closed without saving.\n");
            }
            return;
        }

        if (key == KEY_UP) {
            if (cy > 0) cy--;
            editor_clamp_cursor(buffer, count, &cy, &cx);
            continue;
        }
        if (key == KEY_DOWN) {
            if (cy + 1 < count) cy++;
            editor_clamp_cursor(buffer, count, &cy, &cx);
            continue;
        }
        if (key == KEY_LEFT) {
            if (cx > 0) cx--;
            else if (cy > 0) { cy--; cx = editor_line_len(buffer[cy]); }
            continue;
        }
        if (key == KEY_RIGHT) {
            int len = editor_line_len(buffer[cy]);
            if (cx < len) cx++;
            else if (cy + 1 < count) { cy++; cx = 0; }
            continue;
        }
        if (key == KEY_HOME) { cx = 0; continue; }
        if (key == KEY_END) { cx = editor_line_len(buffer[cy]); continue; }

        if (key == '\n') {
            if (count >= EDIT_LINES_MAX) {
                strcpy(message, "Line limit reached; delete lines before adding more.");
                continue;
            }
            char tail[EDIT_LINE_MAX];
            strncpy(tail, buffer[cy] + cx, EDIT_LINE_MAX - 1);
            tail[EDIT_LINE_MAX - 1] = '\0';
            buffer[cy][cx] = '\0';
            for (int i = count; i > cy + 1; i--) strcpy(buffer[i], buffer[i - 1]);
            strcpy(buffer[cy + 1], tail);
            count++;
            cy++;
            cx = 0;
            dirty = 1;
            continue;
        }

        if (key == '\b') {
            if (cx > 0) {
                editor_delete_char_at(buffer[cy], cx - 1);
                cx--;
                dirty = 1;
            } else if (cy > 0) {
                int prev_len = editor_line_len(buffer[cy - 1]);
                int cur_len = editor_line_len(buffer[cy]);
                if (prev_len + cur_len < EDIT_LINE_MAX) {
                    strcat(buffer[cy - 1], buffer[cy]);
                    for (int i = cy + 1; i < count; i++) strcpy(buffer[i - 1], buffer[i]);
                    count--;
                    cy--;
                    cx = prev_len;
                    dirty = 1;
                } else {
                    strcpy(message, "Cannot merge: line would exceed limit.");
                }
            }
            continue;
        }

        if (key == KEY_DELETE) {
            int len = editor_line_len(buffer[cy]);
            if (cx < len) {
                editor_delete_char_at(buffer[cy], cx);
                dirty = 1;
            } else if (cy + 1 < count) {
                int next_len = editor_line_len(buffer[cy + 1]);
                if (len + next_len < EDIT_LINE_MAX) {
                    strcat(buffer[cy], buffer[cy + 1]);
                    for (int i = cy + 2; i < count; i++) strcpy(buffer[i - 1], buffer[i]);
                    count--;
                    dirty = 1;
                } else {
                    strcpy(message, "Cannot merge: line would exceed limit.");
                }
            }
            continue;
        }

        if (key >= 32 && key <= 126) {
            int len = editor_line_len(buffer[cy]);
            if (len >= EDIT_LINE_MAX - 1) {
                strcpy(message, "Line is full.");
                continue;
            }
            editor_insert_char_at(buffer[cy], cx, (char)key);
            cx++;
            dirty = 1;
            continue;
        }
    }
}

static void cmd_ls(char* args) {
    (void)args;
    kprintf("/\n");
    kprintf("  etc/\n");
    kprintf("    os-release\n");
    kprintf("    profile\n");
    kprintf("  docs\n");
    kprintf("  help\n");
    kprintf("  motd\n");
    kprintf("  proc/\n");
    kprintf("    cpuinfo\n");
    kprintf("    meminfo\n");
    kprintf("    uptime\n");
    kprintf("    ticks\n");
}

static void cmd_cat(char* args) {
    args = trim_left(args);
    if (!args[0]) {
        kprintf("usage: cat <virtual-file>\n");
        kprintf("try: ls\n");
        return;
    }
    if (strcmp(args, "/proc/meminfo") == 0 || strcmp(args, "proc/meminfo") == 0) {
        cmd_mem(args);
        return;
    }
    if (strcmp(args, "/proc/uptime") == 0 || strcmp(args, "proc/uptime") == 0) {
        cmd_uptime(args);
        return;
    }
    if (strcmp(args, "/proc/ticks") == 0 || strcmp(args, "proc/ticks") == 0) {
        cmd_ticks(args);
        return;
    }
    if (strcmp(args, "/etc/profile") == 0 || strcmp(args, "etc/profile") == 0) {
        kprintf("USER=%s\nHOME=/\nSHELL=/bin/sacramentu-sh\nPATH=/bin:/sbin:/usr/bin\n", username);
        return;
    }
    int ridx = ram_file_find(args);
    if (ridx >= 0) {
        ram_file_print(ridx);
        return;
    }
    const char* data = virtual_file_content(args);
    if (data) {
        terminal_writestring(data);
        return;
    }
    kprintf("file not found. Try: ls or files\n");
}

static void cmd_pwd(char* args) {
    (void)args;
    kprintf("/\n");
}

static void cmd_ver(char* args) {
    (void)args;
    kprintf("%s version %s, codename %s\n", OS_NAME, OS_VERSION, OS_CODENAME);
}


static void cmd_env(char* args) {
    (void)args;
    if (env_count == 0) {
        kprintf("environment is empty. Use: set <key> <value>\n");
        return;
    }
    for (int i = 0; i < env_count; i++) kprintf("%s=%s\n", env_keys[i], env_values[i]);
}

static void cmd_set(char* args) {
    char* cursor = args;
    char* key = next_token(&cursor);
    cursor = trim_left(cursor);
    if (!key || !cursor[0]) {
        kprintf("usage: set <key> <value>\n");
        return;
    }
    if (strlen(key) >= ENV_KEY_MAX) {
        kprintf("key is too long; max %d characters\n", ENV_KEY_MAX - 1);
        return;
    }
    int idx = env_find(key);
    if (idx < 0) {
        if (env_count >= ENV_MAX) {
            kprintf("environment is full; unset a key first\n");
            return;
        }
        idx = env_count++;
        strncpy(env_keys[idx], key, ENV_KEY_MAX - 1);
        env_keys[idx][ENV_KEY_MAX - 1] = '\0';
    }
    strncpy(env_values[idx], cursor, ENV_VALUE_MAX - 1);
    env_values[idx][ENV_VALUE_MAX - 1] = '\0';
    if (strcmp(key, "USER") == 0 || strcmp(key, "user") == 0) {
        set_username(cursor);
        kprintf("set USER=%s and updated prompt\n", username);
        return;
    }
    kprintf("set %s=%s\n", env_keys[idx], env_values[idx]);
}

static void cmd_get(char* args) {
    args = trim_left(args);
    if (!args[0]) { kprintf("usage: get <key>\n"); return; }
    int idx = env_find(args);
    if (idx < 0) { kprintf("not set: %s\n", args); return; }
    kprintf("%s\n", env_values[idx]);
}

static void cmd_unset(char* args) {
    args = trim_left(args);
    if (!args[0]) { kprintf("usage: unset <key>\n"); return; }
    int idx = env_find(args);
    if (idx < 0) { kprintf("not set: %s\n", args); return; }
    for (int i = idx + 1; i < env_count; i++) {
        strcpy(env_keys[i - 1], env_keys[i]);
        strcpy(env_values[i - 1], env_values[i]);
    }
    env_count--;
    kprintf("unset %s\n", args);
}

static void cmd_notes(char* args) {
    (void)args;
    if (note_count == 0) {
        kprintf("no notes. Use: note add <text>\n");
        return;
    }
    for (int i = 0; i < note_count; i++) kprintf("%d. %s\n", i + 1, notes[i]);
}

static void cmd_noteclr(char* args) {
    (void)args;
    note_count = 0;
    kprintf("notes cleared\n");
}

static void cmd_note(char* args) {
    char* cursor = args;
    char* action = next_token(&cursor);
    cursor = trim_left(cursor);
    if (!action || strcmp(action, "list") == 0) { cmd_notes(cursor); return; }
    if (strcmp(action, "clear") == 0) { cmd_noteclr(cursor); return; }
    if (strcmp(action, "add") != 0 || !cursor[0]) {
        kprintf("usage: note add <text> | note list | note clear\n");
        return;
    }
    if (note_count >= NOTE_MAX) {
        kprintf("note storage is full; use note clear\n");
        return;
    }
    strncpy(notes[note_count], cursor, NOTE_LEN - 1);
    notes[note_count][NOTE_LEN - 1] = '\0';
    note_count++;
    kprintf("note saved (%d/%d)\n", note_count, NOTE_MAX);
}

static void cmd_todo(char* args) {
    char* cursor = args;
    char* action = next_token(&cursor);
    cursor = trim_left(cursor);
    if (!action || strcmp(action, "list") == 0) {
        if (todo_count == 0) { kprintf("todo list is empty. Use: todo add <text>\n"); return; }
        for (int i = 0; i < todo_count; i++) {
            kprintf("%d. [%c] %s\n", i + 1, todo_done_flags[i] ? 'x' : ' ', todos[i]);
        }
        return;
    }
    if (strcmp(action, "clear") == 0) { cmd_todoclr(cursor); return; }
    if (strcmp(action, "add") != 0 || !cursor[0]) {
        kprintf("usage: todo add <text> | todo list | todo clear\n");
        return;
    }
    if (todo_count >= TODO_MAX) {
        kprintf("todo storage is full; use todoclr\n");
        return;
    }
    strncpy(todos[todo_count], cursor, TODO_LEN - 1);
    todos[todo_count][TODO_LEN - 1] = '\0';
    todo_done_flags[todo_count] = 0;
    todo_count++;
    kprintf("todo added (%d/%d)\n", todo_count, TODO_MAX);
}

static void cmd_done(char* args) {
    int idx = atoi(args) - 1;
    if (idx < 0 || idx >= todo_count) {
        kprintf("usage: done <todo-number>\n");
        return;
    }
    todo_done_flags[idx] = 1;
    kprintf("done: %s\n", todos[idx]);
}

static void cmd_todoclr(char* args) {
    (void)args;
    todo_count = 0;
    kprintf("todo list cleared\n");
}

static void cmd_base(char* args) {
    args = trim_left(args);
    if (!args[0]) { kprintf("usage: base <number>, supports 42, 0x2A, 0b101010\n"); return; }
    uint32_t value = parse_uint_any(args);
    kprintf("dec: %u\n", value);
    kprintf("hex: 0x%x\n", value);
    kprintf("bin: 0b");
    print_binary32(value);
    terminal_putchar('\n');
}

static void cmd_hash(char* args) {
    args = trim_left(args);
    if (!args[0]) { kprintf("usage: hash <text>\n"); return; }
    uint32_t h = 2166136261U;
    for (size_t i = 0; args[i]; i++) {
        h ^= (uint8_t)args[i];
        h *= 16777619U;
    }
    kprintf("fnv1a32: 0x%x\n", h);
}

static void cmd_rot13(char* args) {
    for (size_t i = 0; args[i]; i++) {
        char c = args[i];
        if (c >= 'a' && c <= 'z') c = (char)('a' + ((c - 'a' + 13) % 26));
        else if (c >= 'A' && c <= 'Z') c = (char)('A' + ((c - 'A' + 13) % 26));
        terminal_putchar(c);
    }
    terminal_putchar('\n');
}

static void cmd_count(char* args) {
    int chars = 0;
    int words = 0;
    int in_word = 0;
    for (size_t i = 0; args[i]; i++) {
        chars++;
        if (is_space(args[i])) in_word = 0;
        else if (!in_word) { words++; in_word = 1; }
    }
    kprintf("chars=%d words=%d\n", chars, words);
}

static void cmd_grep(char* args) {
    char* cursor = args;
    char* term = next_token(&cursor);
    cursor = trim_left(cursor);
    if (!term || !cursor[0]) { kprintf("usage: grep <term> <file>\n"); return; }

    int ridx = ram_file_find(cursor);
    if (ridx >= 0) {
        int matches = 0;
        for (int i = 0; i < edit_line_counts[ridx]; i++) {
            if (contains_text(edit_lines[ridx][i], term)) { kprintf("%s\n", edit_lines[ridx][i]); matches++; }
        }
        if (!matches) kprintf("no matches\n");
        return;
    }

    const char* data = virtual_file_content(cursor);
    if (!data) { kprintf("file not found. Try: ls or files\n"); return; }
    int matches = 0;
    const char* line = data;
    while (*line) {
        const char* endp = line;
        while (*endp && *endp != '\n') endp++;
        char temp[128];
        size_t len = (size_t)(endp - line);
        if (len >= sizeof(temp)) len = sizeof(temp) - 1;
        for (size_t i = 0; i < len; i++) temp[i] = line[i];
        temp[len] = '\0';
        if (contains_text(temp, term)) { kprintf("%s\n", temp); matches++; }
        line = *endp == '\n' ? endp + 1 : endp;
    }
    if (!matches) kprintf("no matches\n");
}

static void cmd_head(char* args) {
    args = trim_left(args);
    if (!args[0]) { kprintf("usage: head <file>\n"); return; }
    int ridx = ram_file_find(args);
    if (ridx >= 0) {
        int max = edit_line_counts[ridx] < 3 ? edit_line_counts[ridx] : 3;
        for (int i = 0; i < max; i++) kprintf("%s\n", edit_lines[ridx][i]);
        return;
    }
    const char* data = virtual_file_content(args);
    if (!data) { kprintf("file not found. Try: ls or files\n"); return; }
    int lines = 0;
    for (size_t i = 0; data[i] && lines < 3; i++) {
        terminal_putchar(data[i]);
        if (data[i] == '\n') lines++;
    }
}

static void cmd_wc(char* args) {
    args = trim_left(args);
    if (!args[0]) { kprintf("usage: wc <file>\n"); return; }
    int lines = 0, words = 0, chars = 0, in_word = 0;
    int ridx = ram_file_find(args);
    if (ridx >= 0) {
        for (int l = 0; l < edit_line_counts[ridx]; l++) {
            lines++;
            for (size_t i = 0; edit_lines[ridx][l][i]; i++) {
                chars++;
                if (is_space(edit_lines[ridx][l][i])) in_word = 0;
                else if (!in_word) { words++; in_word = 1; }
            }
            chars++;
            in_word = 0;
        }
        kprintf("lines=%d words=%d chars=%d %s\n", lines, words, chars, args);
        return;
    }
    const char* data = virtual_file_content(args);
    if (!data) { kprintf("file not found. Try: ls or files\n"); return; }
    for (size_t i = 0; data[i]; i++) {
        chars++;
        if (data[i] == '\n') lines++;
        if (is_space(data[i])) in_word = 0;
        else if (!in_word) { words++; in_word = 1; }
    }
    kprintf("lines=%d words=%d chars=%d %s\n", lines, words, chars, args);
}

static void cmd_ps(char* args) {
    (void)args;
    kprintf("PID  STATE   NAME\n");
    kprintf("1    run     kernel\n");
    kprintf("2    sleep   idle\n");
    kprintf("3    run     sacramentu-shell\n");
}

static void cmd_top(char* args) {
    (void)args;
    uint32_t total = g_mem_lower_kb + g_mem_upper_kb;
    kprintf("SacramentuOS top - uptime %u sec, ticks %u\n", timer_seconds(), timer_ticks());
    kprintf("memory: %u KB total, shell buffers: history=%d env=%d notes=%d todo=%d\n", total, history_count, env_count, note_count, todo_count);
    kprintf("tasks: 3 total, 2 running, 1 sleeping\n");
}

static void cmd_kernel(char* args) {
    (void)args;
    kprintf("kernel: %s %s (%s)\n", OS_NAME, OS_VERSION, OS_CODENAME);
    kprintf("modules: vga idt pic pit ps2-keyboard rtc shell\n");
    kprintf("multiboot_magic=0x%x bootloader=%s\n", g_multiboot_magic, g_bootloader_name);
    kprintf("command_count=%d history_capacity=%d\n", command_count, HISTORY_MAX);
}

static void cmd_log(char* args) {
    (void)args;
    if (syslog_count == 0) { kprintf("log is empty\n"); return; }
    for (int i = 0; i < syslog_count; i++) kprintf("%d  %s\n", i + 1, syslog_buf[i]);
}

static void cmd_shutdown(char* args) {
    cmd_halt(args);
}

static void cmd_reboot(char* args) {
    (void)args;
    system_reboot();
}

static void cmd_halt(char* args) {
    (void)args;
    system_halt();
}
