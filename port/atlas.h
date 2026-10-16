#ifndef GOOSE_ATLAS_H
#define GOOSE_ATLAS_H

#include <stdint.h>

typedef struct AtlasSprite {
    uint16_t x, y, w, h;
    int8_t anchor_x, anchor_y;
} AtlasSprite;

extern const AtlasSprite atlas_tiles[42];
extern const AtlasSprite atlas_ship[11];
extern const AtlasSprite atlas_sprites[];
extern const char *const atlas_sprite_names[];
extern const int atlas_sprite_count;

#endif
