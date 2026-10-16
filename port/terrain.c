#include "terrain.h"

enum { TILE_COLUMNS = 12, TILE_COUNT = 42, PROFILE_BYTES = 1600 };

extern const uint8_t goose_height_delta[TILE_COUNT];
extern const uint8_t goose_variant_a[TILE_COUNT];
extern const uint8_t goose_variant_b[TILE_COUNT];
extern const uint16_t goose_profile_offset[TILE_COUNT];
extern const uint8_t goose_profiles[PROFILE_BYTES];

static int signed8(unsigned bits) {
    return bits < 128u ? (int)bits : (int)bits - 256;
}

TerrainSample terrain_sample(const uint8_t *tiles, unsigned x, unsigned y) {
    TerrainSample sample = {0, 0, 0};
    unsigned shifted = (x + 16u) & 0xffffu;
    unsigned column = shifted / 24u;
    unsigned sub_column = (shifted % 24u) / 3u;
    unsigned world_row = (y & 1023u) >> 4;
    unsigned stored_row = 63u - world_row;
    unsigned sub_row = (y & 15u) >> 1;
    if (column >= TILE_COLUMNS) return sample;

    unsigned tile = tiles[stored_row * TILE_COLUMNS + column];
    if (tile >= TILE_COUNT) return sample;

    unsigned profile_at = (unsigned)goose_profile_offset[tile] + sub_row * 8u + sub_column;
    if (profile_at >= PROFILE_BYTES) return sample;
    unsigned profile = goose_profiles[profile_at];

    unsigned height = profile & 0x1fu;
    if (height & 0x10u) height |= 0xe0u;
    for (unsigned i = 0; i < column; i++)
        height = (height + goose_height_delta[tiles[stored_row * TILE_COLUMNS + i]]) & 0xffu;

    sample.height = signed8(height);
    sample.frame = (profile & 0x80u) ? goose_variant_b[tile] : goose_variant_a[tile];
    sample.row = (uint8_t)world_row;
    return sample;
}
