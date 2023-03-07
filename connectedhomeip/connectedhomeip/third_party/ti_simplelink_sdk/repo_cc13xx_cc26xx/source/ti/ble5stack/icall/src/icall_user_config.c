/******************************************************************************

 @file  icall_user_config.c

 @brief This file contains generic user configurable variables for icall thread.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2016-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
#ifdef ICALL_JT

#include "hal_types.h"
#include <ti/drivers/rf/RF.h>
#include "icall_user_config.h"
#include "icall.h"

#include <ti/drivers/AESCCM.h>
#include <ti/drivers/aesccm/AESCCMCC26XX.h>
#include <ti/drivers/AESECB.h>
#include <ti/drivers/aesecb/AESECBCC26XX.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
#ifndef FREERTOS
#include <ti/sysbios/knl/Swi.h>
#endif
#include <ti_drivers_config.h>
#include <ti/drivers/ECDH.h>

#if !defined(DeviceFamily_CC26X1)
#include <ti/drivers/ecdh/ECDHCC26X2.h>
#else
#include <ti/drivers/ecdh/ECDHCC26X1.h>
#endif

#include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
#include <ti/drivers/cryptoutils/sharedresources/CryptoResourceCC26XX.h>
#include <ti/drivers/utils/Random.h>
#include <ti/drivers/TRNG.h>

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */
#ifdef EXT_HAL_ASSERT
  #define ASSERT_CBACK                  &halAssertCback
#else // !EXT_HAL_ASSERT
  #define ASSERT_CBACK                  &appAssertCback
#endif // EXT_HAL_ASSERT

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

// RF Driver API Table
// This table is populated in the same order as the API are listed in file rf_api.h
// any change of order, or any API added/substracted, should be reflected in icall_jt.h
rfDrvTblPtr_t rfDriverTable[] =
{
  (uint32) RF_open,
  (uint32) RF_close,
  (uint32) RF_postCmd,
  (uint32) RF_pendCmd,
  (uint32) RF_runCmd,
  (uint32) RF_cancelCmd,
  (uint32) RF_flushCmd,
  (uint32) RF_yield,
  (uint32) RF_Params_init,
  (uint32) RF_runImmediateCmd,
  (uint32) RF_runDirectCmd,
  (uint32) RF_ratCompare,
  (uint32) RF_ratCapture,
  (uint32) RF_ratDisableChannel,
  (uint32) RF_getCurrentTime,
  (uint32) RF_getRssi,
  (uint32) RF_getInfo,
  (uint32) RF_getCmdOp,
  (uint32) RF_control,
  (uint32) RF_scheduleCmd,
  (uint32) RF_runScheduleCmd,
  (uint32) RF_requestAccess,
  (uint32) RF_getTxPower,
  (uint32) RF_setTxPower,
  (uint32) RF_TxPowerTable_findPowerLevel,
  (uint32) RF_TxPowerTable_findValue
};

// ECC Driver API Table
eccDrvTblPtr_t eccDriverTable[] =
{
  (uint32) ECDH_init,
  (uint32) ECDH_Params_init,
  (uint32) ECDH_open,
  (uint32) ECDH_close,
  (uint32) ECDH_OperationGeneratePublicKey_init,
  (uint32) ECDH_OperationComputeSharedSecret_init,
  (uint32) ECDH_generatePublicKey,
  (uint32) ECDH_computeSharedSecret
};

// Crypto Driver API Table
cryptoDrvTblPtr_t cryptoDriverTable[] =
{
  (uint32)AESCCM_init,
  (uint32)AESCCM_open,
  (uint32)AESCCM_close,
  (uint32)AESCCM_Params_init,
  (uint32)AESCCM_Operation_init,
  (uint32)AESCCM_oneStepEncrypt,
  (uint32)AESCCM_oneStepDecrypt,
  (uint32)AESECB_init,
  (uint32)AESECB_open,
  (uint32)AESECB_close,
  (uint32)AESECB_Params_init,
  (uint32)AESECB_Operation_init,
  (uint32)AESECB_oneStepEncrypt,
  (uint32)AESECB_oneStepDecrypt,
  (uint32)CryptoKeyPlaintext_initKey,
  (uint32)CryptoKeyPlaintext_initBlankKey
};

// True Random Generator API table
trngDrvTblPtr_t trngDriverTable[] =
{
  (uint32) TRNG_init,
  (uint32) TRNG_open,
  (uint32) TRNG_generateEntropy,
  (uint32) TRNG_close
};

// nvintf NV API function pointer table
// Populated at runtime
NVINTF_nvFuncts_t nvintfFncStruct = {0};

// This table is populated in the same order as the API are listed in file icall_jt.h
// any change of order, or any API added/substracted, should be reflected in icall_jt.h
const icallServiceTblPtr_t icallServiceTable[] =
{
  (uint32_t) ICall_send,
  (uint32_t) ICall_enrollService,
  (uint32_t) ICall_abort,
  (uint32_t) ICall_setTimerMSecs,
  (uint32_t) ICall_setTimer,
  (uint32_t) ICall_wait,
  (uint32_t) ICall_getTicks,
  (uint32_t) ICall_fetchMsg,
  (uint32_t) ICall_stopTimer,
  (uint32_t) ICall_signal,
  (uint32_t) ICall_pwrDispense,
  (uint32_t) ICall_pwrRequire,
  (uint32_t) ICall_pwrUpdActivityCounter,
  (uint32_t) ICall_malloc,
  (uint32_t) ICall_free,
  (uint32_t) ICall_sendServiceComplete,
  (uint32_t) ICall_getHeapStats,
  (uint32_t) ICall_mallocLimited,
};

// this table needs to be field by the application , so it cannot be store in flash.
applicationService_t bleAppServiceInfoTable =
{
  .timerTickPeriod     = 0,               // timerTick_period, This need to be filled at runtime, or the stack will assert an error.
  .timerMaxMillisecond = 0,               // timerMaxMillisecond. This need to be filled at runtime, or the stack will assert an error.
  .assertCback         = ASSERT_CBACK,
  .icallServiceTbl     = icallServiceTable,
};

#endif /* ICALL_JT */

/*******************************************************************************
 */
