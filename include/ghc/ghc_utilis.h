#pragma once

#include <stdint.h>
#include <cstring>

/* Macros de sécurité */
#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

/* Conversion bits → octets */
static inline uint16_t ghc_bits_to_bytes(uint16_t bits)
{
    return (bits + 7) / 8;
}

/* Test si un préfixe IPv6 correspond */
static inline int ghc_prefix_match(const uint8_t *addr,
                                   const uint8_t *prefix,
                                   uint8_t prefix_len)
{
    uint16_t bytes = prefix_len / 8;
    uint8_t  rem   = prefix_len % 8;

    if (bytes && memcmp(addr, prefix, bytes) != 0)
        return 0;

    if (rem) {
        uint8_t mask = 0xFF << (8 - rem);
        if ((addr[bytes] & mask) != (prefix[bytes] & mask))
            return 0;
    }
    return 1;
}
