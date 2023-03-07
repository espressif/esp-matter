#pragma once

#include <mbedtls/bignum.h>

void dump_mpi(const char *tag, const mbedtls_mpi *bn);

size_t mpi_words(const mbedtls_mpi *mpi);
int mpi_clear_upper_bits(mbedtls_mpi *X, size_t bit);
int mpi_mod_2n(mbedtls_mpi *X, mbedtls_mpi_uint n);
int mpi_hensel_quad_mod_inv_prime_n(mbedtls_mpi *U, const mbedtls_mpi *a, mbedtls_mpi_uint m);
int mpi_scalar_encode(const uint32_t *scalar, size_t scalar_len, int w, int8_t *output, size_t *out_len);
