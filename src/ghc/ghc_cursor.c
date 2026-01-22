#include "ghc/ghc_cursor.h"
#include <string.h>

int ghc_read_u8(ghc_cursor_t *c, uint8_t *v)
{
    if (c->pos >= c->len) return -1;
    *v = c->buf[c->pos++];
    return 0;
}

int ghc_read_bytes(ghc_cursor_t *c, void *dst, uint16_t n)
{
    if (c->pos + n > c->len) return -1;
    memcpy(dst, &c->buf[c->pos], n);
    c->pos += n;
    return 0;
}
