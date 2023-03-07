/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef CC_IOT
    #if defined(MBEDTLS_CONFIG_FILE)
    #include MBEDTLS_CONFIG_FILE
    #endif
#endif

#if !defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C))


/************* Include Files ****************/

#include "cc_rsa_error.h"
#include "cc_rsa_types.h"
#include "pka.h"
#include "pki.h"
#include "cc_pal_mutex.h"
#include "pka_error.h"
#include "rsa.h"
#include "rsa_private.h"

extern const int8_t regTemps[PKA_MAX_COUNT_OF_PHYS_MEM_REGS];


/***********     RsaExecPrivKeyExpNonCrt  function      **********************/
/**
 * @brief Executes the RSA primitive: private key CRT exponent
 *
 *    Algorithm [PKCS #1 v2.1]:
 *
 *     1. If NonCRT exponent, then  M  =  C^D  mod N.
 *
 *     Where: M- message representative, C- ciphertext, D- priv.exponent, N- modulus,
 *            ^ - exponentiation symbol.
 *
 *     Note: PKA registers used: r0,r1,r2,r3,r4,  r30,r31.
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
static CCError_t RsaExecPrivKeyExpNonCrt(CCRsaPrivKey_t    *pPrivKey , /*!< [in]  Private key database. */
                     CCRsaPrimeData_t *pPrivData)       /*!< [in/out]  Containing DataIn and DataOut buffers. */
{
    CCError_t error = CC_OK;
    uint32_t modSizeWords, dSizeInWords;
    uint32_t pkaReqRegs = 7;
    /* set virtual registers pointers  */
    uint8_t rT2= regTemps[2]; /*2*/
    uint8_t rT3 = regTemps[3]; /*3*/
    uint8_t rT4 = regTemps[4]; /*4*/

    /* modulus N size in bytes */
    modSizeWords = CALC_FULL_32BIT_WORDS(pPrivKey->nSizeInBits);
    if (modSizeWords > CALC_FULL_32BIT_WORDS(CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS)) {
        return CC_RSA_INVALID_MODULUS_SIZE;
    }

    /* priv. exponent size in bytes */
    dSizeInWords = CALC_FULL_32BIT_WORDS(pPrivKey->PriveKeyDb.NonCrt.dSizeInBits);

    error = PkaInitAndMutexLock(pPrivKey->nSizeInBits,&pkaReqRegs);
    if (error != CC_OK) {
        return error;
    }

    /*      copy the N, Np DataIn and D into PKA registers                  */
    /* N => r0 */
    /* copy modulus N into PKA register: N=>r0 */
    PkaCopyDataIntoPkaReg(PKA_REG_N/*dstReg*/, LEN_ID_MAX_BITS/*LenID*/, pPrivKey->n/*srcPtr*/,modSizeWords);

    /* copy the NP into r1 register NP */
    PkaCopyDataIntoPkaReg(PKA_REG_NP/*dstReg*/, LEN_ID_MAX_BITS/*LenID*/,
                   ((RsaPrivKeyDb_t*)(pPrivKey->ccRSAPrivKeyIntBuff))->NonCrt.NP/*srcPtr*/,
                   CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS);

    /* copy input data into PKI register: DataIn=>r2 */
    PkaCopyDataIntoPkaReg( rT2/*dstReg*/, LEN_ID_MAX_BITS/*LenID*/, pPrivData->DataIn, modSizeWords);

    /* copy input data into PKI register: DataIn=>r2 */
    PkaCopyDataIntoPkaReg( rT3/*dstReg*/, LEN_ID_MAX_BITS/*LenID*/, pPrivKey->PriveKeyDb.NonCrt.d, dSizeInWords);


    /* .. calculate the exponent Res = DataIn^D mod N;                  ... */
    PKA_MOD_EXP(LEN_ID_N_BITS/*LenID*/, rT4/*Res*/, rT2/*OpA*/, rT3/*OpB*/);


    /*  Finish PKA and copy result */
    /* copy result into output buffer */
    /* copy result into output: r4 =>DataOut */
    PkaCopyDataFromPkaReg(pPrivData->DataOut, modSizeWords, rT4/*srcReg*/);

    PkaFinishAndMutexUnlock(pkaReqRegs);

    return error;

}


/***********    RsaExecPrivKeyExpCrt   function      **********************/
/**
 * @brief This function executes the RSA primitive: private key CRT exponent.
 *        adapted for Keys up to 2K bits size.
 *
 *    Algorithm [PKCS #1 v2.1]:
 *
 *   CRT exponentiation algorithm:
 *        1. Mq  =  C^dQ mod Q;
 *        2. Mp  =  C ^dP mod P,
 *        3  h = (Mp-Mq)*qInv mod P;
 *        4. M = Mq + Q * h.
 *
 *     Where: M- message representative, C- ciphertext, D- priv.exponent, N- modulus,
 *            P,Q,dP,dQ, qInv - CRT private key parameters;
 *            ^ - exponentiation symbol.
 *
 *     Note: 9 PKA registers are used: r0-r6,  r30,r31.
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
static CCError_t RsaExecPrivKeyExpCrt(CCRsaPrivKey_t    *pPrivKey , /*!< [in]  Private key database. */
                  CCRsaPrimeData_t *pPrivData)        /*!< [in/out]  Containing DataIn and DataOut buffers. */

{
    CCError_t error = CC_OK;
    uint32_t  modSizeWords, pqSizeInWords;
    uint32_t pkaReqRegs = 10;

    /* virtual registers pointers
       Note: don't change rQ = 6  */
    int8_t  rN  = PKA_REG_N;
    int8_t  rNP = PKA_REG_NP;
    int8_t  rD  = regTemps[2];
    int8_t  rT  = regTemps[3];
    int8_t  rT1 = regTemps[4];
    int8_t  rMq = regTemps[5];
    int8_t  rQ  = regTemps[6];
    int8_t  rqInv = regTemps[7];


    /* modulus N size in bytes */
    modSizeWords = CALC_FULL_32BIT_WORDS(pPrivKey->nSizeInBits);
    if (modSizeWords > CALC_FULL_32BIT_WORDS(CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS)) {
        return CC_RSA_INVALID_MODULUS_SIZE;
    }

    error = PkaInitAndMutexLock(pPrivKey->nSizeInBits, &pkaReqRegs);
    if (error != CC_OK) {
        return error;
    }

    /*  set Sizes table: 0- Nsize, 1- Nsize+1w (is done), 2- Psize  */
    PkaSetLenIds(pPrivKey->PriveKeyDb.Crt.PSizeInBits, LEN_ID_PQ_BITS);

    /* P and Q size in bytes */
    pqSizeInWords = CALC_FULL_32BIT_WORDS(pPrivKey->PriveKeyDb.Crt.PSizeInBits);

    /* PKA modular operations  according to modulus Q:              */

    /* copy CRT parametersrs Q, dQ, QP into PKA registers */
    PkaCopyDataIntoPkaReg( rN/*0 dstReg*/, LEN_ID_MAX_BITS/*LenID*/, pPrivKey->PriveKeyDb.Crt.Q/*src_ptr*/, pqSizeInWords);

    PkaCopyDataIntoPkaReg( rD/*2 dstReg*/, LEN_ID_MAX_BITS/*LenID*/, pPrivKey->PriveKeyDb.Crt.dQ/*src_ptr*/, pqSizeInWords);

    PkaCopyDataIntoPkaReg( rNP/*1 dstReg*/, LEN_ID_MAX_BITS/*LenID*/, ((RsaPrivKeyDb_t*)(pPrivKey->ccRSAPrivKeyIntBuff))->Crt.QP/*src_ptr*/,
                CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS );
    /* copy DataIn into rT and rT! */
    PkaCopyDataIntoPkaReg(rT/*3 dstReg*/, LEN_ID_MAX_BITS/*LenID*/, pPrivData->DataIn/*src_ptr*/, modSizeWords );
    PKA_COPY(LEN_ID_MAX_BITS/*LenID*/, rT1/*4 dest*/, rT/*3 src*/);

    /*  Calculation of Mq          */
    /* reduction of the input data by modulus Q  rT = rT mod Q */
    PKA_DIV(LEN_ID_N_PKA_REG_BITS/*LenID*/, rQ/*6 Res not used*/, rT/*3 OpA*/, rN/*0 OpB=rN=Q*/);

    /* operation changes from p/q size to N size, need clearing rT high bits */

    /*  calculate of Mq = DataIn^dQ mod Q: Mq = rT^rD mod rN        */
    PKA_MOD_EXP(LEN_ID_PQ_BITS/*LenID*/, rMq/*5 Res*/, rT/*3 OpA*/, rD/*2 OpB*/);

    /* PKA modular operations  according to modulus P:              */
    /* copy prime factor P into rQ for swapping with rN */
    PkaCopyDataIntoPkaReg(rQ/*6 dstReg*/, LEN_ID_MAX_BITS/*LenID*/, pPrivKey->PriveKeyDb.Crt.P/*src_ptr*/, pqSizeInWords);
    /* swap rQ <-> rN so that Q->rQ and P->rN */
    rQ = PKA_REG_N; rN = 6;

    /* set new value to N_NP_TO_T1 register according N->6, Np->1,T0->30,T1->31: 0x000FF826*/
    PKA_SET_N_NP_T0_T1_REG(rN, PKA_REG_NP, PKA_REG_T0, PKA_REG_T1);


    /* copy Barrett tag PP: PP=>NP */
    PkaCopyDataIntoPkaReg(rNP/*1 dstReg*/, LEN_ID_MAX_BITS/*LenID*/, ((RsaPrivKeyDb_t*)(pPrivKey->ccRSAPrivKeyIntBuff))->Crt.PP/*src_ptr*/,
                   CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS);

    /* copy priv. exponent factor dP: dP=>rD */
    PkaCopyDataIntoPkaReg(rD/*2 dstReg*/, LEN_ID_MAX_BITS/*LenID*/, pPrivKey->PriveKeyDb.Crt.dP/*src_ptr*/, pqSizeInWords);

    /* copy qInv coefficient: qInv=>rqInv   */
    PkaCopyDataIntoPkaReg(rqInv/*7 dstReg*/, LEN_ID_MAX_BITS/*LenID*/, pPrivKey->PriveKeyDb.Crt.qInv/*src_ptr*/, pqSizeInWords );

    /*  Calculation of Mp          */
    /* reduction of input data by modulus P:  rT = rT1 mod P  */
    PKA_DIV(LEN_ID_N_PKA_REG_BITS/*LenID*/, rT/*3 res not used*/, rT1/*4 OpA and remainder*/, rN/*0 OpB*/);

    /* operation changes from p/q size to N size, need clearing registers high bits */

    /* calculate exponent Mp = DataIn^dP mod P , i.e: rT = rT1^rD mod rP  */
    PKA_MOD_EXP(LEN_ID_PQ_BITS/*LenID*/, rT/*3 Res*/, rT1/*4 OpA*/, rD/*2 exp*/);

    /* Calculation of  h = (Mp-Mq)*qInv mod P    */
    /* rT1 = Mq mod P - needed for right calculating in next operation if Mq>P */
    PKA_MOD_ADD_IM(LEN_ID_PQ_BITS/*LenID*/, rT1/*Res*/, rMq/*5 OpA*/, 0/*immed OpB*/);

    /* rT = Mp - Mq mod P */
    PKA_MOD_SUB(LEN_ID_PQ_BITS/*LenID*/, rT/*Res*/, rT/*OpA*/, rT1/*OpB*/);

    /* rT1 = h = (Mp - Mq)*qInv mod P */
    PKA_MOD_MUL(LEN_ID_PQ_BITS/*LenID*/, rT1/*Res*/, rT/*OpA*/, rqInv/*rqInv*/);

    /*       M = Mq + Q*h;         */
    /*  OpSize according Nsize     */
    /* operation changes from p/q size to N size, need clearing rT high bits */
    PkaClearPkaRegWords(rT1, pqSizeInWords);
    PkaClearPkaRegWords(rT, pqSizeInWords);
    PkaClearPkaRegWords(rQ, pqSizeInWords);
    PkaClearPkaRegWords(rMq, pqSizeInWords);

    /* copy rT1 and Mq in other registers for clearing junk from registers high part  */
    PKA_2CLEAR(LEN_ID_MAX_BITS/*LenID*/, PKA_REG_T0/*dest*/);
    PKA_COPY(LEN_ID_MAX_BITS/*LenID*/, rT/*dest*/, rT1/*src*/);
    PKA_2CLEAR(LEN_ID_MAX_BITS/*LenID*/, PKA_REG_T0/*dest*/);
    PKA_COPY(LEN_ID_MAX_BITS/*LenID*/, rT1/*dest*/, rMq/*src*/);

    /* Q*h => rT = rQ*rT */
    PKA_MUL_LOW(LEN_ID_N_PKA_REG_BITS/*LenID*/, rT/*Res*/, rT/*OpA*/, rQ/*OpB*/);

    PkaClearPkaRegWords(rT, pPrivKey->nSizeInBits);

    /* M = rT1 = rMq + rT */
    PKA_ADD(LEN_ID_N_BITS/*LenID*/, rT/*Res*/, rT1/*OpA*/, rT/*OpB*/);

    /*  Finish PKA and copy result */
    PkaCopyDataFromPkaReg(pPrivData->DataOut,  modSizeWords, rT/*srcReg*/);

    PkaFinishAndMutexUnlock(pkaReqRegs);

    return error;

}


/***********     RsaInitPrivKeyDb  function      **********************/
/**
 * @brief This function initializes the low level key database private structure.
 *        On the HW platform the Barrett tag is initialized
 *
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
CCError_t RsaInitPrivKeyDb( CCRsaPrivKey_t *pPrivKey )  /*!< [in]  Private key database. */
{
    CCError_t error = CC_OK;

    /* calculate NP on NonCRT mode  */
    if (pPrivKey->OperationMode == CC_RSA_NoCrt) {
        /* check key size */
        if (pPrivKey->nSizeInBits > CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS) {
            error = PKA_KEY_ILLEGAL_SIZE_ERROR;
            goto END;
        }

        /* calculate Barrett tag NP by initialization PKA for modular operations.
           Default settings: N=PKA_REG_N, NP=PKA_REG_NP, T0=30, T1=31.
           Our settings for temps: rT0=2, rT1=3, rT2=4 */
        error = PkiCalcNp( ((RsaPrivKeyDb_t *)(pPrivKey->ccRSAPrivKeyIntBuff))->NonCrt.NP,   /*out*/
                    pPrivKey->n,     /*in*/
                    pPrivKey->nSizeInBits); /*in*/


    } else {
        /* on CRT mode calculate the Barrett tags  */
        /*    PP and PQ for P and Q factors        */
        /* check key sizes */
        if (pPrivKey->PriveKeyDb.Crt.PSizeInBits > CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS/2 ||
            pPrivKey->PriveKeyDb.Crt.QSizeInBits > CC_RSA_MAX_VALID_KEY_SIZE_VALUE_IN_BITS/2) {
            error = PKA_KEY_ILLEGAL_SIZE_ERROR;
            goto END;
        }
        /* calculate Barrett tag PP by initialization PKA for modular operations */
        error = PkiCalcNp( ((RsaPrivKeyDb_t *)(pPrivKey->ccRSAPrivKeyIntBuff))->Crt.PP,   /*out*/
                    pPrivKey->PriveKeyDb.Crt.P, /*in*/
                    pPrivKey->PriveKeyDb.Crt.PSizeInBits);
        if (error != CC_SUCCESS) {
            goto END;
        }

        /* calculate Barrett tag PP by initialization PKA for modular operations */
        error = PkiCalcNp( ((RsaPrivKeyDb_t *)(pPrivKey->ccRSAPrivKeyIntBuff))->Crt.QP,   /*out*/
                    pPrivKey->PriveKeyDb.Crt.Q, /*in*/
                    pPrivKey->PriveKeyDb.Crt.QSizeInBits);
        if (error != CC_SUCCESS) {
            goto END;
        }


    }/* end of CRT case */

    END:
    return error;
}



/***********     RsaExecPrivKeyExp  function      **********************/
/**
 * @brief This function executes the RSA private key exponentiation
 *
 *    Algorithm [PKCS #1 v2.1]:
 *
 *     1. If NonCRT exponent, then  M  =  C^D  mod N.
 *
 *     2. If CRT exponent, then:
 *        2.1. M1  =  C^dP mod P,
 *        2.2. M2  =  C^dQ mod Q;
 *        2.3  h = (M1-M2)*qInv mod P;
 *        2.4. M = M2 + Q * h.
 *
 *     Where: M- message representative, C- ciphertext, N- modulus,
 *            P,Q,dP,dQ, qInv - CRT private key parameters;
 *            ^ - exponentiation symbol.
 *
 *     Note: PKA registers used: NonCrt: r0-r4,   r30,r31, size of registers - Nsize;
 *                               Crt:    r0-r10,  r30,r31, size of registers - Nsize;
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
CCError_t RsaExecPrivKeyExp( CCRsaPrivKey_t    *pPrivKey ,   /*!< [in]  Private key database. */
                    CCRsaPrimeData_t *pPrivData )   /*!< [in/out]  Containing DataIn and DataOut buffers. */
{
    CCError_t error = CC_OK;

    /*         1.  NonCRT  case                     */
    if (pPrivKey->OperationMode == CC_RSA_NoCrt) {
        error = RsaExecPrivKeyExpNonCrt(pPrivKey, pPrivData );
    } else {
        /*         2.  CRT  case                         */
        error = RsaExecPrivKeyExpCrt(pPrivKey, pPrivData );
    }

    return error;
}

#endif/* !defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C)) */


