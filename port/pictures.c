#include "pictures.h"
#include "atlas.h"
#include "raylib.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    SCREEN_W = 320,
    SCREEN_H = 200,
    VIEW_X = 32,
    VIEW_Y = 8,
    VIEW_W = 256,
    VIEW_H = 160,
    TILES = 42,
    CLEAR = 255
};

static const Color EGA[16] = {
    {0, 0, 0, 255},       {0, 0, 170, 255},     {0, 170, 0, 255},     {0, 170, 170, 255},
    {170, 0, 0, 255},     {170, 0, 170, 255},   {170, 85, 0, 255},    {170, 170, 170, 255},
    {85, 85, 85, 255},    {85, 85, 255, 255},   {85, 255, 85, 255},   {85, 255, 255, 255},
    {255, 85, 85, 255},   {255, 85, 255, 255},  {255, 255, 85, 255},  {255, 255, 255, 255}
};

static const int8_t TILE_Y_ADJUST[TILES] = {
    0, 0, 0, 0, 0, -8, 0, -8, 0, -8, 0, -8, 0, 0, -8, 0,
    0, 0, 0, -8, 0, 0, -8, 0, 0, 0, 0, 0, 0, 0, -8, -8,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

extern const uint8_t goose_height_delta[TILES];

static uint8_t *atlas_pixels;
static int atlas_w;
static int atlas_h;
static uint8_t screen[SCREEN_W * SCREEN_H];
static Color upload[SCREEN_W * SCREEN_H];
static Texture2D frame;
static int ready;

static int signed8(unsigned bits) {
    return bits < 128u ? (int)bits : (int)bits - 256;
}

static int ega_index(unsigned char r, unsigned char g, unsigned char b) {
    for (int i = 0; i < 16; i++) {
        if (EGA[i].r == r && EGA[i].g == g && EGA[i].b == b) return i;
    }
    return 0;
}

static const char *find_file(const char *relative) {
    static char path[512];
    const char *roots[] = {"", GetApplicationDirectory()};
    const char *parents[] = {"", "../", "../../", "../../../"};
    for (int root = 0; root < 2; root++) {
        for (int parent = 0; parent < 4; parent++) {
            snprintf(path, sizeof path, "%s%s%s", roots[root], parents[parent], relative);
            if (FileExists(path)) return path;
        }
    }
    return 0;
}

int pictures_load(void) {
    const char *path = find_file("assets/atlas/goose.png");
    if (!path) {
        fprintf(stderr, "missing assets/atlas/goose.png\n");
        return 0;
    }
    Image image = LoadImage(path);
    if (!IsImageValid(image)) return 0;
    ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    atlas_w = image.width;
    atlas_h = image.height;
    atlas_pixels = malloc((size_t)atlas_w * (size_t)atlas_h);
    if (!atlas_pixels) {
        UnloadImage(image);
        return 0;
    }
    const unsigned char *pixels = image.data;
    for (int i = 0; i < atlas_w * atlas_h; i++) {
        if (pixels[i * 4 + 3] < 128) atlas_pixels[i] = CLEAR;
        else atlas_pixels[i] = (uint8_t)ega_index(pixels[i * 4], pixels[i * 4 + 1], pixels[i * 4 + 2]);
    }
    UnloadImage(image);

    Image blank = GenImageColor(SCREEN_W, SCREEN_H, BLACK);
    frame = LoadTextureFromImage(blank);
    UnloadImage(blank);
    SetTextureFilter(frame, TEXTURE_FILTER_POINT);
    ready = 1;
    return 1;
}

static void blit_sprite(const AtlasSprite *sprite, int src_dx, int width, int dest_x, int dest_y) {
    if (!sprite || width <= 0 || sprite->h == 0) return;
    for (int dy = 0; dy < sprite->h; dy++) {
        int sy = sprite->y + dy;
        int y = dest_y + dy;
        if (sy < 0 || sy >= atlas_h || y < VIEW_Y || y >= VIEW_Y + VIEW_H) continue;
        for (int dx = 0; dx < width; dx++) {
            int sx = sprite->x + src_dx + dx;
            int x = dest_x + dx;
            if (sx < 0 || sx >= atlas_w || x < VIEW_X || x >= VIEW_X + VIEW_W) continue;
            uint8_t color = atlas_pixels[sy * atlas_w + sx];
            if (color == CLEAR) continue;
            screen[y * SCREEN_W + x] = color;
        }
    }
}

static int screen_y(int world_y, int camera_y) {
    int relative = (world_y - camera_y) & 1023;
    if (relative >= 512) relative -= 1024;
    return VIEW_Y + relative;
}

static void draw_tiles(const Surface *game) {
    const uint8_t *tiles = game->tiles;
    int camera = (int)game->camera_y;
    for (int stored = 0; stored < 64; stored++) {
        unsigned running = 0;
        int world_row = 63 - stored;
        for (int column = 0; column < 12; column++) {
            unsigned z = running;
            int tile = tiles[stored * 12 + column];
            if (tile >= TILES) tile = 0;
            running = (running + goose_height_delta[tile]) & 255u;
            const AtlasSprite *sprite = &atlas_tiles[tile];
            int src_dx = 0;
            int width = sprite->w;
            int dest_x = VIEW_X + 8 + (column - 1) * 24;
            if (column == 0) {
                src_dx = 16;
                width = 8;
                dest_x = VIEW_X;
            } else if (column == 11) {
                width = 8;
                dest_x = VIEW_X + 248;
            }
            int world_y = world_row * 16 + TILE_Y_ADJUST[tile] - signed8(z);
            blit_sprite(sprite, src_dx, width, dest_x, screen_y(world_y, camera));
        }
    }
}

static void draw_ship(const Surface *game) {
    int frame_index = game->frame;
    if (frame_index < 0 || frame_index > 10) return;
    const AtlasSprite *sprite = &atlas_ship[frame_index];
    int z_sum = (game->z + sprite->anchor_y) & 255;
    int world_y = (int)((game->camera_y + 112u) & 1023u) - signed8((unsigned)z_sum);
    int dest_x = VIEW_X + game->x - sprite->anchor_x;
    blit_sprite(sprite, 0, sprite->w, dest_x, screen_y(world_y, (int)game->camera_y));
}

void pictures_draw_surface(const Surface *game) {
    if (!ready) return;
    memset(screen, 0, sizeof screen);
    draw_tiles(game);
    draw_ship(game);
    for (int i = 0; i < SCREEN_W * SCREEN_H; i++) upload[i] = EGA[screen[i]];
    UpdateTexture(frame, upload);
}

void pictures_present(void) {
    DrawTextureEx(frame, (Vector2){0, 0}, 0, 3, WHITE);
}

void pictures_save(const char *path) {
    Image image = {
        .data = upload,
        .width = SCREEN_W,
        .height = SCREEN_H,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };
    ExportImage(image, path);
}

void pictures_shutdown(void) {
    free(atlas_pixels);
    atlas_pixels = 0;
    if (ready) UnloadTexture(frame);
    ready = 0;
}
