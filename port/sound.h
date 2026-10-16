#ifndef GOOSE_SOUND_H
#define GOOSE_SOUND_H

enum {
    SOUND_SIDE_LAUNCH = 1,
    SOUND_SIDE_RELEASE = 2,
    SOUND_HULL_HIT = 3,
    SOUND_SHIELD_TAP = 4,
    SOUND_HEAVY_HIT = 5,
    SOUND_IMPACT = 6,
    SOUND_KILL = 7,
    SOUND_CHARGE = 8,
    SOUND_TUNNEL_PICKUP = 9,
    SOUND_COLLECT = 10,
    SOUND_TUNNEL = 11,
    SOUND_DEATH = 12,
    SOUND_NOTE = 15
};

void sound_init(void);
void sound_reset(void);
void sound_music_start(int name_pending);
void sound_shutdown(void);
void sound_context(int charges, int angle);
void sound_trigger(unsigned id);
void sound_pump(void);

#endif
