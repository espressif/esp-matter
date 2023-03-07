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
#include "cc_ec_edw_api.h"

#include "ec_edw_local.h"
#include "pka_defs.h"
#include "pka_hw_defs.h"
#include "pki.h"
#include "pka.h"
#include "ec_edw.h"
#include "pka_error.h"
#include "pka_ec_edw_glob_regs_def.h"


/******             Definitions              *******/

/*!> Macros for expansion of actual parameters (registers) for EC point "a" */
#define REGS_MODIF_POINT(a) EDW_REG_X##a,EDW_REG_Y##a,EDW_REG_Z##a,EDW_REG_T##a  /*for Modified point*/
#define REGS_PREC_POINT(a) EDW_REG_S##a,EDW_REG_D##a,EDW_REG_P##a                /*for Precalculated point*/
#define REGS_NEG_PREC_POINT(a) EDW_REG_D##a,EDW_REG_S##a,EDW_REG_MP##a           /*for Precalculated negative point*/

/**
 * EC Edwards adding with extended (Ext) coordinates of points.
 *    EEE: (X1,Y1,Z1,T1)  +  (X2,Y2,Z2,T2) -> (X,Y,Z,T)
 *
 */
static void PkaEcEdwAddExtExtExt(
                   uint32_t rX,  uint32_t rY,  uint32_t rZ,  uint32_t rT,  /*!< [out] - virt. pointers to PKA registers, containing
                                                                                coordinates of result extended EC point. */
                   uint32_t rX1, uint32_t rY1, uint32_t rZ1, uint32_t rT1, /*!< [in] - virt. pointers to PKA registers, containing
                                                                                coordinates of inputt extended EC point1. */
                   uint32_t rX2, uint32_t rY2, uint32_t rZ2, uint32_t rT2) /*!< [in] - virt. pointers to PKA registers, containing
                                                                                coordinates of inputt extended EC point2. */
{
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, EDW_REG_T3, EDW_REG_N_4,rX1);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_T3, EDW_REG_T3, rY1);
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, EDW_REG_T4, EDW_REG_N_4,rX2);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_T4, EDW_REG_T4, rY2);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, EDW_REG_T5, EDW_REG_T3, EDW_REG_T4);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_T3, rY1, rX1);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_T4, rY2, rX2);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, EDW_REG_T6, EDW_REG_T3, EDW_REG_T4);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, EDW_REG_T3, rT1, rT2);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, EDW_REG_T3, EDW_REG_T3, EDW_REG_D2);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_T4, rZ1, rZ1);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, EDW_REG_T4, EDW_REG_T4, rZ2);
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, rT, EDW_REG_N_4, EDW_REG_T5);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, rT, rT, EDW_REG_T6);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_T6, EDW_REG_T6, EDW_REG_T5);
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, EDW_REG_T5, EDW_REG_N_4, EDW_REG_T3);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_T5, EDW_REG_T5, EDW_REG_T4);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_T4, EDW_REG_T4, EDW_REG_T3);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, rX, rT, EDW_REG_T5);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, rY, EDW_REG_T4, EDW_REG_T6);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, rZ, EDW_REG_T5, EDW_REG_T4);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, rT, rT, EDW_REG_T6);
        return;
}


/**
 * EC Edwards doubing with extended (Ext) coordinates of point.
 *    EE: 2*(X,Y,Z,T) -> (X,Y,Z,T)
 *
 */
static void PkaEcEdwDoublExtExt(uint32_t rX, uint32_t rY, uint32_t rZ, uint32_t rT) /*!< [in/out] - virt. pointers to PKA registers, containing
                                                                                   coordinates of input/output point. */
{
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, EDW_REG_T3, EDW_REG_N_4, rX);         // hwsub(t3, n_4, x);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_T3, EDW_REG_T3, rY);          // hwadd(t3, t3, y);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, EDW_REG_T5, EDW_REG_T3, EDW_REG_T3);  // hwmmul(t5, t3, t3, n, np);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_T3, rY, rX);                  // hwadd(t3, y,x);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, EDW_REG_T6, EDW_REG_T3, EDW_REG_T3);  // hwmmul(t6, t3, t3, n, np);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, EDW_REG_T3, rT, rT);                  // hwmmul(t3, t, t, n, np);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, EDW_REG_T3, EDW_REG_T3, EDW_REG_D2);  // hwmmul(t3, t3, d2, n, np);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_T4, rZ, rZ);                  // hwadd(t4, z, z);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, EDW_REG_T4, EDW_REG_T4, rZ);          // hwmmul(t4, t4, z, n, np);
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, rT, EDW_REG_N_4, EDW_REG_T5);         // hwsub(t, n_4, t5);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, rT, rT, EDW_REG_T6);                  // hwadd(t, t, t6);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_T6, EDW_REG_T6, EDW_REG_T5);  // hwadd(t6, t6, t5);
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, EDW_REG_T5, EDW_REG_N_4, EDW_REG_T3); // hwsub(t5, n_4, t3);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_T5, EDW_REG_T5, EDW_REG_T4);  // hwadd(t5, t5, t4);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_T4, EDW_REG_T4, EDW_REG_T3);  // hwadd(t4, t4, t3);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, rX, rT, EDW_REG_T5);                  // hwmmul(x, t, t5, n, np);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, rY, EDW_REG_T4, EDW_REG_T6);          // hwmmul(y, t4, t6, n, np);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, rZ, EDW_REG_T5, EDW_REG_T4);          // hwmmul(z, t5, t4, n, np);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, rT, rT, EDW_REG_T6);                  // hwmmul(t, t, t6, n, np);
        return;
}


/**
 * EC Edwards adding with extended (Ext) and proective (Prc) coordinates of
 *     points:  (X1,Y1,Z1,T1) + (S2,D2,P2) -> (X,Y,Z,T)
 *
 */
static void PkaEcEdwAddExtPrcExt(
                   uint32_t rX,  uint32_t rY,  uint32_t rZ,  uint32_t rT,  /*!< [out] - virt. pointers to PKA registers,
                                                                            containing coordinates of result extended EC point. */
                   uint32_t rX1, uint32_t rY1, uint32_t rZ1, uint32_t rT1, /*!< [in] - virt. pointers to PKA registers,
                                                                            containing coordinates of extended EC point1. */
                   uint32_t rS2, uint32_t rD2, uint32_t rP2)               /*!< [in] - virt. pointers to PKA registers,
                                                                            containing coordinates of precomputed EC point2. */
{
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, EDW_REG_T3, EDW_REG_N_4, rX1);            PkiDbgPrintReg("t3: -x1     ", EDW_REG_T3);      // hwsub(t3, n_4, x1);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_T3, EDW_REG_T3, rY1);             PkiDbgPrintReg("t3: t3+y1   ", EDW_REG_T3);      // hwadd(t3, t3, y1);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, EDW_REG_T5, EDW_REG_T3, rD2);             PkiDbgPrintReg("t5: t3*d2   ", EDW_REG_T5);      // hwmmul(t5, t3, d2, n, np);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_T3, rY1, rX1);                    PkiDbgPrintReg("t3: y1+x1   ", EDW_REG_T3);      // hwadd(t3, y1, x1);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, EDW_REG_T6, EDW_REG_T3, rS2);             PkiDbgPrintReg("t6: t3*s2   ", EDW_REG_T6);      // hwmmul(t6, t3, s2, n, np);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, EDW_REG_T3, rT1, rP2);                    PkiDbgPrintReg("t3: t1*p2   ", EDW_REG_T3);      // hwmmul(t3, t1, p2, n, np);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_T4, rZ1, rZ1);                    PkiDbgPrintReg("t4: dbl(z1) ", EDW_REG_T4);      // hwadd(t4, z1, z1);
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, rT, EDW_REG_N_4, EDW_REG_T5);             PkiDbgPrintReg("t: -t5      ", rT);              // hwsub(t, n_4, t5);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, rT, rT, EDW_REG_T6);                      PkiDbgPrintReg("t: t+t6     ", rT);              // hwadd(t, t, t6);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_T6, EDW_REG_T6, EDW_REG_T5);      PkiDbgPrintReg("t6: t6+t5   ", EDW_REG_T6);      // hwadd(t6, t6, t5);
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, EDW_REG_T5, EDW_REG_N_4, EDW_REG_T3);     PkiDbgPrintReg("t5: -t3     ", EDW_REG_T5);      // hwsub(t5, n_4, t3);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_T5, EDW_REG_T5, EDW_REG_T4);      PkiDbgPrintReg("t5: t5+t4   ", EDW_REG_T5);      // wadd(t5, t5, t4);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_T4, EDW_REG_T4, EDW_REG_T3);      PkiDbgPrintReg("t4: t4+t3   ", EDW_REG_T4);      // hwadd(t4, t4, t3);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, rX, rT, EDW_REG_T5);                      PkiDbgPrintReg("x: t*t5     ", rX);              // hwmmul(x, t, t5, n, np);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, rY, EDW_REG_T4, EDW_REG_T6);              PkiDbgPrintReg("y: t4*t6    ", rY);              // hwmmul(y, t4, t6, n, np);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, rZ, EDW_REG_T5, EDW_REG_T4);              PkiDbgPrintReg("z: t5*t4    ", rZ);              // hwmmul(z, t5, t4, n, np);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, rT, rT, EDW_REG_T6);                      PkiDbgPrintReg("t: t*t6     ", rT);              // hwmmul(t, t, t6, n, np);
        return;
}


/**
 * EC Edwards convert affine point (Afn) to precalculated (Prc) form of coordinates.
 *    A2P: (X,Y) -> (S,D,P)
 */
void PkaEcEdwConvertAfn2Prc(
                  uint32_t rS, uint32_t rD, uint32_t rP, /*!< virtual pointers to 3 coordinates
                                                              of output precomputed EC pont */
                  uint32_t rX, uint32_t rY)              /*!< virtual pointers to 2 coordinates
                                                              of input affine point */
{
        /* if we have x,y,z,t regs, than is efficient to define:
           rS->rZ, rD->rT, rP->rX */
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, rS, rY, rX);             // hwadd(s, y, x);
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, rD, EDW_REG_N, rX);      // hwsub(d, n, x);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, rD, rD, rY);             // hwadd(d, d, y);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, rP, EDW_REG_D2, rX);     // hwmmul(p, d2, x, n, np);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, rP, rP, rY);             // hwmmul(p, p, y, n, np);
        return;
}


// this SCA_RESISTANT is unrelated to SCA_PROTECTION in hardware
/**
 * EC Edwards convert point to affine (A) form of coordinates.
 *    ToA: (X1,Y1,Z1) -> (X,Y)
 *
 * @param rX,rY       - virt. pointers to PKA registers, containing
 *                      affine coordinates of result EC point.
 * @param rX1,rY1,rZ1 - virt. pointers to PKA registers, containing
 *                      coordinates of result EC point1.
 * @param EDW_REG_Q - virt. pointer to temp PKA register.
 *
 */
void PkaEcEdwPointToAfn(
                       uint32_t rX,  uint32_t rY,
                       uint32_t rX1, uint32_t rY1, uint32_t rZ1)
{
        // PKA_MOD_INV_W_EXP(rY/*q*/, rZ1, rX/*tmp*/);          // hwinv(q, z1, n, np); /* by exponent*/
        PKA_MOD_INV(LEN_ID_N_BITS, EDW_REG_T4/*q*/, rZ1);       // PkiDbgPrintReg("rY: inv(rZ1)", rY);
        PKA_MOD_MUL(LEN_ID_N_BITS, rX, rX1, EDW_REG_T4/*q*/);   // PkiDbgPrintReg("rX: rX1*rY", rX);
        PKA_MOD_MUL(LEN_ID_N_BITS, rY, rY1, EDW_REG_T4/*q*/);   // PkiDbgPrintReg("rY: rY1*rY", rY);
        // PKA_DIV(LEN_ID_N_BITS, EDW_REG_T4, rX, EDW_REG_N);   // PkiDbgPrintReg("rX: reduce", rX);
        // PKA_DIV(LEN_ID_N_BITS, EDW_REG_T4, rY, EDW_REG_N);   // PkiDbgPrintReg("rY: reduce", rY);

        return;
}


/**
 * The function performs multiplication of base point by scalar:
 *      P(x,y) = k*G(x,y).
 *
 *  Implemented algorithm, enhanced by A.Klimov.
 *
 *  The function can work with any scalar > 0 and used (for example) in Edw.
 *  signature function.
 *
 *  Assuming:
 *  PKA registers are implicitly defined in pka_ec_edw_glob_regs_def.h file, in
 *  partial: output point P(x,y) is set in registers (EDW_REG_XS, EDW_REG_YS).
 *  All needed data must be loaded into PKA registers in caller function.
 *
 * @author reuvenl (11/25/2015)
 *
 * @return CC_OK or an error according to mbedtls_cc_ec_mont_edw_error.h definitions.
 */
CCError_t PkaEcEdwScalarMultBase(
                                uint32_t *pScalar,         /*!< [in] the pointer to the scalsr (LS word is
                                                               leftmost one, MS word - rightmost). */
                                size_t    scalarSizeInBits /*!< exact size of the scalar in bits. */ )
{

        /* Definitions */
        CCError_t err = CC_OK;
        int32_t i, carry = -1;
        uint32_t twoBits;

        if(scalarSizeInBits == 0)
                return CC_EC_EDW_INVALID_SCALAR_SIZE_ERROR;

        /* set bits counter to scalar size, rounded up to even */
        i = ((scalarSizeInBits + 1) & ~1) - 2;
        twoBits = PKI_GET_TWO_BITS_FROM_WORDS_ARRAY(pScalar, i);

        /*-------------------------------------------------------*/
        /*    init point S according to MS bits of the scalar    */
        /*-------------------------------------------------------*/
        switch (twoBits) {
        case 1: /* S = G2 */
                PKA_COPY(LEN_ID_MAX_BITS, EDW_REG_XS, EDW_REG_XG2);
                PKA_COPY(LEN_ID_MAX_BITS, EDW_REG_YS, EDW_REG_YG2);
                PKA_COPY(LEN_ID_MAX_BITS, EDW_REG_TS, EDW_REG_TG2);
                carry = -1;
                break;
        case 2: /* S = G2 */
                PKA_COPY(LEN_ID_MAX_BITS, EDW_REG_XS, EDW_REG_XG2);
                PKA_COPY(LEN_ID_MAX_BITS, EDW_REG_YS, EDW_REG_YG2);
                PKA_COPY(LEN_ID_MAX_BITS, EDW_REG_TS, EDW_REG_TG2);
                carry = 0;
                break;
        case 3: /* S = G4 */
                PKA_COPY(LEN_ID_MAX_BITS, EDW_REG_XS, EDW_REG_XG4);
                PKA_COPY(LEN_ID_MAX_BITS, EDW_REG_YS, EDW_REG_YG4);
                PKA_COPY(LEN_ID_MAX_BITS, EDW_REG_TS, EDW_REG_TG4);
                carry = -1;
                break;
        default:
                return CC_EC_EDW_INVALID_SCALAR_DATA_ERROR;
        }

        /* set ZS = 1 */
        PKA_CLEAR(LEN_ID_MAX_BITS, EDW_REG_ZS);
        PKA_SET_BIT0(LEN_ID_N_BITS, EDW_REG_ZS, EDW_REG_ZS);

        /* calculate 4N */
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_N_4, EDW_REG_N, EDW_REG_N);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_N_4, EDW_REG_N_4, EDW_REG_N_4);

        /*-------------------------------------------------*/
        /*    double/add loop according to scalar bits     */
        /*-------------------------------------------------*/
        for(i -= 2; i >= 0; i-= 2) {
                int32_t swt;

                /* EC doubling */
                PkaEcEdwDoublExtExt(EDW_REG_XS, EDW_REG_YS, EDW_REG_ZS, EDW_REG_TS);
                PkaEcEdwDoublExtExt(EDW_REG_XS, EDW_REG_YS, EDW_REG_ZS, EDW_REG_TS); // S *= 4

                /* get two next MS bits of key */
                twoBits = PKI_GET_TWO_BITS_FROM_WORDS_ARRAY(pScalar, i);

                swt = carry*4 + twoBits;

                /* EC points adding according to MS bits and carry */
                switch (swt) {
                /*negative points adding*/
                case -4: PkaEcEdwAddExtPrcExt(EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_DG4, EDW_REG_SG4, EDW_REG_MPG4);   carry =  0;   break;
                case -3: PkaEcEdwAddExtPrcExt(EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_DG2, EDW_REG_SG2, EDW_REG_MPG2);   carry = -1;   break;
                case -2: PkaEcEdwAddExtPrcExt(EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_DG2, EDW_REG_SG2, EDW_REG_MPG2);   carry =  0;   break;
                case -1: PkaEcEdwAddExtPrcExt(EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_DG,  EDW_REG_SG,  EDW_REG_MPG);    carry =  0;   break;
                /*positive points adding*/
                case  0: PkaEcEdwAddExtPrcExt(EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_SG,  EDW_REG_DG,  EDW_REG_PG);     carry = -1;   break;
                case  1: PkaEcEdwAddExtPrcExt(EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_SG,  EDW_REG_DG,  EDW_REG_PG);     carry =  0;   break;
                case  2: PkaEcEdwAddExtPrcExt(EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_SG2, EDW_REG_DG2, EDW_REG_PG2);    carry =  0;   break;
                case  3: PkaEcEdwAddExtPrcExt(EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_SG4, EDW_REG_DG4, EDW_REG_PG4);    carry = -1;   break;
                default:
                        return CC_EC_EDW_INVALID_SCALAR_DATA_ERROR;
                }

        }
        /* calculation of result for case that carry is -1; note: registers *
        *  of point G4 now are used as temp registers for point S2          */
        PkaEcEdwAddExtPrcExt(EDW_REG_SG4,EDW_REG_DG4,EDW_REG_PG4,EDW_REG_MPG4/*modif S2 -> G4*/,
                             EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS/*modif S*/,
                             EDW_REG_DG,EDW_REG_SG,EDW_REG_MPG/*precalc. G*/);
        /* for afine result used registers of the point G */
        if (carry == -1)
                PkaEcEdwPointToAfn(EDW_REG_SG,EDW_REG_DG,  EDW_REG_SG4,EDW_REG_DG4,EDW_REG_PG4/*S2*/);
        else // carry = 0
                PkaEcEdwPointToAfn(EDW_REG_SG,EDW_REG_DG,  EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS/*S*/);

      return err;
}


/**
 * The function performs multiplication of base point by scalar of special form:
 *      P(x,y) = k*G(x,y).
 *
 *  Implemented algorithm, enhanced by A.Klimov.
 *
 *  The function can work with scalars of special form: exact size is 255
 *  bit and it is a multiple of 8 bit (as requiered in Edw. KeyGen algorithm).
 *
 *  Assuming:
 *  1. PKA registers are implicitly defined in pka_ec_edw_glob_regs_def.h file,
 *     in partial: output point P(x,y) is stated by registers (EDW_REG_XS,
 *     EDW_REG_YS).
 *  2. All needed data must be loaded into PKA registers in caller function.
 *  3. PKA registers are defined in pka_ec_edw_glob_regs_defh file, in partial:
 *      - output point R(x,y) shall be registers (rXR=EDW_REG_XR, rYR=EDW_REG_YR),
 *      - input point P(X,Y) by (rXP=EDW_REG_XP, rYP=EDW_REG_YP).
 *
 * @author reuvenl (11/25/2015)
 *
 * @return CC_OK or an error according to mbedtls_cc_ec_mont_edw_error.h definitions.
 */
CCError_t PkaEcEdwSpecialScalarMultBase(
                                uint32_t *pScalar,         /*!< [in] the pointer to the scalsr (LS word is
                                                               leftmost one, MS word - rightmost). */
                                size_t    scalarSizeInBits /*!< exact size of the scalar in bits. */ )
{

        /* Definitions */
        CCError_t err = CC_OK;
        int32_t carry, i, twoBits;
        size_t  sizeInWords;
        uint32_t word;

        if(scalarSizeInBits != 255)
                return CC_EC_EDW_INVALID_SCALAR_SIZE_ERROR;

        sizeInWords = (scalarSizeInBits + CC_BITS_IN_32BIT_WORD - 1) / CC_BITS_IN_32BIT_WORD;
        word = pScalar[sizeInWords-1];

        if((word >> 30) != 1)
                return CC_EC_EDW_INVALID_SCALAR_DATA_ERROR;


        carry = -1; // always 0 or -1
        /* set counter to size rounded up to even */
        i = ((scalarSizeInBits + 1) & ~1) - 2;

        twoBits = PkiGetNextTwoMsBits(pScalar, &word, i);
        /* RL ! check for EC edw25519  */
        if (twoBits != 1)
                return CC_EC_EDW_INVALID_SCALAR_DATA_ERROR;
        i -= 2;

        PKA_PRINTF("Init loop: i= %d, twoBits= %d\n", i, twoBits);
        PKI_DBG_PRINT_REGS("S=g32: \n", EDW_REG_XS, EDW_REG_YS, EDW_REG_ZS, EDW_REG_TS);

        while (1) {
                int32_t swt;

                /* get two next MS bits of key */
                twoBits = PkiGetNextTwoMsBits(pScalar, &word, i);
                swt = carry*4 + twoBits;

                switch (swt) {
                /*negative points add*/
                case -4: PkaEcEdwAddExtPrcExt(EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_DG16,EDW_REG_SG16,EDW_REG_MPG16);  carry =  0;   break;
                case -3: PkaEcEdwAddExtPrcExt(EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_DG8, EDW_REG_SG8, EDW_REG_MPG8);   carry = -1;   break;
                case -2: PkaEcEdwAddExtPrcExt(EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_DG8, EDW_REG_SG8, EDW_REG_MPG8);   carry =  0;   break;
                case -1: PkaEcEdwAddExtPrcExt(EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_DG4, EDW_REG_SG4, EDW_REG_MPG4);   carry =  0;   break;
                /*positive points add*/
                case  0: PkaEcEdwAddExtPrcExt(EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_SG4, EDW_REG_DG4, EDW_REG_PG4);    carry = -1;   break;
                case  1: PkaEcEdwAddExtPrcExt(EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_SG4, EDW_REG_DG4, EDW_REG_PG4);    carry =  0;   break;
                case  2: PkaEcEdwAddExtPrcExt(EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_SG8, EDW_REG_DG8, EDW_REG_PG8);    carry =  0;   break;
                case  3: PkaEcEdwAddExtPrcExt(EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS, EDW_REG_SG16,EDW_REG_DG16,EDW_REG_PG16);   carry = -1;   break;
                default:
                        return CC_EC_EDW_INVALID_SCALAR_DATA_ERROR;
                }

                PKA_PRINTF("\ni= %d b2= %02X b2c4= %3d carry= %d \n", i, twoBits, swt, carry);
                PKI_DBG_PRINT_REGS("", EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS);

                if ((i -= 2) == 0) {
                        // RL Debug
                        PKA_PRINTF("break: i= %d carry= %d\n", i, carry);
                        break; /*end loop*/
                }

                /* doubling */
                PkaEcEdwDoublExtExt(EDW_REG_XS, EDW_REG_YS, EDW_REG_ZS, EDW_REG_TS);
                PkaEcEdwDoublExtExt(EDW_REG_XS ,EDW_REG_YS, EDW_REG_ZS, EDW_REG_TS); // s *= 4
        }

        /* check, that the LS bit pair is 00 */
        if(PkiGetNextTwoMsBits(pScalar, &word, i) != 0) {
                return CC_EC_EDW_INVALID_SCALAR_DATA_ERROR;
        }

        /* calculation of result for case that carry is -1; note: registers of *
        *  point G16 now are used as temp registers for rXs2,rYs2,rZs2,rTs2    */
        PkaEcEdwAddExtPrcExt(EDW_REG_SG16/*xS2*/,EDW_REG_DG16/*yS2*/,EDW_REG_PG16/*zS2*/,EDW_REG_MPG16/*tS2*/,
                             EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS,EDW_REG_TS,  EDW_REG_DG4,EDW_REG_SG4,EDW_REG_MPG4);

       if (carry == -1)
                PkaEcEdwPointToAfn(EDW_REG_SG8,EDW_REG_DG8,  EDW_REG_SG16/*xS2*/,EDW_REG_DG16/*yS2*/,EDW_REG_PG16/*zS2*/);
        else
                PkaEcEdwPointToAfn(EDW_REG_SG8,EDW_REG_DG8,  EDW_REG_XS,EDW_REG_YS,EDW_REG_ZS);

      return err;
}



/**
 * The function performs two scalar mult. and add of input and base points simultaneously:
 *         R(x,y) = a*P(x,y) + b*G(x,y), where P - point, G - base point.
 *
 *  Implemented algorithm of Bernstein D. etc. (version of A.Klimov).
 *
 *  PKA registers are defined in pka_ec_edw_glob_regs_defh file, in partial:
 *      - output point R(x,y) shall be registers (rXR=EDW_REG_XR, rYR=EDW_REG_YR),
 *      - input point P(X,Y) by (rXP=EDW_REG_XP, rYP=EDW_REG_YP).
 *
 * @author reuvenl (11/25/2015)
 *
 * @return CC_OK or an error according to mbedtls_cc_ec_mont_edw_error.h definitions.
 */
CCError_t PkaEcEdwAddTwoScalarMult(
        uint32_t rXR, uint32_t rYR,       /*!< [out] the ID-s of registers, containing aff.
                                               coordinates of result point P */
        uint32_t rXP, uint32_t rYP,       /*!< [in] the ID-s of registers, containing aff.
                                               coordinates of input point P */
        uint32_t *pScalarA,               /*!< [in] the pointer to the scalsr A (LS word is
                                               leftmost one, MS word - rightmost). */
        size_t    scAsizeInBits,          /*!< exact size of the scalar A in bits. */
        uint32_t *pScalarB,               /*!< [in] the pointer to the scalsr B (LS word is
                                               leftmost one, MS word - rightmost). */
        size_t    scBsizeInBits,          /*!< exact size of the scalar B in bits. */
        const CCEcEdwDomain_t *pEcDomain /*!< [in] pointer to EC domain (curve). */)
{
        /* Definitions */
        uint32_t twoBits;
        int32_t i;
        size_t   edwSizeWords = pEcDomain->ecModSizeInWords;

        if(scAsizeInBits == 0 || scBsizeInBits == 0)
                return CC_EC_EDW_INVALID_SCALAR_SIZE_ERROR;

        /*------------------------------------------------------------------*/
        /* Load and calculate all EC domain and input data for scalar mult. */
        /*------------------------------------------------------------------*/

        /* set D2  */
        PkaCopyDataIntoPkaReg(EDW_REG_D2, LEN_ID_N_PKA_REG_BITS, pEcDomain->ecParamD, edwSizeWords);
        PKA_MOD_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_D2, EDW_REG_D2, EDW_REG_D2);
        /* set EDW_REG_N_4 = 4*N */
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_N_4, EDW_REG_N, EDW_REG_N);
        PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_N_4, EDW_REG_N_4, EDW_REG_N_4);

        /* set modified coordinates of EC point G  */
        PkaCopyDataIntoPkaReg(EDW_REG_XG, LEN_ID_N_PKA_REG_BITS, pEcDomain->ecGenX, edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_YG, LEN_ID_N_PKA_REG_BITS, pEcDomain->ecGenY, edwSizeWords);
        PKA_CLEAR(LEN_ID_N_PKA_REG_BITS, EDW_REG_ZG);
        PKA_SET_BIT0(LEN_ID_N_PKA_REG_BITS, EDW_REG_ZG, EDW_REG_ZG); /*ZG = 1*/
        PkaCopyDataIntoPkaReg(EDW_REG_TG, LEN_ID_N_PKA_REG_BITS, pEcDomain->ecGenT, edwSizeWords);

        /* load EC G point in precalculated form */
        PkaCopyDataIntoPkaReg(EDW_REG_SG1, LEN_ID_N_PKA_REG_BITS, pEcDomain->sg, edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_DG1, LEN_ID_N_PKA_REG_BITS, pEcDomain->dg, edwSizeWords);
        PkaCopyDataIntoPkaReg(EDW_REG_PG1, LEN_ID_N_PKA_REG_BITS, pEcDomain->pg, edwSizeWords);

        /* convert input point P to precalculated form: sp=yp+xp; dp=yp-xp; pp=d2*xp*yp mod n */
        PKA_MOD_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_SP, rYP, rXP);
        //PKA_ADD(LEN_ID_N_PKA_REG_BITS, EDW_REG_DP, rYP, EDW_REG_N);
        PKA_MOD_SUB(LEN_ID_N_PKA_REG_BITS, EDW_REG_DP, rYP, rXP);
        PKA_MOD_MUL(LEN_ID_N_BITS, EDW_REG_PP, EDW_REG_D2, rXP);
        PKA_MOD_MUL(LEN_ID_N_BITS, EDW_REG_PP, EDW_REG_PP, rYP);
        // P+G
        PkaEcEdwAddExtPrcExt(EDW_REG_XPG, EDW_REG_YPG, EDW_REG_ZPG, EDW_REG_TPG, /*modif. P+G*/
                             EDW_REG_XG,  EDW_REG_YG,  EDW_REG_ZG,  EDW_REG_TG,  /*modif. G*/
                             EDW_REG_SP,  EDW_REG_DP,  EDW_REG_PP );             /*precalc. P*/

        /*------------------------------------------------*/
        /* load start points according to scalars MS bits */
        /*------------------------------------------------*/

        i = CC_MAX(scAsizeInBits, scBsizeInBits) - 1;
        twoBits = (PKI_GET_BIT_FROM_WORDS_ARRAY(pScalarA, i) << 1) +
                  PKI_GET_BIT_FROM_WORDS_ARRAY(pScalarB, i);

        switch (twoBits) {
        case 1: // 01: r = G
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, rXR, EDW_REG_XG);
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, rYR, EDW_REG_YG);
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, EDW_REG_ZR, EDW_REG_ZG);
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, EDW_REG_TR, EDW_REG_TG);
                break;
        case 2: // 10: r = P
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, rXR, rXP);
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, rYR, rYP);
                PKA_CLEAR(LEN_ID_N_PKA_REG_BITS, EDW_REG_ZR);
                PKA_SET_BIT0(LEN_ID_N_PKA_REG_BITS, EDW_REG_ZR, EDW_REG_ZR);
                PKA_MOD_MUL_NFR(LEN_ID_N_BITS, EDW_REG_TR, rXR, rYR);
                break;
        case 3: // 11: r = P+G
                /* set result point R = P+G*/
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, rXR, EDW_REG_XPG);
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, rYR, EDW_REG_YPG);
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, EDW_REG_ZR, EDW_REG_ZPG);
                PKA_COPY(LEN_ID_N_PKA_REG_BITS, EDW_REG_TR, EDW_REG_TPG);
                break;

        default:
                return CC_EC_EDW_INVALID_SCALAR_DATA_ERROR;
        }

        /*------------------------------------------------------------------*/
        /*    Perform two points scalar mult. and adding simultaneously     */
        /*------------------------------------------------------------------*/

        while (--i >= 0) {

                /* get two MSBits from scalars A snd B */
                twoBits = (PKI_GET_BIT_FROM_WORDS_ARRAY(pScalarA, i) << 1) +
                           PKI_GET_BIT_FROM_WORDS_ARRAY(pScalarB, i);

                /* point doubling */
                PkaEcEdwDoublExtExt(rXR, rYR, EDW_REG_ZR, EDW_REG_TR);

                switch (twoBits) {
                case 0:
                        break;
                case 1:
                        // 10: r += G
                        PkaEcEdwAddExtPrcExt(rXR, rYR, EDW_REG_ZR, EDW_REG_TR,
                                             rXR, rYR, EDW_REG_ZR, EDW_REG_TR,
                                             EDW_REG_SG1, EDW_REG_DG1, EDW_REG_PG1);
                        break;
                case 2:
                        // 01: r += P
                        PkaEcEdwAddExtPrcExt(rXR, rYR, EDW_REG_ZR, EDW_REG_TR,
                                             rXR, rYR, EDW_REG_ZR, EDW_REG_TR,
                                             EDW_REG_SP, EDW_REG_DP, EDW_REG_PP);
                        break;
                case 3:
                        // 11: r += P+G
                        PkaEcEdwAddExtExtExt(rXR,  rYR,  EDW_REG_ZR,  EDW_REG_TR,
                                             rXR,  rYR,  EDW_REG_ZR,  EDW_REG_TR,
                                             EDW_REG_XPG, EDW_REG_YPG, EDW_REG_ZPG, EDW_REG_TPG);
                        break;
                default:
                        return CC_EC_EDW_INVALID_SCALAR_DATA_ERROR;
                }
        }

        /* convert result to affine */
        PkaEcEdwPointToAfn(rXR, rYR,/*out*/ rXR,   rYR, EDW_REG_ZR/*in*/);

        return CC_OK;
}

/**
 * The function calculated coordinate X of compressed EC point,
 * using the given coordinate Y.
 *
 * Implemented algorithm Bernstain D. etc (stated by Klimov A.).
 *
 * @author reuvenl (1/11/2016)
 *
 * Imlicit parametrs
 * @param  [out] rX - ID of PKA register for output decompressed coordinate X.
 * @param  [in/out] rY - ID of PKA register, containing compressed/decompressed coordinate Y.
 * @param  [in] isOddX - indication: "Is the coordinate X odd".
 *
 */
void PkaEcEdwDecompress(uint32_t rX, uint32_t rY,
                        uint32_t isOddX) /*!< one bit indication: "Is the coordinate X odd" */
{
        uint32_t  bit0; /*used to read values from regs. */

        /* decompress: (YP) -> (XP,YP,ZP=1,TP) */
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, EDW_REG_T3, rY, rY);                 // hwmmul(t3, y, y, n, np);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, EDW_REG_T4, EDW_REG_T3, EDW_REG_D);  // hwmmul(t4, t3, ec_d, n, np);
        PKA_SUB_IM(LEN_ID_N_PKA_REG_BITS, EDW_REG_T3, EDW_REG_T3, 1);       // hwdec(t3, t3);
        PKA_ADD_IM(LEN_ID_N_PKA_REG_BITS, EDW_REG_T4, EDW_REG_T4, 1);       // hwinc(t4, t4);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, EDW_REG_T, EDW_REG_T4, EDW_REG_T4);  // hwmmul(t, t4, t4, n, np);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, EDW_REG_T, EDW_REG_T4, EDW_REG_T);   // hwmmul(t, t4, t, n, np);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, rX, EDW_REG_T, EDW_REG_T);           // hwmmul(x, t, t, n, np);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, rX, rX, EDW_REG_T4);                 // hwmmul(x, x, t4, n, np);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, EDW_REG_T5, rX, EDW_REG_T3);         // hwmmul(t5, x, t3, n, np);
        PKA_MOD_EXP(LEN_ID_N_BITS, rX, EDW_REG_T5, EDW_REG_Q58);            // hwmexp(x, t5, q58, n, np);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, rX, rX, EDW_REG_T3);                 // hwmmul(x, x, t3, n, np);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, rX, rX, EDW_REG_T);                  // hwmmul(x, x, t, n,np);
        PKA_MOD_MUL_NFR(LEN_ID_N_BITS, EDW_REG_T, rX, rX);                  // hwmmul(t, x, x, n,np);
        PKA_MOD_MUL_ACC(LEN_ID_N_BITS, EDW_REG_T, EDW_REG_T4, EDW_REG_T, EDW_REG_T3); // hwmlap(t,t4, t, t3, n, np, 0);

        PKA_DIV(LEN_ID_N_PKA_REG_BITS, EDW_REG_T4, EDW_REG_T, EDW_REG_N);
        PKA_COMPARE_IM_STATUS(LEN_ID_N_PKA_REG_BITS, EDW_REG_T, 0/*im.val*/, bit0/*status*/);
        if(bit0) {
                PKA_MOD_MUL_NFR(LEN_ID_N_BITS, rX, rX, EDW_REG_SQRTM1);
        }

        PKA_DIV(LEN_ID_N_PKA_REG_BITS, EDW_REG_T4, rX, EDW_REG_N);
        PKA_READ_BIT0(LEN_ID_N_PKA_REG_BITS, rX, bit0/*bit0*/);
        if(bit0 != isOddX)
                PKA_SUB(LEN_ID_N_PKA_REG_BITS, rX, EDW_REG_N, rX);

}



