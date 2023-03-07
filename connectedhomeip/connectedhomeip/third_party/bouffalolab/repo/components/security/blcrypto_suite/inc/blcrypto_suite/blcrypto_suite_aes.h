#ifndef BLCRYPTO_SUITE_AES_H
#define BLCRYPTO_SUITE_AES_H

#include <stdint.h>

struct blcrypto_suite_aes;

enum crypto_aes_mode {
    AES_ENC,
    AES_DEC,
};

struct blcrypto_suite_aes *blcrypto_suite_aes_init();
void blcrypto_suite_aes_deinit(struct blcrypto_suite_aes **aes);
int blcrypto_suite_aes_set_key(struct blcrypto_suite_aes *aes, enum crypto_aes_mode mode, const uint8_t *key, int bits);
int blcrypto_suite_aes_crypt(struct blcrypto_suite_aes *aes, enum crypto_aes_mode mode, const uint8_t *in, uint8_t *out);

#endif /* end of include guard: BLCRYPTO_SUITE_AES_H */
