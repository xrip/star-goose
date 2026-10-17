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
    Surface wing;
    int wing_on;
    int wing_ammo;
} Game;

void game_boot(Game *game);
void game_play(Game *game);
void game_tick(Game *game, unsigned controls);
/* Arms the second ship. controls are that ship's stick for this tick. */
void game_partner(int enabled, unsigned controls);
/* Same shield path the enemy shots use, aimed at the second ship. */
void game_wing_strike(Game *game, int amount);
void game_draw(Game *game);
void game_hold_input(int hold);

#endif
