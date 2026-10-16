#ifndef GOOSE_TERRAIN_H
#define GOOSE_TERRAIN_H

#include <stdint.h>

typedef struct TerrainSample {
    int height;
    uint8_t frame;
    uint8_t row;
} TerrainSample;

/* x, y are surface coordinates. y is modulo 1024. */
TerrainSample terrain_sample(const uint8_t *tiles, unsigned x, unsigned y);

#endif
