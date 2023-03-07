/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/************* Include Files ****************/

#include "dx_rng.h"
#include "cc_pal_mem.h"
#include "cc_pal_types.h"
#include "cc_rng_plat.h"
#include "dx_crys_kernel.h"
#include "cc_hal.h"
#include "cc_regs.h"
#include "dx_host.h"
#include "cc_rnd_local.h"
#include "cc_rnd_error.h"
#include "llf_rnd_hwdefs.h"
#include "llf_rnd.h"
#include "llf_rnd_error.h"
#include "cc_sram_map.h"
#include "cc_address_defs.h"
#include "llf_rnd_trng.h"
#include "cc_aes_defs.h"


/********************************* Defines ******************************/
#ifndef max
#define max(a,b) (a) > (b) ? (a) : (b)
#endif

/* definitions used in the Entropy Estimator functions */
#define S(a,n) ((uint32_t)((a) * (1<<(n)))) /* a scaled by n: a \times 2^n */
#define U(a,n) ((uint32_t)(a) >> (n)) /* unscale unsigned: a / 2^n */
#define SQR(x) (((x)&0xffff)*((x)&0xffff))

/* macros for updating histogram for any separate bit;
   where x represents cw  or e1 */
#define   LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, x ) \
    h_ptr[x & 0xff]++;  \
    ec_ptr[x & 0x7f] = ((ec_ptr[x & 0x7f] & 1) == ((x & 0xff) >> 7)) ? ec_ptr[x & 0x7f] + 2 : ec_ptr[x & 0x7f] ^ 1; \
    x >>= 1;

/* Entropy estimation histogram width (prefix size + 1) */
#define LLF_RND_nb   CC_RND_nb
#define LLF_RND_NB   CC_RND_NB
#define halfNB   (LLF_RND_NB / 2)
#define ROSC_INIT_START_BIT   0x80000000


/*********************************** Enums ******************************/
/*********************************Typedefs ******************************/

/**************** Global Data to be read by RNG function ****************/


/* test variables */
#ifdef RND_TEST_TRNG_WITH_ESTIMATOR
uint32_t  gEntrSize[4];
#endif


/******************************************************************************/
/***************   Prototypes and Private functions    ************************/
/******************************************************************************/
/****************************************************************************/
/***********         Functions used for Entropy estimation      *************/
/****************************************************************************/
/**
 * The function calculates low half of 32*32 bits multiplication result
 *
 * @param a
 * @param b
 *
 * @return uint64_t
 */
uint64_t Mult32x32(uint32_t a, uint32_t b)
{
    uint64_t res=0;

    res = (((a>>16)*(b>>16)) + (((a>>16)*(b&0xffff))>>16) + (((b>>16)*(a&0xffff))>>16));
    res <<= 32;
    res += (uint64_t)((a&0xffff)*(b&0xffff)) + (((a>>16)*(b&0xffff))<<16) + (((b>>16)*(a&0xffff))<<16);

    return res;
}

/* Calculate 48*16 bits multiple using 16*16 bit multiplier */
/* Code ASM takes 62 bytes */
uint64_t Mult48x16(uint64_t a, uint32_t b)
{
    uint32_t a3 = (a >> 32), a2 = (a >> 16) & 0xffff, a1 = a & 0xffff;
    uint32_t b1 = (b & 0xffff);
    uint32_t r31 = a3*b1, r21 = a2*b1, r11 = a1*b1;
    return(((uint64_t)r31) << 32) +
    (((uint64_t)r21) << 16) +
    ((uint64_t)r11);
}


/* approximation of entropy  */
/**
 * @brief The function approximates the entropy for separate prefix
 *        ae = n * log2(n/m).
 *
 *    Implementation according A.Klimov algorithm uses approximation by
 *    polynomial: ae = (n-m)*(A1 + A2*x + A3*x^2), where x = (n-m)/n <= 0.5 .
 *    The coefficients are defined above in this file.
 *
 * @param[in] n - The summ of  0-bits and 1-bits in the test.
 * @param[in] m - The maximal from the two above named counts.
 *
 * @return - result value of entropy ae.
 */
static uint32_t ae(uint32_t n, uint32_t m)
{
    /* logarithm calculation constants */
    #define A1 1.4471280
    #define A2 0.6073851
    #define A3 0.9790318


    uint32_t d = n-m,
    x = S(d,16) / n,         /* x; 16 */
        a = S(A3,14) * x,            /* x*A3; 30 */
        b = U(S(A2,30) + a, 16) * x,     /* x*(A2 + x*A3); 30 */
            c = (S(A1,30) + b),          /* (A1 + x*(A2 + x*A3)); 30 */
            r = d * U(c,14);         /* result: 16 bits scaled */

    return r;

}

/*****************************************************************************/
/**
 * @brief The function calculates a histogram of 0-s and 1-s distribution
 *        depending on forgouing bits combination - prefix.
 *
 *     Implementation according A.Klimov algorithm modified by A.Ziv
 *
 * @param[in]  h_ptr - The pointer to the histogramm h buffer.
 * @param[in]  ec_ptr - The pointer to the histogramm equality counter (ec) buffer.
 * @param[in]  r_ptr - The pointer to Entropy source.
 * @param[in]  nr    - The size of Entropy source in words.
 * @param[in/out] pref_ptr - The pointer to last saved prefix.
 * @param[in]  snp_ptr   - The pointer to the flag defining whether the new prefix should be set.
 *
 * @return CCError_t - no return value
 */
static void LLF_RND_HistogramUpdate(
                   uint32_t   *h_ptr,     /* in/out */
                   uint32_t   *ec_ptr,  /* in/out */
                   uint32_t   *r_ptr,     /* in - input sequence */
                   uint32_t     nr)    /* in - input sequence size in words */
{
    int32_t   i;
    uint32_t j = 0;
    uint32_t  cW;   /*current word of sequence*/
    uint32_t  pref;

    /* FUNCTION  LOGIC  */

    /*------------------------------------------------------*/
    /* update for first word of sequence: begin new prefix  */
    /*------------------------------------------------------*/
    cW = r_ptr[0];
    /* 25 sequences are purely from new bits */
    for (i = 0; i < 5; i++) {
        LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, cW );
        LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, cW );
        LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, cW );
        LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, cW );
        LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, cW );
    }

    pref = cW;
    j = 1;

    /*-----------------------------------------------------------------------*/
    /* update for remaining words of sequence: continue with previous prefix */
    /*-----------------------------------------------------------------------*/
    for (; j < nr; j++) {
        uint32_t e1;

        /*current word of random sequence*/
        cW = r_ptr[j];
        /* concatenation of previous saved prefix and new bits */
        e1 = (cW << 7) | pref;

        /* first 7 sequences are combined from previous prefix and new bits  */
        LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, e1 );
        LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, e1 );
        LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, e1 );
        LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, e1 );
        LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, e1 );
        LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, e1 );
        LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, e1 );

        /* next 25 sequences are purely from new bits */
        for (i = 0; i < 5; i++) {
            LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, cW );
            LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, cW );
            LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, cW );
            LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, cW );
            LLF_RND_UpdateHistOneBit( h_ptr, ec_ptr, cW );
        }

        pref = cW;
    }

} /* End of LLF_RND_HistogramUpdate() */


/*****************************************************************************/
/**
 * @brief The function calculates estimation of entropy, generated by TRNG and
 *        used for control the TRNG work.
 *
 *   Implementation based on algorithm developed by A.Klimov.
 *
 * @param[in] h - The pointer to the h-buffer (counts of 0-s and 1-s for each prefix).
 * @param[in] ec - The pointer to the ec-buffer (equality counters).
 * @param[out] e_ptr - The pointer to count of accumulated Entropy (bits multiplied by 2^16).
 *
 * @return CCError_t - according to module definitions
 */
static CCError_t LLF_RND_EntropyEstimate(
                      uint32_t *h, /*in/out*/
                      uint32_t *ec,
                      uint32_t *e_ptr ) /* out - result Entropy size */
{

    uint64_t t = 0;        /* total entropy */
    uint32_t i, ac = 0;        /* number of active prefixes */


    /*-------------  calculate entropy -----------------*/

    for (i = 0; i < halfNB; ++i) {

        uint32_t n = h[i] + h[i+halfNB], m = max(h[i], h[i+halfNB]);

        /* check that n < 2^16, else return overflow error */
        if (n >= (1UL<<16))
            return LLF_RND_TRNG_ENTR_ESTIM_SIZE_EXCEED_ERROR;

        if (n != m) { /* if active prefix */
            uint32_t n2, pp, od;
            uint64_t od2, od2n, var;

            /* increment count of active prefixes */
            ++ac;

            pp = SQR(m) + SQR(n-m);       /* related to theoretical "autocorrelation" probability */
            n2 = Mult16x16((ec[i]>>1),n); /* n2 used as temp */

            /* value, related to observed deviation of autocorrelation */
            if (n2 > pp)
                od = n2 - pp;
            else
                od = pp - n2;

            /* theoretical variance of B(n, pp): always > 0 */
            n2 = SQR(n);
            var = Mult32x32(pp,(n2-pp));

            /* if  n*od^2 < var then accumulate entropy, else return Error;
               Note: that this condition is True only if od < 2^32 */
            if (od != CC_MAX_UINT32_VAL) {
                od2 = Mult32x32(od, od);

                /* scale variables */
                if (od2 > ((uint64_t)1ULL << 48)) {
                    od2 /= (1UL<<16);
                    var /= (1UL<<16);
                }

                od2n = Mult48x16(od2, n);

                if (od2n < var)
                    t += ae(n, m);
            }
        }
    }

    /* output entropy size value in bits (rescaled) */

    *e_ptr = ac > 3 ? (t / (1UL << 16)) : 0;

    return CC_OK;

} /* End of LLF_RND_EntropyEstimate */

/*****************************************************************************/
/**
 * @brief The function calculates estimation of entropy, generated by 4 ROSCs
 *
 * @param[in] ramAddr - The pointer to random source.
 * @param[in] blockSizeWords - The size of each block of random source in words.
 * @param[in] countBlocks - The blocks count (according to given ROSCS).
 * @param[in] h_ptr - The pointer to the h-buffer (counts of 0-s and 1-s for each prefix).
 * @param[in] ec_ptr - The pointer to the ec-buffer (equality counters).
 * @param[out] entrSize_ptr - The pointer to count of accumulated Entropy in bits.
 * @param[in] rndContext_ptr - The pointer to random State.
 *
 * @return CCError_t - according to module definitions
 */
CCError_t LLF_RND_EntropyEstimateFull(
              uint32_t *ramAddr,      /*in*/
              uint32_t  blockSizeWords, /*in*/
              uint32_t  countBlocks,      /*in*/
              uint32_t *entrSize_ptr,     /*out*/
              uint32_t  *rndWorkBuff_ptr)   /*in*/
{

    CCError_t error = 0;
    uint32_t i, totalEntr = 0, currEntr;
    uint32_t *h_ptr, *ec_ptr;
    uint32_t *eachRoscEntr_ptr = rndWorkBuff_ptr + CC_RND_WORK_BUFF_TMP2_OFFSET;


    /* Initialization */

    h_ptr  = rndWorkBuff_ptr + CC_RND_H_BUFF_OFFSET;
    ec_ptr = rndWorkBuff_ptr + CC_RND_EC_BUFF_OFFSET;

    /* estimate entropy for given blocks (ROSCs) */
    for (i = 0; i < countBlocks; i++) {

        /* Zeroe working buffer for entr. estimator */
        CC_PalMemSetZero(h_ptr, H_BUFF_SIZE_WORDS*4);
        CC_PalMemSetZero(ec_ptr, EC_BUFF_SIZE_WORDS*4);

        LLF_RND_HistogramUpdate(
                       h_ptr, ec_ptr,
                       ramAddr + i*blockSizeWords,
                       blockSizeWords);

        error = LLF_RND_EntropyEstimate(
                           h_ptr, ec_ptr,
                           &currEntr);   /* out - result Entropy size */

        if (error)
            goto End;

        /*total entropy and separate ROSCs entropy*/
        totalEntr += currEntr;
        eachRoscEntr_ptr[i] = currEntr;
    }

    /* entropy correction: down ~1.5% */
    totalEntr  -= totalEntr >> 6;

    *entrSize_ptr = totalEntr;

    End:
    return error;
}

/****************************************************************************************/
/***********************      Auxiliary Functions              **************************/
/****************************************************************************************/


/************************************************************************************/
/*!
 * Busy wait upon RNG Interrupt signals.
 *
 * This function waits RNG interrupt and then disables RNG source.
 * It uses CC_HalWaitInterrupt function
 * to receive common RNG interrupt and then reads and
 * outputs the RNG ISR (status) register.
 *
 *
 * \return uint32_t RNG Interrupt status.
 */
CCError_t LLF_RND_WaitRngInterrupt(uint32_t *isr_ptr)
{
    uint32_t tmp = 0;
    CCError_t error = CC_OK;
    /* busy wait upon RNG IRR signals */
    CC_REG_FLD_SET(HOST_RGF, HOST_IRR, RNG_INT, tmp, 1);
    /* wait for watermark signal */
    error = CC_HalWaitInterruptRND(tmp);
    if (error != CC_OK){
        /* stop DMA and the RNG source */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(RNG,RNG_DMA_ENABLE), 0);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(RNG, RND_SOURCE_ENABLE), 0);
        return error;
    }
    /* stop DMA and the RNG source */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(RNG,RNG_DMA_ENABLE), 0);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(RNG, RND_SOURCE_ENABLE), 0);

    /* read specific RNG interrupt status */
    *isr_ptr = CC_HAL_READ_REGISTER(CC_REG_OFFSET(RNG, RNG_ISR));

    /* clear RNG interrupt status besides HW errors */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(RNG, RNG_ICR), *isr_ptr);
        /* clear again HOST_IRR, since it must be cleared after RNG_ISR */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_ICR), tmp);
    return error;
}

/*****************************************************************/

CCError_t LLF_RND_GetRoscSampleCnt(uint32_t rosc, CCRndParams_t *pTrngParams)
{
    switch (rosc) {
    case 0x1:
        pTrngParams->SubSamplingRatio = pTrngParams->userParams.SubSamplingRatio1;
        break;
    case 0x2:
        pTrngParams->SubSamplingRatio = pTrngParams->userParams.SubSamplingRatio2;
        break;
    case 0x4:
        pTrngParams->SubSamplingRatio = pTrngParams->userParams.SubSamplingRatio3;
        break;
    case 0x8:
        pTrngParams->SubSamplingRatio = pTrngParams->userParams.SubSamplingRatio4;
        break;
    default:
        return LLF_RND_TRNG_REQUIRED_ROSCS_NOT_ALLOWED_ERROR;
    }

    return CC_OK;
}

/**
 * The function gets next allowed rosc
 *
 * @author reuvenl (9/12/2012)
 *
 * @param trngParams_ptr - a pointer to params structure.
 * @param rosc_ptr - a pointer to previous rosc /in/, and
 *          to next rosc /out/.
 * @param isNext - defines is increment of rosc ID needed or not.
 *             if isNext = TRUE - the function shifts rosc by one bit; Then
 *             the function checks is this rosc allowed, if yes - updates
 *             the rosc, else repeats previous steps. If no roscs allowed -
 *             returns an error.
 *
 *
 * @return CCError_t
 */
CCError_t LLF_RND_GetFastestRosc(
                  CCRndParams_t *trngParams_ptr,
                  uint32_t *rosc_ptr     /*in/out*/)
{
    /* setting rosc */
    do {

        if (*rosc_ptr & trngParams_ptr->RoscsAllowed) {
            return CC_OK;
        } else {
            *rosc_ptr <<= 1;
        }

    }while (*rosc_ptr <= 0x08);

    return LLF_RND_TRNG_REQUIRED_ROSCS_NOT_ALLOWED_ERROR;

}


/**
 * The macros calculates count ROSCs to start, as count of bits "1" in allowed
 * roscToStart parameter.
 *
 * @author reuvenl (9/20/2012)
 *
 * @param roscsAllowed
 * @param roscToStart
 *
 * @return uint32_t
 */
uint32_t LLF_RND_GetCountRoscs(
                       uint32_t roscsAllowed,
                       uint32_t roscToStart)
{
    uint32_t countRoscs = 0;

    roscToStart &= roscsAllowed;
    while (roscToStart) {
        countRoscs += (roscToStart & 1UL);
        roscToStart >>= 1;
    }

    return countRoscs;
}

/****************************************************************************************/
/*****************************       Public Functions      ******************************/
/****************************************************************************************/


/************************************************************************************/
/**
 * @brief The LLF_RND_TurnOffTrng stops the hardware random bits collection
 *        closes RND clocks and releases HW semaphore.
 *
 *
 *
 * @return CCError_t - On success CC_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */
void LLF_RND_TurnOffTrng(void)
{
    /* LOCAL DECLARATIONS */

    uint32_t temp = 0;


    /* FUNCTION LOGIC */

    /* disable the RND source  */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(RNG,RND_SOURCE_ENABLE), LLF_RND_HW_RND_SRC_DISABLE_VAL);

    /* close the Hardware clock */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(RNG,RNG_CLK_ENABLE), LLF_RND_HW_RND_CLK_DISABLE_VAL);

    /* clear RNG interrupts */
    CC_REG_FLD_SET(HOST_RGF, HOST_ICR, RNG_INT_CLEAR, temp, 1);                                               \
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_ICR), temp);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(RNG,RNG_ICR), 0xFFFFFFFF);


    return;

}/* END OF LLF_RND_TurnOffTrng*/


/**
* @brief: The function performs CPRNGT (Continued PRNG Test) according
*         to NIST 900-80 and FIPS (if defined) standards.
*
* @param[in] prev_ptr - The pointer to previous saved generated random
*                       value of size 16 bytes.
* @param[in] buff_ptr - The pointer to generated random buffer.
* @param[in] last_ptr - The pointer to last generated random block
*                       of size 16 bytes used for output last bytes.
* @param[in] countBlocks - The count of generated random blocks, including
*                          the last block. Assumed countBlocks > 0.
*
* @return CCError_t - On success CC_OK is returned, on failure a
*                        value MODULE_* as defined in cc_error.h
*/
CCError_t LLF_RND_RndCprngt(uint8_t            *prev_ptr,        /*in*/
                  uint8_t            *buff_ptr,        /*in*/
                  uint8_t            *last_ptr,        /*in*/
                  int32_t             countBlocks)   /*in*/
{
        /* LOCAL DECLARATIONS */

        CCError_t error = CC_OK;
        int32_t  i;

        /*  FUNCTION LOGIC */

    /* compare the previous Value and last block */
    if (countBlocks == 1) {
        if (CC_PalMemCmp(prev_ptr, /*prev*/
                   last_ptr,/*last block*/
                   CC_AES_BLOCK_SIZE_IN_BYTES) == 0) {
            error =  CC_RND_CPRNG_TEST_FAIL_ERROR;
            goto End;
        }
    } else { /* countBlocks > 1, compare first and last blocks */
        if (CC_PalMemCmp(prev_ptr,  /*prev*/
                   buff_ptr, /*first block*/
                   CC_AES_BLOCK_SIZE_IN_BYTES) == 0) {
            error =  CC_RND_CPRNG_TEST_FAIL_ERROR;
            goto End;
        }

        if (CC_PalMemCmp(buff_ptr + (countBlocks-2)*CC_AES_BLOCK_SIZE_IN_BYTES, /*prev*/
                   last_ptr,/*last block*/
                   CC_AES_BLOCK_SIZE_IN_BYTES) == 0) {
            error =  CC_RND_CPRNG_TEST_FAIL_ERROR;
            goto End;
        }
    }
    /* compare intermediate blocks */
    if (countBlocks > 2 && error == CC_OK) {
        for (i = 0; i < countBlocks-2; i++) {
            /* compare all current with previous blocks */
            if (CC_PalMemCmp(buff_ptr + i*CC_AES_BLOCK_SIZE_IN_BYTES,
                       buff_ptr + (i+1)*CC_AES_BLOCK_SIZE_IN_BYTES,
                       CC_AES_BLOCK_SIZE_IN_BYTES) == 0) {
                error = CC_RND_CPRNG_TEST_FAIL_ERROR;
                goto End;
            }
        }
    }

        End:


        return error;
}


