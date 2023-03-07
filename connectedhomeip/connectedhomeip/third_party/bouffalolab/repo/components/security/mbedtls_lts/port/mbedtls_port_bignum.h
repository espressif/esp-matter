#ifndef MBEDTLS_PORT_BIGNUM_H_ERFVHYLQ
#define MBEDTLS_PORT_BIGNUM_H_ERFVHYLQ

#include <mbedtls/bignum.h>

int mbedtls_mpi_exp_mod_original( mbedtls_mpi *X, const mbedtls_mpi *A,
                                  const mbedtls_mpi *E, const mbedtls_mpi *N,
                                  mbedtls_mpi *_RR );

int bl_mpi_exp_mod_wo_lock(mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *E, const mbedtls_mpi *N, mbedtls_mpi *_RN);
int bl_mpi_exp_mod(mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *E, const mbedtls_mpi *N, mbedtls_mpi *_RN);

#endif /* end of include guard: MBEDTLS_PORT_BIGNUM_H_ERFVHYLQ */
