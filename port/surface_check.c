#include "surface.h"
#include "terrain.h"
#include <stdio.h>

static int failures;

static void expect(int condition, const char *name) {
    if (condition) return;
    failures += 1;
    fprintf(stderr, "fail: %s\n", name);
}

static void check_start(void) {
    Surface game;
    surface_start(&game);
    TerrainSample ground = terrain_sample(goose_level0_tiles, 128, (0x350u + 112u) & 1023u);
    expect(game.x == 128, "start x");
    expect(game.velocity == 0, "start velocity");
    expect(game.camera_y == 0x350u, "start camera");
    expect(game.scroll_speed == 16 && game.base_scroll_speed == 16, "start speed");
    expect(game.fuel == 44 && game.shield == 56, "start gauges");
    expect(game.alive && game.frame == 1 && game.z == -24, "start terrain");
    expect(ground.height == -24 && ground.frame == 1 && ground.row == 60, "sample at start");
}

static void check_motion(void) {
    Surface game;
    surface_start(&game);
    surface_tick(&game, CONTROL_RIGHT);
    expect(game.velocity == 1 && game.x == 129, "right accelerates");

    surface_start(&game);
    surface_tick(&game, CONTROL_LEFT | CONTROL_RIGHT);
    expect(game.velocity == -1, "left wins when both are held");

    surface_start(&game);
    game.velocity = 10;
    surface_tick(&game, 0);
    expect(game.velocity == 9, "drag pulls toward zero");

    surface_start(&game);
    for (int i = 0; i < 48; i++) surface_tick(&game, CONTROL_UP);
    expect(game.scroll_speed == 64, "up stops at 64");
    for (int i = 0; i < 30; i++) surface_tick(&game, CONTROL_DOWN);
    expect(game.scroll_speed == 16, "down stops at base speed");

    surface_start(&game);
    game.x = 236;
    game.velocity = 24;
    surface_tick(&game, CONTROL_RIGHT);
    expect(game.x == 236 && game.velocity == 0, "right edge stops the ship");

    surface_start(&game);
    game.x = 20;
    game.velocity = -24;
    surface_tick(&game, CONTROL_LEFT);
    expect(game.x == 20 && game.velocity == 0, "left edge stops the ship");
}

static void check_fuel_and_crash(void) {
    Surface game;
    surface_start(&game);
    for (int i = 0; i < 69; i++) surface_tick(&game, 0);
    expect(game.fuel == 44 && game.camera_y == 0x350u - 69u, "fuel waits for distance 140");
    surface_tick(&game, 0);
    expect(game.fuel == 43 && game.camera_y == 0x350u - 70u, "fuel drops on the next tick");

    surface_start(&game);
    game.shield = 0;
    surface_tick(&game, 0);
    expect(!game.alive, "empty shield crashes");

    surface_start(&game);
    game.frame = 0;
    for (int i = 0; i < 3; i++) surface_tick(&game, 0);
    expect(game.alive, "three void ticks are still alive");
    surface_tick(&game, 0);
    expect(!game.alive, "fourth void tick crashes");
}

int main(void) {
    check_start();
    check_motion();
    check_fuel_and_crash();
    if (failures) {
        fprintf(stderr, "%d failed\n", failures);
        return 1;
    }
    puts("surface checks passed");
    return 0;
}
