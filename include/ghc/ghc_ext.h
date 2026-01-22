#pragma once

#include <stdint.h>
#include "ghc_cursor.h"

#define IPV6_MAX_EXT 4

typedef struct {
    uint8_t  type;
    uint8_t  next_header;
    uint16_t len;
    uint8_t  data[32];
} ipv6_ext_hdr_t;

/* DÉCLARATION */
int ghc_decompress_ext_hdr(ghc_cursor_t *c, ipv6_ext_hdr_t *ext);
