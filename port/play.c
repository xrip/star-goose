#include "play.h"
#include "goose_model.h"
#include "pictures.h"
#include "terrain.h"
#include "tunnel.h"
#include <stdio.h>
#include <string.h>

enum { ACTORS = 40, AMMO_FULL = 48, LIVES_FULL = 3 };

typedef struct Actor {
    uint8_t flags;
    uint8_t handler;
    uint8_t param;
    uint8_t frame;
    uint8_t height;
    uint8_t state[4];
    int8_t z;
    int8_t anchor_y;
    int animated;
    int waiting;
    int x;
    unsigned y;
    char sprite[16];
} Actor;

typedef struct Kind {
    uint8_t handler;
    uint8_t param;
    uint8_t hp;
    int animated;
    const char *sprite;
} Kind;

/* Handlers from the map templates. Kind 15 and 16 are collectibles. */
static const Kind KINDS[] = {
    {2, 0, 0, 1, "fs_0E2B"},
    {3, 0, 0, 1, "fs_0E52"},
    {4, 0, 0, 0, "map_kind_02"},
    {5, 0, 0, 1, "fs_0E73"},
    {6, 0, 0, 1, "fs_0E9C"},
    {7, 0, 0, 1, "fs_0EC5"},
    {8, 0, 0, 0, "map_kind_06"},
    {9, 0, 0, 0, "map_kind_07"},
    {10, 0, 0, 0, "map_kind_08"},
    {11, 0, 0, 0, "map_kind_09"},
    {13, 0, 0, 0, "map_kind_10"},
    {26, 0, 0x32, 1, "fs_0EEE"},
    {26, 1, 0x28, 1, "fs_0F3F"},
    {28, 0, 0, 0, "map_kind_13"},
    {26, 2, 0x3c, 1, "fs_0F90"},
    {32, 0, 0, 0, "collectible_00"},
    {32, 1, 0, 0, "collectible_00"}
};

/* Level 1, part 1. y = stored_row * 16, x = 24 * column - 4. */
static const uint8_t LEVEL_OBJECTS[][3] = {
    {13, 4, 59}, {9, 4, 10}, {7, 4, 3}, {1, 1, 51}, {1, 8, 51},
    {2, 6, 28}, {2, 9, 28}, {0, 3, 37}, {0, 2, 14}, {1, 8, 21},
    {1, 7, 19}, {3, 0, 43}, {3, 9, 43}, {3, 2, 24}, {12, 0, 25},
    {12, 4, 13}, {15, 2, 29}, {16, 4, 12}
};

static const uint8_t PINGPONG[6] = {0, 1, 2, 3, 2, 1};
static const int8_t LEFT_DX[10] = {-4, -4, -4, -4, -2, -5, -3, -5, -2, -4};
static const int8_t RIGHT_DX[10] = {4, 4, 4, 4, 5, 2, 5, 3, 5, 3};
static const uint8_t LEFT_DZ[10] = {0, 4, 5, 3, 5, 2, 6, 4, 5, 2};
static const uint8_t RIGHT_DZ[10] = {0, 4, 5, 3, 3, 5, 4, 6, 2, 4};
static const uint8_t SHOT_FRAME[10] = {0, 0, 1, 2, 0, 0, 1, 1, 2, 2};
static const uint8_t SHOT_HEIGHT[3] = {4, 3, 5};
static const int8_t AIM_DX[5] = {-16, -13, 0, 13, 17};
static const int8_t AIM_DY[5] = {3, 9, 12, 9, 3};
static const uint8_t VARIATION[16] = {
    0, 1, 2, 3, 1, 0, 3, 1, 0, 2, 3, 2, 0, 1, 3, 2
};

extern const uint8_t goose_height_delta[42];

static Actor actors[ACTORS];
static unsigned ammo_use;
static uint8_t variation;
static int angular;
static unsigned resume_y;
static int resume_x;
static int level_part;
static int death_hold;
static int tunnel_pending;
static unsigned last_spawn_row;
static uint8_t pending_reward;
static unsigned pending_resume;
static uint8_t ship_angle;

static int signed_byte(unsigned bits) {
    bits &= 255u;
    return bits < 128u ? (int)bits : (int)bits - 256;
}

static unsigned player_y(const Game *game) {
    return (game->flight.camera_y + 112u) & 1023u;
}

static int column_z(int stored, int column) {
    unsigned z = 0;
    for (int c = 0; c < column; c++) {
        int tile = goose_level0_tiles[stored * 12 + c];
        if (tile > 41) tile = 0;
        z = (z + goose_height_delta[tile]) & 255u;
    }
    return signed_byte(z);
}

static Actor *spawn(void) {
    for (int i = 0; i < ACTORS; i++) {
        if (actors[i].flags == 0 && !actors[i].waiting) return &actors[i];
    }
    return 0;
}

static void clear_actors(void) {
    memset(actors, 0, sizeof actors);
}

static unsigned spawn_row(unsigned camera) {
    return ((camera >> 4) - 3u) & 63u;
}

static void activate_row(unsigned row) {
    for (int i = 0; i < ACTORS; i++) {
        if (!actors[i].waiting || (actors[i].y >> 4) != row) continue;
        actors[i].waiting = 0;
        actors[i].flags = GOOSE_RENDER | GOOSE_UPDATE;
    }
}

static void place_map(unsigned camera) {
    clear_actors();
    for (unsigned i = 0; i < sizeof LEVEL_OBJECTS / sizeof LEVEL_OBJECTS[0]; i++) {
        unsigned kind = LEVEL_OBJECTS[i][0];
        if (kind >= sizeof KINDS / sizeof KINDS[0]) continue;
        const Kind *info = &KINDS[kind];
        Actor *actor = spawn();
        if (!actor) return;
        *actor = (Actor){0};
        actor->waiting = 1;
        actor->handler = info->handler;
        actor->param = info->param;
        actor->animated = info->animated;
        actor->state[2] = info->hp;
        actor->x = 24 * LEVEL_OBJECTS[i][1] - 4;
        actor->y = (unsigned)LEVEL_OBJECTS[i][2] << 4;
        actor->z = (int8_t)column_z(LEVEL_OBJECTS[i][2], LEVEL_OBJECTS[i][1]);
        snprintf(actor->sprite, sizeof actor->sprite, "%s", info->sprite);
        char name[24];
        if (actor->animated) snprintf(name, sizeof name, "%s_00", actor->sprite);
        else snprintf(name, sizeof name, "%s", actor->sprite);
        int anchor_y = 0;
        pictures_anchor(name, 0, &anchor_y);
        actor->anchor_y = (int8_t)anchor_y;
    }
    last_spawn_row = spawn_row(camera);
    for (unsigned n = 0; n < 15; n++) activate_row((last_spawn_row + n) & 63u);
}

static void advance_spawns(unsigned camera) {
    unsigned row = spawn_row(camera);
    if (row == last_spawn_row) return;
    activate_row(row);
    last_spawn_row = row;
}

static int cull_after(const Actor *actor, unsigned camera) {
    int zsum = signed_byte((unsigned)actor->z + (unsigned)actor->anchor_y);
    int ax = ((int)actor->y - zsum) & 1023;
    int bx = (int)((camera + 160u) & 1023u);
    if (ax < bx) ax += 1024;
    return (ax - bx) < 0x200;
}

static int cull_before(const Actor *actor, unsigned camera) {
    unsigned mixed = (unsigned)actor->height - (unsigned)(uint8_t)actor->z - (unsigned)(uint8_t)actor->anchor_y;
    int ax = ((int)actor->y + signed_byte(mixed)) & 1023;
    int bx = (int)(camera & 1023u);
    if (ax < bx) ax += 1024;
    return (ax - bx) > 0x200;
}

static void sprite_name(const Actor *actor, char *name, size_t size) {
    if (actor->animated) snprintf(name, size, "%s_%02u", actor->sprite, actor->frame);
    else snprintf(name, size, "%s", actor->sprite);
}

static int near_player(const Game *game, const Actor *actor, int xlim, int ylim) {
    int dx = game->flight.x - actor->x;
    int dy = (int)player_y(game) - (int)actor->y;
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    return dx < xlim && dy < ylim;
}

static void hurt(Game *game, int amount) {
    game->flight.shield -= amount;
    if (game->flight.shield < 0) game->flight.shield = 0;
}

static void spawn_shot(Game *game, int x, unsigned y, int z, int frame, int height) {
    Actor *shot = spawn();
    if (!shot) return;
    *shot = (Actor){0};
    shot->flags = GOOSE_RENDER | GOOSE_UPDATE | GOOSE_PROJECTILE;
    shot->handler = 19;
    shot->frame = (uint8_t)frame;
    shot->height = (uint8_t)height;
    shot->x = x;
    shot->y = y & 1023u;
    shot->z = (int8_t)signed_byte((unsigned)z);
    snprintf(shot->sprite, sizeof shot->sprite, "player_shot");
}

static void fire(Game *game) {
    int frame = game->flight.frame;
    if (frame < 0) frame = 0;
    if (frame > 9) frame = 9;
    int shot_frame = SHOT_FRAME[frame];
    unsigned y = (player_y(game) - 4u) & 1023u;
    int z = game->flight.z;
    spawn_shot(game, game->flight.x + LEFT_DX[frame], y, z + LEFT_DZ[frame], shot_frame, SHOT_HEIGHT[shot_frame]);
    variation = (uint8_t)((variation + 1u) & 255u);
    Actor *flash = spawn();
    if (flash) {
        *flash = (Actor){0};
        flash->flags = GOOSE_RENDER | GOOSE_UPDATE;
        flash->handler = 22;
        flash->frame = VARIATION[variation & 15u];
        flash->animated = 1;
        flash->x = game->flight.x + LEFT_DX[frame];
        flash->y = y;
        flash->z = (int8_t)signed_byte((unsigned)(z + LEFT_DZ[frame]));
        snprintf(flash->sprite, sizeof flash->sprite, "fs_100E");
    }
    spawn_shot(game, game->flight.x + RIGHT_DX[frame], y, z + RIGHT_DZ[frame], shot_frame, SHOT_HEIGHT[shot_frame]);
    flash = spawn();
    if (flash) {
        *flash = (Actor){0};
        flash->flags = GOOSE_RENDER | GOOSE_UPDATE;
        flash->handler = 22;
        flash->frame = VARIATION[variation & 15u];
        flash->animated = 1;
        flash->x = game->flight.x + RIGHT_DX[frame];
        flash->y = y;
        flash->z = (int8_t)signed_byte((unsigned)(z + RIGHT_DZ[frame]));
        snprintf(flash->sprite, sizeof flash->sprite, "fs_100E");
    }
    ammo_use = (ammo_use + (variation & 15u)) & 0xffffu;
    if (ammo_use > 16u) {
        ammo_use = 0;
        if (game->ammo > 0) game->ammo--;
    }
}

static void spawn_enemy_shot(Actor *from, int direction) {
    if (direction < 0) direction = 0;
    if (direction > 4) direction = 4;
    Actor *shot = spawn();
    if (!shot) return;
    *shot = (Actor){0};
    shot->flags = GOOSE_RENDER | GOOSE_UPDATE;
    shot->handler = 20;
    shot->param = (uint8_t)direction;
    shot->x = from->x + AIM_DX[direction];
    shot->y = (from->y + (unsigned)AIM_DY[direction]) & 1023u;
    shot->z = (int8_t)signed_byte((unsigned)from->z + 4u);
    shot->height = 4;
    snprintf(shot->sprite, sizeof shot->sprite, "enemy_shot");
}

static int aim(Game *game, Actor *actor) {
    int dy = (int)((int16_t)((uint16_t)player_y(game) - (uint16_t)actor->y));
    if (dy > 0x8c) {
        actor->frame = 2;
        return 0;
    }
    if (dy < -16) {
        if (actor->flags & GOOSE_SIDE) {
            if (actor->frame < 4) actor->frame++;
            else {
                actor->flags = (uint8_t)(actor->flags & (uint8_t)~GOOSE_SIDE);
                actor->frame = 4;
            }
        } else if (actor->frame > 0) {
            actor->frame--;
        } else {
            actor->flags |= GOOSE_SIDE;
            actor->frame = 0;
        }
        return 0;
    }
    int dx = (int)((int16_t)((uint16_t)game->flight.x - (uint16_t)actor->x));
    if (dx < 0) {
        actor->frame = (uint8_t)(dx > -32 ? 2 : 0);
    } else if (dx < 32) {
        actor->frame = 2;
    } else if (dx == 32) {
        actor->frame = 4;
    } else {
        actor->frame = 3;
    }
    return 1;
}

static void update_shot(Game *game, Actor *shot) {
    if (cull_before(shot, game->flight.camera_y)) {
        shot->flags = 0;
        return;
    }
    shot->y = (shot->y - 16u) & 1023u;
    if (shot->frame == 1 && (uint8_t)shot->z > 0x9c) shot->z = (int8_t)((uint8_t)shot->z - 8u);
    if (shot->frame == 2 && (uint8_t)shot->z < 0x64) shot->z = (int8_t)((uint8_t)shot->z + 8u);
    TerrainSample ground = terrain_sample(game->flight.tiles, (unsigned)shot->x, shot->y);
    if (ground.height >= shot->z) {
        shot->flags = 0;
        return;
    }
    for (int i = 0; i < ACTORS; i++) {
        Actor *target = &actors[i];
        if (target == shot || target->flags == 0 || target->handler != 26) continue;
        int dx = shot->x - target->x;
        int dy = (int)shot->y - (int)target->y;
        int dz = signed_byte((unsigned)shot->z) - signed_byte((unsigned)target->z);
        if (dx < 0) dx = -dx;
        if (dy < 0) dy = -dy;
        if (dz < 0) dz = -dz;
        if (dx >= 0x14 || dy >= 0x10 || dz >= 0x10) continue;
        if (target->state[2] == 0) target->state[2] = 0xff;
        else target->state[2]--;
        if (target->state[2] & 0x80) {
            target->flags = 0;
            game->score += 150;
        } else {
            game->score += 10;
        }
        shot->flags = 0;
        return;
    }
}

static void update_enemy_shot(Game *game, Actor *shot) {
    if (cull_after(shot, game->flight.camera_y)) {
        shot->flags = 0;
        return;
    }
    if (shot->param == 2) shot->y = (shot->y + 8u) & 1023u;
    else if (shot->param == 0) shot->x -= 8;
    else if (shot->param == 1) {
        shot->x -= 6;
        shot->y = (shot->y + 4u) & 1023u;
    } else if (shot->param == 3) {
        shot->x += 6;
        shot->y = (shot->y + 4u) & 1023u;
    } else shot->x += 8;
    shot->y &= 1023u;
    if (shot->x < 4 || shot->x > 0xfc) {
        shot->flags = 0;
        return;
    }
    TerrainSample ground = terrain_sample(game->flight.tiles, (unsigned)shot->x, shot->y);
    if (ground.height >= shot->z) {
        shot->flags = 0;
        return;
    }
    int dx = shot->x - game->flight.x;
    int dy = (int)shot->y - (int)player_y(game);
    int dz = signed_byte((unsigned)shot->z) - game->flight.z;
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    if (dz < 0) dz = -dz;
    if (dx < 0x0c && dy < 8 && dz < 0x10) {
        hurt(game, 1);
        shot->flags = 0;
    }
}

static void update_mobile(Game *game, Actor *actor) {
    actor->y = (actor->y + 1u) & 1023u;
    TerrainSample ground = terrain_sample(game->flight.tiles, (unsigned)actor->x, actor->y);
    actor->z = (int8_t)ground.height;
    if (ground.frame < 10) actor->frame = ground.frame;
    if (game->flight.frame != 0 && near_player(game, actor, 0x1a, 0x12)) {
        hurt(game, 12);
        actor->state[2] = 0xff;
    }
    if (actor->state[2] & 0x80) {
        actor->flags = 0;
        game->score += 150;
        return;
    }
    if (cull_after(actor, game->flight.camera_y)) actor->flags = 0;
}

static void update_emitter(Game *game, Actor *actor, int period) {
    if (!aim(game, actor)) {
        if (cull_after(actor, game->flight.camera_y)) actor->flags = 0;
        return;
    }
    int8_t timer = (int8_t)actor->state[3];
    timer = (int8_t)(timer - 1);
    if (timer < 0) {
        actor->state[3] = (uint8_t)period;
        spawn_enemy_shot(actor, actor->frame);
    } else {
        actor->state[3] = (uint8_t)timer;
    }
    if (cull_after(actor, game->flight.camera_y)) actor->flags = 0;
}

static void nudge_x(Game *game, int dx) {
    if (dx < 0) {
        if (-dx <= 0x18) game->flight.x++;
    } else if (dx > 0 && dx <= 0x18) {
        game->flight.x--;
    }
}

static void update_gate(Game *game, Actor *actor, int entrance_b) {
    int dx = game->flight.x - actor->x - 12;
    if (dx < 0) dx = -dx;
    int dy = (int)player_y(game) - (int)actor->y;
    if (dy < 0) dy = -dy;
    int open = actor->frame >> 1;
    if (dx + dy <= 0x40) {
        if (open != 6) actor->frame++;
    } else if (open != 0) {
        actor->frame--;
    }
    if (open >= 1 && game->flight.frame != 0) {
        int pull = game->flight.x - actor->x - 12;
        if (pull < 0) pull = -pull;
        nudge_x(game, game->flight.x - actor->x - 12);
        if (pull <= 8 && dy <= 8) {
            resume_x = game->flight.x;
            if (entrance_b) {
                static const uint16_t DEST_B[] = {
                    0x4310, 0x82f0, 0xc3c0, 0x4340, 0x8330, 0xc3a0, 0x43c0, 0xc370
                };
                unsigned index = (unsigned)level_part;
                if (index > 7) index = 0;
                pending_resume = DEST_B[index] & 1023u;
                pending_reward = (uint8_t)(DEST_B[index] >> 8) & 0xc0;
                level_part++;
                if (level_part > 2) level_part = 0;
            }
            tunnel_pending = 1;
        }
    }
    if (cull_after(actor, game->flight.camera_y)) actor->flags = 0;
}

static void update_exit(Game *game, Actor *actor) {
    int dx = game->flight.x - actor->x - 12;
    if (dx < 0) dx = -dx;
    int dy = (int)player_y(game) - 8 - (int)actor->y;
    if (dy < 0) dy = -dy;
    int open = actor->frame >> 1;
    if (dx + dy <= 0x40) {
        if (open != 8) actor->frame++;
    } else if (open != 0) {
        actor->frame--;
    }
    if (open >= 2) {
        int pull = game->flight.x - actor->x - 12;
        if (pull < 0) pull = -pull;
        nudge_x(game, game->flight.x - actor->x - 12);
        if (pull <= 8 && dy <= 8) {
            if (game->flight.frame == 0) {
                pending_reward = 0xf0;
                pending_resume = resume_y;
                tunnel_pending = 1;
            } else {
                game->flight.frame = 0;
            }
        }
    }
    if (cull_after(actor, game->flight.camera_y)) actor->flags = 0;
}

static void update_actor(Game *game, Actor *actor) {
    if (actor->flags == 0) return;
    if (actor->handler == 19) update_shot(game, actor);
    else if (actor->handler == 20) update_enemy_shot(game, actor);
    else if (actor->handler == 22) {
        if (actor->frame == 0) actor->flags = 0;
        else actor->frame--;
    } else if (actor->handler == 2) {
        actor->state[3]++;
        actor->frame = (uint8_t)((actor->state[3] >> 1) & 3u);
        if (cull_after(actor, game->flight.camera_y)) actor->flags = 0;
    } else if (actor->handler == 3) {
        actor->state[3]++;
        unsigned step = (actor->state[3] >> 1) & 7u;
        if (step >= 6) {
            step = 0;
            actor->state[3] = 0;
        }
        actor->frame = PINGPONG[step];
        if (cull_after(actor, game->flight.camera_y)) actor->flags = 0;
    } else if (actor->handler == 5) update_emitter(game, actor, 8);
    else if (actor->handler == 6) update_emitter(game, actor, 0x12);
    else if (actor->handler == 7) update_emitter(game, actor, 0x28);
    else if (actor->handler == 26) update_mobile(game, actor);
    else if (actor->handler == 10) update_gate(game, actor, 0);
    else if (actor->handler == 11) update_gate(game, actor, 1);
    else if (actor->handler == 28) update_exit(game, actor);
    else if (actor->handler == 32) {
        if (cull_after(actor, game->flight.camera_y)) actor->flags = 0;
        else if (game->flight.frame != 0 && near_player(game, actor, 0x12, 0x10)) {
            actor->flags = 0;
            game->score += 150;
        }
    } else if (cull_after(actor, game->flight.camera_y)) {
        actor->flags = 0;
    }
}

static void enter_tunnel(Game *game) {
    game->mode = MODE_TUNNEL;
    game->tunnel_distance = 0;
    game->tunnel_reward = pending_reward;
    resume_y = pending_resume;
    angular = 0;
    ship_angle = 0;
    game->flight.camera_y = 0x1a;
    tunnel_player_position(ship_angle, &game->tunnel_x, &game->tunnel_y);
    game->tunnel_frame = tunnel_player_frame(ship_angle);
    clear_actors();
    tunnel_pending = 0;
}

static void leave_tunnel(Game *game) {
    game->mode = MODE_SURFACE;
    game->flight.camera_y = resume_y & 1023u;
    game->flight.x = resume_x;
    game->flight.velocity = 0;
    game->flight.frame = 1;
    game->flight.zero_frame_age = 0;
    game->tunnel_distance = 0;
}

static void tunnel_tick(Game *game, unsigned controls) {
    surface_update_speed(&game->flight, controls);
    int step = ((game->flight.scroll_speed + 7) >> 3) + 2;
    game->tunnel_distance += step;
    unsigned fuel = (game->flight.fuel_distance + (unsigned)step) & 0xffffu;
    if (fuel > 0x2e8u) {
        fuel = 0;
        game->flight.fuel_delta = -1;
    }
    game->flight.fuel_distance = fuel;
    surface_apply_fuel(&game->flight);
    if (game->tunnel_distance >= 0x380) {
        leave_tunnel(game);
        return;
    }
    int force = 0;
    int speed = game->flight.scroll_speed >> 3;
    if (controls & CONTROL_LEFT) force = -speed - 3;
    else if (controls & CONTROL_RIGHT) force = speed + 3;
    uint8_t ang = ship_angle & 0x7fu;
    int passive;
    if (ang > 0x3f) {
        uint8_t folded = (uint8_t)((uint8_t)(-ang) + 0x7fu);
        passive = -goose_tunnel_angle_force(folded);
    } else {
        passive = goose_tunnel_angle_force(ang);
    }
    angular += force + passive;
    if (angular < 0) {
        angular++;
        if (angular < 0) angular++;
    } else if (angular > 0) {
        angular--;
        if (angular > 0) angular--;
    }
    if (angular > 64) angular = 64;
    if (angular < -64) angular = -64;
    ship_angle = (uint8_t)(ship_angle + (uint8_t)goose_signed16(goose_sar16((uint16_t)(int16_t)angular, 2)));
    tunnel_player_position(ship_angle, &game->tunnel_x, &game->tunnel_y);
    game->tunnel_frame = tunnel_player_frame(ship_angle);
}

static void begin_level(Game *game) {
    surface_start(&game->flight);
    game->ammo = AMMO_FULL;
    game->mode = MODE_SURFACE;
    angular = 0;
    ship_angle = 0;
    game->tunnel_distance = 0;
    ammo_use = 0;
    variation = 0;
    death_hold = 0;
    tunnel_pending = 0;
    level_part = 0;
    resume_y = 0;
    place_map(game->flight.camera_y);
}

void game_boot(Game *game) {
    memset(game, 0, sizeof *game);
    game->mode = MODE_TITLE;
    game->lives = LIVES_FULL;
    surface_start(&game->flight);
}

void game_play(Game *game) {
    int score = game->score;
    int lives = game->lives > 0 ? game->lives : LIVES_FULL;
    begin_level(game);
    game->score = score;
    game->lives = lives;
}

void game_tick(Game *game, unsigned controls) {
    if (game->mode == MODE_TITLE) {
        if (controls & CONTROL_FIRE) game_play(game);
        return;
    }
    if (game->mode == MODE_TUNNEL) {
        tunnel_tick(game, controls);
        return;
    }
    if (!game->flight.alive) {
        death_hold++;
        if (death_hold < 20) return;
        game->lives--;
        if (game->lives > 0) {
            int score = game->score;
            int lives = game->lives;
            begin_level(game);
            game->score = score;
            game->lives = lives;
        } else {
            game_boot(game);
        }
        return;
    }
    surface_tick(&game->flight, controls);
    advance_spawns(game->flight.camera_y);
    if (game->flight.alive && game->flight.frame != 0 && game->ammo > 0 && (controls & CONTROL_FIRE)) {
        fire(game);
    }
    tunnel_pending = 0;
    for (int i = 0; i < ACTORS; i++) update_actor(game, &actors[i]);
    if (tunnel_pending) enter_tunnel(game);
}

void game_draw(Game *game) {
    pictures_clear();
    if (game->mode == MODE_TITLE) {
        pictures_intro();
    } else if (game->mode == MODE_TUNNEL) {
        uint8_t bits[TUNNEL_BYTES];
        tunnel_build_bitmap(bits, (unsigned)game->tunnel_distance, 3);
        pictures_tunnel(bits, tunnel_line_color(game->tunnel_reward));
        char name[24];
        snprintf(name, sizeof name, "fs_1594_%02u", game->tunnel_frame);
        pictures_sprite(name, game->tunnel_x, game->tunnel_y, 0, 0x1a);
    } else {
        pictures_draw_tiles(&game->flight);
        for (int i = 0; i < ACTORS; i++) {
            if (actors[i].flags == 0) continue;
            char name[24];
            sprite_name(&actors[i], name, sizeof name);
            pictures_sprite(name, actors[i].x, (int)actors[i].y, actors[i].z, (int)game->flight.camera_y);
        }
        pictures_draw_ship(&game->flight);
    }
    pictures_finish();
}
