#include "ghc/ghc_ipv6.h"
#include "ghc/ghc_encoding.h"
#include "ghc/ghc_cursor.h"
#include <string.h>
#include <arpa/inet.h>   /* htonl */

/* ============================================================
 * Décompression d’une adresse IPv6 à partir d’un contexte GHC
 * ============================================================ */
static int ghc_decompress_addr(ghc_cursor_t *c,
                               ghc_context_table_t *table,
                               uint8_t *addr)
{
    uint8_t ctx_id;

    /* Lecture de l’ID de contexte */
    if (ghc_read_u8(c, &ctx_id) < 0)
        return -1;

    for (int i = 0; i < GHC_MAX_CONTEXTS; i++) {
        ghc_context_t *ctx = &table->ctx[i];

        if (!ctx->valid)
            continue;

        if (ctx->id == ctx_id) {
            /* Reconstruction adresse = prefix + IID */
            memcpy(addr, ctx->prefix, 8);

            if (ghc_read_bytes(c, &addr[8], 8) < 0)
                return -1;

            ctx->last_used = time(NULL);
            return 0;
        }
    }

    /* Contexte inconnu */
    return -1;
}

/* ============================================================
 * Décompression IPv6 GHC complète
 * ============================================================ */
int ghc_decompress_ipv6(ghc_cursor_t *c,
                         ghc_context_table_t *table,
                         ipv6_raw_header_t *ipv6,
                         ipv6_ext_hdr_t *ext,
                         uint8_t *ext_count)
{
    uint8_t nh;

    if (!c || !table || !ipv6 || !ext || !ext_count)
        return -1;

    /* ------------------------------------------------------------
     * Champs IPv6 élidés → reconstruits
     * ------------------------------------------------------------ */
    ipv6->ver_tc_fl = htonl(6 << 28); /* Version = 6 */
    ipv6->payload_len = 0;            /* Calculé plus tard */

    /* ------------------------------------------------------------
     * Champs transmis
     * ------------------------------------------------------------ */
    if (ghc_read_u8(c, &ipv6->next_header) < 0)
        return -1;

    if (ghc_read_u8(c, &ipv6->hop_limit) < 0)
        return -1;

    /* ------------------------------------------------------------
     * Adresses IPv6 (contextuelles)
     * ------------------------------------------------------------ */
    if (ghc_decompress_addr(c, table, ipv6->src) < 0)
        return -1;

    if (ghc_decompress_addr(c, table, ipv6->dst) < 0)
        return -1;

    /* ------------------------------------------------------------
     * Chaîne d’extension headers IPv6
     * ------------------------------------------------------------ */
    nh = ipv6->next_header;
    *ext_count = 0;

    while (nh == GHC_ENC_EXT_HBH ||
           nh == GHC_ENC_EXT_ROUTING ||
           nh == GHC_ENC_EXT_FRAG ||
           nh == GHC_ENC_EXT_DEST) {

        if (*ext_count >= IPV6_MAX_EXT)
            return -1;

        ipv6_ext_hdr_t *e = &ext[*ext_count];
        e->type = nh;

        if (ghc_decompress_ext_hdr(c, e) < 0)
            return -1;

        nh = e->next_header;
        (*ext_count)++;
    }

    /* ------------------------------------------------------------
     * Le Next Header final pointe vers L4 (UDP/TCP/ICMPv6)
     * ------------------------------------------------------------ */
    ipv6->next_header = nh;

    return 0;
}
