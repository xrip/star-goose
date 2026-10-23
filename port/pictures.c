#include "pictures.h"
#include "goose_model.h"
#include "tunnel.h"
#include "atlas.h"
#include "raylib.h"
#if defined(__SWITCH__)
#include "rlgl.h"
#include <switch.h>
#endif
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
    PRESENT_SCALE = 3,
    VGA_COLORS = 256,
    LEGACY_COLORS = 16,
    TUNNEL_SHADES = 128
};

static const Color EGA[16] = {
    {0, 0, 0, 255},       {0, 0, 170, 255},     {0, 170, 0, 255},     {0, 170, 170, 255},
    {170, 0, 0, 255},     {170, 0, 170, 255},   {170, 85, 0, 255},    {170, 170, 170, 255},
    {85, 85, 85, 255},    {85, 85, 255, 255},   {85, 255, 85, 255},   {85, 255, 255, 255},
    {255, 85, 85, 255},   {255, 85, 255, 255},  {255, 255, 85, 255},  {255, 255, 255, 255}
};

/* Index 255 is a real color. VGA transparency lives in vga_atlas_opaque. */
static Color vga_palette[VGA_COLORS];
static uint8_t vga_dim[VGA_COLORS];
static uint8_t vga_fill[LEGACY_COLORS];
static uint8_t vga_tunnel_line[LEGACY_COLORS][TUNNEL_SHADES];
static uint8_t vga_tunnel_glow[LEGACY_COLORS][TUNNEL_SHADES];
static uint8_t vga_tunnel_wall[LEGACY_COLORS][TUNNEL_SHADES];
static uint8_t vga_tunnel_depth[VIEW_W * VIEW_H];

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

typedef struct GraphicsBank {
    uint8_t pixels[SCREEN_W * SCREEN_H];
    uint8_t coverage[SCREEN_W * SCREEN_H];
} GraphicsBank;

static uint8_t *vga_atlas_pixels;
static uint8_t *vga_atlas_opaque;
static uint8_t vga_intro_pixels[SCREEN_W * SCREEN_H];
static GraphicsBank vga_bank_bird1;
static GraphicsBank vga_bank_bird2;
static GraphicsBank vga_bank_blox;
static int use_vga;

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

static int vga_palette_index(unsigned char r, unsigned char g, unsigned char b) {
    for (int i = 0; i < VGA_COLORS; i++) {
        if (vga_palette[i].r == r && vga_palette[i].g == g && vga_palette[i].b == b) return i;
    }
    return -1;
}

static uint8_t vga_nearest(int r, int g, int b) {
    unsigned best_distance = ~0u;
    uint8_t best = 0;
    for (int i = 0; i < VGA_COLORS; i++) {
        int dr = (int)vga_palette[i].r - r;
        int dg = (int)vga_palette[i].g - g;
        int db = (int)vga_palette[i].b - b;
        unsigned distance = (unsigned)(dr * dr * 2 + dg * dg * 3 + db * db);
        if (distance < best_distance) {
            best_distance = distance;
            best = (uint8_t)i;
        }
    }
    return best;
}

static void build_vga_shades(void) {
    for (int i = 0; i < VGA_COLORS; i++) {
        vga_dim[i] = vga_nearest(vga_palette[i].r / 5 + 7,
                                 vga_palette[i].g / 5 + 10,
                                 vga_palette[i].b / 5 + 16);
    }
    for (int i = 0; i < LEGACY_COLORS; i++) {
        vga_fill[i] = (uint8_t)i;
        for (int depth = 0; depth < TUNNEL_SHADES; depth++) {
            int light = 96 + depth;
            int wall = 8 + depth / 8;
            vga_tunnel_line[i][depth] = vga_nearest(
                vga_palette[i].r * light / 255 + 28,
                vga_palette[i].g * light / 255 + 28,
                vga_palette[i].b * light / 255 + 28);
            vga_tunnel_glow[i][depth] = vga_nearest(
                vga_palette[i].r * light / 768 + 4,
                vga_palette[i].g * light / 768 + 6,
                vga_palette[i].b * light / 768 + 10);
            vga_tunnel_wall[i][depth] = vga_nearest(
                vga_palette[i].r * wall / 255 + 2,
                vga_palette[i].g * wall / 255 + 3,
                vga_palette[i].b * wall / 255 + 6);
        }
    }
    /* Indices 2 and 5 fill dialogs. They are not the transparent key. */
    vga_fill[2] = vga_nearest(13, 20, 35);
    vga_fill[5] = vga_nearest(76, 88, 112);
    for (int y = 0; y < VIEW_H; y++) {
        for (int x = 0; x < VIEW_W; x++) {
            int dx = abs(x - TUNNEL_CENTER_X);
            int dy = abs(y + TUNNEL_CAMERA - TUNNEL_CENTER_Y) * 6 / 5;
            int depth = dx > dy ? dx + dy / 2 : dy + dx / 2;
            if (depth >= TUNNEL_SHADES) depth = TUNNEL_SHADES - 1;
            vga_tunnel_depth[y * VIEW_W + x] = (uint8_t)depth;
        }
    }
}

static Image load_rgba(const char *relative) {
    const char *path = find_file(relative);
    Image image = {0};
    if (path) image = LoadImage(path);
    if (!IsImageValid(image)) {
        fprintf(stderr, "VGA256: cannot load %s\n", relative);
        return (Image){0};
    }
    ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    return image;
}

static int load_vga_palette(void) {
    const char *relative = "assets/vga/palette.png";
    Image image = load_rgba(relative);
    if (!IsImageValid(image)) return 0;
    if (image.width != VGA_COLORS || image.height != 1) {
        fprintf(stderr, "VGA256: %s must be 256x1\n", relative);
        UnloadImage(image);
        return 0;
    }
    const unsigned char *pixels = image.data;
    for (int i = 0; i < VGA_COLORS; i++) {
        if (pixels[i * 4 + 3] != 255) {
            fprintf(stderr, "VGA256: palette entry %d is not opaque\n", i);
            UnloadImage(image);
            return 0;
        }
        vga_palette[i] = (Color){pixels[i * 4], pixels[i * 4 + 1], pixels[i * 4 + 2], 255};
    }
    UnloadImage(image);
    if (vga_palette[0].r || vga_palette[0].g || vga_palette[0].b) {
        fprintf(stderr, "VGA256: palette entry 0 must be black\n");
        return 0;
    }
    build_vga_shades();
    return 1;
}

static int index_vga_image(Image image, uint8_t *indices, uint8_t *opaque, const char *relative) {
    const unsigned char *pixels = image.data;
    for (int i = 0; i < image.width * image.height; i++) {
        int visible = pixels[i * 4 + 3] >= 128;
        if (opaque) opaque[i] = (uint8_t)visible;
        if (!visible) {
            indices[i] = 0;
            continue;
        }
        int color = vga_palette_index(pixels[i * 4], pixels[i * 4 + 1], pixels[i * 4 + 2]);
        if (color < 0) {
            fprintf(stderr, "VGA256: %s pixel (%d,%d) RGB %u,%u,%u is outside the shared palette\n",
                    relative, i % image.width, i / image.width,
                    pixels[i * 4], pixels[i * 4 + 1], pixels[i * 4 + 2]);
            return 0;
        }
        indices[i] = (uint8_t)color;
    }
    return 1;
}

static int load_vga_screen(const char *relative, uint8_t *indices) {
    Image image = load_rgba(relative);
    if (!IsImageValid(image)) return 0;
    int ok = image.width == SCREEN_W && image.height == SCREEN_H;
    if (!ok) fprintf(stderr, "VGA256: %s must be 320x200\n", relative);
    if (ok) {
        const unsigned char *pixels = image.data;
        for (int i = 0; i < SCREEN_W * SCREEN_H; i++) {
            if (pixels[i * 4 + 3] != 255) {
                fprintf(stderr, "VGA256: %s must be opaque; bank coverage comes from a separate _MASK.png\n", relative);
                ok = 0;
                break;
            }
        }
    }
    if (ok) ok = index_vga_image(image, indices, 0, relative);
    UnloadImage(image);
    return ok;
}

static unsigned vga_coverage_bit(int key) {
    if (key == 0) return 1;
    if (key == 2) return 2;
    if (key == 13) return 4;
    return 0;
}

static int decode_vga_coverage(Image image, uint8_t *coverage, const char *relative) {
    const unsigned char *pixels = image.data;
    for (int i = 0; i < image.width * image.height; i++) {
        unsigned bits = 0;
        for (int channel = 0; channel < 3; channel++) {
            unsigned value = pixels[i * 4 + channel];
            if (value != 0 && value != 255) {
                fprintf(stderr, "VGA256: %s mask pixel (%d,%d) channel %d must be 0 or 255\n",
                        relative, i % image.width, i / image.width, channel);
                return 0;
            }
            if (value) bits |= 1u << channel;
        }
        if (pixels[i * 4 + 3] != 255) {
            fprintf(stderr, "VGA256: %s mask PNG must be opaque\n", relative);
            return 0;
        }
        coverage[i] = (uint8_t)bits;
    }
    return 1;
}

static int load_vga_bank(const char *name, GraphicsBank *bank) {
    char relative[128];
    snprintf(relative, sizeof relative, "assets/vga/%s.png", name);
    if (!load_vga_screen(relative, bank->pixels)) return 0;
    snprintf(relative, sizeof relative, "assets/vga/%s_MASK.png", name);
    Image image = load_rgba(relative);
    if (!IsImageValid(image)) return 0;
    int ok = image.width == SCREEN_W && image.height == SCREEN_H;
    if (!ok) fprintf(stderr, "VGA256: %s must be 320x200\n", relative);
    if (ok) ok = decode_vga_coverage(image, bank->coverage, relative);
    UnloadImage(image);
    return ok;
}

static int load_vga_atlas(void) {
    const char *relative = "assets/vga/atlas/goose.png";
    Image image = load_rgba(relative);
    if (!IsImageValid(image)) return 0;
    size_t size = (size_t)atlas_w * (size_t)atlas_h;
    int ok = image.width == atlas_w && image.height == atlas_h;
    if (!ok) fprintf(stderr, "VGA256: %s must match the EGA atlas\n", relative);
    if (ok) {
        vga_atlas_pixels = malloc(size);
        vga_atlas_opaque = malloc(size);
        ok = vga_atlas_pixels && vga_atlas_opaque;
        if (!ok) fprintf(stderr, "VGA256: cannot allocate atlas\n");
    }
    if (ok) ok = index_vga_image(image, vga_atlas_pixels, vga_atlas_opaque, relative);
    UnloadImage(image);
    return ok;
}

static int load_vga_pack(void) {
    if (!load_vga_palette()) return 0;
    if (!load_vga_atlas()) return 0;
    if (!load_vga_screen("assets/vga/INTRO.png", vga_intro_pixels)) return 0;
    if (!load_vga_bank("BIRD1", &vga_bank_bird1)) return 0;
    if (!load_vga_bank("BIRD2", &vga_bank_bird2)) return 0;
    if (!load_vga_bank("BLOX", &vga_bank_blox)) return 0;
    use_vga = 1;
    return 1;
}

static void release_vga_pack(void) {
    free(vga_atlas_pixels);
    free(vga_atlas_opaque);
    vga_atlas_pixels = 0;
    vga_atlas_opaque = 0;
    use_vga = 0;
}

static const GraphicsBank *vga_bank(int bank_offset) {
    if (bank_offset == 0) return &vga_bank_bird1;
    if (bank_offset == BANK_BLOX) return &vga_bank_blox;
    return &vga_bank_bird2;
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
    if (!load_vga_pack()) {
        fprintf(stderr, "VGA256: pack not used; original EGA graphics stay on\n");
        release_vga_pack();
    }
    ready = 1;
    return 1;
}

void pictures_toggle_vga(void) {
    if (!vga_atlas_pixels) return;
    use_vga = !use_vga;
}

static void blit_sprite(const AtlasSprite *sprite, int src_dx, int width, int dest_x, int dest_y) {
    const uint8_t *pixels = use_vga ? vga_atlas_pixels : atlas_pixels;
    const uint8_t *opaque = use_vga ? vga_atlas_opaque : 0;
    if (!sprite || !pixels || width <= 0 || sprite->h == 0) return;
    for (int dy = 0; dy < sprite->h; dy++) {
        int sy = sprite->y + dy;
        int y = dest_y + dy;
        if (sy < 0 || sy >= atlas_h || y < VIEW_Y || y >= VIEW_Y + VIEW_H) continue;
        for (int dx = 0; dx < width; dx++) {
            int sx = sprite->x + src_dx + dx;
            int x = dest_x + dx;
            if (sx < 0 || sx >= atlas_w || x < VIEW_X || x >= VIEW_X + VIEW_W) continue;
            int index = sy * atlas_w + sx;
            if (opaque) {
                if (!opaque[index]) continue;
            } else if (pixels[index] == CLEAR) {
                continue;
            }
            screen[y * SCREEN_W + x] = pixels[index];
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
    if (!use_vga && !bank_ready) return;
    int dest_x, dest_y;
    sprite_dest(world_x, world_y, z, sprite->anchor_x, sprite->anchor_y, camera_y, &dest_x, &dest_y);
    int src_y = (int)(offset / PACKED_STRIDE);
    int src_x = (int)(offset % PACKED_STRIDE) * 2;
    const GraphicsBank *art = vga_bank(bank_offset);
    unsigned coverage = vga_coverage_bit(key);
    if (use_vga && !coverage) return;
    for (int dy = 0; dy < sprite->h; dy++) {
        int y = dest_y + dy;
        if (y < VIEW_Y || y >= VIEW_Y + VIEW_H || src_y + dy < 0 || src_y + dy >= SCREEN_H) continue;
        for (int dx = 0; dx < sprite->w; dx++) {
            int x = dest_x + dx;
            if (x < VIEW_X || x >= VIEW_X + VIEW_W || src_x + dx < 0 || src_x + dx >= SCREEN_W) continue;
            int src = (src_y + dy) * SCREEN_W + src_x + dx;
            if (use_vga) {
                if (!(art->coverage[src] & coverage)) continue;
                screen[y * SCREEN_W + x] = art->pixels[src];
                continue;
            }
            int color = bank[src];
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
    if (use_vga) memcpy(screen, vga_intro_pixels, sizeof screen);
    else if (intro_ready) memcpy(screen, intro_pixels, sizeof screen);
}

void pictures_tunnel(const uint8_t *bits, int color) {
    if (color < 0 || color > 15) color = 3;
    if (use_vga) {
        for (int row = 0; row < VIEW_H; row++) {
            const uint8_t *src = bits + (row + TUNNEL_CAMERA) * TUNNEL_STRIDE;
            for (int x = 0; x < VIEW_W; x++) {
                int depth = vga_tunnel_depth[row * VIEW_W + x];
                unsigned mask = BIT7 >> (x & 7);
                uint8_t pixel = vga_tunnel_wall[color][depth];
                if (src[x >> 3] & mask) {
                    pixel = vga_tunnel_line[color][depth];
                } else {
                    int glow = ((src[x / 8 - TUNNEL_STRIDE] | src[x / 8 + TUNNEL_STRIDE]) & mask) != 0;
                    if (x > 0) glow |= (src[(x - 1) >> 3] & (BIT7 >> ((x - 1) & 7))) != 0;
                    if (x + 1 < VIEW_W) glow |= (src[(x + 1) >> 3] & (BIT7 >> ((x + 1) & 7))) != 0;
                    if (glow) pixel = vga_tunnel_glow[color][depth];
                }
                screen[(VIEW_Y + row) * SCREEN_W + VIEW_X + x] = pixel;
            }
        }
        return;
    }
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
    for (int i = 0; i < SCREEN_W * SCREEN_H; i++) {
        upload[i] = use_vga ? vga_palette[screen[i]] : EGA[screen[i]];
    }
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
    const GraphicsBank *art = vga_bank(bank_offset);
    if (!use_vga && !bank_ready) return;
    if (width <= 0 || height <= 0) return;
    int src_y = (int)(offset / PACKED_STRIDE);
    int src_x = (int)(offset % PACKED_STRIDE) * 2;
    int live = live_columns < 0 ? width : live_columns;
    unsigned coverage = vga_coverage_bit(KEY_COLOR);
    for (int dy = 0; dy < height; dy++) {
        int y = dest_y + dy;
        if (y < 0 || y >= SCREEN_H || src_y + dy < 0 || src_y + dy >= SCREEN_H) continue;
        for (int dx = 0; dx < width; dx++) {
            int x = dest_x + dx;
            if (x < 0 || x >= SCREEN_W || src_x + dx < 0 || src_x + dx >= SCREEN_W) continue;
            int src = (src_y + dy) * SCREEN_W + src_x + dx;
            if (use_vga) {
                int visible = (art->coverage[src] & coverage) != 0;
                uint8_t color = art->pixels[src];
                if (dx >= live) color = vga_dim[visible ? color : 0];
                else if (!visible) continue;
                screen[y * SCREEN_W + x] = color;
                continue;
            }
            int color = bank[src];
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
    if (use_vga) {
        if (color < 0 || color >= VGA_COLORS) color = 0;
        if (color < LEGACY_COLORS) color = vga_fill[color];
    } else if (color < 0 || color > 15) {
        color = 0;
    }
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

#if defined(__SWITCH__)
/* Switch Lite is 1280x720. 320x200 at 3x is 960x600, drawn at (160, 60):
 * 60 black rows above and below, 160 black columns on each side.
 * libnx already opens the window at this size; the viewport is set to match
 * so the 960x600 picture is measured on the panel, not inside a smaller box. */
enum { PANEL_W = 1280, PANEL_H = 720 };

void pictures_switch_window(void) {
    nwindowSetDimensions(nwindowGetDefault(), PANEL_W, PANEL_H);
}

void pictures_prepare_screen(void) {
    rlViewport(0, 0, PANEL_W, PANEL_H);
    rlMatrixMode(RL_PROJECTION);
    rlLoadIdentity();
    rlOrtho(0, PANEL_W, PANEL_H, 0, 0.0f, 1.0f);
    rlMatrixMode(RL_MODELVIEW);
    rlLoadIdentity();
}
#else
void pictures_switch_window(void) {}
void pictures_prepare_screen(void) {}
#endif

void pictures_present(void) {
    /* Windowed play stays at 3x. A maximized desktop window and the web
     * canvas use the largest whole factor of 320x200 that fits. */
    int scale = PRESENT_SCALE;
    int origin_x = 0;
    int origin_y = 0;
#if defined(__SWITCH__)
    int fit_x = PANEL_W / SCREEN_W;
    int fit_y = PANEL_H / SCREEN_H;
    scale = fit_x < fit_y ? fit_x : fit_y;
    if (scale < 1) scale = 1;
    origin_x = (PANEL_W - SCREEN_W * scale) / 2;
    origin_y = (PANEL_H - SCREEN_H * scale) / 2;
#elif defined(__EMSCRIPTEN__)
    int fit = 1;
#else
    int fit = IsWindowMaximized();
#endif
#if !defined(__SWITCH__)
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
#endif
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
    release_vga_pack();
    if (ready) UnloadTexture(frame);
    ready = 0;
}
