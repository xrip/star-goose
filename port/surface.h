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

enum {
    X_MIN = 20,
    X_MAX = 236,
    VELOCITY_MIN = -24,
    VELOCITY_MAX = 24,
    TURN_SNAP = 4,
    SCROLL_MAX = 64,
    BASE_SCROLL = 16,
    SCROLL_STAGE3 = 32,
    SCROLL_STAGE6 = 48,
    FUEL_FULL = 44,
    SHIELD_FULL = 56,
    AMMO_FULL = 48,
    LIVES_FULL = 3,
    SIDE_CHARGES_FULL = 6,
    FUEL_DISTANCE_LIMIT = 139,
    WORLD_MASK = 1023,
    PLAYER_AHEAD = 112,
    START_X = 128,
    START_CAMERA = 0x350,
    VOID_FRAMES = 4,
    BRAKE_STEP = 3,
    BRAKE_FRAME = 10
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
/* Handler 17: slow the scroll by one, then keep the camera and the terrain moving. */
void surface_coast(Surface *game);
/* Handler 35. Subtract three from the scroll. A negative result starts the crash
 * and leaves the previous speed in place. The camera still advances, and the
 * ship is held on its last frame. Returns 1 when the crash starts. */
int surface_brake(Surface *game);

#endif
