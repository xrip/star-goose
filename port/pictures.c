#include "pictures.h"
#include "goose_model.h"
#include "tunnel.h"
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
    CLEAR = 255,
    MAP_ROWS = 64,
    MAP_COLS = 12,
    TILE_W = 24,
    ROW_H = 16,
    EDGE_W = 8,
    CROP = 16,
    SHIP_LAST = 10,
    BYTE_MASK = 255,
    WORLD_HALF = 512,
    PACKED_STRIDE = 160,
    BANK_BLOX = 4,
    TUNNEL_STRIDE = 32,
    BIT7 = 0x80,
    KEY_COLOR = 2,
    GAUGE_FLIP = 7,
    PRESENT_SCALE = 3
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
static uint8_t bank_bird1[SCREEN_W * SCREEN_H];
static uint8_t bank_bird2[SCREEN_W * SCREEN_H];
static uint8_t bank_blox[SCREEN_W * SCREEN_H];
static int bank_bird1_ready;
static int bank_bird2_ready;
static int bank_blox_ready;

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
    load_bank("assets/BIRD1.X", bank_bird1, &bank_bird1_ready);
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
    int relative = (world_y - camera_y) & WORLD_MASK;
    if (relative >= WORLD_HALF) relative -= WORLD_MASK + 1;
    return VIEW_Y + relative;
}

static void sprite_dest(int world_x, int world_y, int z, int anchor_x, int anchor_y,
                        int camera_y, int *dest_x, int *dest_y) {
    int z_sum = (z + anchor_y) & BYTE_MASK;
    *dest_x = VIEW_X + world_x - anchor_x;
    *dest_y = screen_y(world_y - goose_signed8((unsigned)z_sum), camera_y);
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

int pictures_metrics(const char *name, int *height, int *anchor_y) {
    const AtlasSprite *sprite = find_sprite(name);
    if (!sprite) return 0;
    if (height) *height = sprite->h;
    if (anchor_y) *anchor_y = sprite->anchor_y;
    return 1;
}

void pictures_bank_sprite(int bank, unsigned offset, int width, int height,
                          int anchor_x, int anchor_y,
                          int world_x, int world_y, int z, int camera_y) {
    int dest_x, dest_y;
    sprite_dest(world_x, world_y, z, anchor_x, anchor_y, camera_y, &dest_x, &dest_y);
    pictures_blit_bank(bank, offset, width, height, dest_x, dest_y, width);
}

void pictures_sprite(const char *name, int world_x, int world_y, int z, int camera_y) {
    const AtlasSprite *sprite = find_sprite(name);
    if (!sprite) return;
    int dest_x, dest_y;
    sprite_dest(world_x, world_y, z, sprite->anchor_x, sprite->anchor_y, camera_y, &dest_x, &dest_y);
    blit_sprite(sprite, 0, sprite->w, dest_x, dest_y);
}

void pictures_sprite_bank(const char *name, int bank_offset, unsigned offset, int key,
                          int world_x, int world_y, int z, int camera_y) {
    const AtlasSprite *sprite = find_sprite(name);
    if (!sprite || sprite->w == 0 || sprite->h == 0) return;
    const uint8_t *bank = bank_bird2;
    int bank_ready = bank_bird2_ready;
    if (bank_offset == 0) {
        bank = bank_bird1;
        bank_ready = bank_bird1_ready;
    } else if (bank_offset == BANK_BLOX) {
        bank = bank_blox;
        bank_ready = bank_blox_ready;
    }
    if (!bank_ready) return;
    int dest_x, dest_y;
    sprite_dest(world_x, world_y, z, sprite->anchor_x, sprite->anchor_y, camera_y, &dest_x, &dest_y);
    int src_y = (int)(offset / PACKED_STRIDE);
    int src_x = (int)(offset % PACKED_STRIDE) * 2;
    for (int dy = 0; dy < sprite->h; dy++) {
        int y = dest_y + dy;
        if (y < VIEW_Y || y >= VIEW_Y + VIEW_H || src_y + dy < 0 || src_y + dy >= SCREEN_H) continue;
        for (int dx = 0; dx < sprite->w; dx++) {
            int x = dest_x + dx;
            if (x < VIEW_X || x >= VIEW_X + VIEW_W || src_x + dx < 0 || src_x + dx >= SCREEN_W) continue;
            int color = bank[(src_y + dy) * SCREEN_W + src_x + dx];
            if (color == key) continue;
            screen[y * SCREEN_W + x] = (uint8_t)color;
        }
    }
}

void pictures_score_text(const char *sprite, int world_x, int world_y, int z, int camera_y,
                         const uint8_t *chars, int count, int digits) {
    enum {
        GLYPH_W = 12,
        GLYPH_H = 11,
        GLYPH_STEP = 6,
        GLYPH_SPACE = 0x6546,
        GLYPH_DOT = 0x6f6c,
        GLYPH_MARK = 0x764c,
        GLYPH_DIGIT = 0x44c0,
        BANK_BIRD2 = 2
    };
    const AtlasSprite *frame = find_sprite(sprite);
    if (!frame || !chars || count <= 0) return;
    int dest_x, dest_y;
    sprite_dest(world_x, world_y, z, frame->anchor_x, frame->anchor_y, camera_y, &dest_x, &dest_y);
    int leading = 1;
    for (int i = 0; i < count; i++) {
        unsigned char ch = chars[i];
        unsigned glyph;
        if (digits) {
            if (ch != 0) leading = 0;
            glyph = leading ? GLYPH_DOT : GLYPH_DIGIT + (unsigned)ch * GLYPH_STEP;
        } else if (ch == ' ') {
            glyph = GLYPH_SPACE;
        } else if (ch == '.') {
            glyph = GLYPH_DOT;
        } else if (ch == '_') {
            glyph = GLYPH_MARK;
        } else {
            glyph = (unsigned)(uint8_t)(ch - 'A') * GLYPH_STEP;
        }
        pictures_blit_bank(BANK_BIRD2, glyph, GLYPH_W, GLYPH_H,
                           dest_x + i * GLYPH_W, dest_y, GLYPH_W);
    }
}

void pictures_intro(void) {
    if (intro_ready) memcpy(screen, intro_pixels, sizeof screen);
}

void pictures_tunnel(const uint8_t *bits, int color) {
    if (color < 0 || color > 15) color = 3;
    for (int row = 0; row < VIEW_H; row++) {
        const uint8_t *src = bits + (row + TUNNEL_CAMERA) * TUNNEL_STRIDE;
        for (int x = 0; x < VIEW_W; x++) {
            if (src[x >> 3] & (uint8_t)(BIT7 >> (x & 7))) {
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
    for (int stored = 0; stored < MAP_ROWS; stored++) {
        unsigned running = 0;
        int world_row = MAP_ROWS - 1 - stored;
        for (int column = 0; column < MAP_COLS; column++) {
            unsigned z = running;
            int tile = tiles[stored * MAP_COLS + column];
            if (tile >= TILES) tile = 0;
            running = (running + goose_height_delta[tile]) & BYTE_MASK;
            const AtlasSprite *sprite = &atlas_tiles[tile];
            int src_dx = 0;
            int width = sprite->w;
            int dest_x = VIEW_X + EDGE_W + (column - 1) * TILE_W;
            if (column == 0) {
                src_dx = CROP;
                width = EDGE_W;
                dest_x = VIEW_X;
            } else if (column == MAP_COLS - 1) {
                width = EDGE_W;
                dest_x = VIEW_X + VIEW_W - EDGE_W;
            }
            int world_y = world_row * ROW_H + TILE_Y_ADJUST[tile] - goose_signed8(z);
            blit_sprite(sprite, src_dx, width, dest_x, screen_y(world_y, camera));
        }
    }
}

void pictures_draw_ship(const Surface *game) {
    int frame_index = game->frame;
    if (frame_index < 0 || frame_index > SHIP_LAST) return;
    const AtlasSprite *sprite = &atlas_ship[frame_index];
    int world_y = (int)((game->camera_y + PLAYER_AHEAD) & WORLD_MASK);
    int dest_x, dest_y;
    sprite_dest(game->x, world_y, game->z, sprite->anchor_x, sprite->anchor_y,
                (int)game->camera_y, &dest_x, &dest_y);
    blit_sprite(sprite, 0, sprite->w, dest_x, dest_y);
}

void pictures_blit_bank(int bank_offset, unsigned offset, int width, int height,
                        int dest_x, int dest_y, int live_columns) {
    const uint8_t *bank = bank_bird2;
    int bank_ready = bank_bird2_ready;
    if (bank_offset == 0) {
        bank = bank_bird1;
        bank_ready = bank_bird1_ready;
    } else if (bank_offset == BANK_BLOX) {
        bank = bank_blox;
        bank_ready = bank_blox_ready;
    }
    if (!bank_ready || width <= 0 || height <= 0) return;
    int src_y = (int)(offset / PACKED_STRIDE);
    int src_x = (int)(offset % PACKED_STRIDE) * 2;
    int live = live_columns < 0 ? width : live_columns;
    for (int dy = 0; dy < height; dy++) {
        int y = dest_y + dy;
        if (y < 0 || y >= SCREEN_H || src_y + dy < 0 || src_y + dy >= SCREEN_H) continue;
        for (int dx = 0; dx < width; dx++) {
            int x = dest_x + dx;
            if (x < 0 || x >= SCREEN_W || src_x + dx < 0 || src_x + dx >= SCREEN_W) continue;
            int color = bank[(src_y + dy) * SCREEN_W + src_x + dx];
            if (dx >= live) {
                if (color == KEY_COLOR || (color & 1) == 0) color = (color == KEY_COLOR ? 0 : color) ^ GAUGE_FLIP;
            } else if (color == KEY_COLOR) {
                continue;
            }
            screen[y * SCREEN_W + x] = (uint8_t)color;
        }
    }
}

void pictures_fill(int x, int y, int width, int height, int color) {
    if (color < 0 || color > 15) color = 0;
    for (int dy = 0; dy < height; dy++) {
        int row = y + dy;
        if (row < 0 || row >= SCREEN_H) continue;
        for (int dx = 0; dx < width; dx++) {
            int col = x + dx;
            if (col < 0 || col >= SCREEN_W) continue;
            screen[row * SCREEN_W + col] = (uint8_t)color;
        }
    }
}

void pictures_present(void) {
    /* Windowed play stays at 3x. A maximized desktop window, and the web
     * canvas, use the largest whole factor of 320x200 that fits. */
    int scale = PRESENT_SCALE;
    int origin_x = 0;
    int origin_y = 0;
#if defined(__EMSCRIPTEN__)
    int fit = 1;
#else
    int fit = IsWindowMaximized();
#endif
    if (fit) {
        int width = GetScreenWidth();
        int height = GetScreenHeight();
        int fit_x = width / SCREEN_W;
        int fit_y = height / SCREEN_H;
        scale = fit_x < fit_y ? fit_x : fit_y;
        if (scale < 1) scale = 1;
        origin_x = (width - SCREEN_W * scale) / 2;
        origin_y = (height - SCREEN_H * scale) / 2;
    }
    DrawTextureEx(frame, (Vector2){(float)origin_x, (float)origin_y}, 0, (float)scale, WHITE);
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
