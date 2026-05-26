#include "string.h"

size_t strlen(const char* s) {
    size_t len = 0;
    while (s && s[len]) len++;
    return len;
}

int strcmp(const char* a, const char* b) {
    while (*a && (*a == *b)) { a++; b++; }
    return (unsigned char)*a - (unsigned char)*b;
}

int strncmp(const char* a, const char* b, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (a[i] != b[i] || a[i] == '\0' || b[i] == '\0') {
            return (unsigned char)a[i] - (unsigned char)b[i];
        }
    }
    return 0;
}

char* strcpy(char* dst, const char* src) {
    char* ret = dst;
    while ((*dst++ = *src++));
    return ret;
}

char* strncpy(char* dst, const char* src, size_t n) {
    size_t i = 0;
    for (; i < n && src[i]; i++) dst[i] = src[i];
    for (; i < n; i++) dst[i] = '\0';
    return dst;
}

char* strcat(char* dst, const char* src) {
    char* ret = dst;
    while (*dst) dst++;
    while ((*dst++ = *src++));
    return ret;
}

char* strchr(const char* s, int c) {
    while (*s) {
        if (*s == (char)c) return (char*)s;
        s++;
    }
    return c == '\0' ? (char*)s : 0;
}

void* memset(void* dest, int value, size_t count) {
    unsigned char* p = (unsigned char*)dest;
    while (count--) *p++ = (unsigned char)value;
    return dest;
}

void* memcpy(void* dest, const void* src, size_t count) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    while (count--) *d++ = *s++;
    return dest;
}

int is_space(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\v' || c == '\f';
}

int is_digit(char c) {
    return c >= '0' && c <= '9';
}

int is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int to_upper(int c) {
    if (c >= 'a' && c <= 'z') return c - 32;
    return c;
}

int to_lower(int c) {
    if (c >= 'A' && c <= 'Z') return c + 32;
    return c;
}

int atoi(const char* s) {
    int sign = 1;
    int value = 0;
    while (is_space(*s)) s++;
    if (*s == '-') { sign = -1; s++; }
    else if (*s == '+') { s++; }
    while (is_digit(*s)) {
        value = value * 10 + (*s - '0');
        s++;
    }
    return value * sign;
}

char* trim_left(char* s) {
    while (is_space(*s)) s++;
    return s;
}

void trim_right(char* s) {
    size_t len = strlen(s);
    while (len > 0 && is_space(s[len - 1])) {
        s[len - 1] = '\0';
        len--;
    }
}

void reverse(char* s) {
    size_t i = 0;
    size_t j = strlen(s);
    if (j == 0) return;
    j--;
    while (i < j) {
        char t = s[i];
        s[i] = s[j];
        s[j] = t;
        i++;
        j--;
    }
}

char* utoa(uint32_t value, char* buffer, int base) {
    const char* digits = "0123456789ABCDEF";
    int i = 0;
    if (base < 2 || base > 16) {
        buffer[0] = '\0';
        return buffer;
    }
    if (value == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return buffer;
    }
    while (value != 0) {
        buffer[i++] = digits[value % (uint32_t)base];
        value /= (uint32_t)base;
    }
    buffer[i] = '\0';
    reverse(buffer);
    return buffer;
}

char* itoa(int value, char* buffer, int base) {
    if (base == 10 && value < 0) {
        buffer[0] = '-';
        utoa((uint32_t)(-value), buffer + 1, base);
        return buffer;
    }
    return utoa((uint32_t)value, buffer, base);
}
