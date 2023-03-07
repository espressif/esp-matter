#ifndef BLCRYPTO_SUITE_HACC_GLUE_H
#define BLCRYPTO_SUITE_HACC_GLUE_H

#include <stdio.h>

#include <blcrypto_suite/blcrypto_suite_bignum.h>
#include <blcrypto_suite/blcrypto_suite_ecp.h>
#include <blcrypto_suite/blcrypto_suite_hacc.h>

#define BLCRYPTO_SUITE_HACC_GLUE_DEBUG 0

#if BLCRYPTO_SUITE_HACC_GLUE_DEBUG
#define GLUE_DBG(fmt, ...) \
    do { printf("BLCRYPTO_SUITE_PKA_HACC: %s:%d " fmt, __func__, __LINE__, ##__VA_ARGS__); } while (0)
#else
#define GLUE_DBG(fmt, ...) \
    do { } while (0)
#endif

int blcrypto_suite_hacc_ecp_mul(blcrypto_suite_ecp_group *grp, blcrypto_suite_ecp_point *R, const blcrypto_suite_mpi *m, const blcrypto_suite_ecp_point *P, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng);
int blcrypto_suite_hacc_mpi_exp_mod_secp256r1(blcrypto_suite_mpi *X, const blcrypto_suite_mpi *A, const blcrypto_suite_mpi *E);

#endif /* end of include guard: BLCRYPTO_SUITE_HACC_GLUE_H */
