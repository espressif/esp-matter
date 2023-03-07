#ifndef BLCRYPTO_SUITE_HACC_H
#define BLCRYPTO_SUITE_HACC_H

#include <stdint.h>

int blcrypto_suite_hacc_reset();
int blcrypto_suite_hacc_gf2mont_256_bin(uint32_t *result, uint32_t *src, uint32_t *p);

int blcrypto_suite_hacc_secp256r1_mul_bin(uint8_t id, const uint32_t *m, const uint32_t *Px, const uint32_t *Py, uint32_t *Rx, uint32_t *Ry);
int blcrypto_suite_hacc_secp256r1_exp_mod(uint32_t *result, uint32_t *A, uint32_t *E);

#endif /* end of include guard: BLCRYPTO_SUITE_HACC_H */
