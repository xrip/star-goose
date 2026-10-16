#include "pictures.h"
#include "raylib.h"
#include "surface.h"
#include <stdlib.h>

enum { SCREEN_W = 320, SCREEN_H = 200, SCALE = 3 };

static unsigned read_controls(void) {
    unsigned controls = 0;
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) controls |= CONTROL_LEFT;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) controls |= CONTROL_RIGHT;
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) controls |= CONTROL_UP;
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) controls |= CONTROL_DOWN;
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

    Surface game;
    surface_start(&game);
    float step = 0.0f;
    const float tick = 1.0f / 20.0f;
    int shot = getenv("GOOSE_SHOT") != 0;

    while (!WindowShouldClose()) {
        if (!shot) {
            step += GetFrameTime();
            if (step > 0.25f) step = 0.25f;
            while (step >= tick) {
                step -= tick;
                if (IsKeyPressed(KEY_R)) surface_start(&game);
                else surface_tick(&game, read_controls());
            }
        }
        pictures_draw_surface(&game);
        BeginDrawing();
        ClearBackground(BLACK);
        pictures_present();
        DrawText(TextFormat("fuel %d  shield %d  speed %d", game.fuel, game.shield, game.scroll_speed),
                 8, SCREEN_H * SCALE - 28, 20, WHITE);
        if (!game.alive) DrawText("crash   R restart", 8, SCREEN_H * SCALE - 52, 20, RED);
        if (shot) pictures_save("shot.png");
        EndDrawing();
        if (shot) break;
    }
    pictures_shutdown();
    CloseWindow();
    return 0;
}
