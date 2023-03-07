/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * All the includes that are needed for code using this module to
 * compile correctly should be #included here.
 */
#include "cc_pal_mem.h"
#include "cc_pal_types.h"
#include "cc_hal_plat.h"
#include "cc_common_math.h"
#include "mbedtls_cc_ec_mont_edw_error.h"
#include "cc_ec_mont_api.h"
#include "ec_mont_local.h"

#include "pka_hw_defs.h"
#include "pki.h"
#include "pka.h"
#include "pka_error.h"
#include "pka_ec_mont_glob_regs_def.h"


/*!
 * The function executes scalar multiplication by ladder algorithm and PKA HW.
 *
 *    Note: 1. SCA protection is not active.
 *          2. Assumed that PKA was initialized according to EC modulus
 *             and registers definitions in above included ...def.h file before
 *             calling the function.
 *          3. Scalar buffer size in words is equall to EC order (modulus) size.
 *
 *  Implicitly defined in/out parameters (registers ID-s):
 *    EC_MONT_REG_RES [out] - ID of PKA register for output result point in
 *                  compressed form (coordinate X).
 *    EC_MONT_REG_X1 [in] - ID of PKA register, containing the input point in
 *             compressed form (coordinate X1).
 *
 * \return CCError_t
 */
static CCError_t EcMontPkaScalarMultWithLadderExe(
                                                   uint32_t *scalar,        /*!< [in] pointer to scalar in LE order of bytes. */
                                                   uint32_t  scalarSizeBits /*!< [in] exact scalar size in bits. */)
{
        /* DEFINITIONS */

        CCError_t err = CC_OK;
        uint32_t swap = 0;
        uint32_t tmp, currBit;
        int32_t  i;
        uint32_t rX2, rX3, rZ2, rZ3; /* variable regisres ID-s */

        /*   FUNCTION LOGIC  */

        /* set internally used registers: x2=1; z2=0; x3=x1; z3=1; */
        rX2 = EC_MONT_REG_X2; rX3 = EC_MONT_REG_X3; rZ2 = EC_MONT_REG_Z2; rZ3 = EC_MONT_REG_Z3;

        /* clean 4 registers */
        PkaClearBlockOfRegs(rX2/*firstReg*/, 4/*countOfRegs*/, LEN_ID_N_PKA_REG_BITS);
        /* set */
        PKA_SET_BIT0(LEN_ID_N_PKA_REG_BITS, rX2, rX2);
        PKA_COPY(LEN_ID_N_PKA_REG_BITS, rX3, EC_MONT_REG_X1);
        PKA_SET_BIT0(LEN_ID_N_PKA_REG_BITS, rZ3, rZ3);

        /* set EC_MONT_REG_N4 = 4*Mod */
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EC_MONT_REG_N4, EC_MONT_REG_N, EC_MONT_REG_N);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EC_MONT_REG_N4, EC_MONT_REG_N4, EC_MONT_REG_N4);

        i = scalarSizeBits - 1;
        tmp = scalar[(i+CC_BITS_IN_32BIT_WORD-1) / CC_BITS_IN_32BIT_WORD - 1];

        for (; i >= 0; --i) {

                /* get next word of scalar */
                if ((i & (CC_BITS_IN_32BIT_WORD - 1)) == (CC_BITS_IN_32BIT_WORD - 1) ) {
                        tmp = scalar[i / CC_BITS_IN_32BIT_WORD];
                }

                /* get next bit of scalar */
                currBit = (tmp >> (i & (CC_BITS_IN_32BIT_WORD - 1))) & 1;

                /* c of registers ID-s */
                swap ^= currBit;                                                                // PkiDbgPrintReg("\ni=%3d b=%3d 1-swap=%3d ", i, currBit, swap);
                PkiConditionalSecureSwapUint32(&rX2, &rX3, swap);
                PkiConditionalSecureSwapUint32(&rZ2, &rZ3, swap);
                swap = currBit;                                                                 // PkiDbgPrintReg("2-swap=%3d  start HW loop: \n", swap);

                /* ladder: (X1,1),(X2,Z2),(X3,Z3) -> (X2,Z2),(X3,Z3) */

                PKA_SUB(LEN_ID_N_PKA_REG_BITS, EC_MONT_REG_T2, EC_MONT_REG_N4, rZ3);            // PkiDbgPrintReg("t2: -z3 =", EC_MONT_REG_T2);
                PKA_ADD(LEN_ID_N_PKA_REG_BITS, EC_MONT_REG_T, rX3, EC_MONT_REG_T2);             // PkiDbgPrintReg("t: x3+t2 =", EC_MONT_REG_T);
                PKA_SUB(LEN_ID_N_PKA_REG_BITS, EC_MONT_REG_T2, EC_MONT_REG_N4, rZ2);            // PkiDbgPrintReg("t2: -z2= ", EC_MONT_REG_T2);
                PKA_ADD(LEN_ID_N_PKA_REG_BITS, EC_MONT_REG_T1, rX2, EC_MONT_REG_T2);            // PkiDbgPrintReg("t1: x2+t2 =", EC_MONT_REG_T1);
                PKA_ADD(LEN_ID_N_PKA_REG_BITS, rX2, rZ2, rX2);                                  // PkiDbgPrintReg("x2: z2+x2 =", rX2);
                PKA_ADD(LEN_ID_N_PKA_REG_BITS, rZ2, rX3, rZ3);                                  // PkiDbgPrintReg("z2: x3+z3 =", rZ2);
                PKA_MOD_MUL_NFR(LEN_ID_N_BITS, rZ3, EC_MONT_REG_T, rX2);                        // PkiDbgPrintReg("z3: t*x2 =", rZ3);
                PKA_MOD_MUL_NFR(LEN_ID_N_BITS, rZ2, EC_MONT_REG_T1, rZ2);                       // PkiDbgPrintReg("z2: t1*z2 =", rZ2);
                PKA_MOD_MUL_NFR(LEN_ID_N_BITS, EC_MONT_REG_T, EC_MONT_REG_T1, EC_MONT_REG_T1);  // PkiDbgPrintReg("t: t1*t1 =", EC_MONT_REG_T);
                PKA_MOD_MUL_NFR(LEN_ID_N_BITS, EC_MONT_REG_T1, rX2, rX2);                       // PkiDbgPrintReg("t1: x2*x2 =", EC_MONT_REG_T1);
                PKA_ADD(LEN_ID_N_PKA_REG_BITS, rX3, rZ3, rZ2);                                  // PkiDbgPrintReg("x3: z3+z2 =", rX3);
                PKA_SUB(LEN_ID_N_PKA_REG_BITS, EC_MONT_REG_T2, EC_MONT_REG_N4, rZ2);            // PkiDbgPrintReg("t2: -z2 =", EC_MONT_REG_T2);
                PKA_ADD(LEN_ID_N_PKA_REG_BITS, rZ2, rZ3, EC_MONT_REG_T2);                       // PkiDbgPrintReg("z2: z3+t2 =", rZ2);
                PKA_MOD_MUL_NFR(LEN_ID_N_BITS, rX2, EC_MONT_REG_T1, EC_MONT_REG_T);             // PkiDbgPrintReg("x2: t1*t =", rX2);
                PKA_SUB(LEN_ID_N_PKA_REG_BITS, EC_MONT_REG_T2, EC_MONT_REG_N4, EC_MONT_REG_T);  // PkiDbgPrintReg("t2: -t =", EC_MONT_REG_T2);
                PKA_ADD(LEN_ID_N_PKA_REG_BITS, EC_MONT_REG_T1, EC_MONT_REG_T1, EC_MONT_REG_T2); // PkiDbgPrintReg("t1: t1+t2 =", EC_MONT_REG_T1);
                PKA_MOD_MUL_NFR(LEN_ID_N_BITS, rZ2, rZ2, rZ2);                                  // PkiDbgPrintReg("z2: z2*z2 =", rZ2);
                PKA_MOD_MUL_NFR(LEN_ID_N_BITS, rX3, rX3, rX3);                                  // PkiDbgPrintReg("x3: x3*x3 =", rX3);
                PKA_MOD_MUL_ACC_NFR(LEN_ID_N_BITS, EC_MONT_REG_T, EC_MONT_REG_A24,
                                    EC_MONT_REG_T1, EC_MONT_REG_T);                             // PkiDbgPrintReg("t: a24*t1+t= ", EC_MONT_REG_T);
                PKA_MOD_MUL_NFR(LEN_ID_N_BITS, rZ3, EC_MONT_REG_X1, rZ2);                       // PkiDbgPrintReg("z3: x1*z2 =", rZ3);
                PKA_MOD_MUL_NFR(LEN_ID_N_BITS, rZ2, EC_MONT_REG_T1, EC_MONT_REG_T);             // PkiDbgPrintReg("z2: t1*t =", rZ2);
        }
        /* Swapping of registers ID-s */
        PkiConditionalSecureSwapUint32(&rX2, &rX3, swap);
        PkiConditionalSecureSwapUint32(&rZ2, &rZ3, swap);
        PKA_MOD_INV_W_EXP(EC_MONT_REG_T, rZ2, rX3/*as temp*/);                                  // PkiDbgPrintReg("t: 1/z2 =", EC_MONT_REG_T);
        PKA_MOD_MUL(LEN_ID_N_BITS, EC_MONT_REG_RES, rX2, EC_MONT_REG_T);                        // PkiDbgPrintReg("r: x2*t =", EC_MONT_REG_RES);
// ???       PKA_REDUCE(LEN_ID_N_BITS, EC_MONT_REG_RES, EC_MONT_REG_RES);                             PkiDbgPrintReg("r: r reduced =", EC_MONT_REG_RES);

        return err;
}


/*!
 * The function performs input/output parameters for scalar
 * multiplication using ladder algorithm.
 *
 *         resPoint(X,_) = k*inPoint(X,_), where:
 *         both points are given in compressed form (only X coordinates ) with
 *         LE order of the words.
 *
 *         Assuming: the PKA HW is turned on and initialized yet.
 *
 * \return CCError_t
 */
CCError_t EcMontPkaScalMultWithLadder(
                                       uint32_t  *resPoint,  /*!< [out] pointer to result EC point (coordinate X). */
                                       uint32_t  *scalar,    /*!< [in] pointer to scalar. */
                                       uint32_t   scalarSizeBits, /*!< [in] scalar size in bits. */
                                       uint32_t  *inPoint,   /*!< [in] pointer to input ECMONT point (coordinate X). */
                                       const CCEcMontDomain_t *pEcDomain /*!< [in] pointer to EC domain (curve). */)
{
        /* Definitions */

        CCError_t err = CC_OK;
        uint32_t scalarSizeWords;

        /* set scalar bits according to EC Montgomery algorithm:
           byte[31] = (byte[31] & 127) | 64; byte[0] &= 248; */
        scalarSizeWords = (scalarSizeBits + CC_BITS_IN_32BIT_WORD - 1) / CC_BITS_IN_32BIT_WORD;

        /* ********************************************* */
        /* load all needed data to defined pka registers */
        /* ********************************************* */

        /* EC modulus */
        PkaCopyDataIntoPkaReg(EC_MONT_REG_N/*dstReg*/, LEN_ID_N_PKA_REG_BITS,
                              pEcDomain->ecModP/*src*/, pEcDomain->ecModSizeInWords);
        /*Barr. tag.*/
        PkaCopyDataIntoPkaReg(EC_MONT_REG_NP/*dstReg*/, LEN_ID_N_PKA_REG_BITS,
                              pEcDomain->ecModBarrTag/*src*/,
                              CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS/*Barr.tag.*/);
        /*EC parameter*/
        PkaCopyDataIntoPkaReg(EC_MONT_REG_A24/*dstReg*/, LEN_ID_N_PKA_REG_BITS,
                              pEcDomain->ecParam/*src*/, pEcDomain->ecModSizeInWords/*param.size*/);
        /* input point X1 */
        PkaCopyDataIntoPkaReg(EC_MONT_REG_X1/*dstReg*/, LEN_ID_N_PKA_REG_BITS,
                              inPoint/*src*/, pEcDomain->ecModSizeInWords);


        /*--------------------------------------------------------------------*
         * perform EC scalar multiplication:  used PKA registers defined in   *
         *   "pka_ec_mont_glob_regs_def."h file: output is EC_MONT_REG_RES    *
         *--------------------------------------------------------------------*/
        err = EcMontPkaScalarMultWithLadderExe(scalar, scalarSizeBits);
        if (err)
                goto EndCommand;

        /* output result point */
        PkaCopyDataFromPkaReg(resPoint/*dst*/, scalarSizeWords, EC_MONT_REG_RES/*srcReg*/);

EndCommand:
        return err;
}
