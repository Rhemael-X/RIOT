#include "ghc/ghc.h"
#include "ghc/ghc_context.h"

ghc_context_t ghc_ctx_table;

void sixlowpan_rx(const uint8_t *frame, uint16_t len)
{
    ipv6_raw_header_t ipv6;
    ipv6_ext_hdr_t ext[IPV6_MAX_EXT];
    uint8_t ext_count;

    ghc_decompress(frame, len, &ghc_ctx_table,
                   &ipv6, ext, &ext_count);

    /* Injection IPv6 stack */
}
