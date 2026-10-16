#include "pictures.h"
#include "play.h"
#include "raylib.h"
#include "surface.h"
#include <stdio.h>
#include <stdlib.h>

enum { SCREEN_W = 320, SCREEN_H = 200, SCALE = 3 };

static unsigned read_controls(void) {
    unsigned controls = 0;
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) controls |= CONTROL_LEFT;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) controls |= CONTROL_RIGHT;
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) controls |= CONTROL_UP;
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) controls |= CONTROL_DOWN;
    if (IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_Z) || IsKeyDown(KEY_LEFT_CONTROL)) controls |= CONTROL_FIRE;
    return controls;
}

int main(void) {
    InitWindow(SCREEN_W * SCALE, SCREEN_H * SCALE, "Star Goose");
    SetTargetFPS(60);
    SetExitKey(KEY_ESCAPE);
    if (!pictures_load()) {
        CloseWindow();
        return 1;
    }

    Game game;
    game_boot(&game);
    float step = 0.0f;
    const float tick = 1.0f / 20.0f;
    int shot = getenv("GOOSE_SHOT") != 0;
    const char *scripted = getenv("GOOSE_TICKS");
    if (shot || scripted) game_play(&game);
    if (scripted) {
        int count = atoi(scripted);
        unsigned held = 0;
        if (getenv("GOOSE_UP")) held |= CONTROL_UP;
        if (getenv("GOOSE_LEFT")) held |= CONTROL_LEFT;
        if (getenv("GOOSE_RIGHT")) held |= CONTROL_RIGHT;
        if (getenv("GOOSE_FIRE")) held |= CONTROL_FIRE;
        for (int i = 0; i < count; i++) game_tick(&game, held);
        shot = 1;
    }

    while (!WindowShouldClose()) {
        if (!shot) {
            step += GetFrameTime();
            if (step > 0.25f) step = 0.25f;
            while (step >= tick) {
                step -= tick;
                if (IsKeyPressed(KEY_R)) game_boot(&game);
                else game_tick(&game, read_controls());
            }
        }
        game_draw(&game);
        BeginDrawing();
        ClearBackground(BLACK);
        pictures_present();
        if (game.mode != MODE_TITLE) {
            DrawText(TextFormat("score %d   fuel %d   shield %d   ammo %d   lives %d",
                                game.score, game.flight.fuel, game.flight.shield, game.ammo, game.lives),
                     8, SCREEN_H * SCALE - 28, 20, WHITE);
        }
        if (shot) {
            pictures_save("shot.png");
            printf("mode %d level %d part %d score %d ammo %d lives %d tunnel %d fuel %d shield %d\n",
                   game.mode, game.level, game.part, game.score, game.ammo, game.lives,
                   game.tunnel_distance, game.flight.fuel, game.flight.shield);
        }
        EndDrawing();
        if (shot) break;
    }
    pictures_shutdown();
    CloseWindow();
    return 0;
}
