#ifndef GOOSE_MAPS_H
#define GOOSE_MAPS_H

#include <stdint.h>

typedef struct GooseMapObject {
    uint8_t kind;
    uint8_t column;
    uint8_t stored_row;
} GooseMapObject;

typedef struct GooseMap {
    const uint8_t *tiles;
    const GooseMapObject *objects;
    int object_count;
} GooseMap;

/* level_index * 3 + level_part. Eight levels, three parts each. */
extern const GooseMap goose_maps[24];

#endif
