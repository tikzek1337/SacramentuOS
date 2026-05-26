#include "keyboard.h"
#include "idt.h"
#include "io.h"

#define KBD_BUFFER_SIZE 256

static volatile int buffer[KBD_BUFFER_SIZE];
static volatile unsigned int head = 0;
static volatile unsigned int tail = 0;
static volatile int ctrl_c_pending = 0;
static int shift_down = 0;
static int ctrl_down = 0;
static int caps_lock = 0;
static int extended = 0;

static const char scancode_ascii[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0, '\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' ',
};

static const char scancode_shift_ascii[128] = {
    0,  27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n', 0,
    'A','S','D','F','G','H','J','K','L',':','"','~', 0, '|',
    'Z','X','C','V','B','N','M','<','>','?', 0, '*', 0, ' ',
};

static int is_letter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static char flip_case(char c) {
    if (c >= 'a' && c <= 'z') return (char)(c - 32);
    if (c >= 'A' && c <= 'Z') return (char)(c + 32);
    return c;
}

static void buffer_put(int key) {
    unsigned int next = (head + 1) % KBD_BUFFER_SIZE;
    if (next != tail) {
        buffer[head] = key;
        head = next;
    }
}

int keyboard_trygetkey(int* out) {
    if (head == tail) return 0;
    *out = buffer[tail];
    tail = (tail + 1) % KBD_BUFFER_SIZE;
    return 1;
}

int keyboard_getkey(void) {
    int key;
    while (!keyboard_trygetkey(&key)) {
        cpu_halt();
    }
    return key;
}

int keyboard_trygetchar(char* out) {
    int key;
    if (!keyboard_trygetkey(&key)) return 0;
    if (key >= 0 && key <= 255) {
        *out = (char)key;
        return 1;
    }
    return 0;
}

char keyboard_getchar(void) {
    int key;
    while (1) {
        key = keyboard_getkey();
        if (key >= 0 && key <= 255) return (char)key;
    }
}

void keyboard_flush(void) {
    head = tail = 0;
    ctrl_c_pending = 0;
}

int keyboard_cancel_requested(void) {
    if (!ctrl_c_pending) return 0;
    ctrl_c_pending = 0;
    return 1;
}

void keyboard_clear_cancel(void) {
    ctrl_c_pending = 0;
}

static int map_extended_key(uint8_t sc) {
    switch (sc) {
        case 0x48: return KEY_UP;
        case 0x50: return KEY_DOWN;
        case 0x4B: return KEY_LEFT;
        case 0x4D: return KEY_RIGHT;
        case 0x47: return KEY_HOME;
        case 0x4F: return KEY_END;
        case 0x53: return KEY_DELETE;
        default: return KEY_NONE;
    }
}

static int map_function_key(uint8_t sc) {
    switch (sc) {
        case 0x41: return KEY_F7;
        case 0x42: return KEY_F8;
        case 0x43: return KEY_F9;
        default: return KEY_NONE;
    }
}

static void keyboard_callback(registers_t* regs) {
    (void)regs;
    uint8_t sc = inb(0x60);

    if (sc == 0xE0) {
        extended = 1;
        return;
    }

    if (sc & 0x80) {
        uint8_t released = (uint8_t)(sc & 0x7F);
        if (released == 42 || released == 54) shift_down = 0;
        if (released == 29) ctrl_down = 0;
        extended = 0;
        return;
    }

    if (extended) {
        int key = map_extended_key(sc);
        extended = 0;
        if (key) buffer_put(key);
        return;
    }

    if (sc == 42 || sc == 54) {
        shift_down = 1;
        return;
    }

    if (sc == 29) {
        ctrl_down = 1;
        return;
    }

    if (sc == 58) {
        caps_lock = !caps_lock;
        return;
    }

    int function_key = map_function_key(sc);
    if (function_key) {
        buffer_put(function_key);
        return;
    }

    if (sc >= 128) return;
    char c = shift_down ? scancode_shift_ascii[sc] : scancode_ascii[sc];
    if (caps_lock && is_letter(c)) c = flip_case(c);
    if (!c) return;

    if (ctrl_down) {
        char lower = c;
        if (lower >= 'A' && lower <= 'Z') lower = (char)(lower + 32);
        if (lower == 'c') {
            ctrl_c_pending = 1;
            buffer_put(KEY_CTRL_C);
            return;
        }
    }

    buffer_put((int)c);
}

void keyboard_install(void) {
    keyboard_flush();
    register_interrupt_handler(33, keyboard_callback);
}
