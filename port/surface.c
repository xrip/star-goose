#include "surface.h"
#include "terrain.h"
#include "goose_model.h"

enum {
    X_MIN = 20,
    X_MAX = 236,
    VELOCITY_MIN = -24,
    VELOCITY_MAX = 24,
    TURN_SNAP = 4,
    SCROLL_MAX = 64,
    BASE_SCROLL = 16,
    FUEL_FULL = 44,
    SHIELD_FULL = 56,
    FUEL_DISTANCE_LIMIT = 139,
    WORLD_MASK = 1023,
    PLAYER_AHEAD = 112,
    START_X = 128,
    START_CAMERA = 0x350,
    VOID_FRAMES = 4
};

static void sync_player(Surface *game) {
    unsigned y = (game->camera_y + PLAYER_AHEAD) & WORLD_MASK;
    TerrainSample ground = terrain_sample(game->tiles, (unsigned)game->x, y);
    game->z = ground.height;
    game->frame = ground.frame;
}

static void update_scroll_speed(Surface *game, unsigned controls) {
    unsigned bits = game->fuel > 0 ? controls : CONTROL_DOWN;
    int speed = game->scroll_speed;
    if (bits & CONTROL_UP) {
        speed += 1;
        if (speed > SCROLL_MAX) speed = SCROLL_MAX;
    } else if (bits & CONTROL_DOWN) {
        speed -= 2;
        if (speed < game->base_scroll_speed) speed = game->base_scroll_speed;
    }
    game->scroll_speed = speed;
}

static void advance_scroll(Surface *game) {
    unsigned coarse = ((unsigned)game->scroll_speed + 15u) >> 3;
    unsigned fuel_step = (coarse - 1u) & 0xffffu;
    unsigned camera_step = coarse >> 1;
    unsigned distance = (game->fuel_distance + fuel_step) & 0xffffu;
    if (distance > FUEL_DISTANCE_LIMIT) {
        distance = 0;
        game->fuel_delta = -1;
    }
    game->fuel_distance = distance;
    game->camera_y = (game->camera_y - camera_step) & WORLD_MASK;
}

static void move_x(Surface *game, unsigned controls) {
    int velocity = game->velocity;
    if (controls & CONTROL_LEFT) {
        velocity -= 1;
        if (velocity > TURN_SNAP) velocity = TURN_SNAP;
        if (velocity < VELOCITY_MIN) velocity = VELOCITY_MIN;
    } else if (controls & CONTROL_RIGHT) {
        velocity += 1;
        if (velocity < -TURN_SNAP) velocity = -TURN_SNAP;
        if (velocity > VELOCITY_MAX) velocity = VELOCITY_MAX;
    } else if (velocity != 0) {
        velocity += (velocity < 0) - (velocity > 0);
    }
    game->velocity = velocity;
    game->x += goose_surface_dx((int16_t)velocity);
    if (game->x < X_MIN) {
        game->x = X_MIN;
        game->velocity = 0;
    }
    if (game->x > X_MAX) {
        game->x = X_MAX;
        game->velocity = 0;
    }
}

static void apply_fuel(Surface *game) {
    int delta = game->fuel_delta;
    game->fuel_delta = 0;
    if (delta < 0) {
        if (game->fuel > 0) game->fuel -= 1;
    } else {
        while (delta > 0) {
            if (game->fuel < FUEL_FULL) game->fuel += 1;
            delta -= 1;
        }
    }
}

void surface_start(Surface *game) {
    *game = (Surface){0};
    game->tiles = goose_level0_tiles;
    game->x = START_X;
    game->camera_y = START_CAMERA;
    game->base_scroll_speed = BASE_SCROLL;
    game->scroll_speed = BASE_SCROLL;
    game->fuel = FUEL_FULL;
    game->shield = SHIELD_FULL;
    game->alive = 1;
    sync_player(game);
}

void surface_tick(Surface *game, unsigned controls) {
    if (!game->alive) return;
    update_scroll_speed(game, controls);
    advance_scroll(game);
    move_x(game, controls);
    if (game->frame != 0) sync_player(game);
    if (game->shield <= 0) {
        game->alive = 0;
    } else if (game->frame == 0) {
        game->zero_frame_age = (uint8_t)(game->zero_frame_age + 1u);
        if (game->zero_frame_age >= VOID_FRAMES) game->alive = 0;
    }
    apply_fuel(game);
}
