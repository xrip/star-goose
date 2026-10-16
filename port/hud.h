#ifndef GOOSE_HUD_H
#define GOOSE_HUD_H

#include <stdint.h>

/* Draws the original margin gauges. collected[6] is 0 until that item is taken. */
void hud_draw(int fuel, int ammo, int shield, int lives, int charges,
              const uint8_t collected[6], const uint8_t digits[6]);

#endif
