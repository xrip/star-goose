#ifndef GOOSE_TUNNEL_H
#define GOOSE_TUNNEL_H

#include <stdint.h>

enum { TUNNEL_BYTES = 256 * 32 };

/* 1bpp scratch, stride 32, bit 7 is the leftmost pixel. */
void tunnel_build_bitmap(uint8_t *bits, unsigned distance, int min_radius);
int tunnel_line_color(uint8_t reward_kind);
uint8_t tunnel_player_frame(uint8_t angle);
void tunnel_player_position(uint8_t angle, int *x, int *y);

#endif
