#include <stdbool.h>

#include <FreeRTOS.h>
#include <semphr.h>

#include <mbedtls/bignum.h>
#include <mbedtls_port_bignum.h>
#include <mbedtls/platform.h>

#include "bignum_ext.h"
#include "hw_common.h"

// Enable large number exp_mod to support RSA-4096, etc operations
#define BL_MPI_LARGE_NUM_SOFTWARE_MPI

#define MBEDTLS_INTERNAL_VALIDATE_RET( cond, ret )  \
    do {                                            \
        if( !(cond) )                               \
        {                                           \
            return( ret );                          \
        }                                           \
    } while( 0 )
#define MPI_VALIDATE_RET( cond )                                       \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_MPI_BAD_INPUT_DATA )

#ifdef HW_PORTED

int bl_mpi_exp_mod_wo_lock(mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *E, const mbedtls_mpi *N, mbedtls_mpi *_RN)
{
    int ret;
    uint8_t *tmp;
    int wsize;
    int i, j;
    MPI_VALIDATE_RET(X != NULL);
    MPI_VALIDATE_RET(A != NULL);
    MPI_VALIDATE_RET(E != NULL);
    MPI_VALIDATE_RET(N != NULL);

    const size_t a_words = mpi_words(A);
    const size_t e_words = mpi_words(E);
    const size_t n_words = mpi_words(N);

    const size_t m_words = HW_MAX(a_words, HW_MAX(e_words, n_words));

    const int reg_size = mpi_words_to_reg_size(m_words);
    const int words = mpi_reg_size_to_words(reg_size);
    const int reg_size_N = mpi_words_to_reg_size(n_words);
    const int words_N = mpi_reg_size_to_words(reg_size_N);
    const int reg_size_E = mpi_words_to_reg_size(e_words);
    const int words_E = mpi_reg_size_to_words(reg_size_E);
    const int d_reg_size = mpi_words_to_reg_size(mpi_reg_size_to_words(reg_size) * 2);
    mbedtls_mpi PrimeN_new;
    mbedtls_mpi *PrimeN = &PrimeN_new;
    int reg_idx_E;
    const int e_bitlen = mbedtls_mpi_bitlen(E);
    int e_bit_idx;
    bool first = true;
    const int sN = reg_size_N;

    // TODO more precise check
    if (reg_size > SEC_ENG_PKA_REG_SIZE_256) {
#ifdef BL_MPI_LARGE_NUM_SOFTWARE_MPI
        return mbedtls_mpi_exp_mod_original(X, A, E, N, _RN);
#else
        return MBEDTLS_ERR_MPI_NOT_ACCEPTABLE;
#endif
    }

    if (mbedtls_mpi_cmp_int(N, 0) <= 0 || (N->p[0] & 1) == 0)
        return MBEDTLS_ERR_MPI_BAD_INPUT_DATA;

    if (mbedtls_mpi_cmp_int(E, 0) < 0)
        return MBEDTLS_ERR_MPI_BAD_INPUT_DATA;

    if (mbedtls_mpi_cmp_int(E, 0) == 0) {
        return mbedtls_mpi_lset(X, 1);
    }

    if ((ret = mbedtls_mpi_grow(X, words))) {
        return ret;
    }

    if ((tmp = mbedtls_calloc(1, words * 4)) == NULL) {
        return MBEDTLS_ERR_MPI_ALLOC_FAILED;
    }

    Sec_Eng_PKA_Reset();
    Sec_Eng_PKA_BigEndian_Enable();

    mbedtls_mpi_init(&PrimeN_new);

    if (_RN == NULL || _RN->p == NULL) {
        MBEDTLS_MPI_CHK(mpi_hensel_quad_mod_inv_prime_n(&PrimeN_new, N, words_N * 32));
        if (_RN != NULL) {
            MBEDTLS_MPI_CHK(mbedtls_mpi_copy(_RN, PrimeN));
        }
    } else {
        PrimeN = _RN;
    }

    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(N, tmp, words_N * 4));
    Sec_Eng_PKA_Write_Data(sN, 0, (uint32_t *)tmp, words_N, 1);
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(A, tmp, words * 4));
    Sec_Eng_PKA_Write_Data(reg_size, 2, (uint32_t *)tmp, words, 1);
    Sec_Eng_PKA_GF2Mont(sN, 3, reg_size, 2, words_N * 32, d_reg_size, 2, sN, 0);

    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(PrimeN, tmp, words_N * 4));
    Sec_Eng_PKA_Write_Data(sN, 1, (uint32_t *)tmp, words_N, 1);

    i = e_bitlen;
    wsize = i > 79 ? 4 : i > 23 ? 3 : 1;
    if (words_N > 128 / 4 && wsize > 1)
        wsize = 3;

    if (wsize > 1) {
        int ri = 5;
        Sec_Eng_PKA_Move_Data(sN, 4, sN, 3, 1);
        Sec_Eng_PKA_MSQR(sN, 3, sN, 3, sN, 0, 1);

        j = (1 << (wsize - 1)) - 1;
        for (i = 0; i < j; ++i, ++ri)
            Sec_Eng_PKA_MMUL(sN, ri, sN, ri - 1, sN, 3, sN, 0, 1);
    } else {
        reg_idx_E = (4 * words_N + words_E - 1) / words_E;
        MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(E, tmp, words_E * 4));
        Sec_Eng_PKA_Write_Data(reg_size_E, reg_idx_E, (uint32_t *)tmp, words_E, 1);

        Sec_Eng_PKA_MEXP(sN, 2, sN, 3, reg_size_E, reg_idx_E, sN, 0, 1);
        goto cvt_back;
    }

    e_bit_idx = e_bitlen - 1;
    while (e_bit_idx >= 0) {
        if (mbedtls_mpi_get_bit(E, e_bit_idx) == 0) {
            Sec_Eng_PKA_MSQR(sN, 2, sN, 2, sN, 0, 1);
            e_bit_idx--;
        } else {
            int bitstring_len = wsize;
            int bitstring_val = 0;
            if (e_bit_idx + 1 < bitstring_len)
                bitstring_len = e_bit_idx + 1;

            while (bitstring_len > 0) {
                if (mbedtls_mpi_get_bit(E, e_bit_idx - bitstring_len + 1))
                    break;
                bitstring_len--;
            }
            for (i = 0; i < bitstring_len; ++i) {
                int cur_bit = mbedtls_mpi_get_bit(E, e_bit_idx - i);
                if (i == 0)
                    bitstring_val = cur_bit;
                else
                    bitstring_val = (bitstring_val << 1) + cur_bit;
                if (!first)
                    Sec_Eng_PKA_MSQR(sN, 2, sN, 2, sN, 0, 1);
            }
            i = ((bitstring_val - 1) >> 1)  + 4;
            if (first) {
                Sec_Eng_PKA_Move_Data(sN, 2, sN, i, 1);
                first = false;
            } else {
                Sec_Eng_PKA_MMUL(sN, 2, sN, 2, sN, i, sN, 0, 1);
            }

            e_bit_idx -= bitstring_len;
        }
    }

cvt_back:
    memset(tmp, 0, words_N * 4);
    tmp[words_N * 4 - 1] = 1;
    Sec_Eng_PKA_Write_Data(sN, 3, (uint32_t *)tmp, words_N, 1);
    Sec_Eng_PKA_MMUL(sN, 4, sN, 2, sN, 3, sN, 0, 1);
    Sec_Eng_PKA_Read_Data(sN, 4, (uint32_t *)tmp, words_N);
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(X, tmp, words_N * 4));

    // Compensate for negative X
    if (A->s == -1 && (E->p[0] & 1) != 0) {
        X->s = -1;
        MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(X, N, X));
    } else {
        X->s = 1;
    }

cleanup:
    mbedtls_free(tmp);
    mbedtls_mpi_free(&PrimeN_new);
    return ret;
}

int bl_mpi_exp_mod(mbedtls_mpi *X, const mbedtls_mpi *A, const mbedtls_mpi *E, const mbedtls_mpi *N, mbedtls_mpi *_RN)
{
    int ret;
    if (bl_sec_pka_mutex_take())
        return -1;
    ret = bl_mpi_exp_mod_wo_lock(X, A, E, N, _RN);
    if (bl_sec_pka_mutex_give())
        return -1;
    return ret;
}

#endif // HW_PORTED
