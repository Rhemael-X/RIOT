#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "ghc/ghc.h"
#include "ghc/ghc_context.h"

// Données fictives pour un en-tête IPv6 compressé (exemple simplifié)
// Dispatch = 0xE0 (GHC), Encoding = 0x01 (IPv6)
// Adresses : src et dst avec contexte ID 0
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

// Test avec extension header Hop-by-Hop
static const uint8_t test_frame_ext[] = {
    0xE0,  // Dispatch
    0x01,  // Encoding ID (IPv6)
    0x10,  // Next Header (Hop-by-Hop)
    0x40,  // Hop Limit
    0x00,  // Context ID src
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // IID src
    0x00,  // Context ID dst
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // IID dst
    0x3A,  // Next Header final (ICMPv6)
    0x00,  // Hdr Ext Len (0 = 8 octets)
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08   // Données ext
};

// Test avec contexte invalide (ID 1, mais seulement 0 défini)
static const uint8_t test_frame_invalid_ctx[] = {
    0xE0,  // Dispatch
    0x01,  // Encoding ID (IPv6)
    0x00,  // Next Header
    0x40,  // Hop Limit
    0x01,  // Context ID src invalide
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // IID src
    0x00,  // Context ID dst
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   // IID dst
};

// Test avec dispatch invalide
static const uint8_t test_frame_invalid_dispatch[] = {
    0xD0,  // Dispatch invalide (pas 0xE0)
    0x01,  // Encoding ID
    0x00,  // Next Header
    0x40,  // Hop Limit
    0x00,  // Context ID src
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // IID src
    0x00,  // Context ID dst
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   // IID dst
};

// Fonction de test de base
int test_basic_ipv6(ghc_context_table_t *table) {
    ipv6_raw_header_t ipv6;
    ipv6_ext_hdr_t ext[IPV6_MAX_EXT];
    uint8_t ext_count;
    int result;

    printf("Test : Décompression IPv6 basique\n");
    result = ghc_decompress(test_frame, sizeof(test_frame), table, &ipv6, ext, &ext_count);

    if (result < 0) {
        printf("Échec : %d\n", result);
        return 0;
    }

    if (ipv6.hop_limit != 0x40 || ipv6.next_header != 0x00 || ext_count != 0) {
        printf("Échec : valeurs incorrectes\n");
        return 0;
    }

    printf("Succès : Octets %d, NH %d, HL %d\n", result, ipv6.next_header, ipv6.hop_limit);
    return 1;
}

// Fonction de test avec extension header
int test_with_extension_header(ghc_context_table_t *table) {
    ipv6_raw_header_t ipv6;
    ipv6_ext_hdr_t ext[IPV6_MAX_EXT];
    uint8_t ext_count;
    int result;

    printf("Test : Décompression avec extension header\n");
    result = ghc_decompress(test_frame_ext, sizeof(test_frame_ext), table, &ipv6, ext, &ext_count);

    if (result < 0) {
        printf("Échec : %d\n", result);
        return 0;
    }

    if (ipv6.next_header != 0x3A || ext_count != 1 || ext[0].type != 0x10) {
        printf("Échec : extension incorrecte\n");
        return 0;
    }

    printf("Succès : Extension détectée, NH final %d\n", ipv6.next_header);
    return 1;
}

// Fonction de test avec contexte invalide
int test_invalid_context(ghc_context_table_t *table) {
    ipv6_raw_header_t ipv6;
    ipv6_ext_hdr_t ext[IPV6_MAX_EXT];
    uint8_t ext_count;
    int result;

    printf("Test : Contexte invalide\n");
    result = ghc_decompress(test_frame_invalid_ctx, sizeof(test_frame_invalid_ctx), table, &ipv6, ext, &ext_count);

    if (result >= 0) {
        printf("Échec : devrait échouer\n");
        return 0;
    }

    printf("Succès : Échec attendu (%d)\n", result);
    return 1;
}

// Fonction de test avec dispatch invalide
int test_invalid_dispatch(ghc_context_table_t *table) {
    ipv6_raw_header_t ipv6;
    ipv6_ext_hdr_t ext[IPV6_MAX_EXT];
    uint8_t ext_count;
    int result;

    printf("Test : Dispatch invalide\n");
    result = ghc_decompress(test_frame_invalid_dispatch, sizeof(test_frame_invalid_dispatch), table, &ipv6, ext, &ext_count);

    if (result >= 0) {
        printf("Échec : devrait échouer\n");
        return 0;
    }

    printf("Succès : Échec attendu (%d)\n", result);
    return 1;
}

int main(void) {
    ghc_context_table_t table;
    int passed = 0;
    int total = 4;

    // Initialiser la table de contextes
    ghc_ctx_table_init(&table);

    // Ajouter un contexte fictif (ID 0, prefix 2001:db8::/32)
    uint8_t prefix[8] = {0x20, 0x01, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00};
    ghc_ctx_allocate(&table, 0, prefix, 32);

    // Exécuter les tests
    passed += test_basic_ipv6(&table);
    passed += test_with_extension_header(&table);
    passed += test_invalid_context(&table);
    passed += test_invalid_dispatch(&table);

    printf("\nRésultats : %d/%d tests réussis\n", passed, total);

    return (passed == total) ? 0 : 1;
}
