#pragma once

#define GHC_DISPATCH_MASK   0xF0
#define GHC_DISPATCH_VALUE  0xE0

typedef enum {
    GHC_ENC_IPV6        = 0x01,
    GHC_ENC_UDP         = 0x02,
    GHC_ENC_EXT_HBH     = 0x10,
    GHC_ENC_EXT_ROUTING = 0x11,
    GHC_ENC_EXT_FRAG    = 0x12,
    GHC_ENC_EXT_DEST    = 0x13,
} ghc_encoding_id_t;
