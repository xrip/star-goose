#ifndef GOOSE_SURFACE_H
#define GOOSE_SURFACE_H

#include <stdint.h>

enum {
    CONTROL_LEFT = 0x01,
    CONTROL_RIGHT = 0x02,
    CONTROL_UP = 0x04,
    CONTROL_DOWN = 0x08,
    CONTROL_RIGHT_DEVICE = 0x20,
    CONTROL_LEFT_DEVICE = 0x40,
    CONTROL_FIRE = 0x80
};

/* Surface flight. Weapons and the tunnel live in the session around this. */
typedef struct Surface {
    int x;
    int velocity;
    unsigned camera_y;
    int scroll_speed;
    int base_scroll_speed;
    int fuel;
    int fuel_delta;
    unsigned fuel_distance;
    int shield;
    int z;
    uint8_t frame;
    uint8_t zero_frame_age;
    int alive;
    const uint8_t *tiles;
} Surface;

extern const uint8_t goose_level0_tiles[768];

void surface_start(Surface *game);
void surface_tick(Surface *game, unsigned controls);
void surface_update_speed(Surface *game, unsigned controls);
void surface_apply_fuel(Surface *game);

#endif
