#ifndef GOOSE_MODEL_H
#define GOOSE_MODEL_H
/* Небольшие reference-модели для будущего порта, НЕ полная игра.
 * По умолчанию это обычная host-структура с возможным padding.
 * Дисковый формат всегда читается/пишется явно, через функции ниже. */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum { GOOSE_ENTITY_BYTES = 23, GOOSE_ENTITY_CAPACITY = 48 };
enum {
    GOOSE_RENDER = 0x01, GOOSE_UPDATE = 0x02,
    GOOSE_MAP_OBJECT = 0x08, GOOSE_LARGE_ANCHOR = 0x10,
    GOOSE_SIDE = 0x20, GOOSE_PROJECTILE = 0x40, GOOSE_SENTINEL = 0x80
};
typedef struct GooseEntity {
    uint8_t flags;               /* Serialized +00 */
    uint8_t param;               /* +01: handler-specific, NOT universal health */
    uint8_t layer;               /* +02: render passes 7 .. 1 */
    int8_t z;                   /* +03: signed projection displacement */
    uint8_t frame;               /* +04 */
    uint16_t x, y;              /* +05, +07: little-endian on disk */
    uint8_t width, height;       /* +09, +0A */
    int8_t anchor_x, anchor_y;   /* +0B, +0C */
    uint16_t bank_offset;       /* +0D: BYTE offset in segment pointer table */
    uint16_t sprite_offset;     /* +0F: offset in that bank */
    uint8_t transparent;        /* +11 */
    uint8_t handler_id;         /* +12: 0 skipped; 1..35 uses table[id-1] */
    uint8_t state[4];           /* +13..+16: overlapping bytes/words by handler */
} GooseEntity;

bool goose_entity_read(GooseEntity *out, const uint8_t *data, size_t size);
bool goose_entity_write(uint8_t *out, size_t size, const GooseEntity *entity);
/* Returns first reusable index, or -1. Tests RENDER, not flags == 0. */
int goose_entity_allocate(GooseEntity pool[GOOSE_ENTITY_CAPACITY], const GooseEntity *source);
/* Clears flags only; original data in every other field remain unchanged. */
void goose_entity_clear(GooseEntity pool[GOOSE_ENTITY_CAPACITY]);
/* Apply FrameSet SoA data, matching CS:2121 on in-range inputs.
 * This host interface rejects malformed buffers; DOS code lacks those checks. */
bool goose_apply_frame(GooseEntity *entity, const uint8_t *descriptor, size_t size);
/* Exact bit-pattern helpers: no implementation-defined negative right shifts. */
int16_t goose_signed16(uint16_t bits);
uint16_t goose_sar16(uint16_t bits, unsigned shift);
int16_t goose_surface_dx(int16_t velocity);
int16_t goose_tunnel_angle_force(uint8_t angle);
#endif
