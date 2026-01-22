#include <stdio.h>
#include <stdint.h>
#include "ghc/ghc.h"
#include "ghc/ghc_context.h"

// Données fictives pour un en-tête IPv6 compressé (exemple simplifié)
// Dispatch = 0xE0 (GHC), Encoding = 0x01 (IPv6)
// Adresses : src et dst avec contexte ID 0
// (À adapter selon la RFC 7400 pour des tests réels)
static const uint8_t test_frame[] = {
    0xE0,  // Dispatch
    0x01,  // Encoding ID (IPv6)
    0x00,  // Next Header
    0x40,  // Hop Limit
    0x00,  // Context ID src
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // IID src (8 octets)
    0x00,  // Context ID dst
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   // IID dst (8 octets)
};

int main(void) {
    ghc_context_table_t table;
    ipv6_raw_header_t ipv6;
    ipv6_ext_hdr_t ext[IPV6_MAX_EXT];
    uint8_t ext_count;
    int result;

    // Initialiser la table de contextes
    ghc_ctx_table_init(&table);

    // Ajouter un contexte fictif (ID 0, prefix 2001:db8::/32)
    uint8_t prefix[8] = {0x20, 0x01, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00};
    ghc_ctx_allocate(&table, 0, prefix, 32);

    // Appeler la décompression
    result = ghc_decompress(test_frame, sizeof(test_frame), &table, &ipv6, ext, &ext_count);

    if (result < 0) {
        printf("Erreur de décompression : %d\n", result);
        return 1;
    }

    // Vérifier le résultat (exemple : hop limit et adresses)
    if (ipv6.hop_limit != 0x40) {
        printf("Hop limit incorrect\n");
        return 1;
    }

    printf("Décompression réussie ! Octets consommés : %d\n", result);
    printf("Next Header : %d, Hop Limit : %d\n", ipv6.next_header, ipv6.hop_limit);
    // Ajouter plus de vérifications si besoin (adresses, etc.)

    return 0;
}
