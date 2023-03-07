#include "bignum_ext.h"

#include <string.h>
#include <FreeRTOS.h>

void dump_mpi(const char *tag, const mbedtls_mpi *bn)
{
    const size_t buf_len = 4096 + 1;
    char *buf = (char *)pvPortMalloc(buf_len);
    if (!buf)
        return;
    size_t olen;
    if (tag)
        printf("%s: ", tag);
    if (bn) {
        mbedtls_mpi_write_string(bn, 16, buf, buf_len, &olen);
    } else {
        strcpy(buf, "<null>");
    }
    puts(buf);
    puts("\r\n");
    vPortFree(buf);
}

size_t mpi_words(const mbedtls_mpi *mpi)
{
    for (size_t i = mpi->n; i > 0; i--) {
        if (mpi->p[i - 1] != 0) {
            return i;
        }
    }
    return 0;
}

// clear upper bits starting from bit(inclusion, zero-indexed)
int mpi_clear_upper_bits(mbedtls_mpi *X, size_t bit)
{
    const size_t limb_bits = sizeof(mbedtls_mpi_uint) * 8;

    if (!X)
        return -1;
    size_t higher_limb_idx = (bit + limb_bits - 1) / limb_bits;

    for (size_t i = higher_limb_idx; i < X->n; ++i)
        X->p[i] = 0;

    size_t bit_offset = bit % limb_bits;
    if (bit_offset && higher_limb_idx - 1 < X->n) {
        mbedtls_mpi_uint mask = 1;
        mask <<= bit_offset;
        mask -= 1;
        X->p[higher_limb_idx - 1] &= mask;
    }

    return 0;
}

int mpi_mod_2n(mbedtls_mpi *X, mbedtls_mpi_uint n)
{
    int ret = 0;
    mbedtls_mpi R;
    if (!X)
        return -1;

    mpi_clear_upper_bits(X, n);
    mbedtls_mpi_init(&R);
    if (X->s < 0) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_set_bit(&R, n, 1));
        MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(X, X, &R));
        MBEDTLS_MPI_CHK(mbedtls_mpi_set_bit(X, n, 0));
    }
cleanup:
    mbedtls_mpi_free(&R);
    return ret;
}

int mpi_hensel_quad_mod_inv_prime_n(mbedtls_mpi *U, const mbedtls_mpi *a, mbedtls_mpi_uint m)
{
    int ret;
    if (!(U && a && m))
        return -1;

    if ((a->p[0] & 1) == 0)
        return -1;

    mbedtls_mpi tmp;
    mbedtls_mpi_init(&tmp);

    MBEDTLS_MPI_CHK(mbedtls_mpi_lset(U, 1));
    for (size_t i = 2; i < m; i <<= 1) {
        MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&tmp, U, U));
        MBEDTLS_MPI_CHK(mpi_mod_2n(&tmp, i));
        MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&tmp, &tmp, a));
        MBEDTLS_MPI_CHK(mpi_mod_2n(&tmp, i));
        MBEDTLS_MPI_CHK(mbedtls_mpi_shift_l(U, 1));
        MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(U, U, &tmp));
    }
    MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&tmp, U, U));
    MBEDTLS_MPI_CHK(mpi_mod_2n(&tmp, m));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&tmp, &tmp, a));
    MBEDTLS_MPI_CHK(mpi_mod_2n(&tmp, m));
    MBEDTLS_MPI_CHK(mbedtls_mpi_shift_l(U, 1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(U, U, &tmp));
    MBEDTLS_MPI_CHK(mpi_mod_2n(U, m));

    mbedtls_mpi zero;
    mbedtls_mpi_uint z_ = 0;
    zero.s = 1;
    zero.n = 1;
    zero.p = &z_;
    MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(U, &zero, U));
    MBEDTLS_MPI_CHK(mpi_mod_2n(U, m));

cleanup:
    mbedtls_mpi_free(&tmp);

    return 0;
}

// scalar_len: number of uint32_t
// output: length should be >= scalar_len * 4 + 1
int mpi_scalar_encode(const uint32_t *scalar, size_t scalar_len, int w, int8_t *output, size_t *out_len)
{
    if (!(scalar && scalar_len && output && out_len))
        return -1;
    if (!(w >= 2 && w <= 8))
        return -1;

    const size_t bits = scalar_len * 32;
    memset(output, 0, bits + 1);
    const uint32_t width = 1 << w;
    const uint32_t window_mask = width - 1;

    size_t output_len = 0;
    size_t pos = 0;
    uint32_t carry = 0;
    while (pos < bits) {
        size_t u32_idx = pos / 32;
        size_t bit_idx = pos % 32;
        uint32_t bit_buf;
        if (bit_idx < 32 - w) {
            bit_buf = scalar[u32_idx] >> bit_idx;
        } else {
            if (u32_idx + 1 < scalar_len)
                bit_buf = (scalar[u32_idx] >> bit_idx) | (scalar[u32_idx + 1] << (32 - bit_idx));
            else
                bit_buf = (scalar[u32_idx] >> bit_idx);
        }

        const uint32_t window = carry + (bit_buf & window_mask);
        if ((window & 1) == 0) {
            pos += 1;
            continue;
        }
        output_len = pos + 1;
        if (window < width / 2) {
            carry = 0;
            output[pos] = window;
        } else {
            carry = 1;
            output[pos] = window - width;
        }

        pos += w;
    }
    if (carry) {
        output[pos] = 1;
        output_len = pos + 1;
    }
    if (output_len > 0)
        *out_len = output_len;
    else
        *out_len = 1;
    return 0;
}
