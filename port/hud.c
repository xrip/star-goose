#include "hud.h"
#include "pictures.h"

/* Video byte offset to the pixel at the left of that EGA byte. Stride is 40. */
static void ega_origin(unsigned offset, int *x, int *y) {
    *y = (int)(offset / 40u);
    *x = (int)(offset % 40u) * 8;
}

static void blit(int bank, unsigned sprite, unsigned dest, int width, int height, int live) {
    int x, y;
    ega_origin(dest, &x, &y);
    pictures_blit_bank(bank, sprite, width, height, x, y, live);
}

void hud_draw(int fuel, int ammo, int shield, int lives, int charges,
              const uint8_t collected[6], const uint8_t digits[6]) {
    /* CS:21DC chrome, CS:250B/2553/259B gauges. Width is the full cell count. */
    blit(2, 0x403c, 0x1e11, 56, 7, -1);
    if (fuel < 0) fuel = 0;
    if (ammo < 0) ammo = 0;
    if (shield < 0) shield = 0;
    blit(2, 0x48f6, 0x1b88, 44, 7, fuel);
    blit(2, 0x2ce6, 0x1cc8, 48, 7, ammo);
    blit(2, 0x4496, 0x1e08, 56, 7, shield);

    /* CS:22E1 lives, three icons, 24 pixels apart. */
    static const unsigned life_at[3] = {0x1b90, 0x1b93, 0x1b96};
    if (lives > 3) lives = 3;
    for (int i = 0; i < lives; i++) blit(2, 0x43e6, life_at[i], 16, 15, -1);

    /* CS:23D4 six side-device charges. */
    static const unsigned charge_at[6] = {0x1b99, 0x1b9b, 0x1b9d, 0x1b9f, 0x1ba1, 0x1ba3};
    if (charges > 6) charges = 6;
    for (int i = 0; i < charges; i++) blit(2, 0x43ee, charge_at[i], 16, 15, -1);

    /* CS:2A7F empty collectible marks. CS:2B59 replaces a taken one with 0x2F78. */
    static const unsigned collect_sprite[6] = {0x1178, 0x1678, 0x1b78, 0x2078, 0x2578, 0x2a78};
    static const unsigned collect_at[6] = {0x1b82, 0x1cc2, 0x1e02, 0x1b85, 0x1cc5, 0x1e05};
    for (int i = 0; i < 6; i++) {
        unsigned sprite = collected && collected[i] ? 0x2f78u : collect_sprite[i];
        blit(4, sprite, collect_at[i], 16, 8, -1);
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
        if (digit < 10 && place < 6) blit(2, digit_sprite[digit], digit_at[place], 16, 9, -1);
        place++;
    }
}
