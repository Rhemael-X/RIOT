#include "ghc/ghc_context.h"
#include <string.h>

/*
 * Initialise la table de contextes GHC
 */
void ghc_ctx_table_init(ghc_context_table_t *t)
{
    memset(t, 0, sizeof(*t));
}

/*
 * Recherche un contexte correspondant à une adresse IPv6
 * (comparaison sur le préfixe)
 */
ghc_context_t *ghc_ctx_lookup(ghc_context_table_t *t,
                              const uint8_t *addr)
{
    for (int i = 0; i < GHC_MAX_CONTEXTS; i++) {
        ghc_context_t *c = &t->ctx[i];

        if (!c->valid)
            continue;

        /* comparaison du préfixe /64 */
        if (memcmp(addr, c->prefix, 8) == 0) {
            c->last_used = time(NULL);
            return c;
        }
    }
    return NULL;
}

/*
 * Alloue dynamiquement un contexte (LRU si plein)
 */
ghc_context_t *ghc_ctx_allocate(ghc_context_table_t *t,
                                uint8_t id,
                                const uint8_t *prefix,
                                uint8_t prefix_len)
{
    int lru = -1;
    time_t oldest = time(NULL);

    for (int i = 0; i < GHC_MAX_CONTEXTS; i++) {
        ghc_context_t *c = &t->ctx[i];

        /* Slot libre */
        if (!c->valid) {
            lru = i;
            break;
        }

        /* Recherche du moins récemment utilisé */
        if (c->last_used < oldest) {
            oldest = c->last_used;
            lru = i;
        }
    }

    if (lru < 0)
        return NULL;

    ghc_context_t *c = &t->ctx[lru];
    c->id = id;
    memcpy(c->prefix, prefix, 8);
    c->prefix_len = prefix_len;
    c->valid = 1;
    c->last_used = time(NULL);

    return c;
}

/*
 * Expire automatiquement les contextes inactifs
 */
void ghc_ctx_expire(ghc_context_table_t *t)
{
    time_t now = time(NULL);

    for (int i = 0; i < GHC_MAX_CONTEXTS; i++) {
        ghc_context_t *c = &t->ctx[i];

        if (c->valid &&
            (now - c->last_used) > GHC_CONTEXT_LIFETIME) {
            c->valid = 0;
        }
    }
}
