#ifndef GOOSE_SOUND_H
#define GOOSE_SOUND_H

/* PC speaker driven by the original descriptors. The timer IRQ is PIT0 divisor 0x4D0E. */
void sound_init(void);
void sound_reset(void);
void sound_shutdown(void);
void sound_context(int charges, int angle);
void sound_trigger(unsigned id);
void sound_pump(void);

#endif
