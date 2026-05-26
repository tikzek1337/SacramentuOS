#ifndef SACRAMENTUOS_KEYBOARD_H
#define SACRAMENTUOS_KEYBOARD_H

#include <stddef.h>

#define KEY_NONE     0
#define KEY_CTRL_C   3
#define KEY_UP       1001
#define KEY_DOWN     1002
#define KEY_LEFT     1003
#define KEY_RIGHT    1004
#define KEY_HOME     1005
#define KEY_END      1006
#define KEY_DELETE   1007
#define KEY_F7       1107
#define KEY_F8       1108
#define KEY_F9       1109

void keyboard_install(void);
int keyboard_getkey(void);
int keyboard_trygetkey(int* out);
char keyboard_getchar(void);
int keyboard_trygetchar(char* out);
void keyboard_flush(void);
int keyboard_cancel_requested(void);
void keyboard_clear_cancel(void);

#endif
