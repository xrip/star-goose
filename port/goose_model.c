#include "goose_model.h"
#include <string.h>

static uint16_t read16(const uint8_t *p) {
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}
static void write16(uint8_t *p, uint16_t value) {
    p[0] = (uint8_t)value;
    p[1] = (uint8_t)(value >> 8);
}
static int8_t signed8(uint8_t bits) {
    /* Convert only in-range signed values: no out-of-range unsigned -> signed cast. */
    return (int8_t)(bits < 128u ? (int)bits : (int)bits - 256);
}
bool goose_entity_read(GooseEntity *out, const uint8_t *p, size_t size) {
    if (!out || !p || size < GOOSE_ENTITY_BYTES) return false;
    out->flags = p[0]; out->param = p[1]; out->layer = p[2];
    out->z = signed8(p[3]); out->frame = p[4];
    out->x = read16(p + 5); out->y = read16(p + 7);
    out->width = p[9]; out->height = p[10];
    out->anchor_x = signed8(p[11]); out->anchor_y = signed8(p[12]);
    out->bank_offset = read16(p + 13); out->sprite_offset = read16(p + 15);
    out->transparent = p[17]; out->handler_id = p[18];
    memcpy(out->state, p + 19, 4);
    return true;
}
bool goose_entity_write(uint8_t *p, size_t size, const GooseEntity *e) {
    if (!e || !p || size < GOOSE_ENTITY_BYTES) return false;
    p[0] = e->flags; p[1] = e->param; p[2] = e->layer;
    p[3] = (uint8_t)e->z; p[4] = e->frame;
    write16(p + 5, e->x); write16(p + 7, e->y);
    p[9] = e->width; p[10] = e->height;
    p[11] = (uint8_t)e->anchor_x; p[12] = (uint8_t)e->anchor_y;
    write16(p + 13, e->bank_offset); write16(p + 15, e->sprite_offset);
    p[17] = e->transparent; p[18] = e->handler_id;
    memcpy(p + 19, e->state, 4);
    return true;
}
int goose_entity_allocate(GooseEntity pool[GOOSE_ENTITY_CAPACITY], const GooseEntity *source) {
    if (!pool || !source) return -1;
    for (size_t i = 0; i < GOOSE_ENTITY_CAPACITY; ++i) {
        if (pool[i].flags & GOOSE_SENTINEL) break;
        if (!(pool[i].flags & GOOSE_RENDER)) {
            pool[i] = *source;
            return (int)i;
        }
    }
    return -1;
}
void goose_entity_clear(GooseEntity pool[GOOSE_ENTITY_CAPACITY]) {
    if (!pool) return;
    for (size_t i = 0; i < GOOSE_ENTITY_CAPACITY; ++i) {
        if (pool[i].flags & GOOSE_SENTINEL) break;
        pool[i].flags = 0;
    }
}
bool goose_apply_frame(GooseEntity *e, const uint8_t *d, size_t size) {
    if (!e || !d || size == 0) return false;
    const size_t count = d[0], index = e->frame;
    if (index >= count || size < 1 + 8 * count) return false;
    e->width = d[1 + index];
    e->height = d[1 + count + index];
    e->anchor_x = signed8(d[1 + 2 * count + index]);
    e->anchor_y = signed8(d[1 + 3 * count + index]);
    e->bank_offset = read16(d + 1 + 4 * count + 4 * index);
    e->sprite_offset = read16(d + 1 + 4 * count + 4 * index + 2);
    return true;
}
int16_t goose_signed16(uint16_t bits) {
    return (int16_t)(bits < UINT16_C(0x8000)
                    ? (int32_t)bits : (int32_t)bits - INT32_C(65536));
}
uint16_t goose_sar16(uint16_t bits, unsigned shift) {
    if (shift == 0) return bits;
    if (shift >= 16) return (bits & UINT16_C(0x8000)) ? UINT16_MAX : 0;
    uint32_t result = (uint32_t)bits >> shift;
    if (bits & UINT16_C(0x8000)) result |= UINT32_C(0xffff) << (16 - shift);
    return (uint16_t)result;
}
int16_t goose_surface_dx(int16_t velocity) {
    /* CS:115B..1160: add AX,3; sar AX,1; sar AX,1. Wrap precedes sign. */
    uint16_t bits = (uint16_t)((uint16_t)velocity + UINT32_C(3));
    return goose_signed16(goose_sar16(bits, 2));
}
int16_t goose_tunnel_angle_force(uint8_t angle) {
    /* CS:3084..309C: test SF of the 8-bit subtraction, NOT signed(angle) < 5. */
    uint8_t adjusted = (uint8_t)(angle - 5u);
    if (adjusted & 0x80u) return 0;
    if (adjusted > 56u) adjusted = 56u;
    return (int16_t)(-(int)(adjusted >> 1) - 3);
}
