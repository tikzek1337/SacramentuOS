#ifndef SACRAMENTUOS_STRING_H
#define SACRAMENTUOS_STRING_H

#include <stddef.h>
#include <stdint.h>

size_t strlen(const char* s);
int strcmp(const char* a, const char* b);
int strncmp(const char* a, const char* b, size_t n);
char* strcpy(char* dst, const char* src);
char* strncpy(char* dst, const char* src, size_t n);
char* strcat(char* dst, const char* src);
char* strchr(const char* s, int c);
void* memset(void* dest, int value, size_t count);
void* memcpy(void* dest, const void* src, size_t count);
int is_space(char c);
int is_digit(char c);
int is_alpha(char c);
int to_upper(int c);
int to_lower(int c);
int atoi(const char* s);
char* trim_left(char* s);
void trim_right(char* s);
void reverse(char* s);
char* itoa(int value, char* buffer, int base);
char* utoa(uint32_t value, char* buffer, int base);

#endif
