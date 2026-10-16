#ifndef GOOSE_MODEL_H
#define GOOSE_MODEL_H
#include <stdint.h>

int goose_signed8(unsigned bits);
int16_t goose_signed16(uint16_t bits);
uint16_t goose_sar16(uint16_t bits, unsigned shift);
int16_t goose_surface_dx(int16_t velocity);
int16_t goose_tunnel_angle_force(uint8_t angle);
#endif
