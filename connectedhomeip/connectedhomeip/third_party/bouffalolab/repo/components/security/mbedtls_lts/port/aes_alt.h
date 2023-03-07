#ifndef MBEDTLS_AES_ALT_H
#define MBEDTLS_AES_ALT_H

#include <bl_sec_aes.h>

/**
 * \brief The AES context-type definition.
 */
typedef struct mbedtls_aes_context
{
    bl_sec_aes_t ctx;
}
mbedtls_aes_context;

#if defined(MBEDTLS_CIPHER_MODE_XTS)
/**
 * \brief The AES XTS context-type definition.
 */
typedef struct mbedtls_aes_xts_context
{
    mbedtls_aes_context crypt; /*!< The AES context to use for AES block
                                        encryption or decryption. */
    mbedtls_aes_context tweak; /*!< The AES context used for tweak
                                        computation. */
} mbedtls_aes_xts_context;
#endif /* MBEDTLS_CIPHER_MODE_XTS */

#endif /* aes_alt.h */
