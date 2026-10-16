#include "goose_model.h"

int goose_signed8(unsigned bits) {
    bits &= 255u;
    return bits < 128u ? (int)bits : (int)bits - 256;
}
int16_t goose_signed16(uint16_t bits) {
    return (int16_t)(bits < UINT16_C(0x8000)
                    ? (int32_t)bits : (int32_t)bits - INT32_C(65536));
}
uint16_t goose_sar16(uint16_t bits, unsigned shift) {
    if (shift == 0) return bits;
    if (shift >= 16) return (bits & UINT16_C(0x8000)) ? UINT16_MAX : 0;
    uint32_t result = (uint32_t)bits >> shift;
    if (bits & UINT16_C(0x8000)) result |= UINT32_C(0xffff) << (16 - shift);
    return (uint16_t)result;
}
int16_t goose_surface_dx(int16_t velocity) {
    /* CS:115B..1160: add AX,3; sar AX,1; sar AX,1. Wrap precedes sign. */
    uint16_t bits = (uint16_t)((uint16_t)velocity + UINT32_C(3));
    return goose_signed16(goose_sar16(bits, 2));
}
int16_t goose_tunnel_angle_force(uint8_t angle) {
    /* CS:3084..309C: test SF of the 8-bit subtraction, NOT signed(angle) < 5. */
    uint8_t adjusted = (uint8_t)(angle - 5u);
    if (adjusted & 0x80u) return 0;
    if (adjusted > 56u) adjusted = 56u;
    return (int16_t)(-(int)(adjusted >> 1) - 3);
}
