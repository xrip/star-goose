#ifndef GOOSE_PLAY_H
#define GOOSE_PLAY_H

#include "surface.h"

enum { MODE_TITLE = 0, MODE_SURFACE = 1, MODE_TUNNEL = 2 };

typedef struct Game {
    int mode;
    Surface flight;
    int score;
    int ammo;
    int lives;
    int tunnel_distance;
    int tunnel_x;
    int tunnel_y;
    int tunnel_frame;
    uint8_t tunnel_reward;
    int level;
    int part;
} Game;

void game_boot(Game *game);
void game_play(Game *game);
void game_tick(Game *game, unsigned controls);
void game_draw(Game *game);

#endif
