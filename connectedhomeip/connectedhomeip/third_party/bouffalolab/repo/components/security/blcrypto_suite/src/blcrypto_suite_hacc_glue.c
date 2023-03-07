#include <blcrypto_suite/blcrypto_suite_top_config.h>
#include <blcrypto_suite/blcrypto_suite_hacc_glue.h>

#if USE_HWCRYPTO
/*
 * Multiplication R = m * P
 */
int blcrypto_suite_hacc_ecp_mul(blcrypto_suite_ecp_group *grp, blcrypto_suite_ecp_point *R, const blcrypto_suite_mpi *m, const blcrypto_suite_ecp_point *P, int (*f_rng)(void *, unsigned char *, size_t), void *p_rng)
{
    int ret;
    uint32_t PxMont[8], PyMont[8];

    blcrypto_suite_mpi *mpi_mod;
    uint32_t modulo[8];
    uint32_t tmp[8];

    uint32_t Rx[8], Ry[8];

    if (!(grp->id == BLCRYPTO_SUITE_ECP_DP_SECP256R1)) {
        GLUE_DBG("ECP group NOT secp256r1!\r\n");
        return -1;
    }

    mpi_mod = &grp->P;
    if ((ret = blcrypto_suite_mpi_write_binary(mpi_mod, (unsigned char *)modulo, sizeof(modulo)))) {
        GLUE_DBG("READ P failed\r\n");
        return -1;
    }

    blcrypto_suite_hacc_reset();

    if ((ret = blcrypto_suite_mpi_write_binary(&P->X, (unsigned char *)tmp, sizeof(tmp)))) {
        GLUE_DBG("READ P->X failed\r\n");
        return -1;
    }
    blcrypto_suite_hacc_gf2mont_256_bin(PxMont, tmp, modulo);

    if ((ret = blcrypto_suite_mpi_write_binary(&P->Y, (unsigned char *)tmp, sizeof(tmp)))) {
        GLUE_DBG("READ P->Y failed\r\n");
        return -1;
    }
    blcrypto_suite_hacc_gf2mont_256_bin(PyMont, tmp, modulo);

    if ((ret = blcrypto_suite_mpi_write_binary(m, (unsigned char *)tmp, sizeof(tmp)))) {
        GLUE_DBG("READ m failed\r\n");
        return -1;
    }

    blcrypto_suite_hacc_reset();
    blcrypto_suite_hacc_secp256r1_mul_bin(0, tmp, PxMont, PyMont, Rx, Ry);

    blcrypto_suite_mpi_read_binary(&R->X, (uint8_t *)Rx, sizeof(Rx));
    blcrypto_suite_mpi_read_binary(&R->Y, (uint8_t *)Ry, sizeof(Ry));
    blcrypto_suite_mpi_lset(&R->Z, 1);

    return 0;
}

int blcrypto_suite_hacc_mpi_exp_mod_secp256r1(blcrypto_suite_mpi *X, const blcrypto_suite_mpi *A, const blcrypto_suite_mpi *E)
{
    int ret;
    uint32_t result[8];
    uint32_t A_bin[8], E_bin[8];

    if ((ret = blcrypto_suite_mpi_write_binary(A, (uint8_t *)A_bin, sizeof(A_bin)))) {
        GLUE_DBG("READ A failed\r\n");
        return -1;
    }
    if ((ret = blcrypto_suite_mpi_write_binary(E, (uint8_t *)E_bin, sizeof(E_bin)))) {
        GLUE_DBG("READ E failed\r\n");
        return -1;
    }

    blcrypto_suite_hacc_reset();
    blcrypto_suite_hacc_secp256r1_exp_mod(result, A_bin, E_bin);

    blcrypto_suite_mpi_read_binary(X, (uint8_t *)result, sizeof(result));

    return 0;
}
#endif
