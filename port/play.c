#include "play.h"
#include "goose_model.h"
#include "maps.h"
#include "hud.h"
#include "pictures.h"
#include "sound.h"
#include "terrain.h"
#include "tunnel.h"
#include <stdio.h>
#include <string.h>

enum { ACTORS = 48, AMMO_FULL = 48, LIVES_FULL = 3 };

typedef struct Actor {
    uint8_t flags;
    uint8_t handler;
    uint8_t kind;
    uint8_t param;
    uint8_t frame;
    uint8_t height;
    uint8_t state[4];
    int8_t z;
    int8_t anchor_y;
    int animated;
    int waiting;
    int object_index;
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

/* Handlers from the map templates. Kinds 15..20 share the collectible handler. */
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
    {32, 1, 0, 0, "collectible_00"},
    {32, 2, 0, 0, "collectible_00"},
    {32, 3, 0, 0, "collectible_03"},
    {32, 4, 0, 0, "collectible_04"},
    {32, 5, 0, 0, "collectible_05"}
};

/* DS:1495 and DS:14C5. Low 10 bits are the resume camera, high 2 bits are the reward. */
static const uint16_t DEST_A[24] = {
    0x0380, 0xc0e0, 0x0380, 0x0380, 0x0380, 0xc180, 0x80d0, 0x0380,
    0xc060, 0x0380, 0x0380, 0x0380, 0x0380, 0x0380, 0x0380, 0x0380,
    0x0380, 0x0380, 0xc050, 0x0380, 0x0380, 0x0380, 0x0380, 0x0380
};
static const uint16_t DEST_B[24] = {
    0x4310, 0x82f0, 0xc3c0, 0x4340, 0x8330, 0xc3a0, 0x43c0, 0xc370,
    0x83a0, 0x8320, 0xc300, 0x43a0, 0x43a0, 0x83c0, 0xc3a0, 0x8350,
    0x4330, 0xc3a0, 0x42f0, 0x8330, 0xc3a0, 0xc3a0, 0x4330, 0x8390
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
static int level_index;
static int level_part;
static int stage_counter;
static int reload_pending;
static int pending_object;
static int pending_part;
static uint8_t collected[6];
static uint8_t score_digits[6];
static int hud_frame;
static int side_charges;
static uint8_t left_device;
static uint8_t right_device;
static int hist_x[4];
static unsigned hist_y[4];
static uint8_t hist_frame[4];
static uint8_t hist_z[4];
static uint32_t consumed[3];
static const uint8_t *map_tiles;
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

/* CS:269E adds one place at a time and carries at most once. A carry out of
 * the 10000 digit grants a life. The 100000 digit wraps without a further life. */
static void score_sync(Game *game) {
    int value = 0;
    for (int i = 0; i < 6; i++) value = value * 10 + score_digits[i];
    game->score = value;
}

static void score_add(Game *game, int thousands, int hundreds, int tens, int units) {
    unsigned ones = score_digits[5] + (unsigned)(units & 255);
    if (ones > 9) {
        ones -= 10;
        score_digits[4]++;
    }
    score_digits[5] = (uint8_t)ones;
    unsigned ten = score_digits[4] + (unsigned)(tens & 255);
    if (ten > 9) {
        ten -= 10;
        score_digits[3]++;
    }
    score_digits[4] = (uint8_t)ten;
    unsigned hundred = score_digits[3] + (unsigned)(hundreds & 255);
    if (hundred > 9) {
        hundred -= 10;
        score_digits[2]++;
    }
    score_digits[3] = (uint8_t)hundred;
    unsigned thousand = score_digits[2] + (unsigned)(thousands & 255);
    if (thousand > 9) {
        thousand -= 10;
        score_digits[1]++;
    }
    score_digits[2] = (uint8_t)thousand;
    if (score_digits[1] > 9) {
        score_digits[1] = (uint8_t)(score_digits[1] - 10);
        score_digits[0]++;
        if (game->lives < LIVES_FULL) game->lives++;
    }
    if (score_digits[0] > 9) score_digits[0] = (uint8_t)(score_digits[0] - 10);
    score_sync(game);
}

static void hud_tick(Game *game) {
    hud_frame++;
    if ((hud_frame & 3) == 0) score_add(game, 0, 0, 0, stage_counter);
}

static unsigned player_y(const Game *game) {
    return (game->flight.camera_y + 112u) & 1023u;
}

static int column_z(int tile_row, int count) {
    unsigned z = 0;
    if (tile_row < 0) tile_row = 0;
    if (tile_row > 63) tile_row = 63;
    for (int c = 0; c < count; c++) {
        int tile = map_tiles[tile_row * 12 + c];
        if (tile > 41) tile = 0;
        z = (z + goose_height_delta[tile]) & 255u;
    }
    return signed_byte(z);
}

/* CS:06D6 column bits 0x0200..0x0001, CS:0902 state byte starts at 10.
 * Stream column 0 is bit 9 and is placed when that byte has fallen to 1.
 * x = 24 * grid - 4. Height sums that many tiles on row 63-stored. */
static int map_object_x(int stream_column) {
    return 24 * (stream_column + 1) - 4;
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

static void consume_actor(const Actor *actor) {
    if (!actor || actor->object_index < 0 || actor->object_index >= 32) return;
    if (level_part < 0 || level_part > 2) return;
    consumed[level_part] |= 1u << actor->object_index;
}

static void activate_row(unsigned row) {
    for (int i = 0; i < ACTORS; i++) {
        if (!actors[i].waiting || (actors[i].y >> 4) != row) continue;
        actors[i].waiting = 0;
        actors[i].flags = GOOSE_RENDER | GOOSE_UPDATE;
    }
}

static int map_number(void) {
    int level = level_index;
    int part = level_part;
    if (level < 0) level = 0;
    if (level > 7) level = 7;
    if (part < 0) part = 0;
    if (part > 2) part = 2;
    return level * 3 + part;
}

static void remember(Game *game) {
    game->level = level_index;
    game->part = level_part;
}

static void place_map(Game *game, unsigned camera) {
    const GooseMap *map = &goose_maps[map_number()];
    map_tiles = map->tiles;
    game->flight.tiles = map->tiles;
    clear_actors();
    for (int i = 0; i < map->object_count; i++) {
        unsigned kind = map->objects[i].kind;
        if (kind >= sizeof KINDS / sizeof KINDS[0]) continue;
        if (kind >= 15 && kind - 15 < 6 && collected[kind - 15]) continue;
        if (level_part >= 0 && level_part < 3 && (consumed[level_part] & (1u << i))) continue;
        const Kind *info = &KINDS[kind];
        Actor *actor = spawn();
        if (!actor) return;
        *actor = (Actor){0};
        actor->object_index = i;
        actor->waiting = 1;
        actor->handler = info->handler;
        actor->kind = (uint8_t)kind;
        actor->param = info->param;
        actor->animated = info->animated;
        actor->state[2] = info->hp;
        actor->x = map_object_x(map->objects[i].column);
        actor->y = (unsigned)map->objects[i].stored_row << 4;
        actor->z = (int8_t)column_z(63 - map->objects[i].stored_row, map->objects[i].column + 1);
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
    shot->object_index = -1;
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
        flash->object_index = -1;
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
        flash->object_index = -1;
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
    shot->object_index = -1;
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
        sound_trigger(6);
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
        sound_trigger(3);
        if (target->state[2] & 0x80) {
            consume_actor(target);
            target->flags = 0;
            score_add(game, 0, 1, 5, 0);
            sound_trigger(7);
        } else {
            score_add(game, 0, 0, 1, 0);
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
        sound_trigger(6);
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
        sound_trigger(4);
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
        consume_actor(actor);
        actor->flags = 0;
        score_add(game, 0, 1, 5, 0);
        sound_trigger(7);
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
            pending_object = actor->object_index;
            pending_part = level_part;
            unsigned index = (unsigned)(level_index * 3 + level_part);
            if (index > 23) index = 0;
            uint16_t dest = entrance_b ? DEST_B[index] : DEST_A[index];
            pending_resume = dest & 1023u;
            pending_reward = (uint8_t)((dest >> 8) & 0xc0);
            if (entrance_b) {
                reload_pending = 1;
                level_part++;
                if (level_part > 2) level_part = 0;
            }
            tunnel_pending = 1;
        }
    }
    if (cull_after(actor, game->flight.camera_y)) actor->flags = 0;
}

static void update_exit(Game *game, Actor *actor) {
    /* CS:18C7: the exit stays shut until every collectible flag is set. */
    int ready = 1;
    for (int i = 0; i < 6; i++) ready &= collected[i] != 0;
    if (!ready) {
        if (cull_after(actor, game->flight.camera_y)) actor->flags = 0;
        return;
    }
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
        if (open == 8 && pull <= 8 && dy <= 8) {
            if (game->flight.frame == 0) {
                pending_object = actor->object_index;
                pending_part = level_part;
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

static const int8_t SIDE_Z[10] = {0, 0, 0, 0, 4, -4, 4, -4, 4, -4};
static const int GROUND_HALF[6] = {14, 8, 8, 14, 10, 14};

static void reset_history(const Game *game) {
    for (int i = 0; i < 4; i++) {
        hist_x[i] = game->flight.x;
        hist_y[i] = player_y(game);
        hist_frame[i] = game->flight.frame;
        hist_z[i] = (uint8_t)game->flight.z;
    }
}

static void push_history(const Game *game) {
    for (int i = 3; i > 0; i--) {
        hist_x[i] = hist_x[i - 1];
        hist_y[i] = hist_y[i - 1];
        hist_frame[i] = hist_frame[i - 1];
        hist_z[i] = hist_z[i - 1];
    }
    hist_x[0] = game->flight.x;
    hist_y[0] = player_y(game);
    hist_frame[0] = game->flight.frame;
    hist_z[0] = (uint8_t)game->flight.z;
}

static void position_side(Actor *actor) {
    int side = (actor->flags & GOOSE_SIDE) != 0;
    unsigned frame = hist_frame[3];
    int zoff = frame < 10 ? SIDE_Z[frame] : 0;
    int z = (hist_z[3] + 6u) & 255u;
    actor->x = hist_x[3] + (side ? 8 : -8);
    actor->y = (hist_y[3] + 6u) & 1023u;
    actor->frame = frame < 10 ? (uint8_t)frame : 9;
    actor->z = (int8_t)signed_byte((unsigned)(side ? z - zoff : z + zoff));
}

static int launch_side(int side) {
    Actor *actor = spawn();
    if (!actor) return 0;
    *actor = (Actor){0};
    actor->object_index = -1;
    actor->flags = GOOSE_RENDER | GOOSE_UPDATE | GOOSE_PROJECTILE;
    if (side) actor->flags |= GOOSE_SIDE;
    actor->handler = 24;
    actor->animated = 1;
    actor->state[2] = 5;
    snprintf(actor->sprite, sizeof actor->sprite, "fs_0DDA");
    position_side(actor);
    side_charges--;
    sound_trigger(1);
    return 1;
}

/* CS:11F0. Press from 0 launches. Press from 2 detaches. Release walks 1→2 and 3→0. */
static void device_button(uint8_t *state, int held, int side) {
    if (held) {
        if (*state == 0) {
            if (side_charges > 0 && launch_side(side)) *state = 1;
        } else if (*state == 2) {
            *state = 3;
            sound_trigger(2);
        }
    } else if (*state == 1) {
        *state = 2;
    } else if (*state == 3) {
        *state = 0;
    }
}

static void update_devices(Game *game, unsigned controls) {
    if (!game->flight.alive || game->flight.frame == 0) return;
    device_button(&left_device, (controls & CONTROL_LEFT_DEVICE) != 0, 0);
    device_button(&right_device, (controls & CONTROL_RIGHT_DEVICE) != 0, 1);
}

static void update_side_follow(Game *game, Actor *actor) {
    if (!game->flight.alive) {
        actor->flags = 0;
        return;
    }
    position_side(actor);
    uint8_t state = (actor->flags & GOOSE_SIDE) ? right_device : left_device;
    if (state == 3) actor->handler = 25;
}

static void update_ground_shot(Game *game, Actor *actor) {
    if (cull_before(actor, game->flight.camera_y)) {
        actor->flags = 0;
        return;
    }
    actor->y = (actor->y - 6u) & 1023u;
    TerrainSample ground = terrain_sample(game->flight.tiles, (unsigned)actor->x, actor->y);
    actor->z = (int8_t)(ground.height + 4);
    actor->frame = ground.frame;
    unsigned row = ground.row;
    if (actor->frame == 0) {
        actor->y = (actor->y + 4u) & 1023u;
        actor->state[2]--;
        if (actor->state[2] & 0x80) {
            actor->flags = 0;
            return;
        }
    }
    for (int i = 0; i < ACTORS; i++) {
        Actor *target = &actors[i];
        if (target->flags == 0 || target->object_index < 0 || target->kind >= 6) continue;
        if ((target->y >> 4) != row) continue;
        int dx = actor->x - target->x;
        if (dx < 0) dx = -dx;
        if (dx > GROUND_HALF[target->kind]) continue;
        consume_actor(target);
        target->flags = 0;
        sound_trigger(6);
        actor->flags = 0;
        return;
    }
}

static void update_actor(Game *game, Actor *actor) {
    if (actor->flags == 0) return;
    if (actor->handler == 19) update_shot(game, actor);
    else if (actor->handler == 24) update_side_follow(game, actor);
    else if (actor->handler == 25) update_ground_shot(game, actor);
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
            if (actor->param < 6) collected[actor->param] = 1;
            consume_actor(actor);
            actor->flags = 0;
            score_add(game, 0, 1, 5, 0);
            sound_trigger(0x0a);
        }
    } else if (cull_after(actor, game->flight.camera_y)) {
        actor->flags = 0;
    }
}

static void enter_tunnel(Game *game) {
    sound_trigger(0x0b);
    if (pending_object >= 0 && pending_part >= 0 && pending_part < 3) {
        consumed[pending_part] |= 1u << pending_object;
    }
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
    left_device = 0;
    right_device = 0;
    tunnel_pending = 0;
}

static void leave_tunnel(Game *game) {
    sound_trigger(0x0b);
    game->mode = MODE_SURFACE;
    game->tunnel_distance = 0;
    game->flight.velocity = 0;
    game->flight.frame = 1;
    game->flight.zero_frame_age = 0;
    game->flight.alive = 1;
    left_device = 0;
    right_device = 0;
    if (game->tunnel_reward == 0xf0) {
        stage_counter++;
        int base = 16;
        if (stage_counter >= 6) base = 0x30;
        else if (stage_counter >= 3) base = 0x20;
        level_index++;
        if (level_index > 7) level_index = 1;
        memset(collected, 0, sizeof collected);
        memset(consumed, 0, sizeof consumed);
        game->flight.base_scroll_speed = base;
        game->flight.scroll_speed = base;
        game->flight.camera_y = 0x350;
        game->flight.x = 128;
        game->flight.fuel_distance = 0;
        reload_pending = 0;
    } else {
        game->flight.camera_y = resume_y & 1023u;
        game->flight.x = resume_x;
        reload_pending = 0;
    }
    place_map(game, game->flight.camera_y);
    remember(game);
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
    level_index = 0;
    level_part = 0;
    stage_counter = 1;
    side_charges = 6;
    left_device = 0;
    right_device = 0;
    reload_pending = 0;
    pending_object = -1;
    memset(collected, 0, sizeof collected);
    memset(consumed, 0, sizeof consumed);
    game->flight.base_scroll_speed = 16;
    resume_y = 0;
    place_map(game, game->flight.camera_y);
    reset_history(game);
    remember(game);
}

static void respawn(Game *game) {
    death_hold = 0;
    game->flight.alive = 1;
    game->flight.fuel = 44;
    game->flight.shield = 56;
    game->ammo = AMMO_FULL;
    game->flight.x = 128;
    game->flight.velocity = 0;
    game->flight.camera_y = 0x350;
    game->flight.scroll_speed = game->flight.base_scroll_speed;
    game->flight.fuel_distance = 0;
    game->flight.frame = 1;
    game->flight.zero_frame_age = 0;
    side_charges = 6;
    left_device = 0;
    right_device = 0;
    if (level_part != 0) level_part = 0;
    place_map(game, game->flight.camera_y);
    reset_history(game);
    remember(game);
}

void game_boot(Game *game) {
    memset(game, 0, sizeof *game);
    game->mode = MODE_TITLE;
    game->lives = LIVES_FULL;
    memset(score_digits, 0, sizeof score_digits);
    hud_frame = 0;
    side_charges = 6;
    sound_reset();
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
    hud_tick(game);
    sound_context(side_charges, ship_angle);
    if (game->mode == MODE_TUNNEL) {
        tunnel_tick(game, controls);
        return;
    }
    if (!game->flight.alive) {
        death_hold++;
        if (death_hold < 20) return;
        game->lives--;
        if (game->lives > 0) respawn(game);
        else {
            game_boot(game);
        }
        return;
    }
    int was_alive = game->flight.alive;
    surface_tick(&game->flight, controls);
    if (was_alive && !game->flight.alive) sound_trigger(0x0c);
    push_history(game);
    update_devices(game, controls);
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
    if (game->mode != MODE_TITLE) {
        hud_draw(game->flight.fuel, game->ammo, game->flight.shield, game->lives, side_charges,
                 collected, score_digits);
    }
    pictures_finish();
}
