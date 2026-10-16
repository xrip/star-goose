#include "raylib.h"
#include "surface.h"

enum { PLAY_W = 256, PLAY_H = 160, SCALE = 3 };

static Color tile_color(uint8_t tile) {
    static const Color ega[16] = {
        {0, 0, 0, 255},       {0, 0, 170, 255},     {0, 170, 0, 255},     {0, 170, 170, 255},
        {170, 0, 0, 255},     {170, 0, 170, 255},   {170, 85, 0, 255},    {170, 170, 170, 255},
        {85, 85, 85, 255},    {85, 85, 255, 255},   {85, 255, 85, 255},   {85, 255, 255, 255},
        {255, 85, 85, 255},   {255, 85, 255, 255},  {255, 255, 85, 255},  {255, 255, 255, 255}
    };
    return ega[tile & 15u];
}

static unsigned read_controls(void) {
    unsigned controls = 0;
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) controls |= CONTROL_LEFT;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) controls |= CONTROL_RIGHT;
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) controls |= CONTROL_UP;
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) controls |= CONTROL_DOWN;
    return controls;
}

static void draw_world(const Surface *game) {
    int camera = (int)(game->camera_y & 1023u);
    for (int screen_y = 0; screen_y < PLAY_H;) {
        int world_y = (camera + screen_y) & 1023;
        int stored_row = 63 - (world_y >> 4);
        int row_height = 16 - (world_y & 15);
        if (screen_y + row_height > PLAY_H) row_height = PLAY_H - screen_y;
        for (int column = 0; column < 12; column++) {
            int left = column * 24 - 16;
            int right = left + 24;
            if (right <= 0 || left >= PLAY_W) continue;
            if (left < 0) left = 0;
            if (right > PLAY_W) right = PLAY_W;
            uint8_t tile = game->tiles[stored_row * 12 + column];
            DrawRectangle(left * SCALE, screen_y * SCALE,
                          (right - left) * SCALE, row_height * SCALE, tile_color(tile));
        }
        screen_y += row_height;
    }

    int ship_x = game->x * SCALE;
    int ship_y = (112 - game->z) * SCALE;
    Color ship = game->alive ? YELLOW : RED;
    DrawTriangle((Vector2){ship_x, ship_y - 8 * SCALE},
                 (Vector2){ship_x - 7 * SCALE, ship_y + 8 * SCALE},
                 (Vector2){ship_x + 7 * SCALE, ship_y + 8 * SCALE}, ship);
}

static void draw_hud(const Surface *game) {
    DrawRectangle(8, 8, 210, game->alive ? 78 : 102, (Color){0, 0, 0, 180});
    DrawText(TextFormat("fuel %d", game->fuel), 16, 14, 20, WHITE);
    DrawText(TextFormat("shield %d", game->shield), 16, 36, 20, WHITE);
    DrawText(TextFormat("speed %d", game->scroll_speed), 16, 58, 20, WHITE);
    if (!game->alive) DrawText("crash   R restart", 16, 84, 20, RED);
}

int main(void) {
    InitWindow(PLAY_W * SCALE, PLAY_H * SCALE, "Star Goose");
    SetTargetFPS(60);
    SetExitKey(KEY_ESCAPE);

    Surface game;
    surface_start(&game);
    float step = 0.0f;
    const float tick = 1.0f / 20.0f;

    while (!WindowShouldClose()) {
        step += GetFrameTime();
        if (step > 0.25f) step = 0.25f;
        while (step >= tick) {
            step -= tick;
            if (IsKeyPressed(KEY_R)) surface_start(&game);
            else surface_tick(&game, read_controls());
        }
        BeginDrawing();
        ClearBackground(BLACK);
        draw_world(&game);
        draw_hud(&game);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
