#pragma once
#include <stdint.h>

typedef struct {
    const uint8_t *buf;
    uint16_t len;
    uint16_t pos;
} ghc_cursor_t;

int ghc_read_u8(ghc_cursor_t *c, uint8_t *v);
int ghc_read_bytes(ghc_cursor_t *c, void *dst, uint16_t n);
