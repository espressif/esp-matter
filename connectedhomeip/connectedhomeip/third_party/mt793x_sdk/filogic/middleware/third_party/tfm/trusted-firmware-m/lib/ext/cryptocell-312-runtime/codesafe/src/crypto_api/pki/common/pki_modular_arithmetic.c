/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



#include "cc_pal_types.h"
#include "cc_pal_mem.h"
#include "cc_common_math.h"
#include "cc_ecpki_types.h"
#include "pka.h"
#include "ec_wrst.h"
#include "pki.h"
#include "pki_modular_arithmetic.h"

#include "pki_dbg.h"
/* virt. pointers to pka regs. (regs. ID-s)*/
#include "pka_point_compress_regs_def.h"



/***********     PkiCalcJacobiSymbol  function     **********************/
/**
 * @brief Calculate Jacobi symbol for a and prime b numbers.
 *  Assumed: a, b are a positive numbers.
 *  Note: PKA_REG_A, PKA_REG_B registers are destroed by the function.
 *
 * @author reuvenl (10/26/2014)
 *
 * @return int32_t jacobi symbol value.
 */
int32_t PkiCalcJacobiSymbol(void)
{
    int32_t r,  t;
    uint32_t stat, w, w1;


    /* Note: Check GCD - not need because b - prime.
       Convert a to  positive numbers - not need  */

    /* case PKA_REG_A = 0 or PKA_REG_A = 1 */
    PKA_COMPARE_IM_STATUS(LEN_ID_N_PKA_REG_BITS, PKA_REG_A, 0, stat); /*if(a==0) r = 0*/
    if(stat == 1) {
        r = 0;
        goto End;
    }

    PKA_COMPARE_IM_STATUS(LEN_ID_N_PKA_REG_BITS, PKA_REG_A, 1, stat); /*if(a==0) r = 0*/
    if(stat == 1) {
        r = 1;
        goto End;
    }

    r = 1;

    /* Evaluate Jacobi symb. */
    do {
        /* 1. Remove 0-LS bits of PKA_REG_A */
         t = 0;
         w = 0;

         while(w == 0) {/* remove 0- words */
            PKA_READ_WORD_FROM_REG(w, 0, PKA_REG_A);
            if(w == 0) {
                PKA_SHR_FILL0(LEN_ID_N_PKA_REG_BITS, PKA_REG_A, PKA_REG_A, 32-1);
                t += 32;
            }
         }

         while((w & 1) == 0){
            w >>= 1;
            t += 1;
         }
         if((t & 0x1F) != 0) {/* removes 0-bits */
            PKA_SHR_FILL0(LEN_ID_N_PKA_REG_BITS, PKA_REG_A, PKA_REG_A, (t & 0x1F) - 1);
         }

         /* 2. Change sign if b mod 8 == 3 or 5 */
         PKA_READ_WORD_FROM_REG(w1, 0, PKA_REG_B);

         if(t & 1) {
            if((w1 & 7) == 3 || (w1 & 7) == 5)
                r = -r;
         }

         /* 3. Quadratic reciprocity law */
         if((w & 3) == 3 && (w1 & 3) == 3){
             r = -r;
         }

         PKA_COPY(LEN_ID_N_PKA_REG_BITS, PKA_REG_C, PKA_REG_A);
         PKA_COPY(LEN_ID_N_PKA_REG_BITS, PKA_REG_A, PKA_REG_B);
         PKA_DIV(LEN_ID_N_PKA_REG_BITS, PKA_REG_B, PKA_REG_A, PKA_REG_C);  /* a = b mod a */
         PKA_COPY(LEN_ID_N_PKA_REG_BITS, PKA_REG_B, PKA_REG_C);     /* b = a prev. */

         PKA_COMPARE_IM_STATUS(LEN_ID_N_PKA_REG_BITS, PKA_REG_A, 0, stat);

    } while(stat != 1); /*while a != 0*/
End:

return r;

} /* End of PkiCalcJacobiSymbol*/


/***********     PkiIsModSquareRootExists  function     **********************/
/**
 * @brief The function calculates square root modulo prime:
 *   PKA_REG_Y1 = PKA_REG_Y2 ^ 1/2 mod rP if root exists, else returns an error.
 *
 *   Assuming: 1. The modulus N is a prime.
 *             2. Y2 is less than modulus.
 *
 * @return true if the root exists, or false if not.
 */
bool PkiIsModSquareRootExists(void)
{
    uint32_t w=0, stat;
    int32_t s, i;
    bool rootEx = false;
    int32_t jcb;


    /* if Y^2 = 0, return Y=0 */
    PKA_COMPARE_IM_STATUS(LEN_ID_N_PKA_REG_BITS, PKA_REG_Y2, 0, stat);
    if(stat == 1){
        PKA_CLEAR(LEN_ID_N_PKA_REG_BITS, PKA_REG_Y1); /* Y1=0*/
        rootEx = true;
        goto End;
    }

    /* read w = mod[0]*/
    PKA_READ_WORD_FROM_REG(w, 0/*i*/, PKA_REG_N);

    /* ----------------------------------------- */
    /* Case P=3 mod 4, then PKA_REG_Y1 = +- PKA_REG_Y2^(P+1)/4 */
    /* ----------------------------------------- */
    if((w & 0x3) == 3) {
        PKA_ADD_IM(LEN_ID_N_PKA_REG_BITS, PKA_REG_Y1, PKA_REG_N, 1);
        PKA_SHR_FILL0(LEN_ID_N_PKA_REG_BITS, PKA_REG_T, PKA_REG_Y1, 2-1);
        PKA_MOD_EXP(LEN_ID_N_BITS, PKA_REG_Y1, PKA_REG_Y2, PKA_REG_T);
        goto End;
    }

    /* ------------------------------------------------ */
    /* Case P=5 mod 8, then PKA_REG_Y1 calculated by algorithm */
    /* ------------------------------------------------ */
    if((w & 0x7) == 5) {
        PKA_SUB_IM(LEN_ID_N_PKA_REG_BITS, PKA_REG_YT, PKA_REG_N, 1);  /* PKA_REG_YT = PKA_REG_N-1*/
        PKA_SHR_FILL0(LEN_ID_N_PKA_REG_BITS, PKA_REG_Z, PKA_REG_YT, 2-1);
        PKA_MOD_EXP(LEN_ID_N_BITS, PKA_REG_T, PKA_REG_Y2, PKA_REG_Z);   /* d = PKA_REG_T = PKA_REG_Y2^((PKA_REG_N-1)/4)*/
        PKA_COMPARE_IM_STATUS(LEN_ID_N_PKA_REG_BITS, PKA_REG_T, 1, stat);
        if(stat == 1) {
            PKA_ADD_IM(LEN_ID_N_PKA_REG_BITS, PKA_REG_T, PKA_REG_N, 3);
            PKA_SHR_FILL0(LEN_ID_N_PKA_REG_BITS, PKA_REG_T, PKA_REG_T, 3-1);
            PKA_MOD_EXP(LEN_ID_N_BITS, PKA_REG_Y1, PKA_REG_Y2, PKA_REG_T);
        }
        else {
            PKA_COMPARE_STATUS(LEN_ID_N_PKA_REG_BITS, PKA_REG_T, PKA_REG_YT, stat);  /* PKA_REG_T =? PKA_REG_N-1*/
            if(stat == 1) {
                PKA_SUB_IM(LEN_ID_N_PKA_REG_BITS, PKA_REG_T, PKA_REG_N, 5);
                PKA_SHR_FILL0(LEN_ID_N_PKA_REG_BITS, PKA_REG_T, PKA_REG_T, 3-1);
                PKA_SHL_FILL0(LEN_ID_N_PKA_REG_BITS, PKA_REG_YT, PKA_REG_Y2, 2-1);  /* PKA_REG_YT = 4*PKA_REG_Y2 */
                PKA_MOD_EXP(LEN_ID_N_BITS, PKA_REG_Z, PKA_REG_YT, PKA_REG_T);
                PKA_SHL_FILL0(LEN_ID_N_PKA_REG_BITS, PKA_REG_YT, PKA_REG_Y2, 1-1);  /* PKA_REG_YT = 2*PKA_REG_Y2 */
                PKA_MOD_MUL(LEN_ID_N_BITS, PKA_REG_Y1, PKA_REG_Z, PKA_REG_YT);   /* PKA_REG_Y1 = 2*PKA_REG_Y2*(4rY2)^((PKA_REG_N-5)/8) */
            }
        }

        goto End;
    }


    /* --------------------------------- */
    /* Case of other modulus structure   */
    /* --------------------------------- */

    /* check if root exist using jacoby symbol */
    PKA_COPY(LEN_ID_N_PKA_REG_BITS, PKA_REG_A, PKA_REG_Y2);
    PKA_COPY(LEN_ID_N_PKA_REG_BITS, PKA_REG_B, PKA_REG_N);
    jcb = PkiCalcJacobiSymbol(/*, PKA_REG_A, PKA_REG_B*/);
    if(jcb == -1)
        goto End;

    /* state P-1 as Q * 2^s, where Q is odd */
    PKA_SUB_IM(LEN_ID_N_PKA_REG_BITS, PKA_REG_Y1, PKA_REG_N, 1);  /**/
    w -= 1;
    s = 0;
    while(w == 0) { /*remove 0-words*/
        PKA_SHR_FILL0(LEN_ID_N_PKA_REG_BITS, PKA_REG_Y1, PKA_REG_Y1, 32-1);
        s += 32;
        PKA_READ_WORD_FROM_REG(w, 0/*i*/, PKA_REG_Y1);
    }
    /* remove 0-bits */
    i = 0;
    while((w & 1) == 0){
        w >>= 1;
        i++;
    }
    s += i;
    if(i > 0)
        PKA_SHR_FILL0(LEN_ID_N_PKA_REG_BITS, PKA_REG_Y1, PKA_REG_Y1, i-1);

    /* find first non residue number (modulo N) starting from 2 */
    jcb = 0;
    PKA_CLEAR(LEN_ID_N_PKA_REG_BITS, PKA_REG_Z);
    PKA_SET_BIT0(LEN_ID_N_PKA_REG_BITS, PKA_REG_Z, PKA_REG_Z); /* z = 1*/
    while (jcb != -1) {
        PKA_ADD_IM(LEN_ID_N_PKA_REG_BITS, PKA_REG_Z, PKA_REG_Z, 1);

        /*set jacoby input values */
        PKA_COPY(LEN_ID_N_PKA_REG_BITS, PKA_REG_A, PKA_REG_Z);
        PKA_COPY(LEN_ID_N_PKA_REG_BITS, PKA_REG_B, PKA_REG_N);
        jcb = PkiCalcJacobiSymbol(/*, PKA_REG_A, PKA_REG_B*/);
    }

    PKA_MOD_EXP(LEN_ID_N_BITS, PKA_REG_Z, PKA_REG_Z, PKA_REG_Y1); /* */
    PKA_ADD_IM(LEN_ID_N_PKA_REG_BITS, PKA_REG_Y1, PKA_REG_Y1, 1);
    PKA_SHR_FILL0(LEN_ID_N_PKA_REG_BITS, PKA_REG_Y1, PKA_REG_Y1, 1-1);  /* PKA_REG_Y1 = (PKA_REG_Y1+1)/2  */
    PKA_MOD_EXP(LEN_ID_N_BITS, PKA_REG_Y1, PKA_REG_Y2, PKA_REG_Y1);  /* PKA_REG_Y1 = PKA_REG_Y2^PKA_REG_Y1  */
    PKA_COPY(LEN_ID_N_PKA_REG_BITS, PKA_REG_YT, PKA_REG_Y2);
    PKA_MOD_INV(LEN_ID_N_BITS, PKA_REG_T, PKA_REG_YT);
    for(;;) {
        PKA_MOD_MUL(LEN_ID_N_BITS, PKA_REG_YT, PKA_REG_Y1, PKA_REG_Y1);   /* PKA_REG_YT = PKA_REG_Y1^2  */
        PKA_MOD_MUL(LEN_ID_N_BITS, PKA_REG_YT, PKA_REG_YT, PKA_REG_T);    /* PKA_REG_YT = PKA_REG_YT * PKA_REG_Y2^-1  */
        i = 0;
        while(1) {
            /*if(PKA_REG_YT == 1) break;*/
            PKA_COMPARE_IM_STATUS(LEN_ID_N_PKA_REG_BITS, PKA_REG_YT, 1, stat);
            if(stat == 1)
                break;
            i++;
            PKA_MOD_MUL(LEN_ID_N_BITS, PKA_REG_YT,PKA_REG_YT, PKA_REG_YT);    /* PKA_REG_YT = PKA_REG_YT^2 */
        }
        /* if PKA_REG_Y1^2 * PKA_REG_Y2^-1 == 1 (mod rP), return */
        if(i == 0) {
            rootEx = true;
            goto End;
        }
        if(s-i == 1) { /* mul instead pow */
            PKA_MOD_MUL(LEN_ID_N_BITS, PKA_REG_Y1, PKA_REG_Y1, PKA_REG_Z);
        } else {
            w  =  1 << ((s-i-1) & 31);
            i = (s-i-1) / 32; /*i was free*/
            PKA_CLEAR(LEN_ID_N_PKA_REG_BITS, PKA_REG_EX);
            PKA_WRITE_WORD_TO_REG(w, i, PKA_REG_EX);
            PKA_MOD_EXP(LEN_ID_N_BITS, PKA_REG_YT, PKA_REG_Z, PKA_REG_EX);
            PKA_MOD_MUL(LEN_ID_N_BITS, PKA_REG_Y1, PKA_REG_Y1, PKA_REG_YT);   /* PKA_REG_Y1 = r * PKA_REG_Z^(2^(s-i-1)) */
        }
    }
End:
    /* Check result for PKA_REG_N mod 8 = {3,5} */
    if((w & 3) == 3 || (w & 7) == 5){
        PKA_MOD_MUL(LEN_ID_N_BITS, PKA_REG_T, PKA_REG_Y1, PKA_REG_Y1);
        PKA_COMPARE_STATUS(LEN_ID_N_PKA_REG_BITS, PKA_REG_T, PKA_REG_Y2, stat);
        if(stat == 1)
            rootEx = true;
    }

    return rootEx;
}

