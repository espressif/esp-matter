/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#define CC_PAL_LOG_CUR_COMPONENT CC_LOG_MASK_CCLIB

#include "cc_pal_types.h"
#include "cc_pal_log.h"
#include "cc_pal_mem.h"
#include "cc_pal_abort.h"
#include "cc_lib.h"
#include "cc_hal.h"
#include "cc_pal_init.h"
#include "cc_pal_mutex.h"
#include "cc_pal_perf.h"
#include "cc_regs.h"
#include "dx_crys_kernel.h"
#include "dx_rng.h"
#include "dx_reg_common.h"
#include "llf_rnd_trng.h"
#include "cc_rng_plat.h"
#include "dx_id_registers.h"
#include "cc_util_pm.h"
#include "dx_nvm.h"
#include "ctr_drbg.h"
#include "entropy.h"
#include "threading.h"
#include "mbedtls_cc_mng_int.h"
#include "mbedtls_cc_mng.h"
#include "cc_rnd_common.h"
#include "cc_int_general_defs.h"

CC_PalMutex CCSymCryptoMutex;
CC_PalMutex CCAsymCryptoMutex;
CC_PalMutex *pCCRndCryptoMutex;
CC_PalMutex CCApbFilteringRegMutex;
CC_PalMutex CCRndCryptoMutex;

static CCError_t RndStartupTest(
        CCRndWorkBuff_t  *workBuff_ptr/*in/out*/)
{
        /* error identifier definition */
        CCError_t error = CC_OK;
        CCRndState_t   rndState;
        CCRndParams_t  trngParams;

        error = RNG_PLAT_SetUserRngParameters(&trngParams);
        if (error != CC_SUCCESS) {
                return error;
        }

        error = CC_PalMutexLock(pCCRndCryptoMutex, CC_INFINITE);
        if (error != CC_SUCCESS) {
                CC_PalAbort("Fail to acquire mutex\n");
        }

        /* verify that the device is not in fatal error state before activating the PKA engine */
        CC_IS_FATAL_ERR_ON(error);
        if (error == CC_TRUE) {
                error = CC_LIB_RET_RND_INST_ERR;
                goto EndUnlockMutex;
        }

        /* increase CC counter at the beginning of each operation */
        error = CC_IS_WAKE;
        if (error != CC_SUCCESS) {
            CC_PalAbort("Fail to increase PM counter\n");
        }

        /* call on Instantiation mode */
        error = LLF_RND_RunTrngStartupTest(&rndState, &trngParams, (uint32_t*)workBuff_ptr);

        /* decrease CC counter at the end of each operation */
        if (CC_IS_IDLE != CC_SUCCESS) {
            CC_PalAbort("Fail to decrease PM counter\n");
        }

EndUnlockMutex:
        if (CC_PalMutexUnlock(pCCRndCryptoMutex) != CC_SUCCESS) {
                CC_PalAbort("Fail to release mutex\n");
        }
        return error;
}

static CClibRetCode_t InitHukRma(CCRndContext_t *rndContext_ptr)
{
    uint32_t lcsVal = 0;
    uint32_t kdrValues[CC_AES_KDR_MAX_SIZE_WORDS];
    CCError_t error = CC_OK;
    uint32_t i = 0;
    CCRndGenerateVectWorkFunc_t RndGenerateVectFunc = rndContext_ptr->rndGenerateVectFunc;

    mbedtls_mng_lcsGet( &lcsVal );

    if (lcsVal == CC_MNG_LCS_RMA){ /* in case lcs == RMA set the KDR*/
        error = RndGenerateVectFunc((void *)rndContext_ptr->rndState,
                        (unsigned char *)kdrValues, (size_t) CC_AES_KDR_MAX_SIZE_BYTES);
        if (error != CC_OK){
            return CC_LIB_RET_RND_INST_ERR;
        }

        /* set the random value to the KDR register */
        for (i = 0; i < CC_AES_KDR_MAX_SIZE_WORDS; i++){
            CC_HAL_WRITE_REGISTER( DX_HOST_SHADOW_KDR_REG_REG_OFFSET, kdrValues[i] );
        }
    }

    return CC_LIB_RET_OK;
}


static CClibRetCode_t VerifyPidVal(void)
{
    uint32_t pidReg[CC_BSV_PID_SIZE_WORDS] = {0};
    uint32_t pidVal1[CC_BSV_PID_SIZE_WORDS] = {CC_BSV_PID_0_VAL, CC_BSV_PID_1_VAL, CC_BSV_PID_2_VAL, CC_BSV_PID_3_VAL, CC_BSV_PID_4_VAL};
    uint32_t pidVal2[CC_BSV_PID_SIZE_WORDS] = {CC_BSV_PID_0_VAL, CC_BSV_PID_1_VAL, CC_BSV_PID_2_1_VAL, CC_BSV_PID_3_VAL, CC_BSV_PID_4_VAL};

    pidReg[0] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, PERIPHERAL_ID_0));
    pidReg[1] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, PERIPHERAL_ID_1));
    pidReg[2] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, PERIPHERAL_ID_2));
    pidReg[3] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, PERIPHERAL_ID_3));
    pidReg[4] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, PERIPHERAL_ID_4));

    if ((CC_PalMemCmp((uint8_t*)pidVal1, (uint8_t*)pidReg, sizeof(pidVal1)) != 0) &&
            (CC_PalMemCmp((uint8_t*)pidVal2, (uint8_t*)pidReg, sizeof(pidVal2)) != 0)) {
        return CC_LIB_RET_EINVAL_PIDR;
    }

    return CC_LIB_RET_OK;
}

static CClibRetCode_t VerifyCidVal(void)
{
    uint32_t cidReg[CC_BSV_CID_SIZE_WORDS] = {0};
    uint32_t cidVal[CC_BSV_CID_SIZE_WORDS] = {CC_BSV_CID_0_VAL, CC_BSV_CID_1_VAL, CC_BSV_CID_2_VAL, CC_BSV_CID_3_VAL};

    cidReg[0] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, COMPONENT_ID_0));
    cidReg[1] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, COMPONENT_ID_1));
    cidReg[2] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, COMPONENT_ID_2));
    cidReg[3] = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, COMPONENT_ID_3));

    if (CC_PalMemCmp((uint8_t*)cidVal, (uint8_t*)cidReg, sizeof(cidVal)) != 0){
        return CC_LIB_RET_EINVAL_CIDR;
    }

    return CC_LIB_RET_OK;
}


/*!
 * TEE (Trusted Execution Environment) entry point.
 * Init CryptoCell for TEE.
 *
 * @param[in/out] rndContext_ptr  - Pointer to the RND context buffer.
 * @param[in/out] rndWorkBuff_ptr  - Pointer to the RND scratch buffer.
 *
 * \return CClibRetCode_t one of the error codes defined in cc_lib.h
 */
CClibRetCode_t CC_LibInit(CCRndContext_t *rndContext_ptr, CCRndWorkBuff_t  *rndWorkBuff_ptr)
{
    int rc = 0;
    CClibRetCode_t retCode = CC_LIB_RET_OK;
    CCError_t error = CC_OK;
    uint32_t reg = 0;
    uint32_t tempVal = 0;

    /* check parameters */
    if (rndContext_ptr == NULL)
        return CC_LIB_RET_EINVAL_CTX_PTR;
    if (rndWorkBuff_ptr == NULL)
        return CC_LIB_RET_EINVAL_WORK_BUF_PTR;
    if (rndContext_ptr->rndState == NULL)
        return CC_LIB_RET_EINVAL_CTX_PTR;
    if (rndContext_ptr->entropyCtx == NULL)
        return CC_LIB_RET_EINVAL_CTX_PTR;

    rc = CC_HalInit();
    if (rc != CC_LIB_RET_OK) {
        retCode = CC_LIB_RET_HAL;
        goto InitErr1;
    }

    rc = CC_PalInit();
    if (rc != CC_LIB_RET_OK) {
        retCode = CC_LIB_RET_PAL;
        goto InitErr;
    }

    /* verify peripheral ID (PIDR) */
    rc = VerifyPidVal();
    if (rc != CC_LIB_RET_OK) {
        retCode = CC_LIB_RET_EINVAL_PIDR;
        goto InitErr2;
    }

    /* verify component ID (CIDR) */
    rc = VerifyCidVal();
    if (rc != CC_LIB_RET_OK) {
        retCode = CC_LIB_RET_EINVAL_CIDR;
        goto InitErr2;
    }

    /* turn off the DFA since Cerberus doen't support it */
    reg = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_AO_LOCK_BITS));
    CC_REG_FLD_SET(0, HOST_AO_LOCK_BITS, HOST_FORCE_DFA_ENABLE, reg, 0x0);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_AO_LOCK_BITS)  ,reg );
    tempVal = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF,HOST_AO_LOCK_BITS));
    if(tempVal != reg) {
        retCode = CC_LIB_AO_WRITE_FAILED_ERR;
        goto InitErr2;
    }

    CC_REG_FLD_SET(0, HOST_AO_LOCK_BITS, HOST_DFA_ENABLE_LOCK, reg, CC_TRUE);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_AO_LOCK_BITS)  ,reg );
    tempVal = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF,HOST_AO_LOCK_BITS));
    if(tempVal != reg) {
        retCode = CC_LIB_AO_WRITE_FAILED_ERR;
        goto InitErr2;
    }

    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, AES_DFA_IS_ON)  ,0x0UL );

#ifdef BIG__ENDIAN
/* Set DMA endianess to big */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_ENDIAN) , 0xCCUL);
#else /* LITTLE__ENDIAN */
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_ENDIAN) , 0x00UL);
#endif

    CC_PAL_PERF_INIT();

    /* Initialize RND module */
    error = RndStartupTest(rndWorkBuff_ptr);
    if (error != 0) {
        retCode = CC_LIB_RET_RND_INST_ERR;
        goto InitErr2;
    }

    /* Initialize mbedTLS random function*/
    mbedtls_ctr_drbg_init(rndContext_ptr->rndState);
    mbedtls_entropy_init( rndContext_ptr->entropyCtx );
    error = mbedtls_ctr_drbg_seed(rndContext_ptr->rndState, mbedtls_entropy_func, rndContext_ptr->entropyCtx,
            NULL, 0);
    if (error != 0) {
        retCode = CC_LIB_RET_RND_INST_ERR;
        goto InitErr2;
    }

    error = CC_RndSetGenerateVectorFunc(rndContext_ptr, mbedtls_ctr_drbg_random);
    if (error != 0) {
        retCode = CC_LIB_RET_RND_INST_ERR;
        goto InitErr2;
    }
    error = InitHukRma(rndContext_ptr);
    if (error != 0) {
        retCode = CC_LIB_RET_RND_INST_ERR;
        goto InitErr2;
    }
    return CC_LIB_RET_OK;
    InitErr2:
    CC_HalTerminate();

    InitErr1:
    CC_PalTerminate();

    InitErr:
    return retCode;
}


/*!
 * TEE (Trusted Execution Environment) exit point.
 * Finalize CryptoCell for TEE operation, release associated resources.
 *                                                                    .
 * @param[in/out] rndContext_ptr  - Pointer to the RND context buffer.
 */
CClibRetCode_t CC_LibFini(CCRndContext_t *rndContext_ptr)
{
    CCError_t rc = CC_OK;
    CClibRetCode_t retCode = CC_LIB_RET_OK;

    /* check parameters */
    if (rndContext_ptr == NULL)
        return CC_LIB_RET_EINVAL_CTX_PTR;

    rc = CC_HalTerminate();
    if (rc != 0){
        retCode = CC_LIB_RET_HAL;
    }
    CC_PalTerminate();

    rndContext_ptr->rndGenerateVectFunc=NULL;
    mbedtls_ctr_drbg_free( rndContext_ptr->rndState );
    mbedtls_entropy_free( rndContext_ptr->entropyCtx );

    CC_PAL_PERF_FIN();

    return retCode;

}

void __cyg_profile_func_enter (void *this_fn, void *call_site) {
    unsigned int i;
    CC_UNUSED_PARAM(i);
    CC_UNUSED_PARAM(this_fn);
    CC_UNUSED_PARAM(call_site);

    CC_PAL_LOG_ERR("Entering: %p -> %p (stack: %p)\n",
                    call_site, this_fn, &i );
}

void __cyg_profile_func_exit (void *this_fn, void *call_site) {
    unsigned int i;
    CC_UNUSED_PARAM(i);
    CC_UNUSED_PARAM(this_fn);
    CC_UNUSED_PARAM(call_site);

    CC_PAL_LOG_ERR("Exiting: %p <- %p (stack: %p)\n",
                     call_site, this_fn, &i );
}

