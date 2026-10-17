#include "surface.h"
#include "terrain.h"
#include "goose_model.h"

static void sync_ground(Surface *ship, int ahead) {
    unsigned y = (ship->camera_y + (unsigned)ahead) & WORLD_MASK;
    TerrainSample ground = terrain_sample(ship->tiles, (unsigned)ship->x, y);
    ship->z = ground.height;
    ship->frame = ground.frame;
}

static void sync_player(Surface *ship) {
    sync_ground(ship, PLAYER_AHEAD);
}

static void add_fuel_distance(Surface *ship, unsigned fuel_step) {
    unsigned distance = (ship->fuel_distance + fuel_step) & 0xffffu;
    if (distance > FUEL_DISTANCE_LIMIT) {
        distance = 0;
        ship->fuel_delta = -1;
    }
    ship->fuel_distance = distance;
}

static void mark_hull(Surface *ship) {
    if (ship->shield <= 0) ship->alive = 0;
    else if (ship->frame == 0) {
        ship->zero_frame_age = (uint8_t)(ship->zero_frame_age + 1u);
        if (ship->zero_frame_age >= VOID_FRAMES) ship->alive = 0;
    }
}

void surface_update_speed(Surface *game, unsigned controls) {
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
    add_fuel_distance(game, fuel_step);
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

void surface_apply_fuel(Surface *game) {
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

void surface_coast(Surface *game) {
    int speed = game->scroll_speed - 1;
    if (speed >= 0) game->scroll_speed = speed;
    advance_scroll(game);
    sync_player(game);
    surface_apply_fuel(game);
}

int surface_brake(Surface *game) {
    int speed = game->scroll_speed - BRAKE_STEP;
    int crash = speed < 0;
    if (crash) game->alive = 0;
    else game->scroll_speed = speed;
    advance_scroll(game);
    sync_player(game);
    game->frame = (uint8_t)BRAKE_FRAME;
    surface_apply_fuel(game);
    return crash;
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

void surface_passenger(Surface *ship, const Surface *lead, unsigned controls) {
    if (!ship->alive) return;
    ship->camera_y = lead->camera_y;
    ship->tiles = lead->tiles;
    if (controls & CONTROL_UP) ship->nose -= 2;
    else if (controls & CONTROL_DOWN) ship->nose += 2;
    if (ship->nose < -32) ship->nose = -32;
    if (ship->nose > 32) ship->nose = 32;
    move_x(ship, controls);
    unsigned coarse = ((unsigned)lead->scroll_speed + 15u) >> 3;
    add_fuel_distance(ship, (coarse - 1u) & 0xffffu);
    if (ship->frame != 0) sync_ground(ship, PLAYER_AHEAD + ship->nose);
    mark_hull(ship);
    surface_apply_fuel(ship);
}

void surface_tick(Surface *game, unsigned controls) {
    if (!game->alive) return;
    surface_update_speed(game, controls);
    advance_scroll(game);
    move_x(game, controls);
    if (game->frame != 0) sync_player(game);
    mark_hull(game);
    surface_apply_fuel(game);
}
