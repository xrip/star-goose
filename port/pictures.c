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
static uint8_t intro_pixels[SCREEN_W * SCREEN_H];
static int intro_ready;
static uint8_t screen[SCREEN_W * SCREEN_H];
static Color upload[SCREEN_W * SCREEN_H];
static Texture2D frame;
static int ready;
static uint8_t bank_bird2[SCREEN_W * SCREEN_H];
static uint8_t bank_blox[SCREEN_W * SCREEN_H];
static int bank_bird2_ready;
static int bank_blox_ready;

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

static void decode_packed(const unsigned char *data, uint8_t *pixels) {
    for (int y = 0; y < SCREEN_H; y++) {
        const unsigned char *row = data + y * 160;
        for (int x = 0; x < SCREEN_W; x++) {
            unsigned byte = row[x >> 1];
            pixels[y * SCREEN_W + x] = (uint8_t)((x & 1) ? (byte & 15) : (byte >> 4));
        }
    }
}

static void load_bank(const char *relative, uint8_t *pixels, int *flag) {
    const char *path = find_file(relative);
    *flag = 0;
    if (!path) return;
    int bytes = 0;
    unsigned char *data = LoadFileData(path, &bytes);
    if (!data || bytes != 32000) {
        if (data) UnloadFileData(data);
        return;
    }
    decode_packed(data, pixels);
    UnloadFileData(data);
    *flag = 1;
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

    const char *intro_path = find_file("assets/png/INTRO.png");
    if (intro_path) {
        Image intro = LoadImage(intro_path);
        if (IsImageValid(intro)) {
            ImageFormat(&intro, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
            const unsigned char *pixels = intro.data;
            int copy_w = intro.width < SCREEN_W ? intro.width : SCREEN_W;
            int copy_h = intro.height < SCREEN_H ? intro.height : SCREEN_H;
            memset(intro_pixels, 0, sizeof intro_pixels);
            for (int y = 0; y < copy_h; y++) {
                for (int x = 0; x < copy_w; x++) {
                    int i = y * intro.width + x;
                    intro_pixels[y * SCREEN_W + x] = pixels[i * 4 + 3] < 128
                        ? 0
                        : (uint8_t)ega_index(pixels[i * 4], pixels[i * 4 + 1], pixels[i * 4 + 2]);
                }
            }
            intro_ready = 1;
            UnloadImage(intro);
        }
    }

    Image blank = GenImageColor(SCREEN_W, SCREEN_H, BLACK);
    frame = LoadTextureFromImage(blank);
    UnloadImage(blank);
    SetTextureFilter(frame, TEXTURE_FILTER_POINT);
    load_bank("assets/BIRD2.X", bank_bird2, &bank_bird2_ready);
    load_bank("assets/BLOX.X", bank_blox, &bank_blox_ready);
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

static const AtlasSprite *find_sprite(const char *name) {
    if (!name) return 0;
    for (int i = 0; i < atlas_sprite_count; i++) {
        if (strcmp(atlas_sprite_names[i], name) == 0) return &atlas_sprites[i];
    }
    return 0;
}

void pictures_clear(void) {
    memset(screen, 0, sizeof screen);
}

void pictures_anchor(const char *name, int *anchor_x, int *anchor_y) {
    const AtlasSprite *sprite = find_sprite(name);
    if (anchor_x) *anchor_x = sprite ? sprite->anchor_x : 0;
    if (anchor_y) *anchor_y = sprite ? sprite->anchor_y : 0;
}

void pictures_sprite(const char *name, int world_x, int world_y, int z, int camera_y) {
    const AtlasSprite *sprite = find_sprite(name);
    if (!sprite) return;
    int z_sum = (z + sprite->anchor_y) & 255;
    int dest_x = VIEW_X + world_x - sprite->anchor_x;
    int dest_y = screen_y(world_y - signed8((unsigned)z_sum), camera_y);
    blit_sprite(sprite, 0, sprite->w, dest_x, dest_y);
}

void pictures_intro(void) {
    if (intro_ready) memcpy(screen, intro_pixels, sizeof screen);
}

void pictures_tunnel(const uint8_t *bits, int color) {
    if (color < 0 || color > 15) color = 3;
    for (int row = 0; row < VIEW_H; row++) {
        const uint8_t *src = bits + (row + 26) * 32;
        for (int x = 0; x < VIEW_W; x++) {
            if (src[x >> 3] & (uint8_t)(0x80u >> (x & 7))) {
                screen[(VIEW_Y + row) * SCREEN_W + VIEW_X + x] = (uint8_t)color;
            }
        }
    }
}

void pictures_finish(void) {
    if (!ready) return;
    for (int i = 0; i < SCREEN_W * SCREEN_H; i++) upload[i] = EGA[screen[i]];
    UpdateTexture(frame, upload);
}

void pictures_draw_tiles(const Surface *game) {
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

void pictures_draw_ship(const Surface *game) {
    int frame_index = game->frame;
    if (frame_index < 0 || frame_index > 10) return;
    const AtlasSprite *sprite = &atlas_ship[frame_index];
    int z_sum = (game->z + sprite->anchor_y) & 255;
    int world_y = (int)((game->camera_y + 112u) & 1023u) - signed8((unsigned)z_sum);
    int dest_x = VIEW_X + game->x - sprite->anchor_x;
    blit_sprite(sprite, 0, sprite->w, dest_x, screen_y(world_y, (int)game->camera_y));
}

void pictures_draw_surface(const Surface *game) {
    pictures_clear();
    pictures_draw_tiles(game);
    pictures_draw_ship(game);
    pictures_finish();
}

void pictures_blit_bank(int bank_offset, unsigned offset, int width, int height,
                        int dest_x, int dest_y, int live_columns) {
    const uint8_t *bank = bank_offset == 4 ? bank_blox : bank_bird2;
    int bank_ready = bank_offset == 4 ? bank_blox_ready : bank_bird2_ready;
    if (!bank_ready || width <= 0 || height <= 0) return;
    int src_y = (int)(offset / 160u);
    int src_x = (int)(offset % 160u) * 2;
    int live = live_columns < 0 ? width : live_columns;
    for (int dy = 0; dy < height; dy++) {
        int y = dest_y + dy;
        if (y < 0 || y >= SCREEN_H || src_y + dy < 0 || src_y + dy >= SCREEN_H) continue;
        for (int dx = 0; dx < width; dx++) {
            int x = dest_x + dx;
            if (x < 0 || x >= SCREEN_W || src_x + dx < 0 || src_x + dx >= SCREEN_W) continue;
            int color = bank[(src_y + dy) * SCREEN_W + src_x + dx];
            if (dx >= live) {
                if (color == 2 || (color & 1) == 0) color = (color == 2 ? 0 : color) ^ 7;
            } else if (color == 2) {
                continue;
            }
            screen[y * SCREEN_W + x] = (uint8_t)color;
        }
    }
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
