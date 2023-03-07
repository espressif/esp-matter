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

#include "cc_pal_types.h"
#include "cc_rsa_types.h"
#include "cc_rsa_error.h"
#include "cc_rsa_local.h"
#include "cc_common_math.h"
#include "cc_rnd_common.h"
#include "cc_rnd_error.h"
#include "pka.h"
#include "pka_error.h"
#include "pki.h"
#include "pka_hw_defs.h"
#include "cc_pal_mem.h"
#include "rsa_public.h"
#include "rsa_private.h"
#include "rsa.h"
#ifdef FIPS_CERTIFICATION
#include "fips_tests_rsa_def.h"
#include "cc_common.h"
#endif

/* canceling the lint warning:
Info 716: while(1) ... */
/*lint --e{716} */
/* canceling the lint warning: Constant value Boolean
Warning 506 regarding while(1) ... */
/*lint --e{506} */

/* canceling the lint warning:
Info 506: Constant value Boolean ... */
/*lint --e{506} */

/* canceling the lint warning:
Info 774: Boolean within 'if' always evaluates to False */
/*lint --e{774} */

/************************ Defines ******************************/

#define RSA_QUICK_PRIME_TEST_DIVISIONS_COUNT  128

#define CALC_PRIME_PRODUCT_NUM ( CC_MIN(CALC_FULL_32BIT_WORDS(CC_RSA_MAX_KEY_GENERATION_SIZE_BITS),CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS) )
#define PRIME_PRODUCT_BUFF_SIZE ( CC_MAX(CALC_FULL_32BIT_WORDS(CC_RSA_MAX_KEY_GENERATION_SIZE_BITS),CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS) )
#define PRIME_NUM 256

#define CALC_PRIME_PRODUCT   (CALC_PRIME_PRODUCT_NUM/2 - 3)

#if (CALC_PRIME_PRODUCT > (CC_PKA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS-6))
        #error ("CALC_PRIME_PRODUCT > (CC_PKA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS-6)")
#endif


/************************ Global Data ******************************/
#ifdef FIPS_CERTIFICATION
rsaKgInternalPrime_t prim1Int;
rsaKgInternalPrime_t prim2Int;
extern rsaKgInternalDataStruct_t rsaKgOutParams;
rsaKgInternalPrime_t  *primeInt = NULL;
#endif


extern const int8_t regTemps[PKA_MAX_COUNT_OF_PHYS_MEM_REGS];

const uint32_t g_PrimeProduct[PRIME_PRODUCT_BUFF_SIZE] = {
        3234846615UL,95041567UL,907383479,4132280413UL,121330189,257557397UL,490995677,842952707,
        1314423991UL,2125525169UL,3073309843UL,16965341,20193023,23300239,29884301,35360399,
        42749359UL,49143869,56466073,65111573,76027969,84208541,94593973,103569859,119319383,133390067UL,
        154769821UL,178433279,193397129,213479407,229580147,250367549,271661713,293158127,319512181,
        357349471UL,393806449,422400701,452366557,507436351,547978913,575204137,627947039,666785731,
        710381447UL,777767161UL,834985999UL,894826021UL,951747481UL,1019050649UL,1072651369UL,1125878063UL,1185362993UL,
        1267745273UL,1322520163UL,1391119619UL,1498299287UL,1608372013UL,1700725291UL,1805418283UL,1871456063UL,
        2008071007UL,2115193573UL,2178429527UL,2246284699UL,2385788087UL
};

const uint16_t g_SmallPrime[PRIME_NUM] =
{
        3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,
        113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,
        229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349,353,
        359,367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461,463,467,479,
        487,491,499,503,509,521,523,541,547,557,563,569,571,577,587,593,599,601,607,613,617,
        619,631,641,643,647,653,659,661,673,677,683,691,701,709,719,727,733,739,743,751,757,
        761,769,773,787,797,809,811,821,823,827,829,839,853,857,859,863,877,881,883,887,907,
        911,919,929,937,941,947,953,967,971,977,983,991,997,1009,1013,1019,1021,1031,1033,1039,1049,
        1051,1061,1063,1069,1087,1091,1093,1097,1103,1109,1117,1123,1129,1151,1153,1163,1171,1181,
        1187,1193,1201,1213,1217,1223,1229,1231,1237,1249,1259,1277,1279,1283,1289,1291,1297,1301,
        1303,1307,1319,1321,1327,1361,1367,1373,1381,1399,1409,1423,1427,1429,1433,1439,1447,1451,
        1453,1459,1471,1481,1483,1487,1489,1493,1499,1511,1523,1531,1543,1549,1553,1559,1567,1571,
        1579,1583,1597,1601,1607,1609,1613,1619,1621
};

const uint16_t g_LastProductPrime[PRIME_PRODUCT_BUFF_SIZE] =
{
        9,14,19,24,28,32,36,40,44,48,52,55,58,61,64,67,70,73,76,79,82,85,88,91,94,97,100,103,106,109,112,
        115,118,121,124,127,130,133,136,139,142,145,148,151,154,157,160,163,166,169,172,175,178,181,
        184,187,190,193,196,199,202,205,208,211,214,217

};


extern CCError_t RndGenerateWordsArrayInRange(CCRndContext_t *pRndContext,
                                                     uint32_t   rndSizeInBits,
                                                     uint32_t  *maxVect_ptr,
                                                     uint32_t  *rndVect_ptr,
                                                     uint32_t  *tmp_ptr);

/***********    PkaRsaKgX931Jacobi   function      **********************/
/**
 * @brief Calculates Jacobi index.
 *  If there is such a vector b, that satisfies the condition b^2 = a mod p, the result is 1.
 *  If there is no such vector, the result is -1.
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
static CCError_t PkaRsaKgX931Jacobi (uint32_t  lenId,       /*!< [in]  The RegsSizesTable entry, containing the exact size of vector p in bits. */
                                       int8_t    rA,          /*!< [in]  Virtual pointer to the base vector. */
                                       int8_t    rP,          /*!< [in]  Virtual pointer to the prime to be tested (the modulos). */
                                       int32_t   *pJacobiRes, /*!< [out]  Pointer to the result var (1,0,-1) as described in the description. */
                                       int8_t    rA1,         /*!< [in]  virtual pointers to temp PKA registers. */
                                       int8_t    rP1,         /*!< [in]  virtual pointers to temp PKA registers. */
                                       int8_t    rT)          /*!< [in]  virtual pointers to temp PKA registers. */
{
        int32_t    k, s;
        uint32_t   residue;
        uint32_t bitVal;
        uint32_t status;
        /* low words of A1 and P1 */
        uint32_t A1_0, P1_0;
        /* temp swap value */
        int8_t  rSw;

        /* copy the input vectors with extension */
        PKA_COPY(LEN_ID_MAX_BITS, rA1/*dest*/, rA/*src*/);
        PKA_COPY(LEN_ID_MAX_BITS, rP1/*dest*/, rP/*src*/);

        /* initialize the result as 1 ( default ) */
        *pJacobiRes = 1;

        /* step 3.  if a1 == 1, return - we have done */
        PKA_COMPARE_IM_STATUS(lenId+1, rA1/*OpA*/, 1/*OpB*/, status);
        if (status == 1) {
                return CC_OK;
        }


        // do loop for finding the jacobi
        do {
                // Step 1.  If a == 0, return the result 0
                PKA_COMPARE_IM_STATUS(lenId+1, rA1/*OpA*/, 0/*OpB*/, status);
                if (status == 1) {
                        *pJacobiRes = 0;
                        return CC_OK;
                }


                // Step 2. Find out larger power of two for A1
                k = 0;

                /* check parity of A1 */
                PKA_READ_BIT0(lenId+1, rA1/*OpA*/, bitVal);
                while (bitVal == 0) {
                        /* divide by 2 */
                        PKA_SHR_FILL0(lenId+1, rA1/*Res*/, rA1/*OpA*/, 1-1/*S*/);
                        PKA_READ_BIT0(lenId+1, rA1/*OpA*/, bitVal);
                        k++;
                }

                // get low bytes of A1 and P1
                PKA_READ_WORD_FROM_REG(A1_0, 0, rA1);
                PKA_READ_WORD_FROM_REG(P1_0, 0, rP1);

                /* initialize s as 0 */
                s = 0;

                // step 3.  if k is even set s=1
                if ((k & 1) == 0) {
                        s = 1;
                } else {
                        /* else set s=1 if p = 1 or 7 (mod 8) or s=-1 if p = 3 or 5 (mod 8) */
                        residue = P1_0 & 7;

                        if (residue == 1 || residue == 7) {
                                s = 1;
                        } else if (residue == 3 || residue == 5) {
                                s = -1;
                        }
                }

                // Step 4.  If p == 3 (mod 4) *and* a1 == 3 (mod 4) then s = -s
                if (((P1_0 & 3) == 3) && ((A1_0 & 3) == 3)) {
                        s = -s;
                }

                /* Step 5 : Update the result                   */
                *pJacobiRes *= s;


                // Step 6.  If a1 == 1, return - done
                PKA_COMPARE_IM_STATUS(lenId+1, rA1/*OpA*/, 1/*OpB*/, status);
                if (status == 1) {
                        return CC_OK;
        }

                /* p1 = p1 mod a1 - the result is at rP1 register  */
                PKA_DIV(lenId+1 , rT/*ResNotUsed*/, rP1/*OpA*/, rA1/*OpB*/);

                // Step 7.  Exchange P1 & A1
                rSw = rP1;
                rP1   = rA1;
                rA1   = rSw;

        }while (1); /* end of do loop */
}



/***********    PkaRsaKgX931MillerRabinTest   function      **********************/
/**
 * @brief Executes the rabin miller test according to the the ANS X9.31 standard.
 *
 *    Algorithm:
 *        1. Let: prime candidate P = 1 + 2^a * m, where: m is odd and a > 0.
 *        2. For( i = 0; i < countTests; i++ ) do
 *             2.1. Generate random number b in range  1 < b < P.
 *             2.2. Calculate z = b^m mod P.
 *             2.3. If z = 1, or z = P-1, then goto st.6.
 *             2.4. For(j = 1; j < a; j++ ) do
 *                   2.4.1.  set z = z^2 mod P
 *                   2.4.2. If  z = P-1, then goto st.6.
 *                   2.4.3. If  z = 1, then output "composite" and stop.
 *                  End for //2.4.
 *             2.5. Output "composite". Stop.
 *           End for //2.
 *        3. Output P is "probable prime". Stop.
 *
 *       Assumings: - PKA is initialised on default mode for prime P as modulus (see near);
 *                  - the registers sizes table and mapping table are set on default mode,
 *                    according to exact P size, that means:
 *                      -- registers sizes table entries are set by default as follows:
 *                           lenId - P_sizeBits, lenId+1 - (32*P_sizeWords + 32 bit);
 *                  - the prime candidate P is inserted in the modulus register PKA_REG_N;
 *                  - the Barrett tag NP for P is inserted into register 1;
 *                  - the PKA clocks are initialized.
 *
 *       NOTE: - It uses 7 PKA data registers: PKA_REG_N,PKA_REG_NP,30,31, and 3 temp registers.
 *
 *  Assumptions : the max size supported is 2112 bits.
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
static  CCError_t PkaRsaKgX931MillerRabinTest(CCRndContext_t *pRndContext, /*!< [in/out]  Pointer to the RND context buffer. */
                                               uint32_t  lenId,           /*!< [in]  The ID of entry in RegsSizesTable, containing modulusSizeInBits. */
                                               uint32_t  modulusSizeInBits,  /*!< [in]  The prime candidate size. */
                                               int8_t   *pSuccessCode,    /*!< [out]  the success code : 0 - the test failed , 1 the test passed. */
                                               uint8_t   testsCount,      /*!< [in]  Count of exponentiations in test. If CountTests = 0, then
                                              CountTests will be set automatically according to prime size. */
                                               int8_t    rT0,         /*!< [in]  Virtual pointer to the base vector. */
                                               int8_t    rT1,         /*!< [in]  Virtual pointer to the base vector. */
                                               int8_t    rT2,         /*!< [in]  Virtual pointer to the base vector. */
                                               uint32_t  *pTempBuff)      /*!< [in]   pointer to temp buffer of size 3*modulusSizeInWords. */
{
        CCError_t error = CC_OK;
        uint32_t  i, j;
        uint32_t aValue, pSizeInWords;
        uint32_t status;
        uint32_t bitVal;
        uint32_t *pTempP, *pTempB;

        /* prime size in words */
        pSizeInWords = CALC_FULL_32BIT_WORDS(modulusSizeInBits);

        // first pSizeInWords of pTempBuff is used as temprary buffer by RndGenerateWordsArrayInRange
        pTempP = &pTempBuff[pSizeInWords];
        pTempB = &pTempBuff[2*pSizeInWords];

        /* clearing the temp registers */
        PKA_2CLEAR(LEN_ID_MAX_BITS, rT0/*regNum*/);
        PKA_2CLEAR(LEN_ID_MAX_BITS, rT1/*regNum*/);
        PKA_2CLEAR(LEN_ID_MAX_BITS, rT2/*regNum*/);

        // St. 1.2. Calculate a and m such, that P = 1 + 2^a * m ; m=>rT0, a=>aValue
        /* copy P into register rT0 */
        PKA_COPY(LEN_ID_MAX_BITS, rT0/*dst*/, PKA_REG_N/*src=P*/);

        /* rT0 = P - 1 */
        PKA_SUB_IM( lenId+1, rT0/*Res*/, rT0/*P*/, 1/*imm*/);

        /* set P-1 in tempP buff */
        PkaCopyDataFromPkaReg(pTempP, pSizeInWords, rT0/*srcReg*/);

        /* a = 1 */
        aValue = 1;

        while (1) {
                /* divide: rT0 = rT0 / 2 */
                PKA_SHR_FILL0(lenId+1, rT0/*Res*/, rT0/*P*/, 1-1/*OpB*/);

                /* test parity of rT0 */
                PKA_READ_BIT0(lenId+1, rT0/*P*/, bitVal);
                if (bitVal == 0) {
                        aValue++;
                } else {
                        break;
                }
        }

        // St. 2. Rabin-Miller test main loop
        *pSuccessCode = CC_TRUE;

        for (i = 0 ; i < testsCount ; ++i) {
                // St. 2.1. Prepare a random number b, used for the Rabin-Miller test  */
                //          as the Base of exponentiation. The number must be not larger, than

                /* generate a random number b=>rT1 for testing the primality of P by  *
                *  exponentiation                                 */
                CC_PalMemSetZero(pTempB, sizeof(uint32_t)*pSizeInWords);
                error = RndGenerateWordsArrayInRange(
                                                         pRndContext,
                                                         modulusSizeInBits,
                                                         pTempP/*(P-1) - maxVect*/,
                                                         pTempB/*Rnd*/,
                                                         pTempBuff/*temp buff*/);
                if (error != CC_OK) {
                        return error;
                }

                PkaCopyDataIntoPkaReg( rT1/*dstReg*/, LEN_ID_MAX_BITS, pTempB/*src_ptr*/, pSizeInWords);

                // St. 2.2. Calculate: z = rT1 = z^m mod P; Set j = 0.
                PKA_MOD_EXP( lenId, rT1/*Res=z*/, rT1/*opA=b*/, rT0/*OpB=m*/);

        // St. 2.3. Check; if z = 1 or z = P-1, then generate next B
                /* z == 1 ? */
                PKA_COMPARE_IM_STATUS(lenId+1, rT1/*z*/, 1/*OpB*/, status);
                if (status == 1) {
                        goto passed_this_iteration;
                }

                /* rT2 = P - 1 */
                PKA_SUB_IM( lenId+1, rT2/*Res*/, PKA_REG_N/*P*/, 1/*OpB*/);

                /* z == P-1 ? */
                PKA_COMPARE_STATUS(lenId+1, rT2/*P*/, rT1/*OpB*/, status);
                if (status == 1) {
                        goto passed_this_iteration;
                }

                // St. 2.4. Loop: do while not meet conditions
                //        (j == 0 && z == 1) or (z== P-1 )
                for (j = 1; j < aValue; j++) {
                        /* St. 2.4.1. z= z^2 mod m  */
                        PKA_MOD_MUL( lenId, rT1/*Res*/, rT1/*P*/, rT1/*OpB*/);

                        /* St. 2.4.2. if z == P-1, then break and next i */
                        PKA_COMPARE_STATUS(lenId+1, rT2/*P*/, rT1/*OpB*/, status);
                        if (status == 1) {
                                goto passed_this_iteration;
                        }

                        /* St. 2.4.3. if z == 1, then output composite and stop */
                        PKA_COMPARE_IM_STATUS(lenId+1, rT1/*P*/, 1/*OpB*/, status);
                        if (status == 1) {
                                *pSuccessCode = CC_FALSE;
                                goto End;
                        }

                }/* end for */

                *pSuccessCode = CC_FALSE;
                goto End;

                passed_this_iteration: ;

        } /* end main for */
End:

        /* delete secure sensitive data and exit */
        aValue = 0;
        /* clear temp and tempP */
        CC_PalMemSetZero(pTempBuff, 3*sizeof(uint32_t)*pSizeInWords);

        return error;
}


/***********    PkaRsaKgX931LucasPrimeTest   function      **********************/
/**
 * @brief Executes the Lucas test according to the the X931 standard.
 *
 *  Assumptions : the max size supported is 2112 bits.
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
static CCError_t PkaRsaKgX931LucasPrimeTest(uint32_t  lenId,          /*!< [in]  The ID of entry in RegsSizesTable, containing exact size of P in bits. */
                                             int8_t   *pSuccessCode,    /*!< [out] Success code : 0 - the test failed , 1 the test passed. */
                                             const int8_t  *pRegTemps,  /*!< [in]  Pointer to temp registers list - 7 registers. */
                                             uint32_t  tempsCount)      /*!< [in]  Count of temp registers in the list. */
{
        CCError_t error;
        uint32_t kSizeInBits;
        uint32_t d_abs;
        int8_t d_is_positive;
        int32_t JacobiRes;
        int32_t i;
        uint32_t status;
        /* virtual pointers to registers */
        int8_t rP, rD, rK, rU, rV, rUnew, rVnew, rT;

        /* check temp registers count */
#ifdef LLF_PKI_PKA_DEBUG
        if (tempsCount < 7)
                return PKA_NOT_ENOUGH_TEMP_REGS_ERROR;
#else
        tempsCount= tempsCount;
#endif

        rP = PKA_REG_N;  /* already is initialized by P */
        rD = pRegTemps[0];
        rK = pRegTemps[1];
        rU = pRegTemps[2];
        rV = pRegTemps[3];
        rUnew = pRegTemps[4];
        rVnew = pRegTemps[5];
        rT = pRegTemps[6];


        // setting the d vector
        /*  clear the temp buffer  */
        PKA_2CLEAR(LEN_ID_MAX_BITS, rD/*regNum*/);

        for (d_abs = 5 , d_is_positive = 1 ; ; d_abs+=2 , d_is_positive = !d_is_positive) {
                /* set D = d_abs  */
                PKA_WRITE_WORD_TO_REG(d_abs, 0, rD);

                /* if D is negative set D = P - D */
                if (d_is_positive == 0) {
                        PKA_SUB( lenId+1, rD/*Res*/, rP/*P*/, rD/*OpB*/);
                }

                error = PkaRsaKgX931Jacobi( lenId, rD, rP, &JacobiRes,
                                            rU/*temp*/, rV/*temp*/, rT/*temp*/);

                if (error != CC_OK) {
                        return error;
                }

                if (JacobiRes == -1) {
                        break;
                }

        }/* end of loop for finding d */


        // init vectors for the test loop
        /* K = P + 1 */
        PKA_ADD_IM( lenId+1, rK/*Res*/, rP/*P*/, 1/*OpB*/);

        /* set the size of K in bits */
        kSizeInBits = PkaGetRegEffectiveSizeInBits(rK/*reg*/);

        /* init U and V to 1 */
        PKA_2CLEAR(LEN_ID_MAX_BITS, rU/*regNum*/);
        PKA_SET_BIT0(lenId+1, rU/*Res*/, rU/*regNum*/);
        PKA_COPY(LEN_ID_MAX_BITS, rV/*dest*/, rU/*src*/);


        // the main test loop
        for (i = (int32_t)(kSizeInBits - 2) ; i >= 0 ; --i) {
                /* a bit value */
                uint32_t bit;

                /* Unew = U*V mod P */
                PKA_MOD_MUL( lenId, rUnew/*Res*/, rU/*OpA*/, rV/*OpB*/);

                /* Vnew = V^2 mod P */
                PKA_MOD_MUL( lenId, rVnew/*Res*/, rV/*OpA*/, rV/*OpB*/);

                /* rT = U^2 */
                PKA_MOD_MUL( lenId, rT/*Res*/, rU/*OpA*/, rU/*OpB*/);

                /* rT= D * U^2 */
                PKA_MOD_MUL( lenId, rT/*Res*/, rD/*OpA*/, rT/*OpB*/);

                /* Vnew = (V^2 + D*U^2) */
                PKA_ADD( lenId+1, rVnew/*Res*/, rT/*OpA*/, rVnew/*OpB*/);
                /* modular division by 2 */
                PkaModDivideBy2( lenId, rVnew, rP/*mod*/, rVnew);

                /* swap V,Vnew and U,Unew */
                SWAP_INT8(rVnew,rV);
                SWAP_INT8(rUnew,rU);

                /* get bit i from register K */
                bit = PkaGetBitFromPkaReg( rK, lenId, i, rT);

                if (bit != 0) {
                        /* Unew = (U+V)/2 */
                        PKA_ADD( lenId+1, rUnew/*Res*/, rV/*OpA*/, rU/*OpB*/);
                        /* modular division by 2 */
                        PkaModDivideBy2( lenId, rUnew, rP/*mod*/, rUnew);

                        /* Vnew = (U*D+V)/2 */
                        PKA_MOD_MUL( lenId, rVnew/*Res*/, rD/*OpA*/, rU/*OpB*/);
                        PKA_ADD( lenId+1, rVnew/*Res*/, rV/*OpA*/, rVnew/*OpB*/);
                        PkaModDivideBy2( lenId, rVnew, rP/*mod*/, rVnew);

                        /* swap V,Vnew and U,Unew */
                        SWAP_INT8(rVnew,rV);
                        SWAP_INT8(rUnew,rU);

                }
        }

        /* U = U mod P */
        PKA_DIV( lenId+1, rT/*ResNotUsed*/, rU/*OpA*/, rP/*OpB*/);

        // If U is equal to 0 return success code = 1, else 0
        PKA_COMPARE_IM_STATUS( lenId+1, rU/*OpA*/, 0/*OpB immed*/, status);
        if (status == 1) {
                *pSuccessCode = 1;
        } else {
                *pSuccessCode = 0;
        }


        return error;

}


/***********    PkaRsaKgX931FindPrime1   function      **********************/
/**
 * @brief Finds a small auxiliary prime (104...176 bits)
 *        for the Key Generation under the X931 standard.
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
static CCError_t PkaRsaKgX931FindPrime1(CCRndContext_t *pRndContext, /*!< [in/out]  Pointer to the RND context buffer. */
                                         int8_t  rP,                 /*!< [in/out]  Virtual pointer to the prime buff. */
                                         RsaKgParams_t *rsaKgPrimeTestParams, /*!< [in]  Pointer to primality testing parameters structure. */
                                         const int8_t *pRegTemps,            /*!< [in]  Pointer to temp PKA registers list (5 single registers). */
                                         uint32_t   tempsCount,          /*!< [in]  Count of temp registers in the list. */
                                         uint32_t  *pTempBuff)           /*!< [in]  Temp buffer of size 2 max RSA buffer size. */
{
        CCError_t error = CC_OK;
        uint32_t i, c, d;
        uint32_t  *pTempRem = NULL;
        uint32_t r;
        /* the reminder and prime product virtual pointers */
        int8_t rPrimeProduct;
        /* temp register */
        int8_t rT, rT1, rT2;
        /* the rabin miller success code */
        int8_t successCode;

        /* check temp registers count */
#ifdef LLF_PKI_PKA_DEBUG
        if (tempsCount < 4)
                return PKA_NOT_ENOUGH_TEMP_REGS_ERROR;
#else
        tempsCount = tempsCount;
#endif


        /* allocation of the temp registers */
        rPrimeProduct = pRegTemps[0];
        rT  = pRegTemps[1];
        rT1 = pRegTemps[2];
        rT2 = pRegTemps[3];


        /*  clearing the extended temp registers  */
        PKA_2CLEAR(LEN_ID_MAX_BITS, rPrimeProduct/*regNum*/);
        PKA_2CLEAR(LEN_ID_MAX_BITS, rT/*regNum*/);

        /* set the LSB of the prime to insure it is an odd number: rP_ptr[0] |= 1 */
        PKA_SET_BIT0( LEN_ID_PQ_PKA_REG_BITS/*lenId*/, rP/*Res*/, rP/*OpA*/);

        //  calculating the prime reminder
        pTempRem = &pTempBuff[0];
        for (i = 0; i < CALC_PRIME_PRODUCT ; ++i) {
                /* load the current prime product into PKA register */
                PKA_WRITE_WORD_TO_REG(g_PrimeProduct[i], 0, rPrimeProduct);

                /* copy rP=>rT and calculate the reminder */
                PKA_COPY(LEN_ID_MAX_BITS, rT/*dest*/, rP/*src*/);
                PKA_DIV(LEN_ID_PQ_PKA_REG_BITS, rT1/*resNotUsed*/, rT/*OpA*/, rPrimeProduct/*OpB*/);

                /* read result rT word[0] and load it into reminder word[i] */
                PKA_READ_WORD_FROM_REG(pTempRem[i], 0, rT);

        }/* end of loop for calculating the reminders */


        // the main loop for finding a prime
        for (d = 0; ; d += 2) {

                PKA_2CLEAR(LEN_ID_MAX_BITS, rT/*regNum*/);

                // finding a candidate for a prime
                for (c = 0, i = 0; i < CALC_PRIME_PRODUCT; ++i) {
                        if (pTempRem[i] + d < d) {        /* remark: [*] */
                                pTempRem[i] -= g_PrimeProduct[i];
                        }

                        r = pTempRem[i] + d;

                        for (; c < g_LastProductPrime[i]; ++c) {
                                if (r % g_SmallPrime[c] == 0)
                                        goto Next_d;
                        }
                }

                /* calculate P = P + d.  */
                /* load d into register rT. Note: rT already cleared, except the LS word  */
                PKA_WRITE_WORD_TO_REG(d, 0, rT);
                PKA_ADD( LEN_ID_PQ_PKA_REG_BITS, rP/*Res*/, rP/*OpA*/, rT/*OpB*/);


                //  initialization of modular operations
                /* copy P into modulus register r0 */
                if (rP != PKA_REG_N) {
                        PKA_COPY(LEN_ID_MAX_BITS, PKA_REG_N/*dst*/, rP/*src*/);
                }

                /* initialization of modular operations, the "modulus" in this case is P or Q which is 1/2 modulus size.*
                *  that's the reason we use PQ Len ID                                 */
                PkaCalcNpIntoPkaReg(LEN_ID_AUX_PRIME_BITS,
                                     rsaKgPrimeTestParams->auxPrimesSizeInBits,
                                     PKA_REG_N, PKA_REG_NP, rT, rT1);

                //  executing the Miller-Rabin test
                error = PkaRsaKgX931MillerRabinTest(pRndContext,
                                                    LEN_ID_AUX_PRIME_BITS,
                                                    rsaKgPrimeTestParams->auxPrimesSizeInBits,
                                                    &successCode, /*out*/
                                                    rsaKgPrimeTestParams->auxPrimesMilRabTestsCount, /*in*/
                                                    rT, rT1, rT2,  /*temp registers*/
                                                    pTempBuff+CALC_PRIME_PRODUCT);
                if (error != CC_OK) {
                        return error;
                }

                /* on sucess return CC_OK we have found a prime */
                if (successCode == CC_TRUE) {
                        return CC_OK;
                }

                /* update d and reminder to avoid overflow of d (unlikely event) */
                for (i = 0; i < CALC_PRIME_PRODUCT; ++i) {
                        pTempRem[i] += d; /* remark: since [*] passed, there is no need to recheck */
                }

                d = 0;

                Next_d:
                continue;

        }/* end of main loop for finding a prime */
}


/***********    PkaRsaKgX931FindPrime2   function      **********************/
/**
 * @brief Finds a valid prime2 (second stage prime) for the Key Gen under the X9.31 standard .
 *
 *      Assumptions : supports a fixed size of 101 bits as required in the standard.
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
static CCError_t PkaRsaKgX931FindPrime2(CCRndContext_t *pRndContext, /*!< [in/out]  Pointer to the RND context buffer. */
                                          int8_t    rP,                /*!< [in/out] Virtual pointer to the prime P (P or Q in RSA). */
                                          int8_t    rDelta,            /*!< [in]  Virtual pointer to the delta factor. */
                                          int8_t    rE,            /*!< [in]  Virtual pointer to public exponent. */
                                          uint32_t  modulusSizeInBits, /*!< [in]  Size of prime to be generated. */
                                          RsaKgParams_t *rsaKgPrimeTestParams, /*!< [in]  Pointer to primality testing parameters structure. */
                                          const int8_t   *pRegTemps,   /*!< [in]  Pointer to temp PKA registers list. */
                                          uint32_t  tempsCount,        /*!< [in]  Count of temp registers in the list (6 single registers). */
                                          uint32_t *pTempBuff)         /*!< [in]  Temp buffer of size 2 max RSA buffer size. */
{
        CCError_t error = CC_OK;
        int8_t successCode;
        uint32_t i, c;
        uint32_t status;
        uint32_t bitVal;
        uint32_t  *pTempRem = NULL;
        uint32_t  *pTempDelta = NULL;
        /* the reminder and prime product virtual pointers */
        int8_t rPrimeProduct;
        /* temp register */
        int8_t rT, rT1, rT2;

        /* check temp registers count */
#ifdef LLF_PKI_PKA_DEBUG
        if (tempsCount < 4)
                return PKA_NOT_ENOUGH_TEMP_REGS_ERROR;
#endif


        /* allocation of the reminder and product on temp registers */
        rPrimeProduct = pRegTemps[0];
        rT  = pRegTemps[1];
        rT1 = pRegTemps[2];
        rT2 = pRegTemps[3];

        // calculating the prime reminder
        /*  clearing the temp registers (extended) */
        PKA_2CLEAR(LEN_ID_MAX_BITS, rPrimeProduct/*regNum*/);
        PKA_2CLEAR(LEN_ID_MAX_BITS, rT/*regNum*/);

        // calculate Rdelta and the Reminder
        /* if the prime candidate P is even add the delta */
        PKA_READ_BIT0(LEN_ID_PQ_PKA_REG_BITS, rP/*OpA*/, bitVal);
        if (bitVal == 0) {
                PKA_ADD( LEN_ID_PQ_PKA_REG_BITS/*lenId*/, rP/*Res*/, rP/*OpA*/, rDelta/*OpB*/);
        }

        /* multiply delta by 2 */
        PKA_ADD( LEN_ID_PQ_PKA_REG_BITS/*lenId*/, rDelta/*Res*/, rDelta/*OpA*/, rDelta/*OpB*/);

        // loop for calculating the products
        pTempRem = &pTempBuff[0];
        pTempDelta = &pTempBuff[CALC_PRIME_PRODUCT];

        for (i = 0; i < CALC_PRIME_PRODUCT ; ++i) {
                /* load the current rPrimeProduct[0] = g_PrimeProduct[i] */
                PKA_WRITE_WORD_TO_REG(g_PrimeProduct[i], 0, rPrimeProduct);

                /* copy rP=>rT and calculate the reminder in reg rT */
                PKA_COPY(LEN_ID_MAX_BITS, rT/*dest*/, rP/*src*/);
                PKA_DIV( LEN_ID_PQ_PKA_REG_BITS, rT1/*ResNotUsed*/, rT/*OpA*/, rPrimeProduct/*OpB*/);

                /* load the next word of reminder: rRem[i] = rT[0] */
                PKA_READ_WORD_FROM_REG(pTempRem[i], 0, rT);

                /* calculate the Rdelta */
                PKA_COPY(LEN_ID_MAX_BITS, rT/*dest*/, rDelta/*src*/);
                PKA_DIV( LEN_ID_PQ_PKA_REG_BITS, rT1/*ResNotUsed*/, rT/*OpA*/, rPrimeProduct/*OpB*/);

                /* load the Rdelta with the result rRdeltam[i] = rT[0]*/
                PKA_READ_WORD_FROM_REG(pTempDelta[i], 0, rT);

        }/* end of loop for calculating the reminders */

        /*  main loop for finding the prime  */
        while (1) {
                //WATCH_DOG_RESET(); // obsolete. Watchdog should be fed by dedicated task

                /* checking if the current prime should be tested */
                for (c=0 , i=0 ; i < CALC_PRIME_PRODUCT ; i++) {
                        for (; c < g_LastProductPrime[i] ; c++) {
                                if ((pTempRem[i] % g_SmallPrime[c]) == 0) {

                                        goto NextPrime;
                                }
                        }
                }

                /*  execute rT = GCD(e,P-1)  */
                PKA_SUB_IM( LEN_ID_PQ_PKA_REG_BITS,  rT/*Res*/, rP/*OpA*/, 1/*imm*/);   /* rP= rP-1 */
                PKA_COPY(LEN_ID_MAX_BITS, PKA_REG_N/*dest*/, rE/*src*/);

                /* rT = GCD */
                PKA_MOD_INV( LEN_ID_PQ_BITS, rT1/*Res*/, rT/*OpA*/);

                /* if the GCD != 1, go to the next prime */
                PKA_COMPARE_IM_STATUS(LEN_ID_PQ_PKA_REG_BITS, rT/*OpA*/, 1/*OpB*/, status);
                if (status != 1) {
                        goto NextPrime;
                }

                /*  initialization of modular operations for modulus P */
                /* reset modulus in register r0 = rP */
                PKA_COPY(LEN_ID_MAX_BITS, PKA_REG_N/*dst*/, rP/*src*/);

                /* initialization of modular operations */
                PkaCalcNpIntoPkaReg(LEN_ID_PQ_BITS, modulusSizeInBits, PKA_REG_N, PKA_REG_NP, rT, rT1);


                /*   perform primality tests   */
                /* init lhe test flag to FALSE */
                successCode = CC_FALSE;

                /* execute the Miller-Rabin test */
                error = PkaRsaKgX931MillerRabinTest(pRndContext,
                                                    LEN_ID_PQ_BITS,
                                                    modulusSizeInBits,
                                                    &successCode,    /*out*/
                                                    rsaKgPrimeTestParams->pqPrimesMilRabTestsCount, /*count R-M Tests */
                                                    rT, rT1, rT2,    /*temp registers*/
                                                    pTempBuff+2*CALC_PRIME_PRODUCT);
                if (error != CC_OK) {
                        goto End; //LR goto ClearAndReturn
                }

                /* if the previous test succeeded, execute the Lucas test */
                if (successCode == CC_TRUE) {
                        error = PkaRsaKgX931LucasPrimeTest(
                                                          LEN_ID_PQ_BITS,
                                                          &successCode,     /*out*/
                                                          pRegTemps + 3, /*temp registers list*/
                                                          tempsCount-3);
                        if (error != CC_OK) {
                                goto End;  //LR goto ClearAndReturn
                        }
                }

                /* if both tests are passed, exit - we have finded a prime */
                if (successCode == CC_TRUE) {
                        return CC_OK;
                }

                /*    finding the next prime candidate        */
                NextPrime:

                /* updating of remainders Rem[i] */
                for (i = 0 ; i < CALC_PRIME_PRODUCT ; i++) {
                        pTempRem[i] += pTempDelta[i];
                        if (pTempRem[i] < pTempDelta[i]) {
                                pTempRem[i] -= g_PrimeProduct[i];
                        }

                }

        //find new P if previous one isn't correct
                /* the new prime candidate: P = P + Delta */
                PKA_ADD(LEN_ID_PQ_PKA_REG_BITS, rP/*Res*/, rP/*OpA*/, rDelta/*OpB*/);

        }/* end of searching for a prime loop */

End:
        // RL  Check and ddd Clearing temp buffers if need !!!!
        return error;

}

/***********    RsaKgX931FindPrime   function      **********************/
/**
 * @brief Finds a valid prime for the Key Gen under the X931 standard .
 *
 *     Assumes: - the PKA is initialized on default mode according to modulusSizeInBits,
 *              - the modulusSizeInBits is set into lenId RegsSizesTable,
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
static CCError_t RsaKgX931FindPrime(CCRndContext_t *pRndContext, /*!< [in/out]  Pointer to the RND context buffer. */
                                      int8_t    rP1,          /*!< [in]  Virtual pointers to PKA registers of auxiliary primes p1. */
                                      int8_t    rP2,          /*!< [in]  Virtual pointers to PKA registers of auxiliary primes p2. */
                                      int8_t    rP,           /*!< [in/out]  Virtual pointer to the register containing P prime. */
                                      uint32_t  modulusSizeInBits, /*!< [in]  Size of the prime P. */
                                      uint32_t  rE,           /*!< [in]  Virtual pointer to public exponent. */
                                      RsaKgParams_t *rsaKgPrimeTestParams, /*!< [in]  Pointer to primality testing parameters structure. */
                                      const int8_t   *pRegTemps, /*!< [in]  Pointer to temp PKA registers list (5 single registers). */
                                      uint32_t  tempsCount,      /*!< [in]  Count of temp registers in the list. */
                                      uint32_t *pTempBuff)       /*!< [in]  Pointer to the temp buffer of size 2 max RSA buffer size. */
{
        CCError_t error = CC_OK;
        uint32_t status, flag;
        /* virtual pointers to PKA registers */
        int8_t    rP12, rPmodP12, rR1, rR2;
        /* virtual pointers to temp PKA registers */
        int8_t rT1;

        /* allocation of registers */
        rP12 = pRegTemps[0];
        rPmodP12 = pRegTemps[1];
        rR1 = pRegTemps[2];
        rR2 = pRegTemps[3];
        rT1 = pRegTemps[4];


        /* check temp registers count */
#ifdef LLF_PKI_PKA_DEBUG
        if (tempsCount < 5)
                return PKA_NOT_ENOUGH_TEMP_REGS_ERROR;
#endif

        /* find the first primes P1, P2  of size 101 bit */
        /* p1 */
        error = PkaRsaKgX931FindPrime1( pRndContext,
                                        rP1,
                                        rsaKgPrimeTestParams,
                                        pRegTemps, tempsCount,
                                        pTempBuff);
        if (error != CC_OK)
                return error;

        /* p2 */
        error = PkaRsaKgX931FindPrime1( pRndContext,
                                        rP2,
                                        rsaKgPrimeTestParams,
                                        pRegTemps, tempsCount,
                                        pTempBuff);
        if (error != CC_OK) {
                return error;
        }

        /* Debug */
#ifdef LLF_PKI_PKA_DEBUG
        PKA_COPY(LEN_ID_MAX_BITS, rP1/*dst*/, rP1/*src*/);
        PKA_COPY(LEN_ID_MAX_BITS, rP2/*dst*/, rP2/*src*/);
#endif

        /*  find P12 = P1*P2 , pModP12 = P mod P12 (operations size from lenId) */
        /*     Note: modulusSizeInBits must be set into lenId entry                   */
        /* P12 = P1 * P2 */
        PKA_MUL_LOW(LEN_ID_PQ_PKA_REG_BITS, rP12/*Res*/, rP1/*OpA*/, rP2/*OpB*/);

        /* PmodP12 = P mod P12 */
        PKA_COPY(LEN_ID_MAX_BITS, rPmodP12/*dst*/, rP/*src*/);
        PKA_DIV( LEN_ID_PQ_PKA_REG_BITS, rT1/*ResNotUsed*/, rPmodP12/*OpA*/, rP12/*OpB*/);


        /* find; R1= (1/P2 mod P1)*P2 - (1/P1 mod P2)*P1; R2= ... similary   .. */
        /* calculate R1 = (1/P2 mod P1)*P2 */
        PKA_COPY(LEN_ID_MAX_BITS, PKA_REG_N/*mod reg*/, rP1/*src*/);
        PKA_2CLEAR(LEN_ID_MAX_BITS, rT1/*dst*/);
        PKA_COPY(LEN_ID_MAX_BITS, rT1/*dst*/, rP2/*src*/);

        /* if P1 > P2 set flag = 1, else flag = 0 */
        PKA_SUB(LEN_ID_PQ_PKA_REG_BITS, RES_DISCARD, rP2/*OpA*/, rP1/*OpB*/);
        PKA_GET_STATUS_CARRY(status);
        if (status == 0) {
                flag = 1;
        } else {
                /* set rT1 = P2 mod P1 = rT1 - rP1 */
                flag = 0;
                PKA_SUB(LEN_ID_PQ_PKA_REG_BITS, rT1/*Res*/, rT1/*OpA*/, rP1/*OpB*/);
        }

        /* R1 = (1/P2 mod P1) */
        /* we know PKA_REG_N - rP1 is prime, so we can use ModInv with the odd number*/
        /* we do not check GCD, since PKA_REG_N is prime and rT1 < PKA_REG_N. therfore GCD must be 1 */
        PKA_MOD_INV(LEN_ID_PQ_BITS, rR1/*Res*/, rT1/*OpB*/);

        PKA_MUL_LOW( LEN_ID_PQ_PKA_REG_BITS, rR1/*Res*/, rR1/*OpA*/, rP2/*OpB*/);

        /* calculate R2 = (1/P1 mod P2)*P1 */
        PKA_COPY(LEN_ID_MAX_BITS, PKA_REG_N/*mod reg*/, rP2/*src*/);
        PKA_2CLEAR(LEN_ID_MAX_BITS, rT1/*dst*/);
        PKA_COPY(LEN_ID_MAX_BITS, rT1/*dst*/, rP1/*src*/);

        /* if flag == 1, i.e. P2 >= P1, then set rT1 = P1 mod P2 = P1 - P2 */
        if (flag == 1) {
                PKA_SUB(LEN_ID_PQ_PKA_REG_BITS, rT1/*Res*/, rT1/*OpA*/, rP2/*OpB*/);
        }

        /* we know PKA_REG_N = rP2 is prime, so we can use ModInv with the odd number*/
        PKA_MOD_INV(LEN_ID_PQ_BITS, rR2/*Res*/, rT1/*OpB*/);

        PKA_MUL_LOW( LEN_ID_PQ_PKA_REG_BITS, rR2/*Res*/, rR2/*OpA*/, rP1/*OpB*/);


        /* R=R1-R2; if(R <0) R= R+P12; */
        /* R1 and R2 are max 202 bits each, so LEN_ID_PQ_BITS should be enought to hold negative number*/
        PKA_SUB(LEN_ID_PQ_PKA_REG_BITS, rR1/*res*/, rR1/*OpA*/, rR2/*OpB*/);
        PKA_GET_STATUS_CARRY(status);
        if (status == 0) {
                PKA_ADD(LEN_ID_PQ_PKA_REG_BITS, rR1/*res*/, rR1/*OpA*/, rP12/*OpB*/);
        }

        /* R=R-PmodP12; if(R<0) R=R+P12; */
        PKA_SUB(LEN_ID_PQ_PKA_REG_BITS, rR1/*res*/, rR1/*OpA*/, rPmodP12/*OpB*/);
        PKA_GET_STATUS_CARRY(status);
        if (status == 0) {
                PKA_ADD(LEN_ID_PQ_PKA_REG_BITS, rR1/*res*/, rR1/*OpA*/, rP12/*OpB*/);
        }

        /* add P = P + R */
        PKA_ADD(LEN_ID_PQ_PKA_REG_BITS, rP/*res*/, rP/*OpA*/, rR1/*OpB*/);

    //now in rP is a number with new conditions

        /* find the prime P */
        error = PkaRsaKgX931FindPrime2( pRndContext, rP, rP12/*rDelta*/, rE,
                                        modulusSizeInBits,
                                        rsaKgPrimeTestParams,
                                        pRegTemps + 1, tempsCount-1,
                                        pTempBuff);

        return error;

}


/***********    RsaKgQuickPrimeTest   function      **********************/
/**
 * @brief Checks primality of big number relating to set of small prime numbers.
 *
 *   Notes:  - 3 PKA registers used: rP, rModRes, rSmallNum,
 *           - the PKA must be initialized according tp P size,
 *           - lenId+1 entry containing the extended register size.
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
static int32_t RsaKgQuickPrimeTest(uint8_t    lenId,    /*!< [in]  The SizesTable entry, containing the exact P size in bits. */
                                   int8_t     rP,       /*!< [in]  The virtual pointer to big number P register to be checked. */
                                   int8_t     rModRes,     /*!< [in]  Virtual pointers to temp registers. */
                                   int8_t     rSmallPrime, /*!< [in]  Virtual pointers to temp registers. */
                                   int8_t     rT,          /*!< [in]  Virtual pointers to temp registers. */
                                   uint32_t   divCount)    /*!< [in]  . */
{
        uint32_t i;
        uint32_t status;

        /* set pointer smallPrime_ptr to PKA register low word */
        /* clear rSmallPrime register (with extension) */
        PKA_2CLEAR(LEN_ID_MAX_BITS, rSmallPrime/*OpA*/);

        /* Check primality by dividing P by small primes */
        for (i = 0; i < divCount; i++) {
                /* copy rP into rModReg for dividing */
                PKA_COPY(LEN_ID_MAX_BITS, rModRes/*dst*/, rP/*src*/);

                /* set the next small prime into PKA register */
                PKA_WRITE_WORD_TO_REG(g_SmallPrime[i], 0, rSmallPrime);

                /* calculate remainder: rModReg = rP % smallPrime */
                PKA_DIV( lenId+1, rT/*ResNotUsed*/, rModRes/*OpA*/, rSmallPrime/*OpB*/);

                /* check is the remainder equaled to 0 by add operation */
                PKA_ADD_IM( lenId+1, RES_DISCARD/*discard Res*/, rModRes/*OpA*/, 0/*OpB*/);
                PKA_GET_STATUS_ALU_OUT_ZERO(status);
                if (status) {
                        return CC_FALSE;
                }
        }

        return CC_TRUE;
}


/***********    PkaRsaKgFindPrime   function      **********************/
/**
 * @brief Finds a valid prime for the Key Gen.
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
static CCError_t PkaRsaKgFindPrime(CCRndContext_t *pRndContext,  /*!< [in/out]  Pointer to the RND context buffer. */
                                     int8_t    rP,            /*!< [in/out]  Virtual pointer to the prime P register. */
                                     uint32_t  modulusSizeInBits, /*!< [in]  Prime size in bits. */
                                     int8_t    rE,            /*!< [in]  Virtual pointer to the public exponent register. */
                                     RsaKgParams_t *rsaKgPrimeTestParams, /*!< [in]  Pointer to primality testing parameters structure. */
                                     const int8_t   *pRegTemps,  /*!< [in]  Pointer to temp PKA registers list. */
                                     uint32_t  tempsCount,       /*!< [in]  Count of temp registers in the list (9). */
                                     uint32_t *pTempBuff)        /*!< [in]  Temp buffer of size 2 max RSA buffer size. */
{
        CCError_t error = CC_OK;
#if !defined RSA_KG_NO_RND
        CCRndState_t *rndState_ptr;
#endif
        CCRndGenerateVectWorkFunc_t RndGenerateVectFunc;
        /* virtual pointers to PKA data registers */
        int8_t rP1, rP2;

        /* temp buffer for auxiliary random number / big end
           1E05B8F18D807DB7A47EF53567 nearest random is: 1E 05 B8 F1 8D 80 7D
           B7 A4 7E F5 35 99*/
        uint32_t rBuff[PKA_RSA_AUX_PRIME_BUFF_SIZE_IN_32BIT_WORDS] = {0};
        uint32_t auxPrimeSizeInBytes, auxPrimeSizeInWords;
        uint32_t mask, msBit;

        if (pRndContext == NULL) {
                return CC_RND_CONTEXT_PTR_INVALID_ERROR;
        }

#if !defined RSA_KG_NO_RND
        rndState_ptr = (CCRndState_t *)(pRndContext->rndState);
#endif
        RndGenerateVectFunc = pRndContext->rndGenerateVectFunc;

        if (RndGenerateVectFunc == NULL) {
                return CC_RND_GEN_VECTOR_FUNC_ERROR;
        }

#ifdef LLF_PKI_PKA_DEBUG
        if (tempsCount < 9)
                return PKA_NOT_ENOUGH_TEMP_REGS_ERROR;
#endif

        /* allocate virtual pointers on temp registers */
        rP1 = pRegTemps[0];
        rP2 = pRegTemps[1];

        /* calculate size of aux. primes in bytes and words */
        auxPrimeSizeInBytes = CALC_FULL_BYTES(rsaKgPrimeTestParams->auxPrimesSizeInBits);
        auxPrimeSizeInWords = CALC_FULL_32BIT_WORDS(rsaKgPrimeTestParams->auxPrimesSizeInBits);

#if defined RSA_KG_NO_RND
#ifdef LLF_PKI_PKA_DEBUG
        if (PQindex == 0) {
                CC_PalMemCopy( rBuff, rBuff1, auxPrimeSizeInBytes); /*for P*/
        } else {
                CC_PalMemCopy( rBuff, rBuff3, auxPrimeSizeInBytes); /*for Q*/
        }


#ifdef BIG__ENDIAN
        CC_COMMON_INVERSE_UINT32_IN_ARRAY(rBuff, auxPrimeSizeInWords);
#endif
#endif

#else
        /* get a random auxiliary number P1      */
        error = RndGenerateVectFunc((void *)rndState_ptr, (unsigned char*)rBuff, (size_t)auxPrimeSizeInBytes );
        if (error != CC_OK) {
                return error;
        }
#endif

      /* calculate mask for aux.prime candidate */
        mask = (~0UL >> (32 - (rsaKgPrimeTestParams->auxPrimesSizeInBits & 0x1F)));
        msBit = 1UL << ((rsaKgPrimeTestParams->auxPrimesSizeInBits & 0x1F)-1);

    /* calculate mask and set MS bit of aux.prime candidate */
        rBuff[auxPrimeSizeInWords-1] &= mask;
        rBuff[auxPrimeSizeInWords-1] |= msBit;
        /* set LSBit = 1 to ensure the odd number */
        rBuff[0] |= 1UL;

    #ifdef FIPS_CERTIFICATION
    primeInt->bitlen1 = rsaKgPrimeTestParams->auxPrimesSizeInBits;
    CC_CommonReverseMemcpy(primeInt->xPrime1, (uint8_t*)rBuff, auxPrimeSizeInBytes);
    #endif

#ifdef LLF_PKI_PKA_DEBUG
#if (defined RSA_KG_NO_RND || defined RSA_KG_FIND_BAD_RND)
        /* set pointers for extern P,Q and aux.primes buffers*/
        if (PQindex == 0) {
                P1R_ptr = P1pR; P2R_ptr = P2pR;
                P1Pr_ptr = P1pPr; P2Pr_ptr = P2pPr;
                PQ_ptr = rBuffP;
        } else {
                P1R_ptr = P1qR; P2R_ptr = P2qR;
                P1Pr_ptr = P1qPr; P2Pr_ptr = P2qPr;
                PQ_ptr = rBuffQ;
        }

        CC_PalMemCopy((uint8_t*)P1R_ptr, (uint8_t*)rBuff, auxPrimeSizeInBytes); /*for P*/ //xp1

#endif
#endif

        /* copy random number into PKA register rP1 */
        PkaCopyDataIntoPkaReg( rP1/*dstReg*/, LEN_ID_MAX_BITS, rBuff/*src_ptr*/, auxPrimeSizeInWords);

#ifdef RSA_KG_NO_RND
#ifdef LLF_PKI_PKA_DEBUG
        CC_PalMemSetZero(rBuff, sizeof(rBuff));
        if (PQindex == 0) {
                CC_PalMemCopy( rBuff, rBuff2, auxPrimeSizeInBytes); /*for P*/
        } else {
                CC_PalMemCopy( rBuff, rBuff4, auxPrimeSizeInBytes); /*for Q*/
        }

#ifdef BIG__ENDIAN
        CC_COMMON_INVERSE_UINT32_IN_ARRAY( rBuff, auxPrimeSizeInWords );
#endif
#endif

#else

        /* get a random auxiliary number P2     */
        CC_PalMemSetZero(rBuff, sizeof(rBuff));
        error = RndGenerateVectFunc((void *)rndState_ptr, (unsigned char *)rBuff, (size_t)auxPrimeSizeInBytes );
        if (error != CC_OK) {
                return error;
        }

#endif
    /* set MS bit of P2*/
        rBuff[auxPrimeSizeInWords-1] &= mask;
        rBuff[auxPrimeSizeInWords-1] |= msBit;
        /* set LSBit = 1 to ensure the odd number */
        rBuff[0] |= 1UL;

#ifdef FIPS_CERTIFICATION
    primeInt->bitlen2 = rsaKgPrimeTestParams->auxPrimesSizeInBits;
    CC_CommonReverseMemcpy(primeInt->xPrime2, (uint8_t*)rBuff, auxPrimeSizeInBytes);
#endif
        /*  Debug  */
#ifdef LLF_PKI_PKA_DEBUG
#if defined RSA_KG_FIND_BAD_RND
        CC_PalMemCopy( (uint8_t*)P2R_ptr, (uint8_t*)rBuff, auxPrimeSizeInBytes); /*for P*/ //xp2
#endif
#endif

        /* copy random number P2 into PKA register rP2 */
        PkaCopyDataIntoPkaReg( rP2/*dstReg*/, LEN_ID_MAX_BITS, rBuff/*src_ptr*/, auxPrimeSizeInWords);

    //at the end of this function rP includes P value
        /* find the primes P1,P2, P */
        error = RsaKgX931FindPrime(pRndContext,
                                      rP1, rP2, /*aux.primes*/
                                      rP, /*prime*/
                                      modulusSizeInBits,
                                      rE, /*exp*/
                                      rsaKgPrimeTestParams,
                                      pRegTemps + 2, tempsCount-2,
                                      pTempBuff);

        /* Debug */
#ifdef LLF_PKI_PKA_DEBUG
#if (defined RSA_KG_NO_RND || defined RSA_KG_FIND_BAD_RND)
        /* save found results: rP1,rP2,rP for P and Q accordingly */
        PkaCopyDataFromPkaReg( P1Pr_ptr/*dst_ptr*/, auxPrimeSizeInWords, rP1/*srcReg*/);

        PkaCopyDataFromPkaReg( P2Pr_ptr/*dst_ptr*/, auxPrimeSizeInWords, rP2/*srcReg*/);

        PkaCopyDataFromPkaReg( PQ_ptr/*dst_ptr*/, modulusSizeInBits/32, rP/*srcReg*/);
#endif
#endif

        return error;

}


/***********    RsaKgPrimeTest   function      **********************/
/**
 * @brief Performs assigned count of Rabin-Miller tests and one Lucas-Lehmer
 *        test according to testing mode:
 *              - for RSA according to ANSI X9.31 standard.
 *              - for DH  according to ANSI X9.42 standard.
 *
 *         NOTE:  For using in RSA module  size of each temp buffer must be of minimum size
 *                of prime number P in words.
 *                For using in ANSI X9.42 standard (DH,DSA algorithms) size of each temp buffer
 *            must be minimum of two size of prime number P in words.
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
static CCError_t RsaKgPrimeTest(CCRndContext_t *pRndContext, /*!< [in/out]  Pointer to the RND context buffer. */
                                  uint32_t *pPrimeP,         /*!< [in]  Pointer to the prime buff. */
                                  int32_t   sizeWords,       /*!< [in]  Prime size in words. */
                                  int32_t   rabinTestsCount, /*!< [in]  Count of Rabin-Miller tests repetition. */
                                  int8_t   *pIsPrime,        /*!< [in]  The flag indicates primality: not prime - CC_FALSE, otherwise - CC_TRUE. */
                                  CCRsaDhPrimeTestMode_t primeTestMode, /*!< [in]  Primality testing mode (RSA or DH - defines how are performed some
                                            operations on temp buffers. */
                                  uint32_t *pTempBuff) /*!< [in]  Temp buffer of size 3*ModSizeWords. */
{
        CCError_t error = CC_OK;
        uint32_t   modulusSizeInBits;
        uint32_t   divCount;
        uint32_t  pkaReqRegs = 11;

        /* virtual pointers to PKA regs*/
        /* set registers pointers, note: r0=PKA_REG_N, r1=PKA_REG_NP by default reserved for N and NP */
        uint8_t rT2 = regTemps[2];
        uint8_t rT3 = regTemps[3];
        uint8_t rT4 = regTemps[4]; /* temp registers */

        /* exact size of P */
        modulusSizeInBits = CC_BITS_IN_32BIT_WORD * sizeWords;

        /* initialize the PKA engine on default mode with size of registers       */
        /* according to operation size = max(Asize,Bsize)                         */
        error = PkiCalcNp(pTempBuff, pPrimeP, modulusSizeInBits);
        if (error != CC_OK) {
                return error;
        }

        error = PkaInitAndMutexLock(modulusSizeInBits, &pkaReqRegs);
        if (error != CC_OK) {
                return error;
        }

        /* set modulus into PKA register r0 */
        PkaCopyDataIntoPkaReg( PKA_REG_N/*dstReg*/, LEN_ID_MAX_BITS/*lenId*/, pPrimeP/*src_ptr*/, sizeWords);

        /* copy Np to PKA register #1*/
        PkaCopyDataIntoPkaReg( PKA_REG_NP/*dstReg*/, LEN_ID_MAX_BITS/*lenId*/, pTempBuff/*src_ptr*/,
                                CC_PKA_BARRETT_MOD_TAG_BUFF_SIZE_IN_WORDS);

        /*   execute primality tests       */
        /* count of small primes (each one is DxUint16) to check:
            - for DH algorithm use predefined value,
              else use maximal value  */

        if (primeTestMode == CC_DH_PRIME_TEST_MODE) {
                divCount = RSA_QUICK_PRIME_TEST_DIVISIONS_COUNT;
        } else {
                divCount = sizeof(g_SmallPrime) / sizeof(uint16_t);
        }

        /* test by small prime numbers */
        *pIsPrime = (int8_t)RsaKgQuickPrimeTest(
                                                     LEN_ID_N_BITS/*lenId*/,
                                                     PKA_REG_N /*prime P*/,
                                                     rT2, rT3, rT4/*temp regs*/,
                                                     divCount);

        /* the Miller-Rabin test */
        if (*pIsPrime == CC_TRUE) {
                error = PkaRsaKgX931MillerRabinTest(pRndContext,
                                                    LEN_ID_N_BITS /*lenId*/,
                                                    modulusSizeInBits,
                                                    pIsPrime, /*out*/
                                                    rabinTestsCount,
                                                    rT2, rT3, rT4/*temp regs*/,
                                                    pTempBuff);
                if (error != CC_OK) {
                        goto End;
                }
        }

        /* the Lucas test  */
        if (*pIsPrime == CC_TRUE) {
                error = PkaRsaKgX931LucasPrimeTest(
                                                  LEN_ID_N_BITS     /*lenId*/,
                                                  pIsPrime, /*out*/
                                                  regTemps+2,
                                                  7 /*tempsCount*/);
        }
        End:
        PkaFinishAndMutexUnlock(pkaReqRegs);

        return error;

}


/***********    RsaCalculateNandD   function      **********************/
/**
 * @brief Calculates RSA modulus and private key in NonCRT mode.
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
CCError_t RsaCalculateNandD(CCRsaPubKey_t *pCcPubKey, /*!< [in] pointer to the public key structure */
                        CCRsaPrivKey_t *pCcPrivKey,   /*!< [in] pointer to the private key structure */
                        CCRsaKgData_t  *pKeyGenData,  /*!< [in] pointer to a structure required for the KeyGen operation, holding P and Q */
                        uint32_t        primeSizeInBits) /*!< [in] Size of the prime factors in bits. */
{
    uint32_t  primeSizeInWords, i;
    uint32_t  pkaRegsCount = 8;
    CCError_t error=CC_OK;
    /* define virtual pointers to PKA registers */
    uint32_t r0   = PKA_REG_N; /*mod*/
    uint32_t rLcm = 1;
    uint32_t rP   = 2;
    uint32_t rQ   = 3;
    uint32_t rD   = 4;
    uint32_t rT   = 5;
    uint32_t *pPrivExp = pCcPrivKey->PriveKeyDb.NonCrt.d;


    /* setting the primes P,Q length in bytes */
    primeSizeInWords = CALC_FULL_32BIT_WORDS(primeSizeInBits);
    /*Mutex lock and PKA init*/
    error = PkaInitAndMutexLock(2*primeSizeInBits, &pkaRegsCount );
    if (error != CC_SUCCESS) {
        return error;
    }
    /* RL  clean the n-buffer */
    CC_PalMemSetZero( pCcPubKey->n, CC_32BIT_WORD_SIZE*CC_RSA_MAXIMUM_MOD_BUFFER_SIZE_IN_WORDS );

    PkaSetLenIds(primeSizeInBits, LEN_ID_PQ_BITS);
    PkaSetLenIds(GET_FULL_OP_SIZE_BITS(primeSizeInBits), LEN_ID_PQ_PKA_REG_BITS);

    /* clear pka memory for new using */
    PkaClearBlockOfRegs(r0/*firstReg*/, pkaRegsCount, LEN_ID_MAX_BITS);


    /*  copy P, Q into PKA registers. Note: now size of registers is full.  */
    PkaCopyDataIntoPkaReg(rP/*dstReg*/, LEN_ID_MAX_BITS, pKeyGenData->KGData.p/*src_ptr*/,
                          primeSizeInWords);

    PkaCopyDataIntoPkaReg(rQ/*dstReg*/, LEN_ID_MAX_BITS, pKeyGenData->KGData.q/*src_ptr*/,
                          primeSizeInWords);

    /*******************************************************************************************/
    /*                     CALCULATIONS WITH LONG REGISTERS                                    */
    /*  Init the PKA again on default mode according to N operation size.                      */
    /*  Note: All PKA memory shall be cleaned, nSizeInBits=> entry 0, nSizeInBits+CC_PKA_WORD_SIZE_IN_BITS=> entry 1 */
    /*******************************************************************************************/
    /*     N= r0= P*Q. lenId = 0 for full reg size   */
    PKA_MUL_LOW(LEN_ID_N_BITS, r0, rP/*OpA*/, rQ/*OpB*/);   // use LEN_ID_N_BITS, since its size is 2*primeSizeInBits

    /* output the modulus N */
    PkaCopyDataFromPkaReg(pCcPubKey->n,  2*primeSizeInWords, r0/*srcReg*/);
    CC_PalMemCopy(pCcPrivKey->n, pCcPubKey->n, pCcPubKey->nSizeInBits / CC_BITS_IN_BYTE);

        if (pCcPrivKey->OperationMode == CC_RSA_NoCrt) {
                bool isTrue = false;
                uint32_t  stat;
                uint32_t rGcd;
                uint32_t bit0P, bit0Q;


        /*    calculate D = E^-1 mod LCM(P-1)*(Q-1)   */
        PKA_FLIP_BIT0(LEN_ID_N_BITS, rP/*Res*/, rP/*OpA*/);
        PKA_FLIP_BIT0(LEN_ID_N_BITS, rQ/*Res*/, rQ/*OpA*/);

        /* remove common factors 2 from P-1, Q-1 to find odd */
        i = 0;
        do {
            PKA_SHR_FILL0(LEN_ID_N_BITS, rP, rP, 0/*shift-1*/);
            PKA_SHR_FILL0(LEN_ID_N_BITS, rQ, rQ, 0/*shift-1*/);
            PKA_READ_BIT0(LEN_ID_N_BITS, rP, bit0P);
            PKA_READ_BIT0(LEN_ID_N_BITS, rQ, bit0Q);
            i++;
        } while (bit0P == 0 && bit0Q == 0);


        /* D = (P-1) * (Q-1) / 2^i (removed only common divider 2^i) */
        PKA_2CLEAR(LEN_ID_MAX_BITS, rD); // ? RL
        PKA_MUL_LOW(LEN_ID_N_BITS, rD/*Res*/, rP/*OpA*/, rQ/*OpB*/);
        PKA_SHL_FILL0(LEN_ID_N_BITS, rD, rD, i-1);

        /* chose odd number as modulus for ModInv operation */
        if (bit0P == 1) {
            PKA_COPY(LEN_ID_N_BITS, r0/*dst*/, rP);
            rGcd = rQ;
        } else {
            PKA_COPY(LEN_ID_N_BITS, r0/*dst*/, rQ);
            rGcd = rP;
        }

        /* calculate GCD(rP,rQ) */
        PKA_MOD_INV(LEN_ID_N_BITS, rT/*temp*/, rGcd);
        /* LCM = ((P-1)*(Q-1) / GCD) = rD/rGcd */
        PKA_DIV(LEN_ID_N_BITS, rLcm/*res: LCM*/, rD, rGcd);


        // Because LCM may be even, but HW ModInw operation works only with odd modulus,
        // we use reverse calculation as follows: D =  1/E mod LCM = LCM - ((1/LCM mod E)*LCM - 1) / E

        /* copy public exp E into r0 register */
        PkaCopyDataIntoPkaReg(r0/*dstReg*/, LEN_ID_MAX_BITS, pCcPubKey->e/*src_ptr*/,
                              CALC_FULL_32BIT_WORDS(pCcPubKey->eSizeInBits));

        /* calc rT = 1/LCM mod E */
        PKA_COPY(LEN_ID_N_BITS, rP/*dst*/, rLcm/*LCM*/); /*rP used as temp*/
        PKA_DIV(LEN_ID_N_BITS, rQ/*Res not used*/, rP/*OpA=LCM*/, r0/*OpB=E*/); /*rP = LCM mod E*/

        PKA_MOD_INV(LEN_ID_N_BITS, rT/*Res*/, rP/*OpB*/); /* rT = 1/LCM mod E (E - odd, gcd(LCM,E)=1) */
        /* RL additional check need if E is not prime */
        PKA_COMPARE_IM_STATUS(LEN_ID_N_BITS, rP, 1/*im*/, stat);
        if (stat != 1) {
            error = PKA_INTERNAL_ERROR;
            goto End;
        }

        /* rK = (rT*LCM - 1) / r0=E  */
        PKA_MUL_LOW(LEN_ID_N_PKA_REG_BITS, rT/*Res*/, rT/*OpA*/, rLcm/*OpB*/); /* Note: size of result < register size, because E is small */
        PKA_SUB_IM(LEN_ID_N_PKA_REG_BITS, rT/*Res*/, rT/*OpA*/, 1/*OpB*/);
        PKA_DIV(LEN_ID_N_PKA_REG_BITS, rD/*Res*/, rT/*OpA*/, r0/*OpB*/); /*rT = rT / e*/
        PKA_SUB(LEN_ID_N_PKA_REG_BITS, rD/*Res*/, rLcm/*OpA*/, rD/*OpB*/);

        /*    output the result value D */
        PkaCopyDataFromPkaReg(pPrivExp, 2*primeSizeInWords, rD/*srcReg*/);

        /* check that d > 2^(nlen/2) [FIPS 186-4, B.3.1] - very rare  *
         *  case.                                                      */
        for (i = 2*primeSizeInWords - 1; i >= primeSizeInWords; i--) {
            isTrue = isTrue || (pPrivExp[i] != 0);
        }
        if (!isTrue) {
            CC_PalMemSetZero(pPrivExp, 2*primeSizeInWords);
            error = CC_RSA_GENERATED_PRIV_KEY_IS_TOO_LOW;
        }

        /* set the length of d in bits */
        pCcPrivKey->PriveKeyDb.NonCrt.dSizeInBits =
        CC_CommonGetWordsCounterEffectiveSizeInBits(pCcPrivKey->PriveKeyDb.NonCrt.d,
                                                    (uint16_t)(CALC_FULL_32BIT_WORDS(pCcPubKey->nSizeInBits)));
        }

    End:

    PkaFinishAndMutexUnlock( pkaRegsCount );

    return error;
}


/***********    RsaKgFindPrime   function      **********************/
/**
 * @brief The RsaKgFindPrime generates a prime for RSA keys.
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
static CCError_t RsaKgFindPrime(
                    CCRndContext_t *pRndContext, /*!< [in/out]  Pointer to the RND context buffer. */
                    uint32_t *pPubExp,     /*!< [in]  Pointer to the public exponent. */
                    uint32_t  eSizeInBits, /*!< [in/out]  Size of public exponent in bits. */
                    uint32_t  nSizeInBits, /*!< [in/out]  Required size of the key in bits. */
                    uint32_t *pSuccess,    /*!< [out]  Pointer to the flag of success generation of P,Q.*/
                    uint32_t *pPrime,      /*!< [out]  Pointer to the first factor - P. */
                    uint32_t *pTempBuff)   /*!< [out]  Temp buffer for internal use. */
{
    CCError_t error=CC_OK;
    uint32_t primeSizeInBits, primeSizeInWords;
    RsaKgParams_t rsaKgPrimeTestParams;
    /* virtual pointers to PKA registers of single size */
    int8_t   rE, rP;
    /* virtual pointers to single temp PKA registers */
    uint32_t  status, maxCountRegs = 20;

    // setting the primes P,Q length ; Note: the size of the modulus n is even
    primeSizeInBits = nSizeInBits / 2;
    primeSizeInWords = CALC_FULL_32BIT_WORDS(primeSizeInBits);

    /*Initialize PKA and mutex lock*/
    error = PkaInitAndMutexLock(primeSizeInBits,  &maxCountRegs);
    if (error != CC_SUCCESS) {
        return error;
    }

    *pSuccess = CC_FALSE;

    // set virtual registers pointers
    rE =regTemps[2]; /*2*/
    rP =regTemps[3]; /*3*/

    /* Set size if P, Q and auxiliary primes p1,p2,q1,q2 according  *
     *   to keysize. The following settings meet to  FIPS 186-4:    *
     *   5.1, C.3: Tab.C3.                                          */
    if (nSizeInBits <= CC_RSA_FIPS_KEY_SIZE_1024_BITS) {
        rsaKgPrimeTestParams.auxPrimesSizeInBits = PKA_RSA_KEY_1024_AUX_PRIME_SIZE_BITS;
        rsaKgPrimeTestParams.auxPrimesMilRabTestsCount = PKA_RSA_KEY_1024_AUX_PRIME_RM_TST_COUNT /*38*/;
        rsaKgPrimeTestParams.pqPrimesMilRabTestsCount  = PKA_RSA_KEY_1024_PQ_PRIME_RM_TST_COUNT  /* 7*/;
    } else if (nSizeInBits <= CC_RSA_FIPS_KEY_SIZE_2048_BITS) {
        rsaKgPrimeTestParams.auxPrimesSizeInBits = PKA_RSA_KEY_2048_AUX_PRIME_SIZE_BITS;
        rsaKgPrimeTestParams.auxPrimesMilRabTestsCount = PKA_RSA_KEY_2048_AUX_PRIME_RM_TST_COUNT /*32*/;
        rsaKgPrimeTestParams.pqPrimesMilRabTestsCount  = PKA_RSA_KEY_2048_PQ_PRIME_RM_TST_COUNT  /* 4*/;
    } else {/* if key size > 2048 */
        rsaKgPrimeTestParams.auxPrimesSizeInBits = PKA_RSA_KEY_3072_AUX_PRIME_SIZE_BITS;
        rsaKgPrimeTestParams.auxPrimesMilRabTestsCount = PKA_RSA_KEY_3072_AUX_PRIME_RM_TST_COUNT /*27*/;
        rsaKgPrimeTestParams.pqPrimesMilRabTestsCount  = PKA_RSA_KEY_3072_PQ_PRIME_RM_TST_COUNT  /* 3*/;
    }

    /**********************************************************************************/
    /*                     CALCULATIONS WITH SHORT REGISTERS                          */
    /* init PKA on default mode according to P,Q operation size for creating P and Q. */
    /*  Note: All PKA memory shall be cleaned, insert nSizeInBits/2 => entry 0,       */
    /*        nSizeInBits/2+CC_PKA_WORD_SIZE_IN_BITS => entry 1                     */
    /**********************************************************************************/

    /* set additional sizes into RegsSizesTable: */
    PkaSetLenIds(nSizeInBits/2, LEN_ID_PQ_BITS);
    PkaSetLenIds(GET_FULL_OP_SIZE_BITS(nSizeInBits/2), LEN_ID_PQ_PKA_REG_BITS);
    PkaSetLenIds(rsaKgPrimeTestParams.auxPrimesSizeInBits, LEN_ID_AUX_PRIME_BITS);

    /* inforcing the prime candidates P,Q so the size of they is keySize/2 */
    pPrime[primeSizeInWords - 1] |= 0x80000000;
    pPrime[primeSizeInWords] = 0;
    pPrime[0] |= 0x01;
#ifdef FIPS_CERTIFICATION
    CC_CommonReverseMemcpy(primeInt->xPrime, (uint8_t *)pPrime, primeSizeInBits/CC_BITS_IN_BYTE);
#endif

    /* copy P,E buffers into PKA registers */
    PkaCopyDataIntoPkaReg(rP/*dstReg*/, LEN_ID_MAX_BITS, pPrime/*src_ptr*/, primeSizeInWords);
    PkaCopyDataIntoPkaReg(rE/*dstReg*/, LEN_ID_MAX_BITS, pPubExp/*src_ptr*/, CALC_FULL_32BIT_WORDS(eSizeInBits));

    /* find the prime vector  */
    error = PkaRsaKgFindPrime( pRndContext, rP, primeSizeInBits,
                               rE,
                               &rsaKgPrimeTestParams,
                               regTemps+5, maxCountRegs-5/*tempsCount*/,
                               pTempBuff);


    if (error != CC_OK) {
            goto End;
    }

    /* temp for debug */
#if defined LLF_PKI_PKA_DEBUG && defined DEBUG
#if (defined RSA_KG_NO_RND || defined RSA_KG_FIND_BAD_RND)
    PQindex = 1;
#endif
#endif

    PkaCopyDataFromPkaReg( pPrime/*dst_ptr*/, primeSizeInWords, rP/*srcReg*/ );

    /* Because now we use only a prime e it's enough to check that (p-1) / e has a residue.
    *  Divide:  rE = (public exponent) mod (generated prime)
    *  NOTE: if e generated as random odd number - it is required to test that GCD(e,p-1)=1, instead of divison*/
    PKA_SUB_IM(LEN_ID_MAX_BITS, rP, rP, 1);
    PKA_DIV(LEN_ID_MAX_BITS, RES_DISCARD/*Res not used*/, rP/*OpA*/, rE/*OpB*/);
    PKA_COMPARE_IM_STATUS(LEN_ID_MAX_BITS, rP/*(p-1) mod e*/, 0, status)

    if (status == 0){ //if rP==0 than status =1
        *pSuccess = CC_TRUE;
    }

End:
    /* Finish PKA operations (waiting PKI done and close PKA clocks) */
    PkaFinishAndMutexUnlock(maxCountRegs);

    return error;


}



/***********    RsaCalculateCrtParams   function      **********************/
/**
 * @brief Calculates a private key on CRT mode
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
 CCError_t RsaCalculateCrtParams(uint32_t *pPubExp,      /*!< [in]  Pointer to the public exponent. */
                                         uint32_t eSizeInBits,  /*!< [in]  Public exponent size in bits. */
                                         uint32_t nSizeInBits,  /*!< [in]  Size of the key modulus in bits. */
                                         uint32_t *pPrimeP,     /*!< [out]  First factor pointer - p. */
                                         uint32_t *pPrimeQ,     /*!< [out]  Second factor pointer - Q. */
                                         uint32_t *pPrivExp1dp, /*!< [out]  Private exponent for first factor - dP. */
                                         uint32_t *pPrivExp2dq, /*!< [out]  Private exponent for second factor - dQ. */
                                         uint32_t *pQInv)       /*!< [out]  The modular inverse of q relatively to modulus p - qInv. */
{
     CCError_t error=CC_OK;
     uint32_t primeSizeInWords;
     /* virtual pointers to PKA registers of single size */
     int8_t  r0, rP, rdP, rQ, rdQ, rQinv, rE;
     /* virtual pointers to single temp PKA registers */
     int8_t rT1, rT2, rT3, rT4, rT5;
     uint32_t maxCountRegs = 15;

     /* setting the primes P,Q length ; Note: the size of the modulus n is even */
     primeSizeInWords = CALC_FULL_32BIT_WORDS(nSizeInBits/2);

     error = PkaInitAndMutexLock(primeSizeInWords*CC_BITS_IN_32BIT_WORD, &maxCountRegs );
     if (error != CC_SUCCESS) {
         return error;
     }

     // set virtual registers pointers
     r0 = regTemps[0]; /* PKA_REG_N */
     rE = regTemps[2]; /*2*/
     rP = regTemps[3]; /*3*/
     rQ = regTemps[4]; /*4*/
     rdP = regTemps[5]; /*5*/
     rdQ = regTemps[6]; /*6*/
     rQinv = regTemps[7]; /*7*/
     rT1 = regTemps[8]; /*8*/
     rT2 = regTemps[9]; /*9*/
     rT3 = regTemps[10]; /*10*/
     rT4 = regTemps[11]; /*11*/
     rT5 = regTemps[12]; /*12*/

     // copy data into PKA registers
     PkaCopyDataIntoPkaReg(rE/*dstReg*/, LEN_ID_MAX_BITS, pPubExp/*src_ptr*/, CALC_FULL_32BIT_WORDS(eSizeInBits));
     PkaCopyDataIntoPkaReg(rP/*dstReg*/, LEN_ID_MAX_BITS, pPrimeP/*src_ptr*/, primeSizeInWords);
     PkaCopyDataIntoPkaReg(rQ/*dstReg*/, LEN_ID_MAX_BITS, pPrimeQ/*src_ptr*/, primeSizeInWords);

     // dQ = E^-1 mod (Q-1);
     // dQ: set mod register r0=Q-1 and perform ModInv operation
     PKA_FLIP_BIT0(LEN_ID_N_PKA_REG_BITS, r0/*res*/, rQ/*opA*/);
     PKA_COPY(LEN_ID_MAX_BITS, rT1/*ds*/, rE/*src*/);
     error = PkaExecFullModInv( rT1/*OpB*/, rdQ/*Res*/, rT2, rT3, rT4, rT5);
     if (error != CC_OK) {
         goto End;
     }


     // dP = E^-1 mod (P-1);
     // dP: set mod register r0<=P-1 and perform ModInv operation
     PKA_FLIP_BIT0(LEN_ID_N_PKA_REG_BITS, r0/*res*/, rP/*dst*/);
     PKA_COPY(LEN_ID_MAX_BITS, rT1/*ds*/, rE/*src*/);
     error = PkaExecFullModInv( rT1/*OpB*/, rdP/*Res*/, rT2, rT3, rT4, rT5);
     if (error != CC_OK) {
         goto End;
     }

     // qInv = Q^-1 mod P;
     // Qinv: set mod register r0<=P and perform ModInv operation
     PKA_FLIP_BIT0(LEN_ID_N_PKA_REG_BITS, r0/*Res*/, r0/*OpA*/); /* r0= P */
     PKA_COPY(LEN_ID_MAX_BITS, rT1/*dst*/, rQ/*src*/);
     PKA_MOD_INV(LEN_ID_N_BITS, rQinv/*Res*/, rT1/*OpB*/);


     //    output of the result values dP,dQ,qInv
     PkaCopyDataFromPkaReg(pPrivExp1dp, primeSizeInWords, rdP/*srcReg*/);
     PkaCopyDataFromPkaReg(pPrivExp2dq, primeSizeInWords, rdQ/*srcReg*/);
     PkaCopyDataFromPkaReg(pQInv, primeSizeInWords, rQinv/*srcReg*/);
 End:
     PkaFinishAndMutexUnlock(maxCountRegs);
     return error;

}






/***********    RsaPrimeTestCall   function      **********************/
/**
 * @brief Test a primality according to ANSI X9.42 standard by
 *  calling the RsaKgPrimeTest() which performs said algorithm.
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
CCError_t RsaPrimeTestCall(CCRndContext_t *pRndContext,          /*!< [in/out]  Pointer to the RND context buffer. */
                             uint32_t *pPrimeP,                        /*!< [in] The pointer to the prime buff. */
                             int32_t   sizeWords,                  /*!< [in] The prime size in words. */
                             int32_t   rabinTestsCount,            /*!< [in] The count of Rabin-Miller tests repetition. */
                             int8_t   *pIsPrime,               /*!< [in] TThe flag indicates primality:
                                        if is not prime - CC_FALSE, otherwise - CC_TRUE. */
                             uint32_t *pTempBuff,                  /*!< [in] The temp buffer of minimum size:
                                        - on HW platform  3*MaxModSizeWords,
                                        - on SW platform  41*MaxModSizeWords. */
                             CCRsaDhPrimeTestMode_t primeTestMode)/*!< [in] primality testing mode (RSA or DH - defines how are performed some
                                        operations on temp buffers. */
{
        CCError_t error;

    error =  RsaKgPrimeTest(
                                  pRndContext,
                                  pPrimeP,
                                  sizeWords,
                                  rabinTestsCount,
                                  pIsPrime,
                                  primeTestMode,
                                  pTempBuff);


    return error;

}


/***********    RsaGenPandQ   function      **********************/
/**
 * @brief Generates P and Q primes for RSA key
 *
 * @return  CC_OK On success, otherwise indicates failure
 */
CCError_t RsaGenPandQ(
                    CCRndContext_t *pRndContext, /*!< [in/out] Pointer to the RND context buffer. */
                    size_t          KeySize,        /* key size in bits */
                    uint32_t        eSizeInBits,
                    uint32_t       *pPubExp,
                    CCRsaKgData_t  *pKeyGenData) /*!< [in] Temporary buffer for internal use. */
{

    /* the error identifier */
    CCError_t error = CC_OK;

    uint32_t *pPrimeP, *pPrimeQ;
    uint32_t pqSizeWords;
    uint32_t success = 0;
#if ((!defined RSA_KG_FIND_BAD_RND && !defined RSA_KG_NO_RND) || defined RSA_KG_FIND_BAD_RND || !defined DEBUG)
    uint32_t buffXp[4]; //buffer for 4 MSB words of Xq
#endif
    uint32_t distPQ , tempSwap, i;
    CCCommonCmpCounter_t cmpRes;

    pPrimeP = pKeyGenData->KGData.p;
    pPrimeQ = pKeyGenData->KGData.q;
    pqSizeWords = CALC_FULL_32BIT_WORDS(KeySize/2);
#if (defined RSA_KG_FIND_BAD_RND && defined DEBUG)
    CC_PalMemCopy( RSA_KG_debugPvect, (uint8_t*)pKeyGenData->KGData.p, KeySize/(2*CC_BITS_IN_BYTE) );
    CC_PalMemCopy( RSA_KG_debugQvect, (uint8_t*)pKeyGenData->KGData.q, KeySize/(2*CC_BITS_IN_BYTE) );
#endif

#if (defined RSA_KG_NO_RND && defined DEBUG)
    CC_PalMemCopy( (uint8_t*)pKeyGenData->KGData.p, RSA_KG_debugPvect, KeySize/(2*CC_BITS_IN_BYTE) );
    CC_PalMemCopy( (uint8_t*)pKeyGenData->KGData.q, RSA_KG_debugQvect, KeySize/(2*CC_BITS_IN_BYTE) );
#endif
#if ((defined RSA_KG_FIND_BAD_RND || defined RSA_KG_NO_RND) && defined DEBUG)
#ifdef BIG__ENDIAN
    /* for big endiannes machine reverse bytes order in words according to Big Endian  */
    CC_COMMON_INVERSE_UINT32_IN_ARRAY( pKeyGenData->KGData.p, KeySize/(2*CC_BITS_IN_32BIT_WORD) );
    CC_COMMON_INVERSE_UINT32_IN_ARRAY( pKeyGenData->KGData.q, KeySize/(2*CC_BITS_IN_32BIT_WORD) );
#endif
#endif



    /* ................ Generate the 1st prime ................................. */
    /* ------------------------------------------------------------------------- */
    do{
        /* clean the word adjacent to the old P (in case there's garbage in it) */
        pPrimeP[pqSizeWords]=0;

        /*Generate Xp*/
#if ((!defined RSA_KG_FIND_BAD_RND && !defined RSA_KG_NO_RND) || defined RSA_KG_FIND_BAD_RND || !defined DEBUG)
        error = CC_RsaGenerateVectorInRangeX931(pRndContext, pqSizeWords, pPrimeP);
        if (error != CC_OK){
            goto End;
        }

        /*save 4 MSB of Xp to temporary buffer for checking it's distance from Xq*/
        CC_PalMemCopy(buffXp, pPrimeP, 4 *CC_32BIT_WORD_SIZE );

#ifdef FIPS_CERTIFICATION
        rsaKgOutParams.pP_PrimeInt = &prim1Int;
        rsaKgOutParams.pQ_PrimeInt = &prim2Int;
        primeInt = &prim1Int;
#endif

#endif
        /* for debug */
#if (defined RSA_KG_FIND_BAD_RND && defined DEBUG)
        CC_PalMemCopy( RSA_KG_debugPvect, (uint8_t*)pPrimeP, KeySize/(2*CC_BITS_IN_BYTE) );
        CC_PalMemCopy( RSA_KG_debugQvect, (uint8_t*)pPrimeQ, KeySize/(2*CC_BITS_IN_BYTE) );
#endif
#if ((defined RSA_KG_FIND_BAD_RND || defined RSA_KG_NO_RND) && defined DEBUG)
#ifdef BIG__ENDIAN
        /* for big endiannes machine reverse bytes order in words according to Big Endian  */
        CC_COMMON_INVERSE_UINT32_IN_ARRAY( pPrimeP, KeySize/(2*CC_BITS_IN_32BIT_WORD) );
        CC_COMMON_INVERSE_UINT32_IN_ARRAY( pPrimeQ, KeySize/(2*CC_BITS_IN_32BIT_WORD) );
#endif
#if defined LLF_PKI_PKA_DEBUG
        PQindex = 0;
#endif
#endif
        /*Generate P:*/

        error = RsaKgFindPrime(
                pRndContext,
                pPubExp,  eSizeInBits,
                KeySize,
                &success,
                pPrimeP,
                pKeyGenData->KGData.kg_buf.ccRSAKGDataIntBuff);
        if (error != CC_OK){
            goto End;
        }

    }while(!success); /* End of loop for generating  P */

    /* ................ Generate the 2nd prime ................................. */
    /* ------------------------------------------------------------------------- */
    while(1){
        /* clean the word adjacent to the old Q (in case there's garbage in it) */
        pPrimeQ[pqSizeWords]=0;

#if ((!defined RSA_KG_FIND_BAD_RND && !defined RSA_KG_NO_RND) || defined RSA_KG_FIND_BAD_RND || !defined DEBUG)
        while(1){/*Generate Xq*/
            error = CC_RsaGenerateVectorInRangeX931(pRndContext, pqSizeWords, pPrimeQ); /*Xq Generation*/
            if (error != CC_OK){
                goto End;
            }

            /*check |Xp - Xq|<= 2^(keySize/2-100):
             * compare 100 MS bits (3 words + 4 bits) of XP and XQ if they are equall, then
             * generate new Xq*/
            distPQ = (buffXp[3] - pPrimeQ[pqSizeWords-1]) != 0 ||
                     (buffXp[2] - pPrimeQ[pqSizeWords-2]) != 0 ||
                     (buffXp[1] - pPrimeQ[pqSizeWords-3]) != 0 ||
                    ((buffXp[0] - pPrimeQ[pqSizeWords-4]) & 0xF0000000) != 0;
            if (distPQ)
            {
                break;
            }
        } /* Generation of Xq is done - pPrimeQ points to Xq */
#endif

        /*Generate Q*/
#if defined LLF_PKI_PKA_DEBUG && defined DEBUG
#if (defined RSA_KG_NO_RND || defined RSA_KG_FIND_BAD_RND)
        PQindex = 1;
#endif
#endif
#ifdef FIPS_CERTIFICATION
        primeInt = &prim2Int;
#endif
        error = RsaKgFindPrime(
                pRndContext,
                pPubExp,  eSizeInBits,
                KeySize,
                &success,
                pPrimeQ,
                pKeyGenData->KGData.kg_buf.ccRSAKGDataIntBuff);
        if (error != CC_OK){
            goto End;
        }


        if(success)
        {
            distPQ = (pPrimeP[3] - pPrimeQ[pqSizeWords-1]) != 0 ||
                     (pPrimeP[2] - pPrimeQ[pqSizeWords-2]) != 0 ||
                     (pPrimeP[1] - pPrimeQ[pqSizeWords-3]) != 0 ||
                    ((pPrimeP[0] - pPrimeQ[pqSizeWords-4]) & 0xF0000000) != 0;

            if (distPQ){
                break;
            }
            else {
                /* In case the distance between P and Q is too small, generate a new prime.
                 * The smaller prime should be discarded and generated again.
                 * If Q is larger then P set P=Q. */

                cmpRes = CC_CommonCmpLsWordsUnsignedCounters(pPrimeP,pqSizeWords,pPrimeQ,pqSizeWords);
                if (cmpRes != CC_COMMON_CmpCounter1GreaterThenCounter2) {  /* if Q > P */
                    CC_PalMemCopy(pPrimeP/*dest*/, pPrimeQ/*src*/, pqSizeWords*CC_32BIT_WORD_SIZE);
#ifdef FIPS_CERTIFICATION
                    rsaKgOutParams.pP_PrimeInt = &prim2Int;
                    rsaKgOutParams.pQ_PrimeInt = &prim1Int;
#endif
                }
            }
        }
    }/* End of loop for generating  Q - pPrimeQ points to Q */

    /* if Q is larger then P exchange the vectors - we want to have P > Q */
    /* copy P,Q, buffers into PKA registers */
    cmpRes = CC_CommonCmpLsWordsUnsignedCounters(pPrimeP, pqSizeWords, pPrimeQ, pqSizeWords);

    if (cmpRes != CC_COMMON_CmpCounter1GreaterThenCounter2) {  /* Q > P */
        /* swap P/Q */
        for(i=0 ; i<pqSizeWords ; i++){
            tempSwap = pKeyGenData->KGData.p[i];
            pKeyGenData->KGData.p[i] = pKeyGenData->KGData.q[i];
            pKeyGenData->KGData.q[i] = tempSwap;
        }

#ifdef FIPS_CERTIFICATION
        rsaKgOutParams.pP_PrimeInt = &prim2Int;
        rsaKgOutParams.pQ_PrimeInt = &prim1Int;
#endif
    }

#if  (defined LLF_PKI_PKA_DEBUG || defined RSA_KG_FIND_BAD_RND || !defined DEBUG)
End:
#endif
    return error;
}

#endif /* !defined(CC_IOT) || ( defined(CC_IOT) && defined(MBEDTLS_RSA_C)) */
