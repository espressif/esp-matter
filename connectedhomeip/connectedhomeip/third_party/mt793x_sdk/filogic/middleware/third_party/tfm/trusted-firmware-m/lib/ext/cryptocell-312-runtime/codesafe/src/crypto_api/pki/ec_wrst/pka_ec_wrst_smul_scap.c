/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "cc_pal_mem.h"
#include "cc_pal_mutex.h"
#include "cc_pal_abort.h"
#include "cc_common_math.h"
#include "cc_ecpki_types.h"
#include "cc_ecpki_error.h"
#include "cc_ecpki_local.h"
#include "pka_ec_wrst.h"
#include "pki.h"
#include "ec_wrst_error.h"
#include "ec_wrst.h"
#include "pka_ec_wrst_glob_regs.h"
#include "pka_ec_wrst_dsa_sign_regs.h"

extern const int8_t regTemps[PKA_MAX_COUNT_OF_PHYS_MEM_REGS];
/***********    DoubleMdf2Mdf   function      **********************/
/**
 * @brief EC point doubling: p = 2*p1  modified-modified.
 *
 * All parameters are ID-s of PKA registers, containing the data.
 *
 * Part of PKA registers are implicitly defined in pka_ec_wrst_glob_regs.h file
*
 * \param x,y,z,t - output point coordinates
*  \param x,y,z,t - input point coordinates
*
 * @return  - On success CC_OK is returned, on failure an error code.
 */
/* static */void DoubleMdf2Mdf(
           const uint32_t x,  const uint32_t y,  const uint32_t z,  const uint32_t t,  /*!< [in] Pointer to the public key structure. */
           const uint32_t x1, const uint32_t y1, const uint32_t z1, const uint32_t t1) /*!< [in] Pointer to the public key structure. */
{ // t cannot be aliased
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, t, y1, y1);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, z, t, z1);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, y, y1, y1);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, t, x1, x1);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, t, t, t);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, t, y, t);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, ECC_REG_T2, x1, x1);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, x, ECC_REG_T2, ECC_REG_T2);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_T2, ECC_REG_T2, x);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_T2, t1, ECC_REG_T2);
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, t, ECC_REG_N4, t);
        PKA_MOD_MUL_ACC_NFR(LEN_ID_N_BITS, x, ECC_REG_T2, ECC_REG_T2, t);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, x, t, x);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, t, x, t);
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, ECC_REG_T3, ECC_REG_N12, t);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, y, y, y);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, y, y, y);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, y, y, y);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, t, y, y);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, t, t, t1);
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, y, ECC_REG_N8, y);
        PKA_MOD_MUL_ACC_NFR(LEN_ID_N_BITS, y, ECC_REG_T3, ECC_REG_T2, y);
        return;
}

/***********    PkaAddJcbJcb2Mdf   function      **********************/
/**
 * @brief pka ecc points adding: jacobian+jacobian=modified
 *
 * All parameters are ID-s of PKA registers, containing the data.
 *
 * @return  - no return code.
 */
/* static */void PkaAddJcbJcb2Mdf(
            const uint32_t x,  const uint32_t y,  const uint32_t z, const uint32_t t,  /*!< [out] EC point modified coordinates. */
            const uint32_t x1, const uint32_t y1, const uint32_t z1, /*!< [in] EC point jacobian coordinates. */
            const uint32_t x2, const uint32_t y2, const uint32_t z2) /*!< [in] EC point jacobian coordinates. */
{
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, t, z2, z2);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, x, x1, t);
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, x, ECC_REG_N4, x);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, t, z2, t);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, y, y1, t);
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, y, ECC_REG_N4, y);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, t, z1, z1);
        PKA_MOD_MUL_ACC_NFR(LEN_ID_N_BITS, ECC_REG_T1, x2, t, x);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, t, z1, t);
        PKA_MOD_MUL_ACC_NFR(LEN_ID_N_BITS, t, y2, t, y);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, z, z1, z2);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, z, z, ECC_REG_T1);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, ECC_REG_T2, ECC_REG_T1, ECC_REG_T1);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, ECC_REG_T1, ECC_REG_T1, ECC_REG_T2);
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, ECC_REG_T1, ECC_REG_N4, ECC_REG_T1);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, y, ECC_REG_T1, y);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, ECC_REG_T2, x, ECC_REG_T2);
        PKA_MOD_MUL_ACC_NFR(LEN_ID_N_BITS, x, t, t, ECC_REG_T1);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, x, ECC_REG_T2, x);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, x, ECC_REG_T2, x);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_T2, x, ECC_REG_T2);
        PKA_MOD_MUL_ACC_NFR(LEN_ID_N_BITS, y, t, ECC_REG_T2, y);
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, y, ECC_REG_N4, y);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, t, z, z);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, t, t, t);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, t, ECC_REG_EC_A, t);
        return;
}


/***********    PkaAddJcbJcb2Jcb   function      **********************/
/**
 * @brief pka ecc points adding: jacob+jacob=jacob
 *
 * @return  - no return code.
 */
/* static */void PkaAddJcbJcb2Jcb(
            const uint32_t x,  const uint32_t y,  const uint32_t z,  /*!< [out] EC point jacobian coordinates. */
            const uint32_t x1, const uint32_t y1, const uint32_t z1, /*!< [in] EC point jacobian coordinates. */
            const uint32_t x2, const uint32_t y2, const uint32_t z2) /*!< [in] EC point jacobian coordinates. */
{
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, ECC_REG_T, z2, z2);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, x, x1, ECC_REG_T);
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, x, ECC_REG_N4, x);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, ECC_REG_T, z2, ECC_REG_T);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, y, y1, ECC_REG_T);
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, y, ECC_REG_N4, y);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, ECC_REG_T, z1, z1);
        PKA_MOD_MUL_ACC_NFR(LEN_ID_N_BITS, ECC_REG_T1, x2, ECC_REG_T, x);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, ECC_REG_T, z1, ECC_REG_T);
        PKA_MOD_MUL_ACC_NFR(LEN_ID_N_BITS, ECC_REG_T, y2, ECC_REG_T, y);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, z, z1, z2);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, z, z, ECC_REG_T1);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, ECC_REG_T2, ECC_REG_T1, ECC_REG_T1);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, ECC_REG_T1, ECC_REG_T1, ECC_REG_T2);
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, ECC_REG_T1, ECC_REG_N4, ECC_REG_T1);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, y, ECC_REG_T1, y);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, ECC_REG_T2, x, ECC_REG_T2);
        PKA_MOD_MUL_ACC_NFR(LEN_ID_N_BITS, x, ECC_REG_T, ECC_REG_T, ECC_REG_T1);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, x, ECC_REG_T2, x);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, x, ECC_REG_T2, x);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_T2, x, ECC_REG_T2);
        PKA_MOD_MUL_ACC_NFR(LEN_ID_N_BITS, y, ECC_REG_T, ECC_REG_T2, y);
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, y, ECC_REG_N4, y);
        return;
}


/***********    PkaScalarMultSca   function      **********************/
/**
 * @brief EC scalar multiplication p = k*p, SCA-resistant
 *
 *  Implemented algorithm, enhanced by A.Klimov
 *
 * @return  - no return code.
 */
/* static */void PkaScalarMultSca(void)
{
        uint32_t isK;
        uint32_t sz1, sz2, sz, b2;
        uint32_t W;
        int32_t i, carry = 0; // always 0 or -1
        uint32_t isNew;

        /* calc. globals */
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_N4 , ECC_REG_N,   ECC_REG_N  );
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_N4 , ECC_REG_N4, ECC_REG_N4);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_N8 , ECC_REG_N4, ECC_REG_N4);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, ECC_REG_N12, ECC_REG_N8, ECC_REG_N4);

        // To mask the size of k we calculate either k*p or -(-k)*p
        PKA_COPY(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_TP, EC_SIGN_REG_RK);
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_ZP, EC_SIGN_REG_ORD, EC_SIGN_REG_TP); // EC_SIGN_REG_ZP is -k
        sz1 = PkaGetRegEffectiveSizeInBits(EC_SIGN_REG_RK);
        sz2 = PkaGetRegEffectiveSizeInBits(EC_SIGN_REG_ZP);
        /* chose k or -k to mask size of scalar */
        if (sz1 > sz2) {
                sz = sz1;
                isK = 1;
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_RK, EC_SIGN_REG_TP); // Used k
        } else {
                sz = sz2;
                isK = 0;
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_RK, EC_SIGN_REG_ZP); // Used -k
        }

        PKA_SET_VAL(EC_SIGN_REG_ZP, 1); // or random and adjust EC_SIGN_REG_XP, EC_SIGN_REG_YP, EC_SIGN_REG_TP

        DoubleMdf2Mdf(EC_SIGN_REG_X2,EC_SIGN_REG_Y2,EC_SIGN_REG_Z2,EC_SIGN_REG_T2, EC_SIGN_REG_XP,EC_SIGN_REG_YP,EC_SIGN_REG_ZP,ECC_REG_EC_A); // 2p
        DoubleMdf2Mdf(EC_SIGN_REG_X4,EC_SIGN_REG_Y4,EC_SIGN_REG_Z4,EC_SIGN_REG_T4, EC_SIGN_REG_X2,EC_SIGN_REG_Y2,EC_SIGN_REG_Z2,EC_SIGN_REG_T2);    // 4p

        i = ((sz+1) & ~1) - 2; // round size up to even

        isNew = 1;
        b2 = PkaGet2MsBits(EC_SIGN_REG_RK, i, &W, &isNew);

        switch (b2) {
        case 1:
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_XS, EC_SIGN_REG_X2);
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_YS, EC_SIGN_REG_Y2);
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_ZS, EC_SIGN_REG_Z2);
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_TS, EC_SIGN_REG_T2);
                carry = -1; /*pnt=2;*/
                break;
        case 2:
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_XS, EC_SIGN_REG_X2);
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_YS, EC_SIGN_REG_Y2);
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_ZS, EC_SIGN_REG_Z2);
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_TS, EC_SIGN_REG_T2);
                carry =  0; /*pnt=2;*/
                break;
        case 3:
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_XS, EC_SIGN_REG_X4);
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_YS, EC_SIGN_REG_Y4);
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_ZS, EC_SIGN_REG_Z4);
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_TS, EC_SIGN_REG_T4);
                carry = -1; /*pnt=4;*/
                break;
        default: ASSERT(0);
        }

        // t of p,2,4 are no longer needed, let us use them for -ry
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_TP, ECC_REG_N4, EC_SIGN_REG_YP); // ry of -p
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_T2, ECC_REG_N4, EC_SIGN_REG_Y2); // ry of -2p
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_T4, ECC_REG_N4, EC_SIGN_REG_Y4); // ry of -4p

        for (i -= 2; i >= 0; i -= 2) {
                int32_t swt;
        // EC_SIGN_REG_ZR as a temporary
                DoubleMdf2Mdf(EC_SIGN_REG_XS,EC_SIGN_REG_YS,EC_SIGN_REG_ZS,EC_SIGN_REG_ZR, EC_SIGN_REG_XS,EC_SIGN_REG_YS,EC_SIGN_REG_ZS,EC_SIGN_REG_TS);
                PkaDoubleMdf2Jcb(EC_SIGN_REG_XS,EC_SIGN_REG_YS,EC_SIGN_REG_ZS, EC_SIGN_REG_XS,EC_SIGN_REG_YS,EC_SIGN_REG_ZS,EC_SIGN_REG_ZR); // s *= 4

                /* get next two bits of key */
                b2 = PkaGet2MsBits(EC_SIGN_REG_RK, i, &W, &isNew);
                swt = carry*4+b2;
                /* choose which point to add or subtract and update the carry*/
                switch (swt) {
                case (uint32_t)-4:
            PkaAddJcbJcb2Mdf(EC_SIGN_REG_XS,EC_SIGN_REG_YS,EC_SIGN_REG_ZS,EC_SIGN_REG_TS, EC_SIGN_REG_XS,
                EC_SIGN_REG_YS,EC_SIGN_REG_ZS, EC_SIGN_REG_X4,EC_SIGN_REG_T4,EC_SIGN_REG_Z4);
            carry =  0;
            break;
                case (uint32_t)-3:
            kaAddJcbJcb2Mdf(EC_SIGN_REG_XS,EC_SIGN_REG_YS,EC_SIGN_REG_ZS,EC_SIGN_REG_TS, EC_SIGN_REG_XS,
                EC_SIGN_REG_YS,EC_SIGN_REG_ZS, EC_SIGN_REG_X2,EC_SIGN_REG_T2,EC_SIGN_REG_Z2);
            carry = -1;
            break;
                case (uint32_t)-2:
            PkaAddJcbJcb2Mdf(EC_SIGN_REG_XS,EC_SIGN_REG_YS,EC_SIGN_REG_ZS,EC_SIGN_REG_TS, EC_SIGN_REG_XS,
                EC_SIGN_REG_YS,EC_SIGN_REG_ZS, EC_SIGN_REG_X2,EC_SIGN_REG_T2,EC_SIGN_REG_Z2);
            carry =  0;
            break;
                case (uint32_t)-1:
            PkaAddJcbJcb2Mdf(EC_SIGN_REG_XS,EC_SIGN_REG_YS,EC_SIGN_REG_ZS,EC_SIGN_REG_TS, EC_SIGN_REG_XS,
                EC_SIGN_REG_YS,EC_SIGN_REG_ZS, EC_SIGN_REG_XP,EC_SIGN_REG_TP,EC_SIGN_REG_ZP);
            carry =  0;
            break;
                case  0:
            PkaAddJcbJcb2Mdf(EC_SIGN_REG_XS,EC_SIGN_REG_YS,EC_SIGN_REG_ZS,EC_SIGN_REG_TS, EC_SIGN_REG_XS,
                EC_SIGN_REG_YS,EC_SIGN_REG_ZS, EC_SIGN_REG_XP,EC_SIGN_REG_YP,EC_SIGN_REG_ZP);
            carry = -1;
            break;
                case +1:
            PkaAddJcbJcb2Mdf(EC_SIGN_REG_XS,EC_SIGN_REG_YS,EC_SIGN_REG_ZS,EC_SIGN_REG_TS, EC_SIGN_REG_XS,
                EC_SIGN_REG_YS,EC_SIGN_REG_ZS, EC_SIGN_REG_XP,EC_SIGN_REG_YP,EC_SIGN_REG_ZP);
            carry =  0;
            break;
                case +2:
            PkaAddJcbJcb2Mdf(EC_SIGN_REG_XS,EC_SIGN_REG_YS,EC_SIGN_REG_ZS,EC_SIGN_REG_TS, EC_SIGN_REG_XS,
                EC_SIGN_REG_YS,EC_SIGN_REG_ZS, EC_SIGN_REG_X2,EC_SIGN_REG_Y2,EC_SIGN_REG_Z2);
            carry =  0;
            break;
                case +3:
            PkaAddJcbJcb2Mdf(EC_SIGN_REG_XS,EC_SIGN_REG_YS,EC_SIGN_REG_ZS,EC_SIGN_REG_TS, EC_SIGN_REG_XS,
                EC_SIGN_REG_YS,EC_SIGN_REG_ZS, EC_SIGN_REG_X4,EC_SIGN_REG_Y4,EC_SIGN_REG_Z4);
            carry = -1;
            break;
                default:
            ASSERT(0);
                }
        }

        PkaAddJcbJcb2Jcb(EC_SIGN_REG_X2,EC_SIGN_REG_Y2,EC_SIGN_REG_Z2, EC_SIGN_REG_XS,EC_SIGN_REG_YS,
        EC_SIGN_REG_ZS, EC_SIGN_REG_XP,EC_SIGN_REG_TP,EC_SIGN_REG_ZP); // used only then carry is -1

        if (carry == -1) {
                PKA_SUB(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_T2, ECC_REG_N4, EC_SIGN_REG_Y2);
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_XP, EC_SIGN_REG_X2);
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_YP, isK == 1 ? EC_SIGN_REG_Y2 : EC_SIGN_REG_T2);
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_ZP, EC_SIGN_REG_Z2);
        } else {
                PKA_SUB(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_TS, ECC_REG_N4, EC_SIGN_REG_YS);
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_XP, EC_SIGN_REG_XS);
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_YP, isK == 1 ? EC_SIGN_REG_YS : EC_SIGN_REG_TS);
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, EC_SIGN_REG_ZP, EC_SIGN_REG_ZS);
        }
        // convert to affine
        PkaJcb2Afn(SCAP_Active, EC_SIGN_REG_XP, EC_SIGN_REG_YP, EC_SIGN_REG_ZP);

        return;
}


/***********    ScalarMultSca   function      **********************/
/**
 * @brief EC scalar multiplication p = k*p, with SCA-protection features.
 *
 *  The function performs:
 *  - PKA init,
 *  - setting input data into PKA registers,
 *  - calls pkaSmul function and then output result data from PKA.
 *
 * @author reuvenl (03/19/2015)
 *
 * @return  - On success CC_OK is returned, on failure an error code.
 *
 */
static CCError_t ScalarMultSca(const CCEcpkiDomain_t *domain,   /*!< [in] Pointer to EC domain. */
                 uint32_t *bxr, uint32_t *byr,        /*!< [out] Pointer to coordinates of result EC point. */
                 const uint32_t *k, uint32_t kSizeBit,/*!< [in] Pointer to the scalar and its size. */
                 const uint32_t *bxp, const uint32_t *byp)        /*!< [in] Pointer to the of input EC point. */
{
        uint8_t ord = regTemps[26];
        uint8_t rk  = regTemps[27];
        uint8_t rxp = regTemps[28];
        uint8_t ryp = regTemps[29];

        uint32_t err;
        uint32_t modSizeInBits, modSizeInWords;
        uint32_t pkaReqRegs = PKA_MAX_COUNT_OF_PHYS_MEM_REGS;

        /* set domain parameters */
        modSizeInBits  = domain->modSizeInBits;
        modSizeInWords = CALC_FULL_32BIT_WORDS(modSizeInBits);

        /*  Init PKA for modular operations: regs mappimg according to max.   *
        *   size of order or modulus                                          */
        err = PkaInitAndMutexLock(CC_MAX(modSizeInBits, domain->ordSizeInBits), &pkaReqRegs);
        if (err != CC_OK) {
                return err;
        }
        /* Set modulus sizes to L0, L1 and order sizes in L2, L3 */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET (CRY_KERNEL, PKA_L0), modSizeInBits);

        /*   Set data into PKA registers  */
        /* set EC parameters */
        PkaCopyDataIntoPkaReg(ECC_REG_N, 1, domain->ecP/*src_ptr*/, modSizeInWords);
        PkaCopyDataIntoPkaReg(ECC_REG_NP, 1, ((EcWrstDomain_t*)&domain->llfBuff)->modTag,
                               CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS);
        PkaCopyDataIntoPkaReg(ECC_REG_EC_A, 1, domain->ecA, modSizeInWords);
        PkaCopyDataIntoPkaReg(ord, 1, domain->ecR, CALC_FULL_32BIT_WORDS(domain->ordSizeInBits));
        /* set point */
        PkaCopyDataIntoPkaReg(rxp, 1, bxp, modSizeInWords);
        PkaCopyDataIntoPkaReg(ryp, 1, byp, modSizeInWords);
        /* set key */
        PkaCopyDataIntoPkaReg(rk, 1, k, CALC_FULL_32BIT_WORDS(kSizeBit));

        /* Call scalar mult */
        PkaScalarMultSca();

        /*  Output data from PKA registers  */
        PkaCopyDataFromPkaReg(bxr, modSizeInWords, rxp);
        PkaCopyDataFromPkaReg(byr, modSizeInWords, ryp);

        /*   Finish the function and clear PKA regs.  */
        PkaFinishAndMutexUnlock(pkaReqRegs);

        return err;
}

/***********    PkaEcWrstScalarMult   function      **********************/
/**
 *@brief ECC scalar multiplication function, with SCA protection
 *        features (NoScap).
 *             outPoint = scalsr * inPoint.
 *
 *   The function performs the following:
 *     1. Checks the validity of input parameters.
 *     2. Calls the low level functions: ScalarMultSca (with SCA protection) to generate EC public key.
 *     3. Outputs the user public and private key structures in little endian form.
 *     4. Cleans temporary buffers.
 *     5. Exits.
 *
 *   Mote: All buffers are given as 32-bit words arrays, where LSWord is a leftmost one.
 *         Sizes of buffers of in/out points coordinates are equal to EC modulus
 *         size.
 *
 *  @return  - On success CC_OK is returned, on failure an error code.
 *
*/
CCError_t PkaEcWrstScalarMult(const CCEcpkiDomain_t *pDomain,   /*!< [in] Pointer to EC domain. */
                             const uint32_t       *scalar,         /*!< [in] Pointer to the scalsr buffer. */
                             uint32_t             scalSizeInWords,/*!< [in] Size of the scalsr in 32-bit words. */
                             uint32_t             *inPointX,       /*!< [in] Pointer to input point X coordinate. */
                             const uint32_t       *inPointY,       /*!< [in] Pointer to input point Y coordinate. */
                             const uint32_t       *outPointX,      /*!< [out] Pointer to output point X coordinate. */
                             uint32_t             *outPointY,      /*!< [out] Pointer to output point Y coordinate. */
                             uint32_t             *tmpBuff)        /*!< [in] Pointer to temporary buffer. */
{
        CCError_t err = CC_OK;
        uint32_t scalarSizeInBits;
        CCCommonCmpCounter_t cmp;

        CC_UNUSED_PARAM(tmpBuff); // remove compilation warning

        /* get exact size of scalar */
        scalarSizeInBits = CC_CommonGetWordsCounterEffectiveSizeInBits(scalar, scalSizeInWords);

        /* compare scalar to EC generator order (0 < scalar < order) */
        cmp = CC_CommonCmpLsWordsUnsignedCounters(scalar, scalSizeInWords,
                                                     pDomain->ecR, CALC_FULL_32BIT_WORDS(pDomain->ordSizeInBits));

        if (scalarSizeInBits == 0 || cmp == CC_COMMON_CmpCounter1GreaterThenCounter2)
                return ECWRST_SCALAR_MULT_INVALID_SCALAR_VALUE_ERROR;

        /* perform scalar mult. with SCA protect features */
        err = ScalarMultSca(pDomain,
                        outPointX, outPointY,
                        scalar, scalarSizeInBits,
                        inPointX, inPointY);

        return err;

}




