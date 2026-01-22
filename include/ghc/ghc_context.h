#ifndef GHC_CONTEXT_H
#define GHC_CONTEXT_H

#include <stdint.h>
#include <time.h>

#define GHC_MAX_CONTEXTS 8
#define GHC_CONTEXT_LIFETIME 300


typedef struct {
    uint8_t id;
    uint8_t prefix[8];
    uint8_t prefix_len;
    uint8_t valid;
    time_t last_used;
} ghc_context_t;

typedef struct {
    ghc_context_t ctx[GHC_MAX_CONTEXTS];
} ghc_context_table_t;

void ghc_ctx_table_init(ghc_context_table_t *t);
ghc_context_t *ghc_ctx_lookup(ghc_context_table_t *t, const uint8_t *addr);
ghc_context_t *ghc_ctx_allocate(ghc_context_table_t *t,
                                uint8_t id,
                                const uint8_t *prefix,
                                uint8_t prefix_len);
void ghc_ctx_expire(ghc_context_table_t *t);

#endif /* GHC_CONTEXT_H */
