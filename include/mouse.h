#ifndef SACRAMENTUOS_MOUSE_H
#define SACRAMENTUOS_MOUSE_H

#include <stdint.h>

typedef struct {
    int x;
    int y;
    int dx;
    int dy;
    uint8_t buttons;
    uint8_t left_pressed;
    uint8_t left_released;
    uint8_t right_pressed;
    uint8_t right_released;
    uint8_t moved;
    uint8_t present;
} mouse_state_t;

void mouse_install(void);
void mouse_set_bounds(int width, int height);
void mouse_poll(void);
int mouse_get_state(mouse_state_t* out);

#define MOUSE_LEFT   1
#define MOUSE_RIGHT  2
#define MOUSE_MIDDLE 4

#endif
