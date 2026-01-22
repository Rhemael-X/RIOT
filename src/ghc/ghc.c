#include "ghc/ghc.h"
#include "ghc/ghc_cursor.h"
#include "ghc/ghc_encoding.h"
#include <stdint.h>

/*
 * Décompression générique GHC
 *
 * Retour :
 *  - nombre d’octets consommés dans le buffer
 *  - -1 en cas d’erreur
 */
int ghc_decompress(const uint8_t *buf,
                    uint16_t len,
                    ghc_context_table_t *table,
                    ipv6_raw_header_t *ipv6,
                    ipv6_ext_hdr_t *ext,
                    uint8_t *ext_count)
{
    ghc_cursor_t cursor;
    uint8_t dispatch;
    uint8_t encoding;

    if (!buf || !table || !ipv6 || !ext || !ext_count)
        return -1;

    cursor.buf = buf;
    cursor.len = len;
    cursor.pos = 0;

    /* ------------------------------------------------------------
     * Dispatch GHC (RFC 7400)
     * ------------------------------------------------------------ */
    if (ghc_read_u8(&cursor, &dispatch) < 0)
        return -1;

    if ((dispatch & GHC_DISPATCH_MASK) != GHC_DISPATCH_VALUE) {
        /* Ce n’est pas un en-tête GHC */
        return -1;
    }

    /* ------------------------------------------------------------
     * Encoding ID
     * ------------------------------------------------------------ */
    if (ghc_read_u8(&cursor, &encoding) < 0)
        return -1;

    /* ------------------------------------------------------------
     * Délégation selon le type d’en-tête compressé
     * ------------------------------------------------------------ */
    switch (encoding) {

    case GHC_ENC_IPV6:
        if (ghc_decompress_ipv6(&cursor,
                                 table,
                                 ipv6,
                                 ext,
                                 ext_count) < 0)
            return -1;
        break;

    default:
        /* Encoding non supporté */
        return -1;
    }

    /* ------------------------------------------------------------
     * Retourne le nombre d’octets consommés
     * ------------------------------------------------------------ */
    return cursor.pos;
}
