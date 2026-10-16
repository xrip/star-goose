#include "hud.h"
#include "pictures.h"

enum {
    EGA_STRIDE = 40,
    BYTE_PX = 8,
    BANK_SHIP = 2,
    BANK_TILES = 4,
    CHROME = 0x403c,
    CHROME_AT = 0x1e11,
    FUEL_CELLS = 0x48f6,
    FUEL_AT = 0x1b88,
    AMMO_CELLS = 0x2ce6,
    AMMO_AT = 0x1cc8,
    SHIELD_CELLS = 0x4496,
    SHIELD_AT = 0x1e08,
    LIFE_ICON = 0x43e6,
    CHARGE_ICON = 0x43ee,
    TAKEN_MARK = 0x2f78,
    GAUGE_H = 7,
    ICON_W = 16,
    LIFE_H = 15,
    MARK_H = 8,
    DIGIT_H = 9
};

static void ega_origin(unsigned offset, int *x, int *y) {
    *y = (int)(offset / EGA_STRIDE);
    *x = (int)(offset % EGA_STRIDE) * BYTE_PX;
}

static void blit(int bank, unsigned sprite, unsigned dest, int width, int height, int live) {
    int x, y;
    ega_origin(dest, &x, &y);
    pictures_blit_bank(bank, sprite, width, height, x, y, live);
}

void hud_draw(int fuel, int ammo, int shield, int lives, int charges,
              const uint8_t collected[6], const uint8_t digits[6]) {
    /* CS:21DC chrome, CS:250B/2553/259B gauges. Width is the full cell count. */
    blit(BANK_SHIP, CHROME, CHROME_AT, SHIELD_FULL, GAUGE_H, -1);
    if (fuel < 0) fuel = 0;
    if (ammo < 0) ammo = 0;
    if (shield < 0) shield = 0;
    blit(BANK_SHIP, FUEL_CELLS, FUEL_AT, FUEL_FULL, GAUGE_H, fuel);
    blit(BANK_SHIP, AMMO_CELLS, AMMO_AT, AMMO_FULL, GAUGE_H, ammo);
    blit(BANK_SHIP, SHIELD_CELLS, SHIELD_AT, SHIELD_FULL, GAUGE_H, shield);

    /* CS:22E1 lives, three icons, 24 pixels apart. */
    static const unsigned life_at[3] = {0x1b90, 0x1b93, 0x1b96};
    if (lives > LIVES_FULL) lives = LIVES_FULL;
    for (int i = 0; i < lives; i++) blit(BANK_SHIP, LIFE_ICON, life_at[i], ICON_W, LIFE_H, -1);

    /* CS:23D4 six side-device charges. */
    static const unsigned charge_at[6] = {0x1b99, 0x1b9b, 0x1b9d, 0x1b9f, 0x1ba1, 0x1ba3};
    if (charges > SIDE_CHARGES_FULL) charges = SIDE_CHARGES_FULL;
    for (int i = 0; i < charges; i++) blit(BANK_SHIP, CHARGE_ICON, charge_at[i], ICON_W, LIFE_H, -1);

    /* CS:2A7F empty collectible marks. CS:2B59 replaces a taken one with 0x2F78. */
    static const unsigned collect_sprite[6] = {0x1178, 0x1678, 0x1b78, 0x2078, 0x2578, 0x2a78};
    static const unsigned collect_at[6] = {0x1b82, 0x1cc2, 0x1e02, 0x1b85, 0x1cc5, 0x1e05};
    for (int i = 0; i < 6; i++) {
        unsigned sprite = collected && collected[i] ? TAKEN_MARK : collect_sprite[i];
        blit(BANK_TILES, sprite, collect_at[i], ICON_W, MARK_H, -1);
    }

    /* CS:155F digit sprites, CS:1573 destinations, CS:2714 skips leading zeros. */
    static const unsigned digit_sprite[10] = {
        0x44fc, 0x4a9c, 0x3a70, 0x3a78, 0x314a, 0x3152, 0x36ea, 0x36f2, 0x44b2, 0x3c98
    };
    static const unsigned digit_at[6] = {0x1e19, 0x1e1b, 0x1e1d, 0x1e1f, 0x1e21, 0x1e23};
    int place = 0;
    int leading = 1;
    for (int i = 0; i < 6; i++) {
        unsigned digit = digits ? digits[i] : 0;
        if (leading && digit == 0) continue;
        leading = 0;
        if (digit < 10 && place < 6) blit(BANK_SHIP, digit_sprite[digit], digit_at[place], ICON_W, DIGIT_H, -1);
        place++;
    }
}

static void fill_ega(unsigned dest, int byte_width, int rows, int color) {
    int x, y;
    ega_origin(dest, &x, &y);
    pictures_fill(x, y, byte_width * BYTE_PX, rows, color);
}

static const unsigned PROMPT_BANK[8] = {4, 4, 4, 4, 2, 2, 4, 4};
static const unsigned PROMPT_SRC[8] = {
    0x1338, 0x3dc8, 0x35fc, 0x4718, 0x73a0, 0x7592, 0x4e98, 0x4e98
};
static const unsigned PROMPT_AT[8] = {
    0x0dcd, 0x0dcd, 0x0dcd, 0x0dcd, 0x0dcd, 0x0dcd, 0x0dd3, 0x0dd4
};
static const int PROMPT_W[8] = {40, 48, 60, 64, 36, 40, 60, 60};
static const int PROMPT_H[8] = {12, 15, 12, 12, 12, 12, 12, 12};

static void prompt_piece(int index) {
    if (index < 0 || index > 7) return;
    blit((int)PROMPT_BANK[index], PROMPT_SRC[index], PROMPT_AT[index], PROMPT_W[index], PROMPT_H[index], -1);
}

void hud_quit_dialog(void) {
    fill_ega(0x0aac, 0x10, 0x28, 5);
    fill_ega(0x0b4d, 0x0e, 0x20, 2);
    blit(BANK_SHIP, 0x7592, 0x0c8f, 0x28, 0x0c, -1);
    blit(BANK_TILES, 0x1ab8, 0x0ce4, 0x20, 0x0d, -1);
}

void hud_redefine_dialog(void) {
    fill_ega(0x0a0b, 0x13, 0x30, 5);
    fill_ega(0x0aac, 0x11, 0x28, 2);
    blit(BANK_TILES, 0x5a5c, 0x0b4d, 0x78, 0x0f, -1);
}

void hud_control_prompt(int index) {
    fill_ega(0x0dcc, 0x0f, 0x11, 2);
    if (index == 6) {
        prompt_piece(0);
        prompt_piece(6);
    } else if (index == 7) {
        prompt_piece(1);
        prompt_piece(7);
    } else {
        prompt_piece(index);
    }
}
