#ifndef GOOSE_PICTURES_H
#define GOOSE_PICTURES_H

#include "surface.h"

/* Loads the atlas and the intro picture. Returns 0 when the atlas is missing. */
int pictures_load(void);
void pictures_clear(void);
void pictures_draw_tiles(const Surface *game);
void pictures_draw_ship(const Surface *game);
void pictures_sprite(const char *name, int world_x, int world_y, int z, int camera_y);
void pictures_anchor(const char *name, int *anchor_x, int *anchor_y);
void pictures_intro(void);
void pictures_tunnel(const uint8_t *bits, int color);
void pictures_finish(void);
void pictures_present(void);
void pictures_save(const char *path);
void pictures_shutdown(void);

#endif
