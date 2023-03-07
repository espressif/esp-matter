/******************************************************************************

 @file  ble_user_config_stack.c

 @brief This file contains user configurable variables for the BLE
        Controller and Host.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2014-2022, Texas Instruments Incorporated
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

#include "hal_types.h"
#include "ll_common.h"
#include "osal.h"
#include "ll_user_config.h"
#include "ble_user_config.h"
#include "ti_radio_config.h"

#if defined( HOST_CONFIG ) && ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )

#include "ble_dispatch.h"
#include "l2cap.h"

#endif // ( CENTRAL_CFG | PERIPHERAL_CFG )


#if (defined RTLS_CTE || defined USE_PERIODIC_ADV || defined USE_PERIODIC_SCAN)
#include DeviceFamily_constructPath(rf_patches/rf_patch_cpe_multi_protocol_rtls.h)
#elif defined (USE_DMM)
#include DeviceFamily_constructPath(rf_patches/rf_patch_cpe_multi_protocol.h)
#endif

/*******************************************************************************
 * GLOBAL VARIABLES
 */
#ifdef ICALL_JT
uint32_t * icallServiceTblPtr = NULL;
#endif /* ICALL_JT */

// The BLE stack is using "RF_BLE_txPowerTable" that is generated via the SysConfig tool
// and can be found in "ti_radio_config.c".
// The RF_TxPowerTable_Entry struct type is incompetiable with the txPwrVal_t struct type,
// which is the type that the controller expects to get.
// The txPwrVal_t and txPwrTbl_t structs are const, in order to pass the controller a competiable
// struct type, the following new non const struct types are defined.
#if !defined(CC13X2P)
PACKED_TYPEDEF_STRUCT
{
  int8   Pout;
  uint16 txPwrVal;
} txPwrVal_non_const_t;

PACKED_TYPEDEF_STRUCT
{
  txPwrVal_t *txPwrValsPtr;
  uint8       numTxPwrVals;
  int8        defaultTxPwrVal;
} txPwrTbl_non_const_t;
#endif //!defined(CC13X2P)
/*******************************************************************************
 * FUNCTIONS
 */

/*******************************************************************************
 * @fn      setBleUserConfig
 *
 * @brief   Set the user configurable variables for the BLE
 *          Controller and Host.
 *
 *          Note: This function should be called at the start
 *                of stack_main.
 *
 * @param   userCfg - pointer to user configuration
 *
 * @return  none
 */
#ifdef ICALL_JT
void setBleUserConfig( icall_userCfg_t *userCfg )
{
  if ( userCfg != NULL )
  {
    stackSpecific_t *stackConfig = (stackSpecific_t*) userCfg->stackConfig;

#if defined( HOST_CONFIG ) && ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )
    l2capUserCfg_t l2capUserCfg;

    // user reconfiguration of Host variables
    l2capUserCfg.maxNumPSM = stackConfig->maxNumPSM;
    l2capUserCfg.maxNumCoChannels = stackConfig->maxNumCoChannels;

    L2CAP_SetUserConfig( &l2capUserCfg );

#endif // ( CENTRAL_CFG | PERIPHERAL_CFG )
    if ( stackConfig->pfnBMAlloc != NULL )
    {
      *stackConfig->pfnBMAlloc = bleDispatch_BMAlloc;
    }

    if ( stackConfig->pfnBMFree != NULL )
    {
      *stackConfig->pfnBMFree = bleDispatch_BMFree;
    }

    // user reconfiguration of Controller variables
    llUserConfig.maxNumConns   = stackConfig->maxNumConns;
    llUserConfig.numTxEntries  = stackConfig->maxNumPDUs;
    llUserConfig_maxPduSize    = bleUserCfg_maxPduSize;
    llUserConfig.maxWlElems    = stackConfig->maxWhiteListElems;
    llUserConfig.maxRlElems    = stackConfig->maxResolvListElems;
    llUserConfig.maxNumCteBufs = stackConfig->maxNumCteBuffers;

    // Copy the RF_mode Object
    memcpy(&rfMode, &RF_modeBle, sizeof(RF_Mode));

    // Use the rtls cpe patch when RTLS is used
#if (defined RTLS_CTE || defined USE_PERIODIC_ADV || defined USE_PERIODIC_SCAN)
    rfMode.cpePatchFxn = &rf_patch_cpe_multi_protocol_rtls;
    // Use the multi_protocol cpe patch when DMM is used
#elif USE_DMM
    rfMode.cpePatchFxn = &rf_patch_cpe_multi_protocol;
#endif

    // RF Front End Mode and Bias (based on package)
    llUserConfig.rfFeModeBias  = userCfg->boardConfig->rfFeModeBias;

    // Privacy Override Offset
    llUserConfig.privOverrideOffset  = userCfg->boardConfig->privOverrideOffset;

    // RF Override Registers
    llUserConfig.rfRegPtr      = userCfg->boardConfig->rfRegTbl;
    llUserConfig.rfReg1MPtr    = userCfg->boardConfig->rfRegTbl1M;
#if defined(BLE_V50_FEATURES) && (BLE_V50_FEATURES & (PHY_2MBPS_CFG | PHY_LR_CFG))
    llUserConfig.rfReg2MPtr    = userCfg->boardConfig->rfRegTbl2M;
    llUserConfig.rfRegCodedPtr = userCfg->boardConfig->rfRegTblCoded;
#endif // PHY_2MBPS_CFG | PHY_LR_CFG

#if defined(CC13X2P)
    llUserConfig.rfRegOverrideTxStdPtr   = userCfg->boardConfig->rfRegOverrideTxStdTblptr;  // Default PA
    llUserConfig.rfRegOverrideTx20Ptr    = userCfg->boardConfig->rfRegOverrideTx20TblPtr;   // High power PA
    llUserConfig.txPwrBackoffTblPtr      = userCfg->boardConfig->txPwrBackoffTbl;           // Tx power Backoff table
#endif //CC13X2P

    llUserConfig.cteAntProp          = userCfg->boardConfig->cteAntennaPropPtr;     // CTE antenna properties
    llUserConfig.rfRegOverrideCtePtr = userCfg->boardConfig->rfRegOverrideCtePtr;   // CTE overrides
    llUserConfig.coexUseCaseConfig   = userCfg->boardConfig->coexUseCaseConfigPtr;  // Coex Configuration

// The BLE stack is using "RF_BLE_txPowerTable" that is generated via the SysConfig tool
// and can be found in "ti_radio_config.c".
// The RF_TxPowerTable_Entry struct type is incompetiable with the txPwrVal_t struct type, which is the type that the
// controller expects to get.
// Therefore, the following code section copies the "RF_BLE_txPowerTable" entries to a competiable
// struct type.
#if !defined(CC13X2P)
    {
      txPwrVal_non_const_t * pTxPowerTblEntries;
      txPwrTbl_non_const_t * pTxPwrTable;

      pTxPowerTblEntries = (txPwrVal_non_const_t*)ICall_malloc(sizeof(txPwrVal_non_const_t)*(userCfg->boardConfig->txPwrTbl->numTxPwrVals));
      pTxPwrTable = (txPwrTbl_non_const_t*)ICall_malloc(sizeof(txPwrTbl_non_const_t));

      if(pTxPowerTblEntries && pTxPwrTable)
      {
        for(int i = 0; i < userCfg->boardConfig->txPwrTbl->numTxPwrVals;i++)
        {
          pTxPowerTblEntries[i].Pout              = RF_BLE_txPowerTable[i].power;
          pTxPowerTblEntries[i].txPwrVal          = (uint16_t)(RF_BLE_txPowerTable[i].value.rawValue & 0xFFFF);
        }
        pTxPwrTable->txPwrValsPtr = (txPwrVal_t*) pTxPowerTblEntries;
        pTxPwrTable->numTxPwrVals = userCfg->boardConfig->txPwrTbl->numTxPwrVals;
        pTxPwrTable->defaultTxPwrVal = userCfg->boardConfig->txPwrTbl->defaultTxPwrVal;

        // Tx Power Table
        llUserConfig.txPwrTblPtr   = (txPwrTbl_t *)pTxPwrTable;
      }
    }
#else
    // Tx Power Table
    llUserConfig.txPwrTblPtr   = userCfg->boardConfig->txPwrTbl;
#endif

    // RF Driver Table
    llUserConfig.rfDrvTblPtr   = userCfg->drvTblPtr->rfDrvTbl;

    // RF Driver Parameters
    llUserConfig.startupMarginUsecs = stackConfig->rfDriverParams.powerUpDurationMargin;
    llUserConfig.inactivityTimeout  = stackConfig->rfDriverParams.inactivityTimeout;
    llUserConfig.powerUpDuration    = stackConfig->rfDriverParams.powerUpDuration;
    llUserConfig.pErrCb             = stackConfig->rfDriverParams.pErrCb;
    
    // ECC Driver Parameter
    llUserConfig.eccCurveParams     = stackConfig->eccParams;

#if defined(USE_CRYPTO_DRIVER) || defined(CC26XX_R2) || defined(CC26X2) || defined(CC13X2) || defined(CC13X2P)
    if ( userCfg->drvTblPtr->cryptoDrvTbl == NULL )
    {
      LL_ASSERT( FALSE );
    }
    else
    {
      // Crypto Driver Table
      llUserConfig.cryptoDrvTblPtr = userCfg->drvTblPtr->cryptoDrvTbl;
    }
#endif // USE_CRYPTO_DRIVER | CC26XX_R2 | CC26X2 | CC13X2 | CC13X2P

    if ( userCfg->drvTblPtr->trngDrvTbl == NULL )
    {
      LL_ASSERT( FALSE );
    }
    else
    {
      // TRNG Driver Table
      llUserConfig.trngDrvTblPtr = userCfg->drvTblPtr->trngDrvTbl;
    }

    if ( userCfg->drvTblPtr->rtosApiTbl == NULL )
    {
      LL_ASSERT( FALSE );
    }
    else
    {
      // RTOS Api Table
      llUserConfig.rtosApiTblPtr = userCfg->drvTblPtr->rtosApiTbl;
    }
#ifdef OSAL_SNV_EXTFLASH
    if ( userCfg->drvTblPtr->extflashDrvTbl == NULL )
    {
      LL_ASSERT( FALSE );
    }
    else
    {
      // TRNG Driver Table
      extflashDrvTblPtr = userCfg->drvTblPtr->extflashDrvTbl;
    }
#endif // OSAL_SNV_EXTFLASH

    // ECC Driver Table
    if ( userCfg->drvTblPtr->eccDrvTbl == NULL )
    {
      LL_ASSERT( FALSE );
    }
    else
    {
      llUserConfig.eccDrvTblPtr = userCfg->drvTblPtr->eccDrvTbl;
    }

    // PM Startup Margin
    llUserConfig.startupMarginUsecs = stackConfig->rfDriverParams.powerUpDurationMargin;

    // Fast State Update Callback
    llUserConfig.fastStateUpdateCb = stackConfig->fastStateUpdateCb;

    // BLE Stack Type
    llUserConfig.bleStackType = stackConfig->bleStackType;

    // Extended stack settings
    llUserConfig.extStackSettings = stackConfig->extStackSettings;

    // save off the application's assert handler
    halAssertInit( **userCfg->appServiceInfo->assertCback, HAL_ASSERT_LEGACY_MODE_ENABLED );

    if ( userCfg->appServiceInfo->icallServiceTbl == NULL )
    {
      LL_ASSERT( FALSE );
    }
    else
    {
      icallServiceTblPtr = (uint32_t *) userCfg->appServiceInfo->icallServiceTbl;
    }

    osal_timer_init( userCfg->appServiceInfo->timerTickPeriod , userCfg->appServiceInfo->timerMaxMillisecond );
  }
  else
  {
      LL_ASSERT( FALSE );
  }

  return;
}
#else /* !(ICALL_JT) */
void setBleUserConfig( bleUserCfg_t *userCfg )
{
  if ( userCfg != NULL )
  {
#if defined( HOST_CONFIG ) && ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )
    l2capUserCfg_t l2capUserCfg;

    // user reconfiguration of Host variables
    l2capUserCfg.maxNumPSM = userCfg->maxNumPSM;
    l2capUserCfg.maxNumCoChannels = userCfg->maxNumCoChannels;

    L2CAP_SetUserConfig( &l2capUserCfg );

    if ( userCfg->pfnBMAlloc != NULL )
    {
      *userCfg->pfnBMAlloc = bleDispatch_BMAlloc;
    }

    if ( userCfg->pfnBMFree != NULL )
    {
      *userCfg->pfnBMFree = bleDispatch_BMFree;
    }
#endif // ( CENTRAL_CFG | PERIPHERAL_CFG )

    // user reconfiguration of Controller variables
    llUserConfig.maxNumConns   = userCfg->maxNumConns;
    llUserConfig.numTxEntries  = userCfg->maxNumPDUs;
    llUserConfig.maxPduSize    = userCfg->maxPduSize;

    // Copy the RF_mode Object
    memcpy(&rfMode, &RF_modeBle, sizeof(RF_Mode));

    // Use the rtls cpe patch when RTLS is used
#if (defined RTLS_CTE || defined USE_PERIODIC_ADV || defined USE_PERIODIC_SCAN)
    rfMode.cpePatchFxn = &rf_patch_cpe_multi_protocol_rtls;
    // Use the multi_protocol cpe patch when DMM is used
#elif USE_DMM
    rfMode.cpePatchFxn = &rf_patch_cpe_multi_protocol;
#endif

    // RF Front End Mode and Bias (based on package)
    llUserConfig.rfFeModeBias = userCfg->rfFeModeBias;

    // Privacy Override Offset
    llUserConfig.privOverrideOffset  = userCfg->privOverrideOffset;

    // RF Override Registers
    llUserConfig.rfRegPtr      = userCfg->rfRegTbl;
    llUserConfig.rfReg1MPtr    = userCfg->rfRegTbl1M;
#if defined(BLE_V50_FEATURES) && (BLE_V50_FEATURES & (PHY_2MBPS_CFG | PHY_LR_CFG))
    llUserConfig.rfReg2MPtr    = userCfg->rfRegTbl2M;
    llUserConfig.rfRegCodedPtr = userCfg->rfRegTblCoded;
#endif // PHY_2MBPS_CFG | PHY_LR_CFG

#if defined(CC13X2P)
    llUserConfig.rfRegOverrideTx20Ptr    = userCfg->RegOverrideTx20Tbl;   // High power PA
    llUserConfig.rfRegOverrideTxStdPtr   = userCfg->RegOverrideTxStdTbl;  // Default PA
    llUserConfig.txPwrBackoffTblPtr      = userCfg->txPwrBackoffTbl;      // Tx power backoff table
#endif //CC13X2P

    llUserConfig.rfRegOverrideCtePtr = userCfg->boardConfig->rfRegOverrideCtePtr;   // CTE overrides
    llUserConfig.coexUseCaseConfig   = userCfg->boardConfig->coexUseCaseConfigPtr;  // Coex Configuration

// The BLE stack is using "RF_BLE_txPowerTable" that is generated via the SysConfig tool
// and can be found in "ti_radio_config.c".
// The RF_TxPowerTable_Entry struct type is incompetiable with the txPwrVal_t struct type, which is the type that the
// controller expects to get.
// Therefore, the following code section copies the "RF_BLE_txPowerTable" entries to a competiable
// struct type.
#if !defined(CC13X2P)
    {
      txPwrVal_non_const_t * pTxPowerTblEntries;
      txPwrTbl_non_const_t * pTxPwrTable;

      pTxPowerTblEntries = (txPwrVal_non_const_t*)ICall_malloc(sizeof(txPwrVal_non_const_t)*(userCfg->txPwrTbl->numTxPwrVals));
      pTxPwrTable = (txPwrTbl_non_const_t*)ICall_malloc(sizeof(txPwrTbl_non_const_t));

      if(pTxPowerTblEntries && pTxPwrTable)
      {
        for(int i = 0; i < userCfg->txPwrTbl->numTxPwrVals; i++)
        {
          pTxPowerTblEntries[i].Pout              = RF_BLE_txPowerTable[i].power;
          pTxPowerTblEntries[i].txPwrVal          = (uint16_t)(RF_BLE_txPowerTable[i].value.rawValue & 0xFFFF);
        }
        pTxPwrTable->txPwrValsPtr = (txPwrVal_t*) pTxPowerTblEntries;
        pTxPwrTable->numTxPwrVals = userCfg->txPwrTbl->numTxPwrVals;
        pTxPwrTable->defaultTxPwrVal = userCfg->txPwrTbl->defaultTxPwrVal;

        // Tx Power Table
        llUserConfig.txPwrTblPtr   = (txPwrTbl_t *)pTxPwrTable;
      }
    }
#else
    // Tx Power Table
    llUserConfig.txPwrTblPtr   = userCfg->txPwrTbl;
#endif

    // RF Driver Table
    llUserConfig.rfDrvTblPtr   = userCfg->rfDrvTbl;

    // ECC Driver Table
    llUserConfig.eccDrvTblPtr = userCfg->eccDrvTbl;

    // Crypto Driver Table
    llUserConfig.cryptoDrvTblPtr = userCfg->cryptoDrvTbl;

    // TRNG Driver Table
    llUserConfig.trngDrvTblPtr = userCfg->trngDrvTbl;

    // PM Startup Margin
    llUserConfig.startupMarginUsecs = userCfg->startupMarginUsecs;

    // Fast State Update Callback
    llUserConfig.fastStateUpdateCb = userCfg->fastStateUpdateCb;

    // BLE Stack Type
    llUserConfig.bleStackType = userCfg->bleStackType;

    // save off the application's assert handler
    halAssertInit( **userCfg->assertCback, HAL_ASSERT_LEGACY_MODE_DISABLED );
  }

  return;
}
#endif /* ICALL_JT */
/*******************************************************************************
 */
