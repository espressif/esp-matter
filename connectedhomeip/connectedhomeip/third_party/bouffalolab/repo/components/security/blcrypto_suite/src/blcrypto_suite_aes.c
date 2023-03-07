#include <blcrypto_suite/blcrypto_suite_top_config.h>
#include "blcrypto_suite/blcrypto_suite_aes.h"

#if USE_HWCRYPTO
#include <string.h>
#include <FreeRTOS.h>
#include <bl_sec_aes.h>

struct blcrypto_suite_aes {
    bl_sec_aes_t ctx;
};

struct blcrypto_suite_aes *blcrypto_suite_aes_init()
{
    struct blcrypto_suite_aes *aes;

    aes = pvPortMalloc(sizeof(*aes));
    if (aes == NULL) {
        return NULL;
    }
    bl_aes_init((bl_sec_aes_t *)aes);

    return aes;
}

void blcrypto_suite_aes_deinit(struct blcrypto_suite_aes **aes)
{
    if (!(aes != NULL && *aes != NULL)) {
        return;
    }

    vPortFree(*aes);
    *aes = NULL;
}

int blcrypto_suite_aes_set_key(struct blcrypto_suite_aes *aes, enum crypto_aes_mode mode, const uint8_t *key, int bits)
{
    if (!aes) {
        return -1;
    }
    (void)mode;
    bl_aes_set_key((bl_sec_aes_t *)aes, BL_AES_ENCRYPT, key, bits / 8);

    return 0;
}

int blcrypto_suite_aes_crypt(struct blcrypto_suite_aes *aes, enum crypto_aes_mode mode, const uint8_t *in, uint8_t *out)
{
    bl_sec_aes_op_t op;
    if (!aes) {
        return -1;
    }
    if (mode == AES_ENC) {
        op = BL_AES_ENCRYPT;
    } else {
        op = BL_AES_DECRYPT;
    }
    bl_aes_acquire_hw();
    bl_aes_transform((bl_sec_aes_t *)aes, op, in, out);
    bl_aes_release_hw();

    return 0;
}
#else
#include <FreeRTOS.h>
#include <mbedtls/aes.h>

struct blcrypto_suite_aes_internal {
    mbedtls_aes_context aes;
};

// only ECB is supported
struct blcrypto_suite_aes *blcrypto_suite_aes_init()
{
    struct blcrypto_suite_aes_internal *aes_internal;

    aes_internal = pvPortMalloc(sizeof(struct blcrypto_suite_aes_internal));
    if (aes_internal == NULL) {
        return NULL;
    }

    mbedtls_aes_init(&aes_internal->aes);
    return (struct blcrypto_suite_aes *)aes_internal;
}

void blcrypto_suite_aes_deinit(struct blcrypto_suite_aes **aes)
{
    if (!(aes != NULL && *aes != NULL)) {
        return;
    }

    vPortFree(*aes);
    *aes = NULL;
}

int blcrypto_suite_aes_set_key(struct blcrypto_suite_aes *aes, enum crypto_aes_mode mode, const uint8_t *key, int bits)
{
    if (!aes) {
        return -1;
    }
    struct blcrypto_suite_aes_internal *aes_internal = (struct blcrypto_suite_aes_internal *)aes;

    if (mode == AES_ENC) {
        mbedtls_aes_setkey_enc(&aes_internal->aes, key, bits);
    } else {
        mbedtls_aes_setkey_dec(&aes_internal->aes, key, bits);
    }
    return 0;
}

int blcrypto_suite_aes_crypt(struct blcrypto_suite_aes *aes, enum crypto_aes_mode mode, const uint8_t *in, uint8_t *out)
{
    if (!aes) {
        return -1;
    }
    struct blcrypto_suite_aes_internal *aes_internal = (struct blcrypto_suite_aes_internal *)aes;

    if (mode == AES_ENC) {
        mbedtls_aes_crypt_ecb(&aes_internal->aes, MBEDTLS_AES_ENCRYPT, in, out);
    } else {
        mbedtls_aes_crypt_ecb(&aes_internal->aes, MBEDTLS_AES_DECRYPT, in, out);
    }
    return 0;
}
#endif
