#include "sound.h"
#include "raylib.h"
#include <stdint.h>
#include <string.h>

enum {
    SAMPLE_RATE = 44100,
    PIT_CLOCK = 1193182,
    TIMER_DIVISOR = 19726,
    STREAM_SAMPLES = 4096,
    CHARGE_PITCH_BASE = 8000,
    CHARGE_PITCH_STEP = 1024,
    ANGLE_MASK = 127,
    ANGLE_HALF = 63,
    ANGLE_FLIP = 64,
    ANGLE_PITCH_BASE = 400,
    MUSIC_END = 0xff,
    MUSIC_REST = 0xfe,
    NOTE_BIAS = 12,
    TITLE_SONG = 160,
    MUSIC_HOLD = 14,
    MUSIC_GAP = 7
};

typedef struct SoundDesc {
    int8_t modulation;
    uint8_t duration;
    uint16_t divisor;
    int16_t step;
} SoundDesc;

/* DS:8C02. Descriptor 8 and 9 divisors are replaced when the effect starts. */
static const SoundDesc DESCRIPTORS[15] = {
    {0, 3, 14000, -500}, {0, 24, 14000, -500}, {0, 4, 5000, 1000}, {0, 4, 6000, 1000},
    {0, 6, 800, 400}, {2, 12, 4000, -200}, {3, 12, 3000, -200}, {0, 2, 8000, 0},
    {0, 3, 600, 300}, {1, 16, 6500, 0}, {2, 10, 2000, 0}, {0, 50, 500, 300},
    {3, 10, 4000, -300}, {3, 10, 1000, 300}, {0, 5, 2000, 0}
};

static const int16_t MODULATION[4][16] = {
    {-1000, -2000, -3000, -4000, -5000, -5500, -6000, -6200, -1000, -2000, -3000, -4000, -5000, -5500, -6000, -6200},
    {0, 1000, 0, -1000, 0, 1000, 0, -1000, 0, 1000, 0, -1000, 0, 1000, 0, -1000},
    {0, 2000, 0, 2000, 0, 2000, 0, 2000, 0, 2000, 0, 2000, 0, 2000, 0, 2000},
    {0, 20, 30, 20, 0, -20, -30, -20, 0, 20, 30, 20, 0, -20, -30, -20}
};

static const uint16_t NOTE_DIVISORS[52] = {
    0x4740, 0x4340, 0x3f7c, 0x3bec, 0x3890, 0x3564, 0x3264, 0x2f90,
    0x2ce4, 0x2a60, 0x2800, 0x25c0, 0x23a0, 0x21a0, 0x1fbe, 0x1df6,
    0x1c48, 0x1ab2, 0x1932, 0x17c8, 0x1672, 0x1530, 0x1400, 0x12e0,
    0x11d0, 0x10d0, 0x0fdf, 0x0efb, 0x0e24, 0x0d59, 0x0c99, 0x0be4,
    0x0b39, 0x0a98, 0x0a00, 0x0970, 0x08e8, 0x0868, 0x07ef, 0x077d,
    0x0712, 0x06ac, 0x064c, 0x05f2, 0x059c, 0x054c, 0x0500, 0x04b8,
    0x0474, 0x0434, 0x03f7, 0x03be
};

/* DS:8D4C and DS:8DEC. FF ends a sequence. */
static const uint8_t MUSIC[273] = {
    0x0a, 0x0d, 0x0f, 0x10, 0x11, 0x14, 0x16, 0x00, 0x19, 0x00, 0x16, 0x19, 0x00, 0x16, 0x19, 0x1b,
    0x1c, 0x19, 0x1b, 0xfe, 0x19, 0xfe, 0x16, 0x00, 0x17, 0xfe, 0x19, 0x16, 0xfe, 0xfe, 0xfe, 0xfe,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x14, 0x16,
    0x17, 0xfe, 0xfe, 0x16, 0xfe, 0xfe, 0x14, 0xfe, 0xfe, 0x10, 0xfe, 0xfe, 0x11, 0xfe, 0xfe, 0xfe,
    0x1b, 0x00, 0x00, 0x19, 0x00, 0x00, 0x17, 0x00, 0x00, 0x13, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00,
    0x0a, 0x0d, 0x0f, 0x10, 0x11, 0x14, 0x16, 0xfe, 0x19, 0xfe, 0x16, 0x19, 0xfe, 0x16, 0x19, 0x1b,
    0x1c, 0x19, 0x1b, 0x00, 0x19, 0x00, 0x16, 0x00, 0x1b, 0x00, 0x16, 0x19, 0x00, 0x00, 0x00, 0x00,
    0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe,
    0x0f, 0x0f, 0x08, 0x08, 0x0d, 0x0d, 0x08, 0x08, 0x12, 0x12, 0x08, 0x08, 0x11, 0x11, 0x08, 0x08,
    0x16, 0x00, 0x00, 0x14, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08,
    0x0f, 0x0f, 0x08, 0x08, 0x0d, 0x0d, 0x08, 0x08, 0x12, 0x00, 0x11, 0x00, 0x0d, 0x00, 0x0f, 0x08,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08,
    0x0f, 0x0f, 0x08, 0x08, 0x0d, 0x0d, 0x08, 0x08, 0x12, 0x12, 0x08, 0x08, 0x11, 0x11, 0x08, 0x08,
    0x16, 0x00, 0x00, 0x14, 0x00, 0x00, 0x0d, 0x00, 0x12, 0x00, 0x00, 0x11, 0x00, 0x00, 0x0d, 0x00,
    0x0f, 0x0f, 0x08, 0x08, 0x0d, 0x0d, 0x08, 0x08, 0x12, 0x00, 0x11, 0x00, 0x0d, 0x00, 0x0f, 0x0d,
    0xff
};

static const uint8_t FILL_NOTES[32] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0x16, 0x19, 0x0d, 0x14, 0x17, 0x0d, 0x12, 0x16, 0x0d, 0x11, 0x14, 0x0d, 0x12, 0x16, 0x0d, 0x11
};

static uint8_t active_id, pending_id, priority_id, enabled, ticks_left;
static uint8_t modulation_id, modulation_phase;
static uint16_t divisor, output_divisor;
static int16_t divisor_step;
static int speaker_on;
static int charges, angle;
static int music_active;
static uint8_t music_divider;
static unsigned music_cursor;
static unsigned music_origin;
static int8_t music_modulation;
static uint8_t music_duration;
static uint16_t music_divisor;
static AudioStream stream;
static int audio_ready;
static uint64_t timer_acc, wave_acc;
static int wave_high;

static void speaker_off(void) {
    speaker_on = 0;
    output_divisor = 0;
    wave_acc = 0;
}

static void sound_stop(void) {
    active_id = 0;
    pending_id = 0;
    priority_id = 0;
    ticks_left = 0;
    speaker_off();
}

static void start_descriptor(unsigned id) {
    if (id < 1 || id > SOUND_NOTE) return;
    const SoundDesc *desc = &DESCRIPTORS[id - 1];
    ticks_left = id == SOUND_NOTE ? music_duration : desc->duration;
    divisor_step = desc->step;
    if (id == SOUND_CHARGE) {
        divisor = (uint16_t)(CHARGE_PITCH_BASE - charges * CHARGE_PITCH_STEP);
    } else if (id == SOUND_TUNNEL_PICKUP) {
        unsigned folded = (unsigned)angle & ANGLE_MASK;
        if (folded > ANGLE_HALF) folded -= ANGLE_FLIP;
        else folded = ANGLE_HALF - folded;
        divisor = (uint16_t)(ANGLE_PITCH_BASE + (folded << 2));
    } else if (id == SOUND_NOTE) {
        divisor = music_divisor;
    } else {
        divisor = desc->divisor;
    }
    modulation_id = (uint8_t)(id == SOUND_NOTE ? music_modulation : desc->modulation);
    modulation_phase = 0;
    output_divisor = divisor;
    speaker_on = 1;
    wave_acc = 0;
    wave_high = 1;
}

static void play_note(unsigned note) {
    if (note == 0) return;
    note--;
    if (note >= 52) return;
    music_divisor = NOTE_DIVISORS[note];
    active_id = SOUND_NOTE;
    start_descriptor(SOUND_NOTE);
}

static void music_tick(void) {
    music_divider--;
    if ((int8_t)music_divider >= 0) return;
    music_divider = MUSIC_GAP;
    music_duration = MUSIC_HOLD;
    music_modulation = 4;
    if (music_origin + music_cursor >= sizeof MUSIC) {
        music_active = 0;
        return;
    }
    unsigned command = MUSIC[music_origin + music_cursor];
    if (command == MUSIC_END) {
        music_active = 0;
        return;
    }
    unsigned note;
    if (command == 0) {
        if (active_id == 0) {
            music_modulation = 0;
            music_duration = 6;
            note = (unsigned)FILL_NOTES[music_cursor & 31] - NOTE_BIAS;
        } else {
            note = 0;
        }
    } else if (command == MUSIC_REST) {
        note = 0;
    } else {
        note = command + NOTE_BIAS;
    }
    play_note(note);
    music_cursor++;
}

static void sound_tick(void) {
    if (music_active) {
        if (enabled) music_tick();
        else {
            music_cursor = 0;
            music_active = 0;
        }
    }
    if (active_id == 0) return;
    ticks_left--;
    if (ticks_left == 0) {
        if (!enabled) {
            sound_stop();
            return;
        }
        if (pending_id != 0) {
            active_id = pending_id;
            pending_id = priority_id;
            priority_id = 0;
            start_descriptor(active_id);
        } else {
            sound_stop();
        }
        return;
    }
    divisor = (uint16_t)(divisor + divisor_step);
    if (modulation_id != 0) {
        unsigned index = modulation_phase & 15u;
        modulation_phase++;
        unsigned which = (unsigned)modulation_id - 1u;
        int16_t extra = which < 4 ? MODULATION[which][index] : 0;
        output_divisor = (uint16_t)(divisor + extra);
    } else {
        output_divisor = divisor;
    }
}

void sound_context(int next_charges, int next_angle) {
    charges = next_charges;
    angle = next_angle;
}

void sound_set_enabled(int on) {
    enabled = on ? 1 : 0;
}

void sound_trigger(unsigned id) {
    if (music_active || !enabled || id < 1 || id > SOUND_NOTE) return;
    if (active_id == 0) {
        active_id = (uint8_t)id;
        start_descriptor(id);
        return;
    }
    if (pending_id == 0) {
        pending_id = (uint8_t)id;
        return;
    }
    if (id > priority_id) priority_id = (uint8_t)id;
}

void sound_music_start(int name_pending) {
    music_divider = 0;
    music_cursor = 0;
    music_active = 1;
    music_origin = name_pending ? 0u : TITLE_SONG;
}

void sound_reset(void) {
    active_id = pending_id = priority_id = ticks_left = 0;
    enabled = 1;
    modulation_id = modulation_phase = 0;
    music_active = 0;
    music_divider = 0;
    music_cursor = 0;
    music_modulation = 0;
    music_duration = 5;
    music_divisor = 2000;
    speaker_off();
}

static void fill_samples(int16_t *out, int count) {
    for (int i = 0; i < count; i++) {
        if (speaker_on && output_divisor != 0) {
            uint64_t limit = (uint64_t)SAMPLE_RATE * output_divisor;
            wave_acc += (uint64_t)PIT_CLOCK * 2u;
            while (limit > 0 && wave_acc >= limit) {
                wave_acc -= limit;
                wave_high ^= 1;
            }
            out[i] = (int16_t)(wave_high ? 5000 : -5000);
        } else {
            out[i] = 0;
        }
        timer_acc += PIT_CLOCK;
        if (timer_acc >= (uint64_t)SAMPLE_RATE * TIMER_DIVISOR) {
            timer_acc -= (uint64_t)SAMPLE_RATE * TIMER_DIVISOR;
            sound_tick();
        }
    }
}

void sound_pump(void) {
    if (!audio_ready) return;
    if (!IsAudioStreamProcessed(stream)) return;
    int16_t samples[STREAM_SAMPLES];
    fill_samples(samples, STREAM_SAMPLES);
    UpdateAudioStream(stream, samples, STREAM_SAMPLES);
}

void sound_init(void) {
    sound_reset();
    InitAudioDevice();
    if (!IsAudioDeviceReady()) return;
    SetAudioStreamBufferSizeDefault(STREAM_SAMPLES);
    stream = LoadAudioStream(SAMPLE_RATE, 16, 1);
    if (!IsAudioStreamValid(stream)) return;
    int16_t prime[STREAM_SAMPLES];
    fill_samples(prime, STREAM_SAMPLES);
    UpdateAudioStream(stream, prime, STREAM_SAMPLES);
    PlayAudioStream(stream);
    audio_ready = 1;
}

void sound_shutdown(void) {
    if (!audio_ready) return;
    StopAudioStream(stream);
    UnloadAudioStream(stream);
    CloseAudioDevice();
    audio_ready = 0;
}
