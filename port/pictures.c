#include "pictures.h"
#include "raylib.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

enum {
    SCREEN_W = 320,
    SCREEN_H = 200,
    VIEW_X = 32,
    VIEW_Y = 8,
    VIEW_W = 256,
    VIEW_H = 160,
    TILES = 42,
    BANKS = 6
};

static const Color EGA[16] = {
    {0, 0, 0, 255},       {0, 0, 170, 255},     {0, 170, 0, 255},     {0, 170, 170, 255},
    {170, 0, 0, 255},     {170, 0, 170, 255},   {170, 85, 0, 255},    {170, 170, 170, 255},
    {85, 85, 85, 255},    {85, 85, 255, 255},   {85, 255, 85, 255},   {85, 255, 255, 255},
    {255, 85, 85, 255},   {255, 85, 255, 255},  {255, 255, 85, 255},  {255, 255, 255, 255}
};

static const uint8_t TILE_HEIGHT[TILES] = {
    16, 16, 8, 24, 24, 24, 16, 16, 32, 32, 16, 16, 24, 24, 24, 24,
    16, 16, 16, 16, 24, 24, 24, 24, 16, 16, 8, 8, 8, 8, 32, 32,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16
};
static const int8_t TILE_Y_ADJUST[TILES] = {
    0, 0, 0, 0, 0, -8, 0, -8, 0, -8, 0, -8, 0, 0, -8, 0,
    0, 0, 0, -8, 0, 0, -8, 0, 0, 0, 0, 0, 0, 0, -8, -8,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const uint16_t TILE_OFFSET[TILES] = {
    0x1466, 0x0000, 0x0a00, 0x000c, 0x0018, 0x0024, 0x2800, 0x280c,
    0x0f48, 0x0f54, 0x2818, 0x2824, 0x0030, 0x003c, 0x0048, 0x0054,
    0x2830, 0x283c, 0x1e00, 0x1e0c, 0x0060, 0x006c, 0x0f00, 0x0f0c,
    0x1e18, 0x1e24, 0x0f30, 0x0f3c, 0x0f18, 0x0f24, 0x0f60, 0x0f6c,
    0x1418, 0x1424, 0x1430, 0x143c, 0x1e30, 0x1e3c, 0x55a0, 0x55ac,
    0x4ba0, 0x4bac
};

static const uint8_t SHIP_W[11] = {32, 32, 32, 32, 28, 28, 32, 32, 28, 28, 32};
static const uint8_t SHIP_H[11] = {19, 24, 23, 27, 25, 27, 24, 25, 29, 30, 23};
static const int8_t SHIP_AX[11] = {14, 14, 14, 14, 15, 12, 14, 14, 13, 12, 14};
static const int8_t SHIP_AY[11] = {12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12};
static const uint16_t SHIP_BANK[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2};
static const uint16_t SHIP_OFFSET[11] = {
    0x1e10, 0x5640, 0x0fae, 0x24e0, 0x35c0, 0x4560, 0x6540, 0x000e, 0x0000, 0x1220, 0x6f02
};

extern const uint8_t goose_height_delta[TILES];

static uint8_t bank_index[BANKS][SCREEN_W * SCREEN_H];
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

static int load_bank(int bank, const char *path) {
    Image image = LoadImage(path);
    if (!IsImageValid(image)) return 0;
    ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    if (image.width != SCREEN_W || image.height != SCREEN_H) {
        UnloadImage(image);
        return 0;
    }
    const unsigned char *pixels = image.data;
    for (int i = 0; i < SCREEN_W * SCREEN_H; i++)
        bank_index[bank][i] = (uint8_t)ega_index(pixels[i * 4], pixels[i * 4 + 1], pixels[i * 4 + 2]);
    UnloadImage(image);
    return 1;
}

static const char *find_png(const char *name) {
    static char path[512];
    const char *roots[] = {"", GetApplicationDirectory()};
    const char *prefixes[] = {
        "assets/png/", "../assets/png/", "../../assets/png/", "../../../assets/png/"
    };
    for (int root = 0; root < 2; root++) {
        for (int prefix = 0; prefix < 4; prefix++) {
            snprintf(path, sizeof path, "%s%s%s", roots[root], prefixes[prefix], name);
            if (FileExists(path)) return path;
        }
    }
    return 0;
}

int pictures_load(void) {
    static const char *names[BANKS] = {
        "BIRD1.png", "BIRD2.png", "BLOX.png", "NEWBIRD.png", "PODZ1.png", "INTRO.png"
    };
    for (int bank = 0; bank < BANKS; bank++) {
        const char *path = find_png(names[bank]);
        if (!path || !load_bank(bank, path)) {
            fprintf(stderr, "missing picture %s\n", names[bank]);
            return 0;
        }
    }
    Image blank = GenImageColor(SCREEN_W, SCREEN_H, BLACK);
    frame = LoadTextureFromImage(blank);
    UnloadImage(blank);
    SetTextureFilter(frame, TEXTURE_FILTER_POINT);
    ready = 1;
    return 1;
}

static void blit(int bank, int offset, int width, int height, int dest_x, int dest_y, int key) {
    if (bank < 0 || bank >= BANKS || width <= 0 || height <= 0) return;
    int src_x = (offset % 160) * 2;
    int src_y = offset / 160;
    for (int dy = 0; dy < height; dy++) {
        int sy = src_y + dy;
        int y = dest_y + dy;
        if (sy < 0 || sy >= SCREEN_H || y < VIEW_Y || y >= VIEW_Y + VIEW_H) continue;
        for (int dx = 0; dx < width; dx++) {
            int sx = src_x + dx;
            int x = dest_x + dx;
            if (sx < 0 || sx >= SCREEN_W || x < VIEW_X || x >= VIEW_X + VIEW_W) continue;
            uint8_t color = bank_index[bank][sy * SCREEN_W + sx];
            if (color == key) continue;
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
            int bank = tile < 0x26 ? 2 : 1;
            int offset = TILE_OFFSET[tile];
            int width = 24;
            int dest_x = VIEW_X + 8 + (column - 1) * 24;
            if (column == 0) {
                offset += 8;
                width = 8;
                dest_x = VIEW_X;
            } else if (column == 11) {
                width = 8;
                dest_x = VIEW_X + 248;
            }
            int world_y = world_row * 16 + TILE_Y_ADJUST[tile] - signed8(z);
            blit(bank, offset, width, TILE_HEIGHT[tile], dest_x, screen_y(world_y, camera), 0);
        }
    }
}

static void draw_ship(const Surface *game) {
    int frame_index = game->frame;
    if (frame_index > 10) return;
    int z_sum = (game->z + SHIP_AY[frame_index]) & 255;
    int world_y = (int)((game->camera_y + 112u) & 1023u) - signed8((unsigned)z_sum);
    int dest_x = VIEW_X + game->x - SHIP_AX[frame_index];
    blit((int)SHIP_BANK[frame_index] / 2, SHIP_OFFSET[frame_index],
         SHIP_W[frame_index], SHIP_H[frame_index],
         dest_x, screen_y(world_y, (int)game->camera_y), 2);
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
    if (ready) UnloadTexture(frame);
    ready = 0;
}
