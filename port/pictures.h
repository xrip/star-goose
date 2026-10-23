#ifndef GOOSE_PICTURES_H
#define GOOSE_PICTURES_H

#include "surface.h"

/* Loads the atlas and the intro picture. Returns 0 when the atlas is missing. */
int pictures_load(void);
void pictures_clear(void);
void pictures_draw_tiles(const Surface *game);
void pictures_draw_ship(const Surface *game);
void pictures_sprite(const char *name, int world_x, int world_y, int z, int camera_y);
/* Same placement as pictures_sprite, but the pixels come from a graphics bank.
 * key is the original transparent color, 0..15. */
void pictures_sprite_bank(const char *name, int bank, unsigned offset, int key,
                          int world_x, int world_y, int z, int camera_y);
/* F8 forces the original picture. VGA is the default when assets/vga loaded. */
void pictures_toggle_vga(void);
/* Stamps a high-score name or digit row onto a drifting title sprite. */
void pictures_score_text(const char *sprite, int world_x, int world_y, int z, int camera_y,
                         const uint8_t *chars, int count, int digits);
void pictures_anchor(const char *name, int *anchor_x, int *anchor_y);
/* Height and anchor of one atlas frame. Returns 0 when the name is missing. */
int pictures_metrics(const char *name, int *height, int *anchor_y);
void pictures_intro(void);
void pictures_tunnel(const uint8_t *bits, int color);
void pictures_finish(void);
/* bank_offset is the original segment-table index: 2 = BIRD2, 4 = BLOX.
 * live_columns < 0 draws the whole sprite. Otherwise columns at and after
 * that index are the emptied gauge cells from CS:25E3. */
void pictures_blit_bank(int bank_offset, unsigned offset, int width, int height,
                        int dest_x, int dest_y, int live_columns);
void pictures_fill(int x, int y, int width, int height, int color);
/* Draws one packed sprite from a graphics bank. Bank 0 is BIRD1. */
void pictures_bank_sprite(int bank, unsigned offset, int width, int height,
                          int anchor_x, int anchor_y,
                          int world_x, int world_y, int z, int camera_y);
/* Switch: size the native window, then the GL view. No-op elsewhere. */
void pictures_switch_window(void);
void pictures_prepare_screen(void);
void pictures_present(void);
void pictures_save(const char *path);
void pictures_shutdown(void);

#endif
