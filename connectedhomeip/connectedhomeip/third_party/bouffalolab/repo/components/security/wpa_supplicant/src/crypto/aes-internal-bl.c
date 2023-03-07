#include "aes.h"
#include <blcrypto_suite/blcrypto_suite_supplicant_api.h>

void *wpa_aes_encrypt_init(const u8 *key, size_t len)
{
    struct blcrypto_suite_aes *aes = blcrypto_suite_aes_init();
    if (!aes) {
        return NULL;
    }
    blcrypto_suite_aes_set_key(aes, AES_ENC, key, len * 8);
    return aes;
}

/* this function shall support in-place operation */
void wpa_aes_encrypt(void *ctx, const u8 *plain, u8 *crypt)
{
    blcrypto_suite_aes_crypt((struct blcrypto_suite_aes *)ctx, AES_ENC, plain, crypt);
}

void wpa_aes_encrypt_deinit(void *ctx)
{
    void *t = ctx;
    blcrypto_suite_aes_deinit((struct blcrypto_suite_aes **)&t);
}

void *wpa_aes_decrypt_init(const u8 *key, size_t len)
{
    struct blcrypto_suite_aes *aes = blcrypto_suite_aes_init();
    if (!aes) {
        return NULL;
    }
    blcrypto_suite_aes_set_key(aes, AES_DEC, key, len * 8);
    return aes;
}

void wpa_aes_decrypt(void *ctx, const u8 *crypt, u8 *plain)
{
    blcrypto_suite_aes_crypt((struct blcrypto_suite_aes *)ctx, AES_DEC, crypt, plain);
}

void wpa_aes_decrypt_deinit(void *ctx)
{
    wpa_aes_encrypt_deinit(ctx);
}
