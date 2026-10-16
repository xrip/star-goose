#include "play.h"
#include "goose_model.h"
#include "maps.h"
#include "hud.h"
#include "pictures.h"
#include "sound.h"
#include "terrain.h"
#include "tunnel.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>

enum { ACTORS = GOOSE_ENTITY_CAPACITY };

enum {
    HANDLER_LOOP = 2,
    HANDLER_PINGPONG = 3,
    HANDLER_PROP = 4,
    HANDLER_EMIT_DIRECT = 5,
    HANDLER_EMIT_ARC = 6,
    HANDLER_EMIT_GROUND = 7,
    HANDLER_REFILL = 8,
    HANDLER_BLOCK = 9,
    HANDLER_GATE_A = 10,
    HANDLER_GATE_B = 11,
    HANDLER_BLOCK_B = 13,
    HANDLER_TUNNEL_EXIT = 14,
    HANDLER_TUNNEL_PICKUP = 15,
    HANDLER_EXPLOSION = 16,
    HANDLER_CULL = 18,
    HANDLER_PLAYER_SHOT = 19,
    HANDLER_ENEMY_SHOT = 20,
    HANDLER_IMPACT = 21,
    HANDLER_COUNTDOWN = 22,
    HANDLER_SIDE_IMPACT = 23,
    HANDLER_SIDE_FOLLOW = 24,
    HANDLER_SIDE_GROUND = 25,
    HANDLER_MOBILE = 26,
    HANDLER_ACCEL = 27,
    HANDLER_LEVEL_EXIT = 28,
    HANDLER_GROUND_ENEMY = 29,
    HANDLER_ARC = 30,
    HANDLER_ARC_BURST = 31,
    HANDLER_COLLECTIBLE = 32,
    HANDLER_DRIFT = 33,
    HANDLER_TUNNEL_SIDE = 34,
    DIRECT_RELOAD = 8,
    ARC_RELOAD = 18,
    GROUND_RELOAD = 40,
    CULL_SPAN = 512,
    AIM_TOO_FAR = 140,
    SHOT_Z_HIGH = 156,
    SHOT_Z_LOW = 100,
    SHOT_Z_STEP = 8,
    HIT_X = 20,
    HIT_Y = 16,
    HIT_Z = 16,
    SHOT_HALF_INSET = 14,
    LARGE_SHIFT_X = 12,
    LARGE_SHIFT_Y = 8,
    KIND_LARGE = 2,
    ACCEL_STEP_MAX = 8,
    PLAYER_HIT_X = 12,
    ENEMY_OFF_LEFT = 4,
    ENEMY_OFF_RIGHT = 252,
    MOBILE_HIT_X = 26,
    MOBILE_HIT_Y = 18,
    MOBILE_AIM_X = 32,
    MOBILE_RELOAD = 7,
    COLLECT_X = 18,
    COLLECT_Y = 16,
    HP_MARK = 0xff,
    HP_DEPLETED = 0x80,
    REFILL_FAR = 32,
    NUDGE_REACH = 24,
    REFILL_CLOSE = 8,
    REFILL_CORE = 2,
    REFILL_KILL = 36,
    GROUND_OFF_LEFT = 12,
    GROUND_OFF_RIGHT = 244,
    GROUND_HIT = 16,
    ARC_OFF_LEFT = 8,
    ARC_OFF_RIGHT = 248,
    ARC_HIT_X = 14,
    ARC_HIT_Y = 10,
    ARC_LIFE = 16,
    GATE_REACH = 64,
    GATE_OPEN = 8,
    OBJECT_SLOTS = 32,
    DAMAGE_KINDS = 6,
    COLUMN_SPAN = 24,
    COLUMN_BIAS = 4,
    PARTS = 3,
    LEVELS = 8,
    PICKUP_AMMO = 6,
    PICKUP_FUEL = 4,
    PICKUP_SHIELD = 8,
    STAGE_QUICK = 3,
    STAGE_FASTER = 6,
    NO_SECTION = 0xff,
    ANGLE_MASK = 127,
    ANGLE_HALF = 63,
    SCORE_PLACES = 6,
    COLLECTIBLES = 6,
    HISTORY_LAG = 3,
    MOBILE_HP_A = 50,
    MOBILE_HP_B = 40,
    MOBILE_HP_C = 60,
    LAYER_TOP = 7,
    LAYER_SHIP = 2,
    TILE_ROWS = 64,
    MAP_COLS = 12,
    HEIGHT_TILES = 42,
    BYTE_MASK = 255,
    SPAWN_AHEAD = 15,
    CULL_AHEAD = 160,
    AMMO_SPEND = 16,
    AIM_BAND = 32,
    AIM_BEHIND = 16,
    SHOT_STEP = 16,
    SHOT_LIFT = 4,
    COLLECTIBLE_KIND = 15,
    SHIP_FIRE_FRAME = 9,
    AIM_LAST = 4,
    VARIATION_MASK = 15,
    DIGIT_BASE = 10,
    SCORE_PHASE = 3,
    ENEMY_STEP = 8,
    ENEMY_DIAG_X = 6,
    ENEMY_DIAG_Y = 4,
    ENEMY_HIT_Y = 8,
    TOUCH_HURT = 1,
    MOBILE_HURT = 12,
    GATE_INSET = 12,
    ENTRANCE_FULL = 6,
    PULL_LIMIT = 8,
    HEAVY_HURT = 100,
    ARC_HURT = 10,
    GROUND_STEP = 5,
    GROUND_DIAG_X = 3,
    GROUND_DIAG_Y = 2,
    LOB_Z = 8,
    ARC_STEP_X = 4,
    ARC_STEP_Y = 3,
    HISTORY = 4,
    SIDE_OFFSET = 8,
    SIDE_TRAIL = 6,
    TUNNEL_SIDE_OFFSET = 8,
    ANGLE_HISTORY = 3,
    FLASH_HOT = 15,
    FLASH_DIM = 7,
    ATTRACT_X = 128,
    ATTRACT_Y = 200,
    DRIFT_STEP = 2,
    ATTRACT_EVERY = 8,
    ATTRACT_INTRO = 15,
    SCRIPT_HOLD = 0xfe,
    SCRIPT_END = 0xff,
    SCRIPT_PAIR = 0x80,
    SCORE_SLOTS = 7,
    SCORE_SHOW = 6,
    NAME_LEN = 12,
    HISCORE_BYTES = 140,
    SCORE_ROW_FIRST = 12,
    SCORE_ROW_LAST = 23,
    DEATH_TICKS = 40,
    FORCE_STOP_LO = 0x28,
    FORCE_STOP_HI = 0x58,
    IMPACT_W = 8,
    IMPACT_H = 5,
    IMPACT_ANCHOR = 2,
    IMPACT_FRAMES = 4,
    BANK_BIRD1 = 0,
    GROUND_LIFE = 5,
    EXPLOSION_FRAMES = 5,
    BURST_FRAMES = 6
};

typedef struct Actor {
    uint8_t flags;
    uint8_t handler;
    uint8_t kind;
    uint8_t param;
    uint8_t layer;
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
    {HANDLER_LOOP, 0, 0, 1, "fs_0E2B"},
    {HANDLER_PINGPONG, 0, 0, 1, "fs_0E52"},
    {HANDLER_PROP, 0, 0, 0, "map_kind_02"},
    {HANDLER_EMIT_DIRECT, 0, 0, 1, "fs_0E73"},
    {HANDLER_EMIT_ARC, 0, 0, 1, "fs_0E9C"},
    {HANDLER_EMIT_GROUND, 0, 0, 1, "fs_0EC5"},
    {HANDLER_REFILL, 0, 0, 0, "map_kind_06"},
    {HANDLER_BLOCK, 0, 0, 0, "map_kind_07"},
    {HANDLER_GATE_A, 0, 0, 0, "map_kind_08"},
    {HANDLER_GATE_B, 0, 0, 0, "map_kind_09"},
    {HANDLER_BLOCK_B, 0, 0, 0, "map_kind_10"},
    {HANDLER_MOBILE, 0, MOBILE_HP_A, 1, "fs_0EEE"},
    {HANDLER_MOBILE, 1, MOBILE_HP_B, 1, "fs_0F3F"},
    {HANDLER_LEVEL_EXIT, 0, 0, 0, "map_kind_13"},
    {HANDLER_MOBILE, 2, MOBILE_HP_C, 1, "fs_0F90"},
    {HANDLER_COLLECTIBLE, 0, 0, 0, "collectible_00"},
    {HANDLER_COLLECTIBLE, 1, 0, 0, "collectible_00"},
    {HANDLER_COLLECTIBLE, 2, 0, 0, "collectible_00"},
    {HANDLER_COLLECTIBLE, 3, 0, 0, "collectible_03"},
    {HANDLER_COLLECTIBLE, 4, 0, 0, "collectible_04"},
    {HANDLER_COLLECTIBLE, 5, 0, 0, "collectible_05"}
};

/* DS:031E. Index is the handler that stood on the obstacle before it was broken. */
static const char *REPLACEMENT_SPRITE[8] = {
    "collectible_00", "collectible_00", "collectible_00", "collectible_03",
    "collectible_04", "collectible_05", "collectible_06", "collectible_05"
};
static const uint8_t REPLACEMENT_H[8] = {11, 11, 11, 5, 5, 13, 13, 13};
static const int8_t REPLACEMENT_ANCHOR[8] = {-2, -2, -2, -4, 4, 0, 1, 0};

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
static int mobile_gate;
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
static uint16_t spawn_rows[TILE_ROWS];
static uint16_t available_masks[PARTS][TILE_ROWS];
static const uint16_t COLUMN_BIT[10] = {
    0x0200, 0x0100, 0x0080, 0x0040, 0x0020, 0x0010, 0x0008, 0x0004, 0x0002, 0x0001
};
static const uint16_t COLUMN_CLEAR[11] = {
    0xffff, 0xfdff, 0xfeff, 0xff7f, 0xffbf, 0xffdf, 0xffef, 0xfff7, 0xfffb, 0xfffd, 0xfffe
};
static const uint8_t *map_tiles;
static int death_hold;
static int dying;
static int forced_stop;
static uint8_t side_impact_mask;
static int tunnel_pending;
static unsigned last_spawn_row;
static uint8_t pending_reward;
static unsigned pending_resume;
static uint8_t ship_angle;
static uint8_t angle_hist[ANGLE_HISTORY];
static uint8_t tunnel_flash;
static int attract_seen;
static unsigned attract_step;
static int attract_base;
static int scores_ready;
static int pending_name;
static int name_editing;
static int name_cursor;
static int name_slot;
static uint8_t names[SCORE_SLOTS][NAME_LEN];
static uint8_t board[SCORE_SLOTS][SCORE_PLACES];
static uint8_t score_order[SCORE_SLOTS];
static uint8_t tunnel_pattern_index;
static uint8_t tunnel_pattern;
static uint8_t tunnel_last_section = NO_SECTION;
static int tunnel_exit_spawned;
static int tunnel_min_radius = 3;

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
    return (game->flight.camera_y + PLAYER_AHEAD) & WORLD_MASK;
}

static int column_z(int tile_row, int count) {
    unsigned z = 0;
    if (tile_row < 0) tile_row = 0;
    if (tile_row > TILE_ROWS - 1) tile_row = TILE_ROWS - 1;
    for (int c = 0; c < count; c++) {
        int tile = map_tiles[tile_row * MAP_COLS + c];
        if (tile >= HEIGHT_TILES) tile = 0;
        z = (z + goose_height_delta[tile]) & BYTE_MASK;
    }
    return signed_byte(z);
}

static int map_object_x(int stream_column) {
    return COLUMN_SPAN * (stream_column + 1) - COLUMN_BIAS;
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
    return ((camera >> 4) - 3u) & (TILE_ROWS - 1);
}

static void consume_actor(const Actor *actor) {
    if (!actor || actor->object_index < 0 || actor->object_index >= OBJECT_SLOTS) return;
    if (level_part < 0 || level_part >= PARTS) return;
    consumed[level_part] |= 1u << actor->object_index;
}

static void activate_row(unsigned row) {
    for (int i = 0; i < ACTORS; i++) {
        if (!actors[i].waiting || (actors[i].y >> 4) != row) continue;
        uint8_t kept = (uint8_t)(actors[i].flags & (GOOSE_MAP_OBJECT | GOOSE_LARGE_ANCHOR));
        actors[i].waiting = 0;
        actors[i].flags = (uint8_t)(GOOSE_RENDER | GOOSE_UPDATE | kept);
    }
}

static void spawn_replaced(int index, const GooseMapObject *object, int active) {
    if (object->kind >= sizeof KINDS / sizeof KINDS[0]) return;
    unsigned handler = KINDS[object->kind].handler;
    if (handler >= sizeof REPLACEMENT_SPRITE / sizeof REPLACEMENT_SPRITE[0]) return;
    Actor *actor = spawn();
    if (!actor) return;
    *actor = (Actor){0};
    actor->object_index = index;
    actor->kind = 0xff;
    actor->handler = HANDLER_CULL;
    actor->height = REPLACEMENT_H[handler];
    actor->anchor_y = REPLACEMENT_ANCHOR[handler];
    actor->x = map_object_x(object->column);
    actor->y = (unsigned)object->stored_row << 4;
    actor->z = (int8_t)column_z(TILE_ROWS - 1 - object->stored_row, object->column + 1);
    snprintf(actor->sprite, sizeof actor->sprite, "%s", REPLACEMENT_SPRITE[handler]);
    actor->waiting = active ? 0 : 1;
    if (active) actor->flags = GOOSE_RENDER | GOOSE_UPDATE;
}

static void apply_replacement(Actor *actor) {
    if (actor->kind >= sizeof KINDS / sizeof KINDS[0]) {
        actor->flags = 0;
        return;
    }
    unsigned handler = KINDS[actor->kind].handler;
    if (handler >= sizeof REPLACEMENT_SPRITE / sizeof REPLACEMENT_SPRITE[0]) {
        actor->flags = 0;
        return;
    }
    int x = actor->x;
    unsigned y = actor->y;
    int8_t z = actor->z;
    int index = actor->object_index;
    actor->kind = 0xff;
    actor->handler = HANDLER_CULL;
    actor->animated = 0;
    actor->frame = 0;
    actor->height = REPLACEMENT_H[handler];
    actor->anchor_y = REPLACEMENT_ANCHOR[handler];
    actor->x = x;
    actor->y = y;
    actor->z = z;
    actor->object_index = index;
    actor->flags = GOOSE_RENDER | GOOSE_UPDATE;
    actor->waiting = 0;
    snprintf(actor->sprite, sizeof actor->sprite, "%s", REPLACEMENT_SPRITE[handler]);
}

static void destroy_map_actor(Actor *actor) {
    unsigned row = actor->y >> 4;
    int column = (actor->x + COLUMN_BIAS) / COLUMN_SPAN;
    consume_actor(actor);
    if (level_part >= 0 && level_part < PARTS && row < TILE_ROWS && column >= 0 && column <= 10)
        available_masks[level_part][row] &= COLUMN_CLEAR[column];
    apply_replacement(actor);
}

static int replacement_present(int index) {
    for (int i = 0; i < ACTORS; i++) {
        if (actors[i].object_index != index) continue;
        if (actors[i].flags || actors[i].waiting) return 1;
    }
    return 0;
}

static int map_number(void);

static void spawn_replacements(unsigned row) {
    if (level_part < 0 || level_part >= PARTS) return;
    const GooseMap *map = &goose_maps[map_number()];
    for (int i = 0; i < map->object_count; i++) {
        if (map->objects[i].stored_row != row) continue;
        if ((consumed[level_part] & (1u << i)) == 0) continue;
        if (replacement_present(i)) continue;
        spawn_replaced(i, &map->objects[i], 1);
    }
}

static int map_number(void) {
    int level = level_index;
    int part = level_part;
    if (level < 0) level = 0;
    if (level > LEVELS - 1) level = LEVELS - 1;
    if (part < 0) part = 0;
    if (part > PARTS - 1) part = PARTS - 1;
    return level * PARTS + part;
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
        if (kind >= COLLECTIBLE_KIND && kind - COLLECTIBLE_KIND < COLLECTIBLES &&
            collected[kind - COLLECTIBLE_KIND]) continue;
        if (level_part >= 0 && level_part < PARTS && (consumed[level_part] & (1u << i))) {
            spawn_replaced(i, &map->objects[i], 0);
            continue;
        }
        const Kind *info = &KINDS[kind];
        Actor *actor = spawn();
        if (!actor) return;
        *actor = (Actor){0};
        actor->object_index = i;
        actor->waiting = 1;
        if (kind < DAMAGE_KINDS) {
            actor->flags = GOOSE_MAP_OBJECT;
            if (kind == KIND_LARGE) actor->flags |= GOOSE_LARGE_ANCHOR;
        }
        actor->handler = info->handler;
        actor->kind = (uint8_t)kind;
        actor->param = info->param;
        actor->animated = info->animated;
        actor->state[2] = info->hp;
        actor->x = map_object_x(map->objects[i].column);
        actor->y = (unsigned)map->objects[i].stored_row << 4;
        actor->z = (int8_t)column_z(TILE_ROWS - 1 - map->objects[i].stored_row, map->objects[i].column + 1);
        snprintf(actor->sprite, sizeof actor->sprite, "%s", info->sprite);
        char name[24];
        if (actor->animated) snprintf(name, sizeof name, "%s_00", actor->sprite);
        else snprintf(name, sizeof name, "%s", actor->sprite);
        int anchor_y = 0;
        pictures_anchor(name, 0, &anchor_y);
        actor->anchor_y = (int8_t)anchor_y;
    }
    last_spawn_row = spawn_row(camera);
    for (unsigned n = 0; n < SPAWN_AHEAD; n++) activate_row((last_spawn_row + n) & (TILE_ROWS - 1));
    memset(spawn_rows, 0, sizeof spawn_rows);
    for (int i = 0; i < map->object_count; i++) {
        unsigned column = map->objects[i].column;
        unsigned row = map->objects[i].stored_row;
        if (column >= 10 || row >= TILE_ROWS) continue;
        spawn_rows[row] |= (uint16_t)(((unsigned)map->objects[i].kind << 10) | COLUMN_BIT[column]);
    }
}

static void advance_spawns(unsigned camera) {
    unsigned row = spawn_row(camera);
    if (row == last_spawn_row) return;
    spawn_replacements(row);
    activate_row(row);
    last_spawn_row = row;
}

static void sprite_name(const Actor *actor, char *name, size_t size);

static void sync_frame(Actor *actor) {
    if (!actor->animated) return;
    char name[24];
    sprite_name(actor, name, sizeof name);
    int height = 0;
    int anchor_y = 0;
    if (!pictures_metrics(name, &height, &anchor_y)) return;
    actor->height = (uint8_t)height;
    actor->anchor_y = (int8_t)anchor_y;
}

static int cull_after(Actor *actor, unsigned camera) {
    sync_frame(actor);
    int zsum = signed_byte((unsigned)actor->z + (unsigned)actor->anchor_y);
    int ax = ((int)actor->y - zsum) & WORLD_MASK;
    int bx = (int)((camera + CULL_AHEAD) & WORLD_MASK);
    if (ax < bx) ax += WORLD_MASK + 1;
    return (ax - bx) < CULL_SPAN;
}

static int cull_before(Actor *actor, unsigned camera) {
    sync_frame(actor);
    unsigned mixed = (unsigned)actor->height - (unsigned)(uint8_t)actor->z - (unsigned)(uint8_t)actor->anchor_y;
    int ax = ((int)actor->y + signed_byte(mixed)) & WORLD_MASK;
    int bx = (int)(camera & WORLD_MASK);
    if (ax < bx) ax += WORLD_MASK + 1;
    return (ax - bx) > CULL_SPAN;
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
    shot->handler = HANDLER_PLAYER_SHOT;
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
    if (frame > SHIP_FIRE_FRAME) frame = SHIP_FIRE_FRAME;
    int shot_frame = SHOT_FRAME[frame];
    unsigned y = (player_y(game) - 4u) & 1023u;
    int z = game->flight.z;
    spawn_shot(game, game->flight.x + LEFT_DX[frame], y, z + LEFT_DZ[frame], shot_frame, SHOT_HEIGHT[shot_frame]);
    variation = (uint8_t)((variation + 1u) & BYTE_MASK);
    Actor *flash = spawn();
    if (flash) {
        *flash = (Actor){0};
        flash->object_index = -1;
        flash->flags = GOOSE_RENDER | GOOSE_UPDATE;
        flash->handler = HANDLER_COUNTDOWN;
        flash->frame = VARIATION[variation & VARIATION_MASK];
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
        flash->handler = HANDLER_COUNTDOWN;
        flash->frame = VARIATION[variation & VARIATION_MASK];
        flash->animated = 1;
        flash->x = game->flight.x + RIGHT_DX[frame];
        flash->y = y;
        flash->z = (int8_t)signed_byte((unsigned)(z + RIGHT_DZ[frame]));
        snprintf(flash->sprite, sizeof flash->sprite, "fs_100E");
    }
    ammo_use = (ammo_use + (variation & VARIATION_MASK)) & 0xffffu;
    if (ammo_use > AMMO_SPEND) {
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
    shot->handler = HANDLER_ENEMY_SHOT;
    shot->param = (uint8_t)direction;
    shot->x = from->x + AIM_DX[direction];
    shot->y = (from->y + (unsigned)AIM_DY[direction]) & 1023u;
    shot->z = (int8_t)signed_byte((unsigned)from->z + 4u);
    shot->height = 4;
    snprintf(shot->sprite, sizeof shot->sprite, "enemy_shot");
}

static int aim(Game *game, Actor *actor) {
    int dy = (int)((int16_t)((uint16_t)player_y(game) - (uint16_t)actor->y));
    if (dy > AIM_TOO_FAR) {
        actor->frame = 2;
        return 0;
    }
    if (dy < -AIM_BEHIND) {
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
        actor->frame = (uint8_t)(dx > -AIM_BAND ? 2 : 0);
    } else if (dx < AIM_BAND) {
        actor->frame = 2;
    } else if (dx == AIM_BAND) {
        actor->frame = 4;
    } else {
        actor->frame = 3;
    }
    return 1;
}

static const unsigned IMPACT_SPRITE[IMPACT_FRAMES] = {0x3b06, 0x3b0a, 0x3b0e, 0x3b12};

static void become_impact(Actor *shot) {
    shot->flags = GOOSE_RENDER | GOOSE_UPDATE;
    shot->handler = HANDLER_IMPACT;
    shot->layer = 3;
    shot->frame = 0;
    shot->state[0] = 0;
    shot->height = IMPACT_H;
    shot->anchor_y = IMPACT_ANCHOR;
    shot->animated = 0;
}

static void become_side_impact(Actor *shot) {
    int right = (shot->flags & GOOSE_SIDE) != 0;
    uint8_t bit = right ? 0x80 : 0x01;
    if (side_impact_mask & bit) {
        shot->flags = 0;
        return;
    }
    side_impact_mask = (uint8_t)(side_impact_mask | bit);
    shot->flags = (uint8_t)(GOOSE_RENDER | GOOSE_UPDATE | (right ? GOOSE_SIDE : 0));
    shot->handler = HANDLER_SIDE_IMPACT;
    shot->layer = 5;
    shot->frame = 0;
    shot->state[3] = 0;
    shot->animated = 1;
    shot->height = 15;
    shot->anchor_y = 7;
    snprintf(shot->sprite, sizeof shot->sprite, "fs_133D");
    sound_trigger(SOUND_HULL_HIT);
}

static void update_side_impact(Actor *actor) {
    actor->state[3]++;
    uint8_t frame = (uint8_t)(actor->state[3] & 7u);
    if (frame >= IMPACT_FRAMES) {
        if (actor->flags & GOOSE_SIDE) side_impact_mask = (uint8_t)(side_impact_mask & 0x7f);
        else side_impact_mask = (uint8_t)(side_impact_mask & 0xfe);
        actor->flags = 0;
        return;
    }
    actor->frame = frame;
}

static void update_impact(Game *game, Actor *actor) {
    uint8_t shown = actor->frame;
    actor->frame++;
    if (shown >= IMPACT_FRAMES) {
        actor->flags = 0;
        return;
    }
    actor->state[0] = shown;
    if (cull_after(actor, game->flight.camera_y)) actor->flags = 0;
}

static int strike_map(Game *game, Actor *shot, unsigned row);

static void update_shot(Game *game, Actor *shot) {
    if (cull_before(shot, game->flight.camera_y)) {
        shot->flags = 0;
        return;
    }
    shot->y = (shot->y - SHOT_STEP) & WORLD_MASK;
    if (shot->frame == 1 && (uint8_t)shot->z > SHOT_Z_HIGH) shot->z = (int8_t)((uint8_t)shot->z - SHOT_Z_STEP);
    if (shot->frame == 2 && (uint8_t)shot->z < SHOT_Z_LOW) shot->z = (int8_t)((uint8_t)shot->z + SHOT_Z_STEP);
    TerrainSample ground = terrain_sample(game->flight.tiles, (unsigned)shot->x, shot->y);
    if (ground.height >= shot->z) {
        become_impact(shot);
        return;
    }
    if (strike_map(game, shot, ground.row)) return;
}

static void update_enemy_shot(Game *game, Actor *shot) {
    if (cull_after(shot, game->flight.camera_y)) {
        shot->flags = 0;
        return;
    }
    if (shot->param == 2) shot->y = (shot->y + ENEMY_STEP) & WORLD_MASK;
    else if (shot->param == 0) shot->x -= ENEMY_STEP;
    else if (shot->param == 1) {
        shot->x -= ENEMY_DIAG_X;
        shot->y = (shot->y + ENEMY_DIAG_Y) & WORLD_MASK;
    } else if (shot->param == 3) {
        shot->x += ENEMY_DIAG_X;
        shot->y = (shot->y + ENEMY_DIAG_Y) & WORLD_MASK;
    } else shot->x += ENEMY_STEP;
    shot->y &= WORLD_MASK;
    if (shot->x < ENEMY_OFF_LEFT || shot->x > ENEMY_OFF_RIGHT) {
        shot->flags = 0;
        return;
    }
    TerrainSample ground = terrain_sample(game->flight.tiles, (unsigned)shot->x, shot->y);
    if (ground.height >= shot->z) {
        become_impact(shot);
        return;
    }
    int dx = shot->x - game->flight.x;
    int dy = (int)shot->y - (int)player_y(game);
    int dz = signed_byte((unsigned)shot->z) - game->flight.z;
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    if (dz < 0) dz = -dz;
    if (dx < PLAYER_HIT_X && dy < ENEMY_HIT_Y && dz < HIT_Z) {
        hurt(game, TOUCH_HURT);
        sound_trigger(SOUND_SHIELD_TAP);
        become_impact(shot);
    }
}

static void spawn_explosion(int x, unsigned y, int z);
static void fire_mobile(Game *game, Actor *actor);
static int strike_ground(Game *game, Actor *shot, unsigned row);

static void update_mobile(Game *game, Actor *actor) {
    actor->y = (actor->y + 1u) & WORLD_MASK;
    TerrainSample ground = terrain_sample(game->flight.tiles, (unsigned)actor->x, actor->y);
    actor->z = (int8_t)ground.height;
    actor->state[1] = (uint8_t)ground.frame;
    if (ground.frame < 10) actor->frame = ground.frame;
    if (!dying && near_player(game, actor, MOBILE_HIT_X, MOBILE_HIT_Y)) {
        hurt(game, MOBILE_HURT);
        actor->state[2] = HP_MARK;
    }
    for (int i = 0; i < ACTORS; i++) {
        Actor *shot = &actors[i];
        if (shot == actor || (shot->flags & GOOSE_PROJECTILE) == 0) continue;
        int dx = shot->x - actor->x;
        int dy = (int)shot->y - (int)actor->y;
        if (dx < 0) dx = -dx;
        if (dy < 0) dy = -dy;
        if (dx >= HIT_X || dy >= HIT_Y) continue;
        if (shot->handler == HANDLER_PLAYER_SHOT) {
            actor->state[2] = HP_MARK;
            shot->flags = 0;
            continue;
        }
        int dz = signed_byte((unsigned)shot->z) - signed_byte((unsigned)actor->z);
        if (dz < 0) dz = -dz;
        if (dz >= HIT_Z) continue;
        actor->state[2]--;
        become_impact(shot);
        sound_trigger(SOUND_HULL_HIT);
        score_add(game, 0, 0, 1, 0);
    }
    if (actor->state[2] & HP_DEPLETED) {
        sound_trigger(SOUND_KILL);
        spawn_explosion(actor->x, actor->y, actor->z);
        actor->flags = 0;
        score_add(game, 0, 1, 5, 0);
        return;
    }
    if (!dying) {
        int dx = game->flight.x - actor->x;
        if (dx < 0) dx = -dx;
        if (player_y(game) > actor->y && dx < MOBILE_AIM_X) {
            uint8_t next = (uint8_t)(actor->state[3] - 1u);
            if (next & HP_DEPLETED) {
                actor->state[3] = MOBILE_RELOAD;
                mobile_gate = 1;
            } else {
                actor->state[3] = next;
                mobile_gate = 0;
            }
            fire_mobile(game, actor);
        }
    }
    if (cull_after(actor, game->flight.camera_y)) actor->flags = 0;
}

static const uint8_t MUZZLE_A_LEFT[10] = {0, 4, 4, 1, 5, 4, 6, 5, 2, 1};
static const uint8_t MUZZLE_A_RIGHT[10] = {0, 4, 4, 1, 4, 5, 5, 6, 1, 2};
static const uint8_t MUZZLE_B[10] = {0, 4, 8, 1, 6, 6, 8, 8, 1, 1};
static const uint8_t MUZZLE_C_LEFT[10] = {0, 4, 4, 8, 7, 4, 4, 1, 8, 8};
static const uint8_t MUZZLE_C_CENTER[10] = {0, 4, 4, 8, 3, 4, 4, 4, 8, 8};
static const uint8_t MUZZLE_C_RIGHT[10] = {0, 4, 5, 1, 4, 6, 1, 4, 1, 1};
static const uint8_t SHOT_FRAME_MAP[10] = {0, 0, 1, 2, 0, 0, 1, 1, 2, 2};
static const uint8_t SHOT_HEIGHTS[3] = {4, 3, 5};

static void catch_countdown(Actor *actor) {
    if (actor->frame == 0) actor->flags = 0;
    else actor->frame--;
}

static void emit_mobile_shot(Game *game, Actor *from, int dx, int dy, int dz) {
    variation = (uint8_t)((variation + 1u) & BYTE_MASK);
    int x = from->x + dx;
    unsigned y = (from->y + (unsigned)dy) & WORLD_MASK;
    int z = signed_byte((unsigned)from->z + (unsigned)dz);
    int slot = (int)(from - actors);
    Actor *flash = spawn();
    if (flash) {
        *flash = (Actor){0};
        flash->object_index = -1;
        flash->flags = GOOSE_RENDER | GOOSE_UPDATE;
        flash->handler = HANDLER_COUNTDOWN;
        flash->layer = 5;
        flash->animated = 1;
        flash->frame = VARIATION[variation & VARIATION_MASK];
        flash->x = x;
        flash->y = y;
        flash->z = (int8_t)z;
        flash->height = 7;
        snprintf(flash->sprite, sizeof flash->sprite, "fs_1039");
        if ((int)(flash - actors) < slot) catch_countdown(flash);
    }
    if (!mobile_gate) return;
    Actor *shot = spawn();
    if (!shot) return;
    unsigned frame = from->frame < 10 ? SHOT_FRAME_MAP[from->frame] : 0;
    *shot = (Actor){0};
    shot->object_index = -1;
    shot->flags = GOOSE_RENDER | GOOSE_UPDATE;
    shot->handler = HANDLER_ENEMY_SHOT;
    shot->param = 2;
    shot->frame = (uint8_t)frame;
    shot->height = SHOT_HEIGHTS[frame];
    shot->x = x;
    shot->y = y;
    shot->z = (int8_t)z;
    snprintf(shot->sprite, sizeof shot->sprite, "enemy_shot");
    if ((int)(shot - actors) < slot) update_enemy_shot(game, shot);
}

static void fire_mobile(Game *game, Actor *actor) {
    unsigned frame = actor->frame < 10 ? actor->frame : 0;
    if (actor->param == 0) {
        emit_mobile_shot(game, actor, -3, 8, MUZZLE_A_LEFT[frame]);
        emit_mobile_shot(game, actor, 3, 8, MUZZLE_A_RIGHT[frame]);
    } else if (actor->param == 1) {
        emit_mobile_shot(game, actor, 0, 8, MUZZLE_B[frame]);
    } else {
        emit_mobile_shot(game, actor, -7, 8, MUZZLE_C_LEFT[frame]);
        emit_mobile_shot(game, actor, 0, 11, MUZZLE_C_CENTER[frame]);
        emit_mobile_shot(game, actor, 7, 8, MUZZLE_C_RIGHT[frame]);
    }
}

static const int8_t ARC_DZ[16] = {
    0, 6, 5, 4, 3, 2, 1, 0, 0, -1, -2, -3, -4, -5, -7, -9
};
static const char *GROUND_SET[5] = {"fs_1109", "fs_115A", "fs_11AB", "fs_11FC", "fs_124D"};

static void spawn_lobbed(Actor *from, int direction, int handler) {
    if (direction < 0) direction = 0;
    if (direction > AIM_LAST) direction = AIM_LAST;
    static const int8_t ground_dx[5] = {-12, -4, 0, 4, 12};
    static const int8_t ground_dy[5] = {6, 7, 7, 7, 6};
    static const int8_t arc_dx[5] = {-9, -5, 0, 6, 10};
    static const int8_t arc_dy[5] = {4, 4, 1, 4, 4};
    int arc = handler == HANDLER_ARC;
    Actor *shot = spawn();
    if (!shot) return;
    *shot = (Actor){0};
    shot->object_index = -1;
    shot->flags = GOOSE_RENDER | GOOSE_UPDATE;
    shot->handler = (uint8_t)handler;
    shot->param = (uint8_t)direction;
    shot->animated = arc ? 0 : 1;
    shot->x = from->x + (arc ? arc_dx[direction] : ground_dx[direction]);
    shot->y = (from->y + (unsigned)(arc ? arc_dy[direction] : ground_dy[direction])) & WORLD_MASK;
    shot->z = (int8_t)signed_byte((unsigned)from->z + LOB_Z);
    snprintf(shot->sprite, sizeof shot->sprite, "%s", arc ? "arc_shot" : GROUND_SET[direction]);
}

static void update_emitter(Game *game, Actor *actor, int period, int shot) {
    if (!aim(game, actor)) {
        if (cull_after(actor, game->flight.camera_y)) actor->flags = 0;
        return;
    }
    int8_t timer = (int8_t)actor->state[3];
    timer = (int8_t)(timer - 1);
    if (timer < 0) {
        actor->state[3] = (uint8_t)period;
        if (shot == HANDLER_ENEMY_SHOT) spawn_enemy_shot(actor, actor->frame);
        else spawn_lobbed(actor, actor->frame, shot);
    } else {
        actor->state[3] = (uint8_t)timer;
    }
    if (cull_after(actor, game->flight.camera_y)) actor->flags = 0;
}

/* CS:1A9E. The center adds a charge. The ring around it wipes the shield. */
static void update_refill(Game *game, Actor *actor) {
    if (cull_after(actor, game->flight.camera_y)) {
        actor->flags = 0;
        return;
    }
    int dy = (int)(int16_t)((uint16_t)player_y(game) - (uint16_t)actor->y);
    if (dy < 0) {
        if (dy < -AIM_BEHIND) actor->layer = 1;
        return;
    }
    if (dy > REFILL_FAR) return;
    int ax = game->flight.x - actor->x - GATE_INSET;
    if (ax < 0) {
        ax = -ax;
        if (ax < NUDGE_REACH) game->flight.x++;
    } else if (ax != 0 && ax < NUDGE_REACH) {
        game->flight.x--;
    }
    if (dy > REFILL_CLOSE) return;
    if (ax <= REFILL_CORE) {
        side_charges++;
        if (side_charges > SIDE_CHARGES_FULL) side_charges = SIDE_CHARGES_FULL;
        else sound_trigger(SOUND_CHARGE);
    } else if (ax < REFILL_KILL) {
        game->flight.shield = 0;
        sound_trigger(SOUND_IMPACT);
    }
}

static int wrapped_delta(unsigned a, unsigned b) {
    return (int)(int16_t)((uint16_t)a - (uint16_t)b);
}

static void update_ground_enemy_shot(Game *game, Actor *shot) {
    if (cull_after(shot, game->flight.camera_y)) {
        shot->flags = 0;
        return;
    }
    if (shot->param == 2) shot->y = (shot->y + GROUND_STEP) & WORLD_MASK;
    else if (shot->param == 0) shot->x -= GROUND_STEP;
    else if (shot->param == 1) {
        shot->x -= GROUND_DIAG_X;
        shot->y = (shot->y + GROUND_DIAG_Y) & WORLD_MASK;
    } else if (shot->param == 3) {
        shot->x += GROUND_DIAG_X;
        shot->y = (shot->y + GROUND_DIAG_Y) & WORLD_MASK;
    } else shot->x += GROUND_STEP;
    shot->y &= WORLD_MASK;
    if (shot->x < GROUND_OFF_LEFT || shot->x > GROUND_OFF_RIGHT) {
        shot->flags = 0;
        return;
    }
    TerrainSample ground = terrain_sample(game->flight.tiles, (unsigned)shot->x, shot->y);
    shot->z = (int8_t)(ground.height + LOB_Z);
    shot->frame = ground.frame < SHIP_FIRE_FRAME + 1 ? ground.frame : 0;
    if (!game->flight.alive) return;
    int dx = shot->x - game->flight.x;
    if (dx < 0) dx = -dx;
    int dy = wrapped_delta(shot->y, player_y(game)) - LOB_Z;
    if (dy < 0) dy = -dy;
    if (dx >= GROUND_HIT || dy >= GROUND_HIT) return;
    sound_trigger(SOUND_HEAVY_HIT);
    hurt(game, HEAVY_HURT);
    spawn_explosion(shot->x, shot->y, shot->z);
    shot->flags = 0;
}

static void update_arcing(Game *game, Actor *shot) {
    if (cull_after(shot, game->flight.camera_y)) {
        shot->flags = 0;
        return;
    }
    if (shot->param == 2) shot->y = (shot->y + ARC_STEP_Y) & WORLD_MASK;
    else if (shot->param == 0) shot->x -= ARC_STEP_X;
    else if (shot->param == 1) {
        shot->x -= ARC_STEP_X;
        shot->y = (shot->y + ARC_STEP_Y) & WORLD_MASK;
    } else if (shot->param == 3) {
        shot->x += ARC_STEP_X;
        shot->y = (shot->y + ARC_STEP_Y) & WORLD_MASK;
    } else shot->x += ARC_STEP_X;
    shot->y &= WORLD_MASK;
    if (shot->x < ARC_OFF_LEFT || shot->x > ARC_OFF_RIGHT) {
        shot->flags = 0;
        return;
    }
    shot->state[3]++;
    if (shot->state[3] >= ARC_LIFE) {
        shot->handler = HANDLER_ARC_BURST;
        shot->animated = 1;
        shot->frame = 0;
        shot->state[3] = 0;
        snprintf(shot->sprite, sizeof shot->sprite, "fs_12F5");
        return;
    }
    shot->z = (int8_t)((uint8_t)shot->z + (uint8_t)ARC_DZ[shot->state[3]]);
    if (!game->flight.alive) return;
    int dx = shot->x - game->flight.x;
    int dy = wrapped_delta(shot->y, player_y(game));
    int dz = signed_byte((unsigned)shot->z) - game->flight.z;
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    if (dz < 0) dz = -dz;
    if (dx >= ARC_HIT_X || dy >= ARC_HIT_Y || dz >= HIT_Z) return;
    hurt(game, ARC_HURT);
    sound_trigger(SOUND_HEAVY_HIT);
}

static void nudge_x(Game *game, int dx) {
    if (dx < 0) {
        if (-dx <= NUDGE_REACH) game->flight.x++;
    } else if (dx > 0 && dx <= NUDGE_REACH) {
        game->flight.x--;
    }
}

static void update_gate(Game *game, Actor *actor, int entrance_b) {
    int dx = game->flight.x - actor->x - GATE_INSET;
    if (dx < 0) dx = -dx;
    int dy = (int)player_y(game) - (int)actor->y;
    if (dy < 0) dy = -dy;
    int open = actor->frame >> 1;
    if (dx + dy <= GATE_REACH) {
        if (open != ENTRANCE_FULL) actor->frame++;
    } else if (open != 0) {
        actor->frame--;
    }
    if (open >= 1 && game->flight.frame != 0) {
        int pull = game->flight.x - actor->x - GATE_INSET;
        if (pull < 0) pull = -pull;
        nudge_x(game, game->flight.x - actor->x - GATE_INSET);
        if (pull <= PULL_LIMIT && dy <= PULL_LIMIT) {
            resume_x = game->flight.x;
            pending_object = actor->object_index;
            pending_part = level_part;
            unsigned index = (unsigned)(level_index * PARTS + level_part);
            if (index >= LEVELS * PARTS) index = 0;
            uint16_t dest = entrance_b ? DEST_B[index] : DEST_A[index];
            pending_resume = dest & WORLD_MASK;
            pending_reward = (uint8_t)((dest >> 8) & REWARD_COLOR_MASK);
            if (entrance_b) {
                reload_pending = 1;
                level_part++;
                if (level_part >= PARTS) level_part = 0;
            }
            tunnel_pending = 1;
        }
    }
    if (cull_after(actor, game->flight.camera_y)) actor->flags = 0;
}

static void update_exit(Game *game, Actor *actor) {
    /* CS:18C7: the exit stays shut until every collectible flag is set. */
    int ready = 1;
    for (int i = 0; i < COLLECTIBLES; i++) ready &= collected[i] != 0;
    if (!ready) {
        if (cull_after(actor, game->flight.camera_y)) actor->flags = 0;
        return;
    }
    int dx = game->flight.x - actor->x - GATE_INSET;
    if (dx < 0) dx = -dx;
    int dy = (int)player_y(game) - PULL_LIMIT - (int)actor->y;
    if (dy < 0) dy = -dy;
    int open = actor->frame >> 1;
    if (dx + dy <= GATE_REACH) {
        if (open != GATE_OPEN) actor->frame++;
    } else if (open != 0) {
        actor->frame--;
    }
    if (open >= 2) {
        int pull = game->flight.x - actor->x - GATE_INSET;
        if (pull < 0) pull = -pull;
        nudge_x(game, game->flight.x - actor->x - GATE_INSET);
        if (open == GATE_OPEN && pull <= PULL_LIMIT && dy <= PULL_LIMIT) {
            if (game->flight.frame == 0) {
                pending_object = actor->object_index;
                pending_part = level_part;
                pending_reward = REWARD_ADVANCE;
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
    unsigned frame = hist_frame[HISTORY_LAG];
    int zoff = frame <= SHIP_FIRE_FRAME ? SIDE_Z[frame] : 0;
    int z = (hist_z[HISTORY_LAG] + SIDE_TRAIL) & BYTE_MASK;
    actor->x = hist_x[HISTORY_LAG] + (side ? SIDE_OFFSET : -SIDE_OFFSET);
    actor->y = (hist_y[HISTORY_LAG] + SIDE_TRAIL) & WORLD_MASK;
    actor->frame = frame <= SHIP_FIRE_FRAME ? (uint8_t)frame : SHIP_FIRE_FRAME;
    actor->z = (int8_t)signed_byte((unsigned)(side ? z - zoff : z + zoff));
}

static int arm_surface_side(int side) {
    Actor *actor = spawn();
    if (!actor) return 0;
    *actor = (Actor){0};
    actor->object_index = -1;
    actor->flags = GOOSE_RENDER | GOOSE_UPDATE | GOOSE_PROJECTILE;
    if (side) actor->flags |= GOOSE_SIDE;
    actor->handler = HANDLER_SIDE_FOLLOW;
    actor->animated = 1;
    actor->state[2] = GROUND_LIFE;
    snprintf(actor->sprite, sizeof actor->sprite, "fs_0DDA");
    position_side(actor);
    return 1;
}

static int launch_side(int side) {
    if (!arm_surface_side(side)) return 0;
    side_charges--;
    sound_trigger(SOUND_SIDE_LAUNCH);
    return 1;
}

/* CS:11F0. Press from 0 launches. Press from 2 detaches. Release walks 1→2 and 3→0. */
static void device_button(uint8_t *state, int held, int side) {
    if (held) {
        if (*state == 0) {
            if (side_charges > 0 && launch_side(side)) *state = 1;
        } else if (*state == 2) {
            *state = 3;
            sound_trigger(SOUND_SIDE_RELEASE);
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
    if (state == 3) actor->handler = HANDLER_SIDE_GROUND;
}

static void update_ground_shot(Game *game, Actor *actor) {
    if (cull_before(actor, game->flight.camera_y)) {
        actor->flags = 0;
        return;
    }
    actor->y = (actor->y - SIDE_TRAIL) & WORLD_MASK;
    TerrainSample ground = terrain_sample(game->flight.tiles, (unsigned)actor->x, actor->y);
    actor->z = (int8_t)(ground.height + SHOT_LIFT);
    actor->frame = ground.frame;
    unsigned row = ground.row;
    if (actor->frame == 0) {
        actor->y = (actor->y + SHOT_LIFT) & WORLD_MASK;
        actor->state[2]--;
        if (actor->state[2] & HP_DEPLETED) {
            spawn_explosion(actor->x, actor->y, actor->z);
            actor->flags = 0;
            return;
        }
    }
    strike_ground(game, actor, row);
}

static const uint8_t MAP_DAMAGE[21] = {
    10, 10, 10, 15, 20, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const int MAP_HALF[6] = {0x1c, 0x16, 0x16, 0x1c, 0x18, 0x1c};
static const int MAP_RIGHT[6] = {0xec, 0xec, 0xe0, 0xec, 0xec, 0xec};

/* CS:06C7. A row word is kind<<10 ORed with the column bit. Column 0 is bit 9. */
static unsigned map_row_mask(unsigned row) {
    if (row >= TILE_ROWS) return 0;
    int part = level_part;
    if (part < 0 || part >= PARTS) part = 0;
    return (unsigned)spawn_rows[row] & available_masks[part][row];
}

/* CS:0A51. Inclusive window around the column cursor. Returns damage, or 0. */
static int query_player_row(const Game *game, unsigned row, int *column) {
    unsigned mask = map_row_mask(row);
    if (mask == 0) return 0;
    unsigned kind = mask >> 10;
    if (kind >= DAMAGE_KINDS || MAP_DAMAGE[kind] == 0) return 0;
    int x = MAP_RIGHT[kind];
    int half = MAP_HALF[kind];
    int at = 10;
    for (int i = 0; i < 10; i++) {
        if (mask & 1u) {
            if (x - half <= game->flight.x && game->flight.x <= x + half) {
                *column = at;
                return MAP_DAMAGE[kind];
            }
        }
        mask >>= 1;
        x -= COLUMN_SPAN;
        at--;
    }
    return 0;
}

static void spawn_explosion(int x, unsigned y, int z) {
    Actor *boom = spawn();
    if (!boom) return;
    *boom = (Actor){0};
    boom->object_index = -1;
    boom->flags = GOOSE_RENDER | GOOSE_UPDATE;
    boom->handler = HANDLER_EXPLOSION;
    boom->animated = 1;
    boom->x = x;
    boom->y = y;
    boom->z = (int8_t)z;
    snprintf(boom->sprite, sizeof boom->sprite, "fs_12B5");
}

static void explode_map(const Actor *actor) {
    int x = actor->x;
    unsigned y = actor->y;
    if (actor->flags & GOOSE_LARGE_ANCHOR) {
        x -= LARGE_SHIFT_X;
        y = (y - LARGE_SHIFT_Y) & WORLD_MASK;
    }
    spawn_explosion(x, y, actor->z);
}

static void apply_map_hit(Game *game, int damage, int column, unsigned row) {
    hurt(game, damage);
    sound_trigger(SOUND_IMPACT);
    int expect_x = COLUMN_SPAN * column - COLUMN_BIAS;
    for (int i = 0; i < ACTORS; i++) {
        Actor *actor = &actors[i];
        if (actor->flags == 0 || actor->object_index < 0) continue;
        if ((actor->y >> 4) != row || actor->x != expect_x) continue;
        score_add(game, 0, 1, 5, 0);
        explode_map(actor);
        destroy_map_actor(actor);
        return;
    }
}

static int query_shot_row(int shot_x, unsigned row, int *column) {
    unsigned mask = map_row_mask(row);
    if (mask == 0) return 0;
    unsigned kind = mask >> 10;
    if (kind >= DAMAGE_KINDS || MAP_DAMAGE[kind] == 0) return 0;
    int x = MAP_RIGHT[kind];
    int half = MAP_HALF[kind] - SHOT_HALF_INSET;
    int at = 10;
    for (int i = 0; i < 10; i++) {
        if (mask & 1u) {
            if (x - half <= shot_x && shot_x <= x + half) {
                *column = at;
                return 1;
            }
        }
        mask >>= 1;
        x -= COLUMN_SPAN;
        at--;
    }
    return 0;
}

/* CS:13F5. The shot query. A missing map actor leaves the shot in flight. */
static int strike_ground(Game *game, Actor *shot, unsigned row) {
    int column = 0;
    if (!query_shot_row(shot->x, row, &column)) return 0;
    int expect_x = COLUMN_SPAN * column - COLUMN_BIAS;
    Actor *target = 0;
    for (int i = 0; i < ACTORS; i++) {
        Actor *actor = &actors[i];
        if (actor->waiting || (actor->flags & GOOSE_MAP_OBJECT) == 0) continue;
        if ((actor->y >> 4) != row || actor->x != expect_x) continue;
        target = actor;
        break;
    }
    if (!target) return 0;
    score_add(game, 0, 1, 5, 0);
    explode_map(target);
    destroy_map_actor(target);
    sound_trigger(SOUND_IMPACT);
    shot->flags = 0;
    return 1;
}

static int strike_map(Game *game, Actor *shot, unsigned row) {
    int column = 0;
    if (!query_shot_row(shot->x, row, &column)) return 0;
    int expect_x = COLUMN_SPAN * column - COLUMN_BIAS;
    Actor *target = 0;
    for (int i = 0; i < ACTORS; i++) {
        Actor *actor = &actors[i];
        if (actor->flags == 0 || actor->object_index < 0) continue;
        if ((actor->y >> 4) != row || actor->x != expect_x) continue;
        if (actor->flags & GOOSE_SENTINEL) return 0;
        target = actor;
        break;
    }
    if (!target) return 0;
    int dz = signed_byte((unsigned)target->z) - signed_byte((unsigned)shot->z);
    if (dz < 0) dz = -dz;
    if (dz >= HIT_Z) return 0;
    target->state[2]--;
    if (target->state[2] & HP_DEPLETED) {
        score_add(game, 0, 1, 5, 0);
        explode_map(target);
        destroy_map_actor(target);
        sound_trigger(SOUND_IMPACT);
    } else {
        score_add(game, 0, 0, 1, 0);
    }
    become_side_impact(shot);
    return 1;
}

/* CS:1192 checks this row, then the next row, and a hit is applied before weapons. */
static void collide_player_map(Game *game) {
    if (!game->flight.alive || game->flight.tiles == 0) return;
    unsigned row = (player_y(game) >> 4) & (TILE_ROWS - 1);
    int column = 0;
    int damage = query_player_row(game, row, &column);
    unsigned hit_row = row;
    if (damage == 0) {
        hit_row = (row + 1u) & (TILE_ROWS - 1);
        damage = query_player_row(game, hit_row, &column);
    }
    if (damage == 0) return;
    apply_map_hit(game, damage, column, hit_row);
    if (game->flight.shield <= 0) game->flight.alive = 0;
}

static void update_actor(Game *game, Actor *actor) {
    if (actor->flags == 0) return;
    if (actor->handler == HANDLER_PLAYER_SHOT) update_shot(game, actor);
    else if (actor->handler == HANDLER_IMPACT) update_impact(game, actor);
    else if (actor->handler == HANDLER_SIDE_IMPACT) update_side_impact(actor);
    else if (actor->handler == HANDLER_EXPLOSION) {
        actor->state[3]++;
        unsigned frame = (actor->state[3] >> 2) & 7u;
        if (frame >= EXPLOSION_FRAMES) actor->flags = 0;
        else {
            actor->frame = (uint8_t)frame;
            if (cull_after(actor, game->flight.camera_y)) actor->flags = 0;
        }
    } else if (actor->handler == HANDLER_SIDE_FOLLOW) update_side_follow(game, actor);
    else if (actor->handler == HANDLER_SIDE_GROUND) update_ground_shot(game, actor);
    else if (actor->handler == HANDLER_ENEMY_SHOT) update_enemy_shot(game, actor);
    else if (actor->handler == HANDLER_COUNTDOWN) {
        if (actor->frame == 0) actor->flags = 0;
        else actor->frame--;
    } else if (actor->handler == HANDLER_LOOP) {
        actor->state[3]++;
        actor->frame = (uint8_t)((actor->state[3] >> 1) & 3u);
        if (cull_after(actor, game->flight.camera_y)) actor->flags = 0;
    } else if (actor->handler == HANDLER_PINGPONG) {
        actor->state[3]++;
        unsigned step = (actor->state[3] >> 1) & 7u;
        if (step >= sizeof PINGPONG / sizeof PINGPONG[0]) {
            step = 0;
            actor->state[3] = 0;
        }
        actor->frame = PINGPONG[step];
        if (cull_after(actor, game->flight.camera_y)) actor->flags = 0;
    } else if (actor->handler == HANDLER_EMIT_DIRECT) update_emitter(game, actor, DIRECT_RELOAD, HANDLER_ENEMY_SHOT);
    else if (actor->handler == HANDLER_EMIT_ARC) update_emitter(game, actor, ARC_RELOAD, HANDLER_ARC);
    else if (actor->handler == HANDLER_EMIT_GROUND) update_emitter(game, actor, GROUND_RELOAD, HANDLER_GROUND_ENEMY);
    else if (actor->handler == HANDLER_REFILL) update_refill(game, actor);
    else if (actor->handler == HANDLER_GROUND_ENEMY) update_ground_enemy_shot(game, actor);
    else if (actor->handler == HANDLER_ARC) update_arcing(game, actor);
    else if (actor->handler == HANDLER_ARC_BURST) {
        actor->state[3]++;
        unsigned frame = (actor->state[3] >> 2) & 7u;
        if (frame >= BURST_FRAMES) actor->flags = 0;
        else {
            actor->frame = (uint8_t)frame;
            if (cull_after(actor, game->flight.camera_y)) actor->flags = 0;
        }
    }
    else if (actor->handler == HANDLER_MOBILE) update_mobile(game, actor);
    else if (actor->handler == HANDLER_GATE_A) update_gate(game, actor, 0);
    else if (actor->handler == HANDLER_GATE_B) update_gate(game, actor, 1);
    else if (actor->handler == HANDLER_LEVEL_EXIT) update_exit(game, actor);
    else if (actor->handler == HANDLER_COLLECTIBLE) {
        if (cull_after(actor, game->flight.camera_y)) actor->flags = 0;
        else if (game->flight.frame != 0 && near_player(game, actor, COLLECT_X, COLLECT_Y)) {
            if (actor->param < COLLECTIBLES) collected[actor->param] = 1;
            consume_actor(actor);
            actor->flags = 0;
            score_add(game, 0, 1, 5, 0);
            sound_trigger(SOUND_COLLECT);
        }
    } else if (actor->handler == HANDLER_ACCEL) {
        actor->state[3]++;
        unsigned step = actor->state[3];
        if (step > ACCEL_STEP_MAX) step = ACCEL_STEP_MAX;
        actor->y = (actor->y - step) & WORLD_MASK;
        if (cull_before(actor, game->flight.camera_y)) actor->flags = 0;
    } else if (cull_after(actor, game->flight.camera_y)) {
        actor->flags = 0;
    }
}

/* DS:1A32. Four streams, one section every 64 distance units. 0 spawns nothing. */
static const uint8_t TUNNEL_PATTERN[4][14] = {
    {5, 6, 7, 8, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0},
    {5, 4, 3, 2, 1, 8, 7, 6, 5, 0, 0, 0, 0, 0},
    {1, 2, 3, 4, 5, 6, 7, 8, 1, 0, 0, 0, 0, 0},
    {1, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0}
};
static const uint8_t TUNNEL_PICKUP_RADIUS[48] = {
    0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
    7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 20, 22, 24, 26,
    28, 30, 32, 34, 36, 38, 40, 44, 46, 49, 52, 56, 60, 64, 69, 75
};
static const uint8_t TUNNEL_SIDE_FRAME[128] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 2, 2,
    2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4,
    4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6,
    6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8,
    8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10,
    10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12,
    12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14,
    14, 14, 14, 14, 15, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const uint8_t TUNNEL_PICKUP_LAYER[24] = {
    7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 2, 2, 2, 1, 1, 1, 1
};
static const uint8_t TUNNEL_EXIT_FRAME[32] = {
    0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08,
    0x07, 0x07, 0x06, 0x06, 0x05, 0x05, 0x04, 0x04, 0x03, 0x03, 0x02, 0x02, 0x01, 0x01, 0x00, 0x00
};
static const uint8_t TUNNEL_EXIT_RADIUS[32] = {
    0x2e, 0x2a, 0x26, 0x22, 0x1d, 0x1a, 0x17, 0x14, 0x12, 0x10, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09,
    0x08, 0x08, 0x07, 0x07, 0x06, 0x06, 0x05, 0x05, 0x04, 0x04, 0x04, 0x03, 0x03, 0x03, 0x03, 0x03
};
static const uint8_t TUNNEL_FRAME_SECTOR[28] = {
    5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 8, 8, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5
};

static void select_tunnel_pattern(void) {
    tunnel_pattern = (uint8_t)(tunnel_pattern_index & 3u);
    tunnel_pattern_index++;
    tunnel_last_section = NO_SECTION;
    tunnel_exit_spawned = 0;
    tunnel_min_radius = 3;
}

static void gain_capped(int *value, int amount, int cap) {
    while (amount > 0 && *value < cap) {
        (*value)++;
        amount--;
    }
}

static void place_tunnel_pickup(Actor *actor, int radius) {
    int x = TUNNEL_CENTER_X;
    int y = TUNNEL_CENTER_Y;
    switch (actor->param) {
    case 1: y = TUNNEL_CENTER_Y - radius; break;
    case 2: y = TUNNEL_CENTER_Y - radius; x = TUNNEL_CENTER_X + radius; break;
    case 3: x = TUNNEL_CENTER_X + radius; break;
    case 4: x = TUNNEL_CENTER_X + radius; y = TUNNEL_CENTER_Y + radius; break;
    case 5: y = TUNNEL_CENTER_Y + radius; break;
    case 6: y = TUNNEL_CENTER_Y + radius; x = TUNNEL_CENTER_X - radius; break;
    case 7: x = TUNNEL_CENTER_X - radius; break;
    case 8: x = TUNNEL_CENTER_X - radius; y = TUNNEL_CENTER_Y - radius; break;
    default: break;
    }
    actor->x = x;
    actor->y = (unsigned)y;
}

static void spawn_tunnel_pickup(const Game *game, uint8_t direction) {
    Actor *actor = spawn();
    if (!actor) return;
    *actor = (Actor){0};
    actor->object_index = -1;
    actor->flags = GOOSE_RENDER | GOOSE_UPDATE;
    actor->handler = HANDLER_TUNNEL_PICKUP;
    actor->param = direction;
    actor->layer = 6;
    actor->animated = 1;
    actor->x = TUNNEL_CENTER_X;
    actor->y = TUNNEL_CENTER_Y;
    actor->state[0] = (uint8_t)game->tunnel_distance;
    actor->state[1] = (uint8_t)((unsigned)game->tunnel_distance >> 8);
    snprintf(actor->sprite, sizeof actor->sprite, "fs_1AC9");
}

static void spawn_tunnel_exit(void) {
    Actor *actor = spawn();
    if (!actor) return;
    *actor = (Actor){0};
    actor->object_index = -1;
    actor->flags = GOOSE_RENDER | GOOSE_UPDATE;
    actor->handler = HANDLER_TUNNEL_EXIT;
    actor->layer = LAYER_TOP;
    actor->animated = 1;
    actor->x = TUNNEL_CENTER_X;
    actor->y = TUNNEL_CENTER_Y;
    snprintf(actor->sprite, sizeof actor->sprite, "fs_1929");
}

/* CS:3059. Green adds ammo, red fuel, blue shield, and the exit color adds 2500. */
static void collect_tunnel_pickup(Game *game, Actor *actor) {
    actor->flags = 0;
    if (game->tunnel_reward == REWARD_AMMO) gain_capped(&game->ammo, PICKUP_AMMO, AMMO_FULL);
    else if (game->tunnel_reward == REWARD_FUEL) gain_capped(&game->flight.fuel, PICKUP_FUEL, FUEL_FULL);
    else if (game->tunnel_reward == REWARD_SHIELD) gain_capped(&game->flight.shield, PICKUP_SHIELD, SHIELD_FULL);
    else score_add(game, 2, 5, 0, 0);
}

static void update_tunnel_pickup(Game *game, Actor *actor) {
    unsigned spawned = (unsigned)actor->state[0] | ((unsigned)actor->state[1] << 8);
    unsigned age = ((unsigned)game->tunnel_distance - spawned) & 0xffffu;
    age >>= 3;
    if (age > PICKUP_AGE_LIMIT) {
        actor->flags = 0;
        return;
    }
    place_tunnel_pickup(actor, TUNNEL_PICKUP_RADIUS[age]);
    actor->frame = (uint8_t)(age >> 1);
    actor->layer = TUNNEL_PICKUP_LAYER[actor->frame];
    if (actor->layer != LAYER_SHIP || game->tunnel_frame >= 28) return;
    if (TUNNEL_FRAME_SECTOR[game->tunnel_frame] != actor->param) return;
    sound_trigger(SOUND_TUNNEL_PICKUP);
    collect_tunnel_pickup(game, actor);
}

static void update_tunnel_exit(const Game *game, Actor *actor) {
    unsigned remaining = game->tunnel_distance >= TUNNEL_LENGTH ? 0u : (unsigned)(TUNNEL_LENGTH - game->tunnel_distance);
    unsigned index = (remaining >> 3) & 31u;
    tunnel_min_radius = TUNNEL_EXIT_RADIUS[index];
    actor->frame = TUNNEL_EXIT_FRAME[index];
}

static void advance_tunnel_objects(Game *game) {
    if (game->tunnel_distance >= TUNNEL_LENGTH) return;
    int remaining = TUNNEL_LENGTH - game->tunnel_distance;
    if (remaining < TUNNEL_EXIT_NEAR) {
        if (!tunnel_exit_spawned) {
            spawn_tunnel_exit();
            tunnel_exit_spawned = 1;
        }
        return;
    }
    uint8_t section = (uint8_t)((unsigned)game->tunnel_distance >> 6);
    if (section == tunnel_last_section || section >= 14) return;
    tunnel_last_section = section;
    uint8_t direction = TUNNEL_PATTERN[tunnel_pattern][section];
    if (direction) spawn_tunnel_pickup(game, direction);
}

static void collect_visible_pickups(Game *game) {
    for (int i = 0; i < ACTORS; i++) {
        if ((actors[i].flags & GOOSE_RENDER) == 0) continue;
        if (actors[i].handler == HANDLER_TUNNEL_PICKUP) collect_tunnel_pickup(game, &actors[i]);
    }
}

static void update_tunnel_side(Game *game, Actor *actor) {
    int right = (actor->flags & GOOSE_SIDE) != 0;
    uint8_t state = right ? right_device : left_device;
    if (state == 0 || state == 3) {
        actor->flags = 0;
        collect_visible_pickups(game);
        return;
    }
    int offset = right ? TUNNEL_SIDE_OFFSET : -TUNNEL_SIDE_OFFSET;
    uint8_t angle = (uint8_t)((uint8_t)offset + angle_hist[ANGLE_HISTORY - 1]) & ANGLE_MASK;
    int x, y;
    tunnel_side_position(angle, &x, &y);
    actor->x = x;
    actor->y = (unsigned)y;
    actor->frame = TUNNEL_SIDE_FRAME[ship_angle & ANGLE_MASK];
}

static void spawn_tunnel_side(Game *game, int side) {
    Actor *actor = spawn();
    if (!actor) return;
    *actor = (Actor){0};
    actor->object_index = -1;
    actor->flags = GOOSE_RENDER | GOOSE_UPDATE;
    if (side) actor->flags |= GOOSE_SIDE;
    actor->handler = HANDLER_TUNNEL_SIDE;
    actor->layer = LAYER_SHIP;
    actor->animated = 1;
    snprintf(actor->sprite, sizeof actor->sprite, "fs_1811");
    update_tunnel_side(game, actor);
}

static void shift_angle_history(void) {
    for (int i = ANGLE_HISTORY - 1; i > 0; i--) angle_hist[i] = angle_hist[i - 1];
    angle_hist[0] = ship_angle;
}

static void tunnel_device_button(uint8_t *state, int held) {
    if (held) {
        if (*state == 2) {
            *state = 3;
            sound_trigger(SOUND_SIDE_RELEASE);
            tunnel_flash = 2;
        }
    } else if (*state == 1) {
        *state = 2;
    } else if (*state == 3) {
        *state = 0;
    }
}

static void fade_tunnel_flash(void) {
    if (tunnel_flash == 1) tunnel_flash = 0;
    else if (tunnel_flash != 0) tunnel_flash = 1;
}

static void carry_surface_sides(void) {
    if (left_device) {
        left_device = 1;
        arm_surface_side(0);
    }
    if (right_device) {
        right_device = 1;
        arm_surface_side(1);
    }
}

static void enter_tunnel(Game *game) {
    sound_trigger(SOUND_TUNNEL);
    if (pending_object >= 0 && pending_part >= 0 && pending_part < PARTS) {
        consumed[pending_part] |= 1u << pending_object;
    }
    game->mode = MODE_TUNNEL;
    game->tunnel_distance = 0;
    game->tunnel_reward = pending_reward;
    resume_y = pending_resume;
    angular = 0;
    ship_angle = 0;
    game->flight.camera_y = TUNNEL_CAMERA;
    tunnel_player_position(ship_angle, &game->tunnel_x, &game->tunnel_y);
    game->tunnel_frame = tunnel_player_frame(ship_angle);
    clear_actors();
    tunnel_flash = 0;
    memset(angle_hist, 0, sizeof angle_hist);
    if (left_device) {
        left_device = 1;
        spawn_tunnel_side(game, 0);
    }
    if (right_device) {
        right_device = 1;
        spawn_tunnel_side(game, 1);
    }
    select_tunnel_pattern();
    tunnel_pending = 0;
}

static void leave_tunnel(Game *game) {
    sound_trigger(SOUND_TUNNEL);
    game->mode = MODE_SURFACE;
    game->tunnel_distance = 0;
    game->flight.velocity = 0;
    game->flight.frame = 1;
    game->flight.zero_frame_age = 0;
    game->flight.alive = 1;
    tunnel_flash = 0;
    if (game->tunnel_reward == REWARD_ADVANCE) {
        stage_counter++;
        int base = BASE_SCROLL;
        if (stage_counter >= STAGE_FASTER) base = SCROLL_STAGE6;
        else if (stage_counter >= STAGE_QUICK) base = SCROLL_STAGE3;
        level_index++;
        if (level_index > LEVELS - 1) level_index = 1;
        memset(collected, 0, sizeof collected);
        memset(consumed, 0, sizeof consumed);
        memset(available_masks, 0xff, sizeof available_masks);
        game->flight.base_scroll_speed = base;
        game->flight.scroll_speed = base;
        game->flight.camera_y = START_CAMERA;
        game->flight.x = START_X;
        game->flight.fuel_distance = 0;
        reload_pending = 0;
        forced_stop = 0;
    } else {
        uint8_t ang = ship_angle & ANGLE_MASK;
        forced_stop = ang > FORCE_STOP_LO && ang < FORCE_STOP_HI;
        game->flight.camera_y = resume_y & WORLD_MASK;
        game->flight.x = resume_x;
        reload_pending = 0;
    }
    place_map(game, game->flight.camera_y);
    reset_history(game);
    carry_surface_sides();
    remember(game);
}

static void tunnel_tick(Game *game, unsigned controls) {
    fade_tunnel_flash();
    surface_update_speed(&game->flight, controls);
    int step = ((game->flight.scroll_speed + 7) >> 3) + 2;
    game->tunnel_distance += step;
    unsigned fuel = (game->flight.fuel_distance + (unsigned)step) & 0xffffu;
    if (fuel > TUNNEL_FUEL_LIMIT) {
        fuel = 0;
        game->flight.fuel_delta = -1;
    }
    game->flight.fuel_distance = fuel;
    surface_apply_fuel(&game->flight);
    if (game->tunnel_distance >= TUNNEL_LENGTH) {
        leave_tunnel(game);
        return;
    }
    advance_tunnel_objects(game);
    int force = 0;
    int speed = game->flight.scroll_speed >> 3;
    if (controls & CONTROL_LEFT) force = -speed - 3;
    else if (controls & CONTROL_RIGHT) force = speed + 3;
    uint8_t ang = ship_angle & ANGLE_MASK;
    int passive;
    if (ang > ANGLE_HALF) {
        uint8_t folded = (uint8_t)((uint8_t)(-ang) + ANGLE_MASK);
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
    shift_angle_history();
    tunnel_device_button(&left_device, (controls & CONTROL_LEFT_DEVICE) != 0);
    tunnel_device_button(&right_device, (controls & CONTROL_RIGHT_DEVICE) != 0);
    for (int i = 0; i < ACTORS; i++) {
        if (actors[i].flags == 0) continue;
        if (actors[i].handler == HANDLER_TUNNEL_EXIT) update_tunnel_exit(game, &actors[i]);
        else if (actors[i].handler == HANDLER_TUNNEL_PICKUP) update_tunnel_pickup(game, &actors[i]);
        else if (actors[i].handler == HANDLER_TUNNEL_SIDE) update_tunnel_side(game, &actors[i]);
    }
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
    dying = 0;
    forced_stop = 0;
    side_impact_mask = 0;
    tunnel_pending = 0;
    level_index = 0;
    level_part = 0;
    stage_counter = 1;
    side_charges = SIDE_CHARGES_FULL;
    left_device = 0;
    right_device = 0;
    tunnel_pattern_index = 0;
    reload_pending = 0;
    pending_object = -1;
    memset(collected, 0, sizeof collected);
    memset(consumed, 0, sizeof consumed);
    memset(available_masks, 0xff, sizeof available_masks);
    game->flight.base_scroll_speed = BASE_SCROLL;
    resume_y = 0;
    place_map(game, game->flight.camera_y);
    reset_history(game);
    remember(game);
}

static void spawn_respawn_effect(const Game *game) {
    Actor *actor = spawn();
    if (!actor) return;
    *actor = (Actor){0};
    actor->object_index = -1;
    actor->flags = GOOSE_RENDER | GOOSE_UPDATE;
    actor->handler = HANDLER_ACCEL;
    actor->layer = 1;
    actor->height = 42;
    actor->anchor_y = 21;
    actor->x = START_X;
    actor->y = player_y(game);
    TerrainSample ground = terrain_sample(game->flight.tiles, (unsigned)actor->x, actor->y);
    actor->z = (int8_t)ground.height;
    snprintf(actor->sprite, sizeof actor->sprite, "respawn_effect");
}

static void respawn(Game *game) {
    death_hold = 0;
    dying = 0;
    forced_stop = 0;
    side_impact_mask = 0;
    game->flight.alive = 1;
    game->flight.fuel = FUEL_FULL;
    game->flight.shield = SHIELD_FULL;
    game->ammo = AMMO_FULL;
    game->flight.x = START_X;
    game->flight.velocity = 0;
    game->flight.camera_y = START_CAMERA;
    game->flight.scroll_speed = game->flight.base_scroll_speed;
    game->flight.fuel_distance = 0;
    game->flight.frame = 1;
    game->flight.zero_frame_age = 0;
    side_charges = SIDE_CHARGES_FULL;
    left_device = 0;
    right_device = 0;
    if (level_part != 0) level_part = 0;
    place_map(game, game->flight.camera_y);
    reset_history(game);
    remember(game);
    spawn_respawn_effect(game);
}

static void load_scores(void) {
    memset(names, '.', sizeof names);
    names[0][0] = '_';
    memset(board, 0, sizeof board);
    for (int i = 0; i < SCORE_SLOTS; i++) score_order[i] = (uint8_t)i;
    FILE *file = fopen("hiscores", "rb");
    if (!file) return;
    uint8_t blob[HISCORE_BYTES];
    if (fread(blob, 1, HISCORE_BYTES, file) == HISCORE_BYTES) {
        memcpy(names, blob, sizeof names);
        memcpy(board, blob + sizeof names, sizeof board);
        memcpy(score_order, blob + HISCORE_BYTES - SCORE_SLOTS, sizeof score_order);
    }
    fclose(file);
}

static void save_scores(void) {
    uint8_t blob[HISCORE_BYTES];
    memset(blob, 0xff, sizeof blob);
    memcpy(blob, names, sizeof names);
    memcpy(blob + sizeof names, board, sizeof board);
    memcpy(blob + HISCORE_BYTES - SCORE_SLOTS, score_order, sizeof score_order);
    FILE *file = fopen("hiscores", "wb");
    if (!file) return;
    fwrite(blob, 1, HISCORE_BYTES, file);
    fclose(file);
}

static void ensure_scores(void) {
    if (scores_ready) return;
    scores_ready = 1;
    load_scores();
}

static int board_greater(int left, int right) {
    for (int place = 0; place < SCORE_PLACES; place++) {
        if (board[left][place] == board[right][place]) continue;
        return board[left][place] > board[right][place];
    }
    return 0;
}

static void sort_scores(void) {
    uint8_t scratch[SCORE_SLOTS];
    memcpy(scratch, score_order, sizeof scratch);
    for (int out = 0; out < SCORE_SLOTS; out++) {
        int chosen = 0;
        while (chosen < SCORE_SLOTS && scratch[chosen] == 0xff) chosen++;
        if (chosen >= SCORE_SLOTS) chosen = 0;
        if (chosen < SCORE_SLOTS - 1) {
            for (int other = chosen + 1; other < SCORE_SLOTS; other++) {
                if (scratch[other] == 0xff) continue;
                if (board_greater(scratch[other], scratch[chosen])) chosen = other;
            }
        }
        score_order[out] = scratch[chosen];
        scratch[chosen] = 0xff;
    }
}

static void finish_name(void) {
    if (name_cursor < NAME_LEN && names[name_slot][name_cursor] == '_')
        names[name_slot][name_cursor] = '.';
    save_scores();
    pending_name = 0;
    name_editing = 0;
}

static void type_name(int ch) {
    if (ch >= 'a' && ch <= 'z') ch -= 'a' - 'A';
    if (name_cursor >= NAME_LEN) return;
    if (!((ch >= 'A' && ch <= 'Z') || ch == ' ')) return;
    names[name_slot][name_cursor] = (uint8_t)ch;
    name_cursor++;
    if (name_cursor >= NAME_LEN) finish_name();
    else names[name_slot][name_cursor] = '_';
}

static void erase_name(void) {
    if (name_cursor < 0 || name_cursor >= NAME_LEN) return;
    names[name_slot][name_cursor] = '.';
    if (name_cursor > 0) name_cursor--;
    names[name_slot][name_cursor] = '_';
}

static int input_held;

void game_hold_input(int hold) {
    input_held = hold;
}

static void poll_name(void) {
    if (input_held || !name_editing) return;
    int ch;
    while (name_editing && (ch = GetCharPressed()) != 0) type_name(ch);
    if (!name_editing) return;
    if (IsKeyPressed(KEY_BACKSPACE)) erase_name();
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) finish_name();
}

static void insert_score(void) {
    ensure_scores();
    memcpy(board[0], score_digits, SCORE_PLACES);
    sort_scores();
    if (score_order[SCORE_SLOTS - 1] == 0) {
        pending_name = 0;
        return;
    }
    pending_name = 1;
    int slot = score_order[SCORE_SLOTS - 1];
    memcpy(board[slot], board[0], SCORE_PLACES);
    memcpy(names[slot], names[0], NAME_LEN);
    int rank = 0;
    while (rank < SCORE_SLOTS && score_order[rank] != 0) rank++;
    if (rank < SCORE_SLOTS) score_order[rank] = (uint8_t)slot;
    score_order[SCORE_SLOTS - 1] = 0;
    name_slot = slot;
    name_cursor = 0;
}

static void attract_begin(void) {
    clear_actors();
    attract_step = 0;
    attract_base = attract_seen ? 0 : ATTRACT_INTRO;
    name_editing = 0;
}

static void spawn_attract(int frame) {
    static const uint8_t height[24] = {
        30, 30, 23, 15, 19, 14, 15, 15, 12, 15, 12, 23, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11
    };
    static const int8_t anchor_y[24] = {
        0, 0, 12, -13, 7, 21, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    if (frame < 0 || frame >= 24) return;
    Actor *actor = spawn();
    if (!actor) return;
    *actor = (Actor){0};
    actor->object_index = -1;
    actor->flags = GOOSE_RENDER | GOOSE_UPDATE;
    actor->handler = HANDLER_DRIFT;
    actor->waiting = 1;
    actor->animated = 1;
    actor->frame = (uint8_t)frame;
    actor->height = height[frame];
    actor->anchor_y = anchor_y[frame];
    actor->x = ATTRACT_X;
    actor->y = ATTRACT_Y;
    snprintf(actor->sprite, sizeof actor->sprite, "fs_0120");
}

static void read_attract_command(Game *game) {
    static const uint8_t script[] = {
        0x00, 0x01, 0x0b, 0x00, 0x8c, 0x8e, 0x90, 0x92, 0x94, 0x96, 0x00, 0xfe, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x02, 0x00, 0x09, 0x00, 0x0a, 0x00, 0x00, 0x06, 0x00, 0x00, 0x07, 0x00, 0x00, 0x03,
        0x04, 0x05, 0x00, 0x08, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff
    };
    unsigned index = (unsigned)attract_base + (attract_step >> 3);
    if (index >= sizeof script) {
        game_play(game);
        return;
    }
    unsigned command = script[index];
    if (command == 0) return;
    if (command == SCRIPT_HOLD) {
        if (pending_name) name_editing = 1;
        return;
    }
    if (command == SCRIPT_END) {
        game_play(game);
        return;
    }
    int frame = (int)(command & (SCRIPT_PAIR - 1));
    spawn_attract(frame);
    if (command & SCRIPT_PAIR) spawn_attract(frame + 1);
}

static void attract_tick(Game *game) {
    if (name_editing) return;
    for (int i = 0; i < ACTORS; i++) {
        Actor *actor = &actors[i];
        if (actor->flags == 0 || actor->handler != HANDLER_DRIFT) continue;
        actor->waiting = 0;
        actor->y = (actor->y - DRIFT_STEP) & WORLD_MASK;
        if (cull_before(actor, TUNNEL_CAMERA)) actor->flags = 0;
    }
    attract_step++;
    if ((attract_step & (ATTRACT_EVERY - 1)) == 0) read_attract_command(game);
}

void game_boot(Game *game) {
    memset(game, 0, sizeof *game);
    game->mode = MODE_TITLE;
    game->lives = LIVES_FULL;
    memset(score_digits, 0, sizeof score_digits);
    hud_frame = 0;
    side_charges = SIDE_CHARGES_FULL;
    sound_reset();
    ensure_scores();
    sound_music_start(pending_name);
    surface_start(&game->flight);
    attract_begin();
}

void game_play(Game *game) {
    int score = game->score;
    int lives = game->lives > 0 ? game->lives : LIVES_FULL;
    begin_level(game);
    game->score = score;
    game->lives = lives;
}

static void finish_death(Game *game) {
    dying = 0;
    if (game->lives == 0) {
        attract_seen = 1;
        insert_score();
        game_boot(game);
        return;
    }
    game->lives--;
    respawn(game);
}

static void begin_surface_death(Game *game) {
    if (dying) return;
    dying = 1;
    death_hold = 0;
    sound_trigger(SOUND_DEATH);
    spawn_explosion(game->flight.x, player_y(game), game->flight.z);
}

static void update_surface_actors(Game *game) {
    tunnel_pending = 0;
    for (int i = 0; i < ACTORS; i++) update_actor(game, &actors[i]);
    if (tunnel_pending) enter_tunnel(game);
}

void game_tick(Game *game, unsigned controls) {
    if (game->mode == MODE_TITLE) {
        if (controls & CONTROL_FIRE) game_play(game);
        else attract_tick(game);
        return;
    }
    hud_tick(game);
    sound_context(side_charges, ship_angle);
    if (game->mode == MODE_TUNNEL) {
        tunnel_tick(game, controls);
        return;
    }
    if (dying) {
        if (death_hold >= DEATH_TICKS) {
            finish_death(game);
            return;
        }
        surface_coast(&game->flight);
        advance_spawns(game->flight.camera_y);
        update_surface_actors(game);
        death_hold++;
        return;
    }
    if (forced_stop) {
        if (surface_brake(&game->flight)) begin_surface_death(game);
        advance_spawns(game->flight.camera_y);
        update_surface_actors(game);
        return;
    }
    int was_alive = game->flight.alive;
    surface_tick(&game->flight, controls);
    push_history(game);
    collide_player_map(game);
    if (was_alive && !game->flight.alive) begin_surface_death(game);
    if (dying) {
        advance_spawns(game->flight.camera_y);
        update_surface_actors(game);
        return;
    }
    update_devices(game, controls);
    advance_spawns(game->flight.camera_y);
    if (game->flight.alive && game->flight.frame != 0 && game->ammo > 0 && (controls & CONTROL_FIRE)) {
        fire(game);
    }
    update_surface_actors(game);
}

static void draw_tunnel_layer(int layer) {
    for (int i = 0; i < ACTORS; i++) {
        if (actors[i].flags == 0 || actors[i].layer != (uint8_t)layer) continue;
        char name[24];
        sprite_name(&actors[i], name, sizeof name);
        pictures_sprite(name, actors[i].x, (int)actors[i].y, 0, TUNNEL_CAMERA);
    }
}

void game_draw(Game *game) {
    pictures_clear();
    if (game->mode == MODE_TITLE) {
        pictures_intro();
        poll_name();
        for (int i = 0; i < ACTORS; i++) {
            if (actors[i].flags == 0 || actors[i].waiting) continue;
            char name[24];
            sprite_name(&actors[i], name, sizeof name);
            pictures_sprite(name, actors[i].x, (int)actors[i].y, actors[i].z, TUNNEL_CAMERA);
            if (actors[i].handler != HANDLER_DRIFT) continue;
            if (actors[i].frame < SCORE_ROW_FIRST || actors[i].frame > SCORE_ROW_LAST) continue;
            int rank = (actors[i].frame - SCORE_ROW_FIRST) / 2;
            if (rank < 0 || rank >= SCORE_SHOW) continue;
            int slot = score_order[rank];
            if (slot >= SCORE_SLOTS) slot = 0;
            int digits = actors[i].frame & 1;
            pictures_score_text(name, actors[i].x, (int)actors[i].y, actors[i].z, TUNNEL_CAMERA,
                                digits ? board[slot] : names[slot],
                                digits ? SCORE_PLACES : NAME_LEN, digits);
        }
    } else if (game->mode == MODE_TUNNEL) {
        uint8_t bits[TUNNEL_BYTES];
        tunnel_build_bitmap(bits, (unsigned)game->tunnel_distance, tunnel_min_radius);
        int color = tunnel_line_color(game->tunnel_reward);
        if (tunnel_flash == 2) color = FLASH_HOT;
        else if (tunnel_flash == 1) color = FLASH_DIM;
        pictures_tunnel(bits, color);
        for (int layer = 7; layer >= 3; layer--) draw_tunnel_layer(layer);
        char name[24];
        snprintf(name, sizeof name, "fs_1594_%02u", game->tunnel_frame);
        pictures_sprite(name, game->tunnel_x, game->tunnel_y, 0, TUNNEL_CAMERA);
        draw_tunnel_layer(2);
        draw_tunnel_layer(1);
    } else {
        pictures_draw_tiles(&game->flight);
        for (int i = 0; i < ACTORS; i++) {
            if (actors[i].flags == 0) continue;
            if (actors[i].handler == HANDLER_IMPACT) {
                unsigned index = actors[i].state[0];
                if (index >= IMPACT_FRAMES) index = IMPACT_FRAMES - 1;
                pictures_bank_sprite(BANK_BIRD1, IMPACT_SPRITE[index], IMPACT_W, IMPACT_H,
                                     IMPACT_ANCHOR, IMPACT_ANCHOR, actors[i].x, (int)actors[i].y,
                                     actors[i].z, (int)game->flight.camera_y);
                continue;
            }
            char name[24];
            sprite_name(&actors[i], name, sizeof name);
            pictures_sprite(name, actors[i].x, (int)actors[i].y, actors[i].z, (int)game->flight.camera_y);
        }
        if (game->flight.alive) pictures_draw_ship(&game->flight);
    }
    if (game->mode != MODE_TITLE) {
        hud_draw(game->flight.fuel, game->ammo, game->flight.shield, game->lives, side_charges,
                 collected, score_digits);
    }
    pictures_finish();
}
