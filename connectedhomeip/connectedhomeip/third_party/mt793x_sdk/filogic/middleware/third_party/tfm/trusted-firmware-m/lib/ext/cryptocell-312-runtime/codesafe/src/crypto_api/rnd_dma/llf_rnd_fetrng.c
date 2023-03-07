/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/************* Include Files ****************/
#include "dx_rng.h"
#include "cc_pal_mem.h"
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
#include "cc_plat.h"
#include "llf_rnd_trng.h"
#include "cc_int_general_defs.h"
#ifndef CMPU_UTIL
#include "cc_pal_mutex.h"
#include "cc_pal_abort.h"
#include "cc_util_pm.h"
#endif
#define ROSC_INIT_START_BIT   0x80000000

#if !defined(CMPU_UTIL) && !defined(SC_TEST_MODE)
extern CC_PalMutex *pCCRndCryptoMutex;

#define MUTEX_LOCK_AND_RETURN_UPON_ERROR(pmutex) \
        if (CC_PalMutexLock(pmutex, CC_INFINITE) != CC_SUCCESS) { \
            CC_PalAbort("Fail to acquire mutex\n"); \
        }

#define MUTEX_UNLOCK(pmutex) \
        if (CC_PalMutexUnlock(pmutex) != CC_SUCCESS) { \
            CC_PalAbort("Fail to release mutex\n"); \
        }

#define DECREASE_CC_COUNTER \
        if (CC_IS_IDLE != CC_SUCCESS) { \
            CC_PalAbort("Fail to decrease PM counter\n"); \
        }

#define INCREASE_CC_COUNTER \
        if (CC_IS_WAKE != CC_SUCCESS) { \
            CC_PalAbort("Fail to increase PM counter\n"); \
        }

#else
#define MUTEX_LOCK_AND_RETURN_UPON_ERROR(mutex)
#define MUTEX_UNLOCK(mutex)
#define DECREASE_CC_COUNTER
#define INCREASE_CC_COUNTER
#endif


/*********************************** Enums ******************************/
/*********************************Typedefs ******************************/

/**************** Global Data to be read by RNG function ****************/

/* test variables */
#ifdef RND_TEST_TRNG_WITH_ESTIMATOR
uint32_t  gEntrSize[4];
#endif


/************************************************************************************/
/**
 * The function checks that parameters, loaded in the TRNG HW
 * are match to parameters, required by trngParams_ptr structures.
 *
 * @author reuvenl (6/25/2012)
 *
 * @param trngParams_ptr
 *
 * @return CCError_t
 */
static CCError_t LLF_RND_TRNG_CheckHwParams(CCRndParams_t *trngParams_ptr)
{
    uint32_t temp;
    CCBool_t isTrue = CC_TRUE;

    /* check Debug control - masked TRNG tests according to mode */
    temp = CC_HAL_READ_REGISTER(CC_REG_OFFSET(RNG, TRNG_DEBUG_CONTROL));
    isTrue &= (temp == LLF_RND_HW_DEBUG_CONTROL_VALUE_ON_FE_MODE);
    /* check samplesCount */
    temp = CC_HAL_READ_REGISTER(CC_REG_OFFSET(RNG,SAMPLE_CNT1));
    isTrue &= (temp == trngParams_ptr->SubSamplingRatio);

    /* if any parameters are not match return an Error */
        if (isTrue == CC_FALSE) {
                return LLF_RND_TRNG_PREVIOUS_PARAMS_NOT_MATCH_ERROR;
        }
        else {
                return CC_OK;
        }
}

static uint32_t LLF_RND_TRNG_RoscMaskToNum(uint32_t mask)
{
        return (mask == LLF_RND_HW_TRNG_ROSC3_BIT) ? LLF_RND_HW_TRNG_ROSC3_NUM :
                (mask == LLF_RND_HW_TRNG_ROSC2_BIT) ? LLF_RND_HW_TRNG_ROSC2_NUM :
                (mask == LLF_RND_HW_TRNG_ROSC1_BIT) ? LLF_RND_HW_TRNG_ROSC1_NUM :
                LLF_RND_HW_TRNG_ROSC0_NUM;
}

static void LLF_RND_TRNG_EnableRngSourceAndWatchdog(CCRndParams_t *trngParams_ptr)
{
        uint32_t maxCycles;
        uint32_t ehrSamples;

        /* set EHR samples = 2 /384 bit/ for both AES128 and AES256 */
        ehrSamples = LLF_RND_HW_DMA_EHR_SAMPLES_NUM_ON_FE_MODE;

        /* Set watchdog threshold to maximal allowed time (in CPU cycles) */
        maxCycles = LLF_RND_CalcMaxTrngTime(ehrSamples, trngParams_ptr->SubSamplingRatio);
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(RNG, RNG_WATCHDOG_VAL), maxCycles);

        /* enable the RND source */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(RNG, RND_SOURCE_ENABLE), LLF_RND_HW_RND_SRC_ENABLE_VAL);
}

static CCError_t LLF_RND_TRNG_ReadEhrData(uint32_t *pSourceOut, bool isFipsSupported)
{
        CCError_t error = CC_OK;
        uint32_t isr = 0;
        uint32_t i;



        /* wait RNG interrupt: isr signals error bits */
        error = LLF_RND_WaitRngInterrupt(&isr);
        if (error != CC_OK){
                return error;
        }

        error = LLF_RND_TRNG_REQUIRED_ROSCS_NOT_ALLOWED_ERROR;
        if (CC_REG_FLD_GET(0, RNG_ISR, EHR_VALID, isr)) {
                error = CC_OK;
        }
        if (CC_REG_FLD_GET(0, RNG_ISR, CRNGT_ERR, isr) && isFipsSupported) {
                /* CRNGT requirements for FIPS 140-2. Should not try the next ROSC in FIPS mode. */
                error = LLF_RND_CRNGT_TEST_FAIL_ERROR;
        }

        /* in case of AUTOCORR_ERR or RNG_WATCHDOG, keep the default error value. will try the next ROSC. */

        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(RNG, RNG_ICR), 0xFFFFFFFF);

        if (error == CC_OK) {
                for (i = 0; i < LLF_RND_HW_TRNG_EHR_WIDTH_IN_WORDS; i++)
                {
                        /* load the current random data to the output buffer */
                        *(pSourceOut++) = CC_HAL_READ_REGISTER(DX_EHR_DATA_0_REG_OFFSET + (i*sizeof(uint32_t)));
                }
                CC_HAL_READ_REGISTER(CC_REG_OFFSET(RNG, RNG_ISR));
        }

        return error;
}

/****************************************************************************************/
/*****************************       Public Functions      ******************************/
/****************************************************************************************/


CCError_t LLF_RND_StartTrngHW(
        CCRndState_t  *rndState_ptr,
        CCRndParams_t *trngParams_ptr,
        CCBool_t           isRestart,
        uint32_t         *roscsToStart_ptr)
{
        /* LOCAL DECLARATIONS */

        CCError_t error = CC_OK;
        uint32_t tmpSamplCnt = 0;
        uint32_t roscNum = 0;
#ifdef CC_IOT
        uint32_t mask = 0;
#endif
        /* FUNCTION LOGIC */

        /* Check pointers */
        if ((rndState_ptr == NULL) || (trngParams_ptr == NULL) ||
                (roscsToStart_ptr == NULL))
                return LLF_RND_TRNG_ILLEGAL_PTR_ERROR;


        /*--------------------------------------------------------------*/
        /* 1. If full restart, get semaphore and set initial ROSCs      */
        /*--------------------------------------------------------------*/
        if (isRestart) {
                /* set ROSC to 1 (fastest)  */
                *roscsToStart_ptr = 1UL;

                /* init rndState flags to zero */
                rndState_ptr->TrngProcesState = 0;
        }


        if (*roscsToStart_ptr == 0)
                return LLF_RND_TRNG_REQUIRED_ROSCS_NOT_ALLOWED_ERROR;

        /* FE mode  */
        /* Get fastest allowed ROSC */
        error = LLF_RND_GetFastestRosc(
                trngParams_ptr,
                roscsToStart_ptr     /*in/out*/);
        if (error)
                return error;

        error = LLF_RND_GetRoscSampleCnt(*roscsToStart_ptr, trngParams_ptr);
        if (error)
                return error;

        roscNum = LLF_RND_TRNG_RoscMaskToNum(*roscsToStart_ptr);

        /*--------------------------------------------------------------*/
        /* 2. Restart the TRNG and set parameters                   */
        /*--------------------------------------------------------------*/
        /* RNG Block HW Specification (10 Programming Reference)        */

        /* enable the HW RND clock   */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(RNG, RNG_CLK_ENABLE), LLF_RND_HW_RND_CLK_ENABLE_VAL);

        /* do software reset */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(RNG, RNG_SW_RESET), 0x1);
        /* in order to verify that the reset has completed the sample count need to be verified */
        do {
                /* enable the HW RND clock   */
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(RNG, RNG_CLK_ENABLE), LLF_RND_HW_RND_CLK_ENABLE_VAL);

                /* set sampling ratio (rng_clocks) between consecutive bits */
                CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(RNG, SAMPLE_CNT1), trngParams_ptr->SubSamplingRatio);

                /* read the sampling ratio  */
                tmpSamplCnt = CC_HAL_READ_REGISTER(CC_REG_OFFSET(RNG, SAMPLE_CNT1));

        } while (tmpSamplCnt != trngParams_ptr->SubSamplingRatio);


        /* disable the RND source for setting new parameters in HW */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(RNG, RND_SOURCE_ENABLE), LLF_RND_HW_RND_SRC_DISABLE_VAL);

        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(RNG, RNG_ICR), 0xFFFFFFFF);

        /* set interrupt mask */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(RNG, RNG_IMR), LLF_RNG_INT_MASK_ON_FETRNG_MODE);

        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(RNG, TRNG_CONFIG), roscNum);

#ifdef CC_IOT
        /* mask RNG_INT - this interrupt should be polled upon,
         * and not handled by the operating system's interrupt handler. */
        mask = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_IMR));
        CC_REG_FLD_SET(HOST_RGF, HOST_IMR, RNG_INT_MASK, mask, 1);
        CC_HalMaskInterrupt(mask);
#endif
        /* Debug Control register: set to 0 - no bypasses */
        CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(RNG, TRNG_DEBUG_CONTROL), LLF_RND_HW_DEBUG_CONTROL_VALUE_ON_FE_MODE);

        LLF_RND_TRNG_EnableRngSourceAndWatchdog(trngParams_ptr);

        /* set indication about current started ROSCs:  */
        /*new started*/
        rndState_ptr->TrngProcesState = (rndState_ptr->TrngProcesState & 0x00ffffff) | (*roscsToStart_ptr << 24);
        /*total started*/
        rndState_ptr->TrngProcesState |= (*roscsToStart_ptr << 8);

        return error;
}


CCError_t LLF_RND_GetTrngSource(
        CCRndState_t  *rndState_ptr,        /*in/out*/
        CCRndParams_t  *trngParams_ptr,     /*in/out*/
        CCBool_t            isContinued,    /*in*/
        uint32_t         *entropySize_ptr,      /*in/out*/
        uint32_t        **sourceOut_ptr_ptr,    /*out*/
        uint32_t         *sourceOutSize_ptr,    /*in/out*/
        uint32_t         *rndWorkBuff_ptr,      /*in*/
        bool              isFipsSupported)      /*in*/
{
        /* LOCAL DECLARATIONS */

        /* The return error identifier */
        CCError_t error = 0;
        int32_t  i;
#ifndef USE_MBEDTLS_CRYPTOCELL
        uint32_t tmp;
#endif
        uint32_t roscToStart;
        uint32_t *ramAddr;
        uint32_t trngBuff[LLF_RND_HW_DMA_EHR_SAMPLES_NUM_ON_FE_MODE * LLF_RND_HW_TRNG_EHR_WIDTH_IN_WORDS] = { 0 }; /* 2 EHR required */
        CC_UNUSED_PARAM(entropySize_ptr);

        /* Lock mutex, check fatal err, and increase cc counter*/
        MUTEX_LOCK_AND_RETURN_UPON_ERROR(pCCRndCryptoMutex);

        CC_IS_FATAL_ERR_ON(error);
        if (error == CC_TRUE) {
                error = LLF_RND_TRNG_GENERATION_NOT_COMPLETED_ERROR;
                goto EndUnlockMutex;
        }

        INCREASE_CC_COUNTER
        /* FUNCTION LOGIC */

        /* ............... local initializations .............................. */
        /* -------------------------------------------------------------------- */

        /* initializing the Error to O.K */
        error = CC_OK;

        /* Set source RAM address with offset 8 bytes from sourceOut address in
          order to remain empty bytes for CC operations */
        *sourceOut_ptr_ptr = rndWorkBuff_ptr;
        ramAddr = *sourceOut_ptr_ptr + CC_RND_TRNG_SRC_INNER_OFFSET_WORDS;
        /* init to 0 for FE mode */
        *sourceOutSize_ptr = 0;
#ifndef USE_MBEDTLS_CRYPTOCELL
        /* Case of RND KAT or TRNG KAT testing  */
        if ((rndState_ptr->StateFlag & CC_RND_KAT_DRBG_Mode) ||
                (rndState_ptr->StateFlag & CC_RND_KAT_TRNG_Mode)) {
                /* set source sizes given by the user in KAT test and placed
                   in the rndWorkBuff with offset CC_RND_SRC_BUFF_OFFSET_WORDS */
                *sourceOutSize_ptr = (*sourceOut_ptr_ptr)[0];
                if (*sourceOutSize_ptr == 0) {
                        error = CC_RND_KAT_DATA_PARAMS_ERROR;
                        goto End;
                }

                /* Go to Estimator */
                if (rndState_ptr->StateFlag & CC_RND_KAT_TRNG_Mode) {
                        /* Assumed, that KAT data is set in the rnd Work      *
                           buffer as follows:                     *
                           - full source size set in buffer[0],           *
                           - count blocks set in buffer[1],               *
                           *  - KAT source begins from buffer[2].         */
                        tmp = (*sourceOut_ptr_ptr)[1]; /*count blocks for estimation*/
                        if (tmp == 0) {
                                error = CC_RND_KAT_DATA_PARAMS_ERROR;
                                goto End;
                        }
                        error = CC_RND_TRNG_KAT_NOT_SUPPORTED_ERROR;
                        goto End;
                        //goto Estimator;
                }
                else {
                        goto End;
                }
        }
#endif
        /* If not continued mode, set TRNG parameters and restart TRNG  */
        /*--------------------------------------------------------------*/
        if (isContinued == CC_FALSE) {
#ifndef USE_MBEDTLS_CRYPTOCELL
                /* Set instantiation, TRNG errors and time   *
                * exceeding bits of State to 0           */
                rndState_ptr->StateFlag &= ~(CC_RND_INSTANTIATED |
                        CC_RND_INSTANTRESEED_AUTOCORR_ERRORS |
                        CC_RND_INSTANTRESEED_TIME_EXCEED |
                        CC_RND_INSTANTRESEED_LESS_ENTROPY);
#endif
                /* Full restart TRNG */
                error = LLF_RND_StartTrngHW(
                        rndState_ptr,
                        trngParams_ptr,
                        CC_TRUE/*isRestart*/,
                        &roscToStart);

                /*Note: in case of error the TRNG HW is still not started*/
                if (error) {
                        goto End;
                }
        }
        /* On continued mode check HW TRNG */
        else {
                /* check TRNG parameters */
                error = LLF_RND_TRNG_CheckHwParams(trngParams_ptr);
                if (error != CC_OK)
                        goto End;

                /* previously started ROSCs */
                roscToStart = (rndState_ptr->TrngProcesState & 0xff000000) >> 24;
        }

        /*====================================================*/
        /*====================================================*/
        /*         Processing after previous start            */
        /*====================================================*/
        /*====================================================*/

        /*====================================================*/
        /* FE mode processing: start Roscs sequentially -   *
        * from fast to slow Rosc                  */
        /*====================================================*/

        for (i = 0; i < LLF_RND_NUM_OF_ROSCS; ++i) {

                /* read the first EHR */
                error = LLF_RND_TRNG_ReadEhrData(trngBuff, isFipsSupported);
                if (error == CC_OK) {
                        /* read the second EHR */
                        LLF_RND_TRNG_EnableRngSourceAndWatchdog(trngParams_ptr);
                        error = LLF_RND_TRNG_ReadEhrData(trngBuff + LLF_RND_HW_TRNG_EHR_WIDTH_IN_WORDS, isFipsSupported);
                        if (error == CC_OK) {
                                break;
                        }
                }
                if (error == LLF_RND_CRNGT_TEST_FAIL_ERROR) {
                        /* LLF_RND_CRNGT_TEST_FAIL_ERROR is set only in FIPS mode. do not continue to the next rosc. */
                        break;
                }
                if (error != CC_OK) {/* try next rosc */
                        /*  if no remain roscs to start, return error */
                        if (roscToStart == 0x8) {
                                error = LLF_RND_TRNG_GENERATION_NOT_COMPLETED_ERROR;
                                break;
                        }
                        else {
                                /* Call StartTrng, with next ROSC */
                                roscToStart <<= 1;
                                error = LLF_RND_StartTrngHW(
                                        rndState_ptr,
                                        trngParams_ptr,
                                        CC_FALSE/*isRestart*/,
                                        &roscToStart);

                                /* if no remain valid roscs, return error */
                                if (error == LLF_RND_TRNG_REQUIRED_ROSCS_NOT_ALLOWED_ERROR && (trngParams_ptr->RoscsAllowed != 0)) {

                                        error = LLF_RND_TRNG_GENERATION_NOT_COMPLETED_ERROR;
                                }

                                if (error != CC_OK) {
                                        goto End;
                                }
                        }
                }


                /* update total processed ROSCs */
                rndState_ptr->TrngProcesState |= ((rndState_ptr->TrngProcesState >> 8) & 0x00FF0000);
                /*clean started & not processed*/
                rndState_ptr->TrngProcesState &= 0x00FFFFFF;

        }

        if (error == CC_OK) {
                CC_PalMemCopy(ramAddr, trngBuff, LLF_RND_HW_DMA_EHR_SAMPLES_NUM_ON_FE_MODE * LLF_RND_HW_TRNG_EHR_WIDTH_IN_BYTES);
                *sourceOutSize_ptr = LLF_RND_HW_DMA_EHR_SAMPLES_NUM_ON_FE_MODE * LLF_RND_HW_TRNG_EHR_WIDTH_IN_BYTES;
        }

        /* end FE mode */

        /* ................. end of function ..................................... */
        /* ----------------------------------------------------------------------- */
End:

        /* turn the RNG off    */
#ifndef USE_MBEDTLS_CRYPTOCELL

        if ((rndState_ptr->StateFlag & CC_RND_KAT_TRNG_Mode) == 0) {
                LLF_RND_TurnOffTrng();
        }
#else
        LLF_RND_TurnOffTrng();

#endif

        /* release mutex and decrease CC counter */
        DECREASE_CC_COUNTER

EndUnlockMutex:
        MUTEX_UNLOCK(pCCRndCryptoMutex);


        return error;


}/* END of LLF_RND_GetTrngSource */

CCError_t LLF_RND_RunTrngStartupTest(
        CCRndState_t        *rndState_ptr,
        CCRndParams_t       *trngParams_ptr,
        uint32_t                *rndWorkBuff_ptr)
{
    CCError_t error = CC_OK;
    CC_UNUSED_PARAM(rndState_ptr);
        CC_UNUSED_PARAM(trngParams_ptr);
        CC_UNUSED_PARAM(rndWorkBuff_ptr);

    return error;
}



