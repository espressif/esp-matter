/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "cc_pal_mem.h"
#include "cc_pal_types.h"
#include "cc_hal_plat.h"
#include "cc_common_math.h"
#include "cc_ecpki_error.h"
#include "pka_hw_defs.h"
#include "pka_error.h"
#include "pki.h"
#include "pka.h"
#include "pka_ec_wrst.h"
#include "ec_wrst.h"
#include "ec_wrst_error.h"
#include "pki_modular_arithmetic.h"
#include "pki_dbg.h"
#include "pka_ec_wrst_glob_regs.h"
#include "pka_ec_wrst_dsa_verify_regs.h"


/***********   PkaDoubleMdf2Mdf    function      **********************/
/**
 * @brief EC point doubling: p = 2*p1  modified-modified.
 *
 * @return  - On success CC_OK is returned, on failure an error code.
 */
void PkaDoubleMdf2Mdf(
             const uint32_t x,  const uint32_t y,  const uint32_t z,  const uint32_t t,  /*!< [out] Output modified point coordinates. */
             const uint32_t x1, const uint32_t y1, const uint32_t z1, const uint32_t t1) /*!< [in] Input modified point coordinates. */
{
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_T4, y1, y1);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, z, ECC_REG_T4, z1);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, y, y1, y1);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_T4, x1, x1);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_T4, ECC_REG_T4, ECC_REG_T4);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, ECC_REG_T4, y, ECC_REG_T4);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, ECC_REG_T2, x1, x1);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, x, ECC_REG_T2, ECC_REG_T2);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_T2, ECC_REG_T2, x);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_T2, t1, ECC_REG_T2);
    PKA_SUB(LEN_ID_N_PKA_REG_BITS, ECC_REG_T4, ECC_REG_N4, ECC_REG_T4);
    PKA_MOD_MUL_ACC_NFR(LEN_ID_N_BITS, x, ECC_REG_T2, ECC_REG_T2, ECC_REG_T4);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, x, ECC_REG_T4, x);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_T4, x, ECC_REG_T4);
    PKA_SUB(LEN_ID_N_PKA_REG_BITS, ECC_REG_T3, ECC_REG_N12, ECC_REG_T4);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, y, y, y);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, y, y, y);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, y, y, y);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_T4, y, y);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, ECC_REG_T4, ECC_REG_T4, t1);
    PKA_SUB(LEN_ID_N_PKA_REG_BITS, y, ECC_REG_N8, y);
    PKA_MOD_MUL_ACC_NFR(LEN_ID_N_BITS, y, ECC_REG_T3, ECC_REG_T2, y);
    PKA_COPY(LEN_ID_N_PKA_REG_BITS, t, ECC_REG_T4);
    return;
}

/***********    PkaDoubleMdf2Jcb   function      **********************/
/**
 * @brief EC point doubling: p = 2*p1  p1 is modified, output is Jacobian
 *
 * @return  - no return value.
 */
void PkaDoubleMdf2Jcb(
             const uint32_t x,  const uint32_t y,  const uint32_t z, /*!< [out] EC point jacobian coordinates. */
             const uint32_t x1, const uint32_t y1, const uint32_t z1, const uint32_t t1)/*!< [in] EC point modified coordinates. */
{
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_T, y1, y1);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, z, ECC_REG_T, z1);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, y, y1, y1);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_T, x1, x1);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_T, ECC_REG_T, ECC_REG_T);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, ECC_REG_T, y, ECC_REG_T);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, ECC_REG_T2, x1, x1);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, x, ECC_REG_T2, ECC_REG_T2);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_T2, ECC_REG_T2, x);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_T2, t1, ECC_REG_T2);
    PKA_SUB(LEN_ID_N_PKA_REG_BITS, ECC_REG_T, ECC_REG_N4, ECC_REG_T);
    PKA_MOD_MUL_ACC_NFR(LEN_ID_N_BITS, x, ECC_REG_T2, ECC_REG_T2, ECC_REG_T);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, x, ECC_REG_T, x);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_T, x, ECC_REG_T);
    PKA_SUB(LEN_ID_N_PKA_REG_BITS, ECC_REG_T3, ECC_REG_N12, ECC_REG_T);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, y, y, y);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, y, y, y);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, y, y, y);
    PKA_SUB(LEN_ID_N_PKA_REG_BITS, y, ECC_REG_N8, y);
    PKA_MOD_MUL_ACC_NFR(LEN_ID_N_BITS, y, ECC_REG_T3, ECC_REG_T2, y);
    return;
}

/***********    PkaAddJcbAfn2Mdf   function      **********************/
/**
 * @brief  Performs adding of EC points p= p2+p1: affine+jacobian=modified.
 *
 * @return  - no return value.
 */
void PkaAddJcbAfn2Mdf(
             const uint32_t x,  const uint32_t y,  const uint32_t z, const uint32_t t, /*!< [out] EC point modified coordinates. */
             const uint32_t x1, const uint32_t y1, const uint32_t z1,  /*!< [in] EC point jacobian coordinates. */
             const uint32_t x2, const uint32_t y2) /*!< [in] EC point affine coordinates. */
{
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, t, z1, z1);
    PKA_SUB(LEN_ID_N_PKA_REG_BITS, x, ECC_REG_N12, x1);
    PKA_MOD_MUL_ACC_NFR(LEN_ID_N_BITS, ECC_REG_T1, x2, t, x);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, t, z1, t);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, t, y2, t);
    PKA_SUB(LEN_ID_N_PKA_REG_BITS, t, ECC_REG_N4, t);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, t, y1, t);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, z, z1, ECC_REG_T1);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, ECC_REG_T2, ECC_REG_T1, ECC_REG_T1);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, ECC_REG_T1, ECC_REG_T1, ECC_REG_T2);
    PKA_SUB(LEN_ID_N_PKA_REG_BITS, ECC_REG_T1, ECC_REG_N4, ECC_REG_T1);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, y, ECC_REG_T1, y1);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, ECC_REG_T2, x, ECC_REG_T2);
    PKA_MOD_MUL_ACC_NFR(LEN_ID_N_BITS, x, t, t, ECC_REG_T1);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, x, ECC_REG_T2, x);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, x, ECC_REG_T2, x);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_T2, x, ECC_REG_T2);
    PKA_MOD_MUL_ACC_NFR(LEN_ID_N_BITS, y, t, ECC_REG_T2, y);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, t, z, z);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, t, t, t);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, t, ECC_REG_EC_A, t);
    return;
}

/***********    PkaJcb2Afn   function      **********************/
/**
 * @brief Converts the jacobian EC point to affine representation:
 *   p(x,y,z) -> p(x,y)
 *
 * @return  - On success CC_OK is returned, on failure an error code.
 */
void PkaJcb2Afn(
           CCEcpkiScaProtection_t scaProtect,  /*!< [in] Flag defining if SCA protection needed (1) or not (0). */
           const uint32_t x, const uint32_t y, /*!< [in/out] as input - EC point jacobian coordinates;
                                   as output - EC point affine coordinates */
           const uint32_t z) /*!< [in] EC point jacobian coordinates. */
{
// RL check is the pka_inv_fast works right and delete compilation dependence
#ifndef INV_FAST_ALLOWED
    scaProtect = SCAP_Active;
#endif
    if (scaProtect == SCAP_Inactive) {
        PKA_MOD_INV(LEN_ID_N_BITS, ECC_REG_AQ, z); // no SCA protect
    } else {
        PKA_MOD_INV_W_EXP(ECC_REG_AQ, z, ECC_REG_A_NM2); // SCA protect
    }

    /* ecc-to-affine */
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, y, y, ECC_REG_AQ);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, ECC_REG_AQ, ECC_REG_AQ, ECC_REG_AQ);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, x, x, ECC_REG_AQ);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, y, y, ECC_REG_AQ);

    PKA_REDUCE(LEN_ID_N_BITS,x, x);
    PKA_REDUCE(LEN_ID_N_BITS,y, y);

    return;
}


/***********    PkaAddAff   function      **********************/
/**
 * @brief pka ecc points adding: affine+affine=affine
 *
 * @return  - No return value
 */
void PkaAddAff(
          const uint32_t x,  const uint32_t y,  /*!< [out] EC point Affine coordinates. */
          const uint32_t x1, const uint32_t y1, /*!< [in] EC point Affine coordinates. */
          const uint32_t x2, const uint32_t y2) /*!< [in] EC point Affine coordinates. */
{
    PKA_SUB(LEN_ID_N_PKA_REG_BITS, x, ECC_REG_N1, x1);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_AAA_Z, x, x2);
    PKA_SUB(LEN_ID_N_PKA_REG_BITS, ECC_REG_T, ECC_REG_N1, y2);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_T, y1, ECC_REG_T);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, ECC_REG_T2, ECC_REG_AAA_Z, ECC_REG_AAA_Z);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, ECC_REG_T1, ECC_REG_AAA_Z, ECC_REG_T2);
    PKA_SUB(LEN_ID_N_PKA_REG_BITS, ECC_REG_T1, ECC_REG_N4, ECC_REG_T1);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, y, ECC_REG_T1, y1);
    PKA_MOD_MUL_NFR(LEN_ID_N_BITS, ECC_REG_T2, x, ECC_REG_T2);
    PKA_MOD_MUL_ACC_NFR(LEN_ID_N_BITS, x, ECC_REG_T, ECC_REG_T, ECC_REG_T1);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, x, ECC_REG_T2, x);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, x, ECC_REG_T2, x);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_T2, x, ECC_REG_T2);
    PKA_MOD_MUL_ACC_NFR(LEN_ID_N_BITS, y, ECC_REG_T, ECC_REG_T2, y);
    PkaJcb2Afn(SCAP_Inactive, x, y, ECC_REG_AAA_Z);

    return;
}

/***********    PkaSum2ScalarMullt   function      **********************/
/**
 * @brief The function calculates simultaneously summ of two scalar
 * multiplications of EC points.
 *
 * Used the Strauss algorithm, optimized by A.Klimow:
 *     R = a*P + b*Q, where R,P,Q - EC points, a,b - scalars.
 *
 * @author reuvenl (8/26/2014)
 *
 * @return  - On success CC_OK is returned, on failure an error code.
 */
uint32_t PkaSum2ScalarMullt(
               const uint32_t xr, const uint32_t yr, /*!< [out] Pka register holding R coordinates. */
               const uint32_t a,             /*!< [in] Pka register holding scalar a. */
               const uint32_t xp, const uint32_t yp, /*!< [in] Pka register holding P coordinates. */
               const uint32_t b,             /*!< [in] Pka register holding scalar b. */
               const uint32_t xq, const uint32_t yq) /*!< [in] Pka register holding Q coordinates. */
{
    uint32_t wA, wB, err = 0;
    uint32_t stat;
    int32_t b2, i;
    uint32_t isNewA=true, isNewB=true;

    /* check that a>0 and b>0 */
    PKA_COMPARE_IM_STATUS(LEN_ID_N_PKA_REG_BITS, a, 0, stat);
    if (stat == 1) {
        err = ECWRST_DSA_VERIFY_2MUL_FACTOR_A_NULL_ERROR;
        goto End;
    }
    PKA_COMPARE_IM_STATUS(LEN_ID_N_PKA_REG_BITS, b, 0, stat);
    if (stat == 1) {
        err = ECWRST_DSA_VERIFY_2MUL_FACTOR_B_NULL_ERROR;
        goto End;
    }

    /* get max effective size of factors minus 1 */
    i = CC_MAX(PkaGetRegEffectiveSizeInBits(a), PkaGetRegEffectiveSizeInBits(b)) - 1;

    PkaAddAff(EC_VERIFY_REG_XPQ, EC_VERIFY_REG_YPQ, xp,yp, xq,yq); // p+q

#ifdef ARM_DSM
    *((volatile uint32_t *)(uint32_t)(0x44440000)) = i;
#endif

    b2 = PkaGetNextMsBit(a, i, &wA, &isNewA)*2 + PkaGetNextMsBit(b, i, &wB, &isNewB);
    switch (b2) {
    case 1:
        PKA_COPY(LEN_ID_N_PKA_REG_BITS, xr, xq);
        PKA_COPY(LEN_ID_N_PKA_REG_BITS, yr, yq);
        break; // 01: r = q
    case 2:
        PKA_COPY(LEN_ID_N_PKA_REG_BITS, xr, xp);
        PKA_COPY(LEN_ID_N_PKA_REG_BITS, yr, yp);
        break; // 10: r = p
    case 3:
        PKA_COPY(LEN_ID_N_PKA_REG_BITS, xr, EC_VERIFY_REG_XPQ);
        PKA_COPY(LEN_ID_N_PKA_REG_BITS, yr, EC_VERIFY_REG_YPQ);
        break; // 11: r = p+q
    default:
        err = ECWRST_DSA_VERIFY_2MUL_FIRST_B2_ERROR;
        goto End;
    }
    PKA_SET_VAL(EC_VERIFY_REG_ZR, 1);
    PKA_COPY(LEN_ID_N_PKA_REG_BITS, EC_VERIFY_REG_TR, ECC_REG_EC_A);

    while (--i >= 0) {
#ifdef ARM_DSM
        *((volatile uint32_t *)(uint32_t)(0x44440000)) = i;
#endif
        b2 = PkaGetNextMsBit(a, i, &wA, &isNewA)*2 + PkaGetNextMsBit(b, i, &wB, &isNewB);
        if (b2 == 0) {
            PkaDoubleMdf2Mdf(xr,yr,EC_VERIFY_REG_ZR,EC_VERIFY_REG_TR, xr,yr,EC_VERIFY_REG_ZR,EC_VERIFY_REG_TR);
        } else {
            PkaDoubleMdf2Jcb(xr,yr,EC_VERIFY_REG_ZR, xr,yr,EC_VERIFY_REG_ZR,EC_VERIFY_REG_TR);
            switch (b2) {
            case 1:
                PkaAddJcbAfn2Mdf(xr,yr,EC_VERIFY_REG_ZR,EC_VERIFY_REG_TR, xr,yr,EC_VERIFY_REG_ZR, xq,yq);
                break; // 01: r += p
            case 2:
                PkaAddJcbAfn2Mdf(xr,yr,EC_VERIFY_REG_ZR,EC_VERIFY_REG_TR, xr,yr,EC_VERIFY_REG_ZR, xp,yp);
                break; // 10: r += q
            case 3:
                PkaAddJcbAfn2Mdf(xr,yr,EC_VERIFY_REG_ZR,EC_VERIFY_REG_TR, xr,yr,EC_VERIFY_REG_ZR, EC_VERIFY_REG_XPQ,EC_VERIFY_REG_YPQ);
                break; // 11: r += p+q
            default:
                err = ECWRST_DSA_VERIFY_2MUL_NEXT_B2_ERROR;
                goto End;
            }
        }
    }
    PkaJcb2Afn(SCAP_Inactive, xr, yr, EC_VERIFY_REG_ZR);

    End:
    return err;
}


