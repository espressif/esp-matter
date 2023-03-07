/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "cc_pal_mem.h"
#include "cc_pal_mutex.h"
#include "cc_pal_abort.h"
#include "cc_common.h"
#include "cc_common_math.h"
#include "cc_ecpki_types.h"
#include "cc_ecpki_error.h"
#include "cc_ecpki_local.h"
#include "pka_hw_defs.h"
#include "pki.h"
#include "pka.h"
#include "pka_ec_wrst.h"
#include "ec_wrst_error.h"
#include "pka_ec_wrst_dsa_verify_regs.h"
#include "pka_ec_wrst_glob_regs.h"

extern const CCEcpkiDomain_t gEcDomans[];

/***********    PkaEcdsaVerify   function      **********************/
/**
 * @brief This function performs verification of ECDSA signature using PKA.
 *
 * 1. Compute  h = d^-1,  h1 = f*h mod r,  h2 = c*h mod r.
 * 2. Compute  P(Xp,Yp) =  h1*G  + h2*W; c1 = Px mod r
 * 3. Compare  If  c1 != c,  then output "Invalid", else - "valid".
 *
 * Assuming: - PKA is initialized, all data is set into SRAM.
 *
 * @author reuvenl (8/7/2014)
 *
 * @return  - On success CC_OK is returned, on failure an error code.
 */
CCError_t PkaEcdsaVerify(void)
{
    CCError_t err = CC_OK;
    int32_t modSizeInBits, ordSizeInBits;
    uint32_t status1, status2;

    /* Get sizes */
    ordSizeInBits = CC_HAL_READ_REGISTER(CC_REG_OFFSET (CRY_KERNEL, PKA_L0));
    modSizeInBits = CC_HAL_READ_REGISTER(CC_REG_OFFSET (CRY_KERNEL, PKA_L2));

    /*  1. If  C or D are not in interval [1,r-1] then output "invalid"        */
    /* temporary set ECC_REG_N = ECC_REG_N - 1 for the following checking */
    PKA_FLIP_BIT0(LEN_ID_N_PKA_REG_BITS, ECC_REG_N, ECC_REG_N);

    /* check C */
    PKA_SUB_IM(LEN_ID_N_PKA_REG_BITS, RES_DISCARD, EC_VERIFY_REG_C, 1/*imm*/);
    PKA_GET_STATUS_CARRY(status1); /* if EC_VERIFY_REG_C >= 1, then status = 0 */
    PKA_SUB(LEN_ID_N_PKA_REG_BITS, RES_DISCARD, ECC_REG_N, EC_VERIFY_REG_C);
    PKA_GET_STATUS_CARRY(status2); /* if EC_VERIFY_REG_C <= ECC_REG_N, then status = 1 */
    if (status1 == 0 || status2 == 0) {
        err = ECWRST_DSA_VERIFY_CALC_SIGN_C_INVALID_ERROR;
        goto End;
    }

    /* check D */
    PKA_SUB_IM(LEN_ID_N_PKA_REG_BITS, RES_DISCARD, EC_VERIFY_REG_D, 1/*imm*/);
    PKA_GET_STATUS_CARRY(status1); /* if EC_VERIFY_REG_D >= 1, then status = 0 */
    PKA_SUB(LEN_ID_N_PKA_REG_BITS, RES_DISCARD, ECC_REG_N, EC_VERIFY_REG_D);
    PKA_GET_STATUS_CARRY(status2); /* if EC_VERIFY_REG_D <= EC_VERIFY_REG_R, then status = 1 */
    if (status1 == 0 || status2 == 0) {
        err = ECWRST_DSA_VERIFY_CALC_SIGN_D_INVALID_ERROR;
        goto End;
    }

    /* restore ECC_REG_N  */
    PKA_FLIP_BIT0(LEN_ID_N_PKA_REG_BITS, ECC_REG_N, ECC_REG_N);

    /* 2. Calculate h, h1, h2 and normalize EC_VERIFY_REG_F     */
    /* 2.1. h = d^-1  mod r */
    PKA_MOD_INV_W_EXP(EC_VERIFY_REG_H, EC_VERIFY_REG_D, EC_VERIFY_REG_TMP);

    PKA_DIV(LEN_ID_N_PKA_REG_BITS, EC_VERIFY_REG_TMP, EC_VERIFY_REG_F/*rem*/, ECC_REG_N/*div*/);
    /* 2.2. h1 = f*h  mod r */
    PKA_MOD_MUL(LEN_ID_N_BITS, EC_VERIFY_REG_H1/*Res*/, EC_VERIFY_REG_F/*OpA*/, EC_VERIFY_REG_H/*OpB*/);
    /* 2.3. h2 = c*h mod r  */
    PKA_MOD_MUL(LEN_ID_N_BITS, EC_VERIFY_REG_H2/*Res*/, EC_VERIFY_REG_C/*OpA*/, EC_VERIFY_REG_H/*OpB*/);

    /* set PKA for operations according to ECC modulus    */
    PKA_CLEAR(LEN_ID_N_PKA_REG_BITS, PKA_REG_T0);
    PKA_CLEAR(LEN_ID_N_PKA_REG_BITS, PKA_REG_T1);
    PKA_WAIT_ON_PKA_DONE();
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET (CRY_KERNEL, PKA_L0), modSizeInBits);
    PKA_COPY(LEN_ID_N_PKA_REG_BITS, EC_VERIFY_REG_TMP, ECC_REG_N);
    PKA_COPY(LEN_ID_N_PKA_REG_BITS, ECC_REG_N, EC_VERIFY_REG_TMP_N);
    PKA_COPY(LEN_ID_N_PKA_REG_BITS, EC_VERIFY_REG_TMP_N, EC_VERIFY_REG_TMP); //swap mod<->ord
    PKA_COPY(LEN_ID_N_PKA_REG_BITS, ECC_REG_NP, EC_VERIFY_REG_TMP_NP);

        /* Auxiliary values: rn_X = X*ECC_REG_N */
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_N4 , ECC_REG_N,   ECC_REG_N  );
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_N4 , ECC_REG_N4, ECC_REG_N4);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_N8 , ECC_REG_N4, ECC_REG_N4);
    PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_N12, ECC_REG_N8, ECC_REG_N4);

    /* 3. Compute EC point  P1 =  h1*G + h2*W by mod P    */
    err = PkaSum2ScalarMullt(EC_VERIFY_REG_P_RX,
        EC_VERIFY_REG_P_RY,
        EC_VERIFY_REG_H1,
        EC_VERIFY_REG_P_GX,
        EC_VERIFY_REG_P_GY,
        EC_VERIFY_REG_H2,
        EC_VERIFY_REG_P_WX,
        EC_VERIFY_REG_P_WY);
    if(err)
        goto End;

    /* 4. Normalize: C' = pRx mod r. Compare C' == C              */
    PKA_WAIT_ON_PKA_DONE();
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET (CRY_KERNEL, PKA_L0), ordSizeInBits);
    PKA_DIV(LEN_ID_N_PKA_REG_BITS, EC_VERIFY_REG_TMP, EC_VERIFY_REG_P_RX/*rem*/, EC_VERIFY_REG_TMP_N/*div*/);
    PKA_COMPARE_STATUS(LEN_ID_N_PKA_REG_BITS, EC_VERIFY_REG_P_RX, EC_VERIFY_REG_C, status1);
    if (status1 != 1) {
        err = ECWRST_DSA_VERIFY_CALC_SIGNATURE_IS_INVALID;
    }
End:
    return err;

}


