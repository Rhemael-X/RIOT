#pragma once
#include "ghc_cursor.h"
#include "ghc_context.h"
#include "ghc_ext.h"

typedef struct {
    uint32_t ver_tc_fl;
    uint16_t payload_len;
    uint8_t  next_header;
    uint8_t  hop_limit;
    uint8_t  src[16];
    uint8_t  dst[16];
} ipv6_raw_header_t;

int ghc_decompress(const uint8_t *buf,
                    uint16_t len,
                    ghc_context_table_t *table,
                    ipv6_raw_header_t *ipv6,
                    ipv6_ext_hdr_t *ext,
                    uint8_t *ext_count);

int ghc_decompress_ipv6(ghc_cursor_t *c,
                         ghc_context_table_t *table,
                         ipv6_raw_header_t *ipv6,
                         ipv6_ext_hdr_t *ext,
                         uint8_t *ext_count);
