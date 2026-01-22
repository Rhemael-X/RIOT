#include "ghc/ghc_ext.h"
#include "ghc/ghc_cursor.h"

/**
 * Décompression d’un IPv6 Extension Header GHC
 */
int ghc_decompress_ext_hdr(ghc_cursor_t *c, ipv6_ext_hdr_t *ext)
{
    uint8_t hdr_len;

    if (ghc_read_u8(c, &ext->next_header) < 0)
        return -1;

    if (ghc_read_u8(c, &hdr_len) < 0)
        return -1;

    /* RFC IPv6 : longueur en multiples de 8 octets */
    ext->len = hdr_len * 8;

    if (ext->len > sizeof(ext->data))
        return -1;

    if (ghc_read_bytes(c, ext->data, ext->len) < 0)
        return -1;

    return 0;
}
