/******************************************************************************

 @file  urfi.c

 @brief This file contains the RF driver interfacing API for the Micro
        BLE Stack.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2009-2022, Texas Instruments Incorporated

 All rights reserved not granted herein.
 Limited License.

 Texas Instruments Incorporated grants a world-wide, royalty-free,
 non-exclusive license under copyrights and patents it now or hereafter
 owns or controls to make, have made, use, import, offer to sell and sell
 ("Utilize") this software subject to the terms herein. With respect to the
 foregoing patent license, such license is granted solely to the extent that
 any such patent is necessary to Utilize the software alone. The patent
 license shall not apply to any combinations which include this software,
 other than combinations with devices manufactured by or for TI ("TI
 Devices"). No hardware patent is licensed hereunder.

 Redistributions must preserve existing copyright notices and reproduce
 this license (including the above copyright notice and the disclaimer and
 (if applicable) source code license limitations below) in the documentation
 and/or other materials provided with the distribution.

 Redistribution and use in binary form, without modification, are permitted
 provided that the following conditions are met:

   * No reverse engineering, decompilation, or disassembly of this software
     is permitted with respect to any software provided in binary form.
   * Any redistribution and use are licensed by TI for use only with TI Devices.
   * Nothing shall obligate TI to provide you with source code for the software
     licensed and provided to you in object code.

 If software source code is provided to you, modification and redistribution
 of the source code are permitted provided that the following conditions are
 met:

   * Any redistribution and use of the source code, including any resulting
     derivative works, are licensed by TI for use only with TI Devices.
   * Any redistribution and use of any object code compiled from the source
     code and any resulting derivative works, are licensed by TI for use
     only with TI Devices.

 Neither the name of Texas Instruments Incorporated nor the names of its
 suppliers may be used to endorse or promote products derived from this
 software without specific prior written permission.

 DISCLAIMER.

 THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include <driverlib/rf_mailbox.h>
#include <driverlib/rf_ble_cmd.h>
#include <driverlib/rf_ble_mailbox.h>
#include <ti/drivers/rf/RF.h>
#include <bcomdef.h>

#include <ll_common.h>
#include <port.h>
#include <urfi.h>
#include <uble.h>
#include <ull.h>

/*********************************************************************
 * CONSTANTS
 */

#if (!defined(RF_SINGLEMODE) && !defined(RF_MULTIMODE)) ||                   \
    (defined(RF_SINGLEMODE) && defined(RF_MULTIMODE))
  #error "Either RF_SINGLEMODE or RF_MULTIMODE should be defined."
#endif /* RF_SINGLEMODE, RF_MULTIMODE */

#if (defined(FEATURE_SCANNER) && defined(FEATURE_MONITOR))
  #error "FEATURE_SCANNER and FEATURE_MONITOR cannot be defined at the same time."
#endif /* FEATURE_SCANNER, FEATURE_MONITOR */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

extern ubleParams_t ubleParams;
extern uint8        ubleBDAddr[];
extern uint8        rfTimeCrit;
extern ubleAntSwitchCB_t ubleAntSwitchSel;

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
extern bStatus_t uble_buildAndPostEvt(ubleEvtDst_t evtDst, ubleEvt_t evt,
                                      ubleMsg_t *pMsg, uint16 len);
extern dataEntryQ_t *ull_setupScanDataEntryQueue( void );
extern dataEntryQ_t *ull_setupMonitorDataEntryQueue( void );

/*********************************************************************
 * GLOBAL VARIABLES
 */

RF_Handle    urfiHandle = NULL;

/* Radio Setup Parameters.
 * config, txPower, and pRegOverride will be initialized at runtime.
 */
RF_RadioSetup urSetup =
{
  .common.commandNo                = CMD_BLE5_RADIO_SETUP,
  .common.status                   = IDLE,
  .common.pNextOp                  = NULL,
  .common.startTime                = 0,
  .common.startTrigger.triggerType = TRIG_NOW,
  .common.startTrigger.bEnaCmd     = 0,
  .common.startTrigger.triggerNo   = 0,
  .common.startTrigger.pastTrig    = 0,
  .common.condition.rule           = COND_NEVER,
  .common.condition.nSkip          = 0,
  .common.mode                     = 0,
};

#if defined(FEATURE_ADVERTISER)
RF_CmdHandle urfiAdvHandle = URFI_CMD_HANDLE_INVALID;

/* CMD_BLE_ADV_XX Params */
rfc_bleAdvPar_t urfiAdvParams =
{
  .pRxQ = 0,
  .rxConfig.bAutoFlushIgnored = 0,
  .rxConfig.bAutoFlushCrcErr = 0,
  .rxConfig.bAutoFlushEmpty = 0,
  .rxConfig.bIncludeLenByte = 0,
  .rxConfig.bIncludeCrc = 0,
  .rxConfig.bAppendRssi = 0,
  .rxConfig.bAppendStatus = 0,
  .rxConfig.bAppendTimestamp = 0,
  .advConfig.advFilterPolicy = 0,
  .advConfig.deviceAddrType = 0,
  .advConfig.peerAddrType = 0,
  .advConfig.bStrictLenFilter = 0,
  .advLen = 0,
  .scanRspLen = 0,
  .pAdvData = ubleParams.advData,
#if defined(FEATURE_SCAN_RESPONSE)
  .pScanRspData = ubleParams.scanRspData,
#else   /* FEATURE_SCAN_RESPONSE */
  .pScanRspData = 0,
#endif  /* FEATURE_SCAN_RESPONSE */
  .pDeviceAddress = (uint16*) ubleBDAddr,
  .pWhiteList = 0,
  .__dummy0 = 0,
  .__dummy1 = 0,
  .endTrigger.triggerType = TRIG_NEVER,
  .endTrigger.bEnaCmd = 0,
  .endTrigger.triggerNo = 0,
  .endTrigger.pastTrig = 0,
  .endTime = 0,
};

#if defined(FEATURE_SCAN_RESPONSE)
/* CMD_BLE_ADV_XX Output */
rfc_bleAdvOutput_t urAdvOutput;
#endif  /* FEATURE_SCAN_RESPONSE */

/* CMD_BLE_ADV_XX Command */
rfc_CMD_BLE_ADV_COMMON_t urfiAdvCmd[3];

#endif  /* FEATURE_ADVERTISER */

#if defined(FEATURE_SCANNER)
RF_CmdHandle urfiScanHandle = URFI_CMD_HANDLE_INVALID;

/* CMD_BLE_SCANNER Params */
rfc_bleScannerPar_t urfiScanParams =
{
  .pRxQ = 0,
  .rxConfig.bAutoFlushIgnored = 1,
  .rxConfig.bAutoFlushCrcErr = 1,
  .rxConfig.bAutoFlushEmpty = 1,
  .rxConfig.bIncludeLenByte = 1,
  .rxConfig.bIncludeCrc = 0,
  .rxConfig.bAppendRssi = 1,
  .rxConfig.bAppendStatus = 1,
  .rxConfig.bAppendTimestamp = 1,
  .scanConfig.scanFilterPolicy = ULL_SCAN_WL_POLICY_ANY_ADV_PKTS,
  .scanConfig.bActiveScan = 0,  /* Passive scan only */
  .scanConfig.deviceAddrType = 0,
  .scanConfig.rpaFilterPolicy = 0,
  .scanConfig.bStrictLenFilter = 1,
  .scanConfig.bAutoWlIgnore = 0,
  .scanConfig.bEndOnRpt = 0, /* Must continue to scan */
  .scanConfig.rpaMode = 0,
  .randomState = 0,
  .backoffCount = 1,
  .backoffPar = 0,
  .scanReqLen = 0,
  .pScanReqData = 0,
  .pDeviceAddress = 0,
  .pWhiteList = 0,
  .__dummy0 = 0,
  .timeoutTrigger.triggerType = TRIG_NEVER,
  .timeoutTrigger.bEnaCmd = 0,
  .timeoutTrigger.triggerNo = 0,
  .timeoutTrigger.pastTrig = 1,
  .endTrigger.triggerType = TRIG_NEVER,
  .endTrigger.bEnaCmd = 0,
  .endTrigger.triggerNo = 0,
  .endTrigger.pastTrig = 0,
  .timeoutTime = 0,
  .endTime = 0,
};

/* CMD_BLE_SCANNER outputs */
rfc_bleScannerOutput_t pScanOutput;

/* CMD_BLE_SCANNER Command */
rfc_CMD_BLE_SCANNER_t urfiScanCmd;

#endif  /* FEATURE_SCANNER */

#if defined(FEATURE_MONITOR)
RF_CmdHandle urfiGenericRxHandle = URFI_CMD_HANDLE_INVALID;

/* CMD_BLE_GENERIC_RX Params */
rfc_bleGenericRxPar_t urfiGenericRxParams =
{
  .pRxQ = 0,
  .rxConfig.bAutoFlushIgnored = 0, // Should never turn on for generic Rx cmd
  .rxConfig.bAutoFlushCrcErr = 0, // TBD: receiving with CRC error for now
  .rxConfig.bAutoFlushEmpty = 1,
  .rxConfig.bIncludeLenByte = 1,
  .rxConfig.bIncludeCrc = 0,
  .rxConfig.bAppendRssi = 1,
  .rxConfig.bAppendStatus = 1,
  .rxConfig.bAppendTimestamp = 1,
  .bRepeat = 1,
  .__dummy0 = 0,
  .accessAddress = 0,
  .crcInit0 = 0x55, // TBD: Adv packets for now
  .crcInit1 = 0x55,
  .crcInit2 = 0x55,
  .endTrigger.triggerType = TRIG_NEVER,
  .endTrigger.bEnaCmd = 0,
  .endTrigger.triggerNo = 0,
  .endTrigger.pastTrig = 1,
  .endTime = 0,
};

/* CMD_BLE_GENERIC_RX outputs */
rfc_bleGenericRxOutput_t pGenericRxOutput;

/* CMD_BLE_GENERIC_RX Command */
rfc_CMD_BLE_GENERIC_RX_t urfiGenericRxCmd;

#endif  /* FEATURE_MONITOR */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

static RF_Object urObject;
static RF_Params urParams;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      urfi_initAdvCmd
 *
 * @brief   Initialize Adv RF command
 *
 * @param   None
 *
 * @return  None
 */
#if defined(FEATURE_ADVERTISER)
void urfi_initAdvCmd(void)
{
  for (uint8 i = 0; i < 3; i++)
  {
    /* Advertising channel */
    urfiAdvCmd[i].channel                = 37 + i;

    urfiAdvCmd[i].whitening.init         = 0; /* No whitening */
    urfiAdvCmd[i].pParams                = &urfiAdvParams;

    urfiAdvCmd[i].startTrigger.bEnaCmd   = 0;

  #if defined(FEATURE_SCAN_RESPONSE)
    urfiAdvCmd[i].pOutput                = &urAdvOutput;
  #else  /* FEATURE_SCAN_RESPONSE */
    urfiAdvCmd[i].pOutput                = NULL;
  #endif /* FEATURE_SCAN_RESPONSE */
  }

  /* 1st channel adv is supposed to start at a certain time */
  urfiAdvCmd[0].startTrigger.triggerType = TRIG_ABSTIME;
  /* 2nd and 3rd channel adv's are supposed to start as soon as
     the previous channel operation ends */
  urfiAdvCmd[1].startTrigger.triggerType =
  urfiAdvCmd[2].startTrigger.triggerType = TRIG_NOW;

  urfiAdvCmd[1].startTime                =
  urfiAdvCmd[2].startTime                = 0;

  urfiAdvCmd[0].condition.rule           =
  urfiAdvCmd[1].condition.rule           = COND_STOP_ON_FALSE;
  urfiAdvCmd[2].condition.rule           = COND_NEVER;

#ifdef RF_MULTIMODE
  if (RF_TIME_RELAXED == rfTimeCrit)
  {
    urfiAdvCmd[0].startTrigger.pastTrig    =
    urfiAdvCmd[1].startTrigger.pastTrig    =
    urfiAdvCmd[2].startTrigger.pastTrig    = 1;
  }
  else
  {
    urfiAdvCmd[0].startTrigger.pastTrig    =
    urfiAdvCmd[1].startTrigger.pastTrig    =
    urfiAdvCmd[2].startTrigger.pastTrig    = 0;
  }
#endif /* RF_MULTIMODE */

  urfiAdvCmd[0].pNextOp                  = (rfc_radioOp_t*) &urfiAdvCmd[1];
  urfiAdvCmd[1].pNextOp                  = (rfc_radioOp_t*) &urfiAdvCmd[2];
  urfiAdvCmd[2].pNextOp                  = NULL;
}
#endif /* FEATURE_ADVERTISER */

/*********************************************************************
 * @fn      urfi_initScanCmd
 *
 * @brief   Initialize Scan RF command
 *
 * @param   None
 *
 * @return  None
 */
#if defined(FEATURE_SCANNER)
void urfi_initScanCmd(void)
{
  urfiScanCmd.commandNo = CMD_BLE_SCANNER;
  urfiScanCmd.status = IDLE;
  urfiScanCmd.pNextOp = NULL;
  urfiScanCmd.startTime = 0;
  urfiScanCmd.startTrigger.triggerType = TRIG_NOW;
  urfiScanCmd.startTrigger.bEnaCmd = 0;
  urfiScanCmd.startTrigger.triggerNo = 0;

  /* uGAP controls the scan timing. Scanning late should be allowed. */
  urfiScanCmd.startTrigger.pastTrig = 1;

  urfiScanCmd.condition.rule = COND_NEVER;
  urfiScanCmd.condition.nSkip = 0;

  urfiScanCmd.channel = 37;
  urfiScanCmd.whitening.init = 0;
  urfiScanCmd.whitening.bOverride = 0;
  urfiScanCmd.pParams = &urfiScanParams;
  urfiScanCmd.pOutput = &pScanOutput;

  urfiScanParams.pRxQ = (dataQueue_t *)ull_setupScanDataEntryQueue();
}
#endif /* FEATURE_SCANNER */

#if defined(FEATURE_MONITOR)
/*********************************************************************
 * @fn      urfi_initGenericRxCmd
 *
 * @brief   Initialize Scan RF command
 *
 * @param   None
 *
 * @return  None
 */
void urfi_initGenericRxCmd(void)
{
  urfiGenericRxCmd.commandNo = CMD_BLE_GENERIC_RX;
  urfiGenericRxCmd.status = IDLE;
  urfiGenericRxCmd.pNextOp = NULL;
  urfiGenericRxCmd.startTime = 0;
  urfiGenericRxCmd.startTrigger.triggerType = TRIG_ABSTIME;
  urfiGenericRxCmd.startTrigger.bEnaCmd = 0;
  urfiGenericRxCmd.startTrigger.triggerNo = 0;

#ifdef RF_MULTIMODE
  urfiGenericRxCmd.startTrigger.pastTrig =
    (RF_TIME_RELAXED == rfTimeCrit) ? 1 : 0;
#endif /* RF_MULTIMODE */

  urfiGenericRxCmd.condition.rule = COND_NEVER;
  urfiGenericRxCmd.condition.nSkip = 0;

  urfiGenericRxCmd.channel = 37;
  urfiGenericRxCmd.whitening.init = 0;
  urfiGenericRxCmd.whitening.bOverride = 0;

  urfiGenericRxCmd.pParams = &urfiGenericRxParams;
  urfiGenericRxCmd.pOutput = &pGenericRxOutput;

  urfiGenericRxParams.pRxQ = (dataQueue_t *)ull_setupMonitorDataEntryQueue();
}
#endif /* FEATURE_MONITOR */

/*********************************************************************
 * @fn      urfi_clientEventCb
 *
 * @brief   Callback function to be invoked by RF driver
 *
 * @param   rfHandle - RF client handle
 *
 * @param   cmdHandle - RF command handle
 *
 * @param   events - RF client events
 * @param   arg - reserved for future use
 *
 * @return  none
 */
void urfi_clientEventCb(RF_Handle h, RF_ClientEvent events, void* arg)
{
  port_key_t key;

  key = port_enterCS_SW();

  if (events & RF_ClientEventRadioFree)
  {
#if defined(FEATURE_ADVERTISER)
    uble_buildAndPostEvt(UBLE_EVTDST_LL, ULL_EVT_ADV_TX_RADIO_AVAILABLE, NULL, 0);
#endif /* FEATURE_ADVERTISER */
#if defined(FEATURE_SCANNER)
    uble_buildAndPostEvt(UBLE_EVTDST_LL, ULL_EVT_SCAN_RX_RADIO_AVAILABLE, NULL, 0);
#endif /* FEATURE_SCANNER */
#if defined(FEATURE_MONITOR)
    uble_buildAndPostEvt(UBLE_EVTDST_LL, ULL_EVT_MONITOR_RX_RADIO_AVAILABLE, NULL, 0);
#endif /* FEATURE_MONITOR */
  }

  if (events & RF_ClientEventSwitchClientEntered)
  {
    if (ubleAntSwitchSel != NULL)
    {
      ubleAntSwitchSel();
    }
  }
  port_exitCS_SW(key);
}

/*********************************************************************
 * @fn      urfi_init
 *
 * @brief   Initialize radio interface and radio commands
 *
 * @param   None
 *
 * @return  SUCCESS - RF driver has been successfully opened
 *          FAILURE - Failed to open RF driver
 */
bStatus_t urfi_init(void)
{
  if (urfiHandle == NULL)
  {
    RF_Params_init(&urParams); /* Get default values from RF driver */
    urParams.nInactivityTimeout = 0; /* Do not use the default value for this */
    urParams.pClientEventCb = urfi_clientEventCb;
    urParams.nClientEventMask = RF_ClientEventRadioFree | RF_ClientEventSwitchClientEntered;

    /* Setup radio setup command.
     * TODO: Make more variables user-configurable
     */
    /* Differential mode */
    urSetup.common.config.frontEndMode  = ubFeModeBias & 0x07;
    /* Internal bias */
    urSetup.common.config.biasMode      = (ubFeModeBias & 0x08) >> 3;
    /* Keep analog configuration */
    urSetup.common.config.analogCfgMode = 0x2D;
    /* Power up frequency synth */
    urSetup.common.config.bNoFsPowerUp  =    0;
    /* 0 dBm */
    urSetup.common.txPower            = urfi_getTxPowerVal(UBLE_PARAM_DFLT_TXPOWER);
    /* Register Overrides */
    urSetup.common.pRegOverride       = (uint32_t*) ubRfRegOverride;

    /* Request access to the radio */
    urfiHandle = RF_open(&urObject, (RF_Mode*) &ubRfMode, &urSetup, &urParams);

    if (urfiHandle == NULL)
    {
      return FAILURE;
    }
  }

#if defined(FEATURE_ADVERTISER)
  urfi_initAdvCmd();
#endif  /* FEATURE_ADVERTISER */

#if defined(FEATURE_SCANNER)
  urfi_initScanCmd();
#endif  /* FEATURE_SCANNER */

#if defined(FEATURE_MONITOR)
  urfi_initGenericRxCmd();
#endif  /* FEATURE_SCANNER */

  return SUCCESS;
}

/*********************************************************************
 * CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      urfi_getTxPowerVal
 *
 * @brief   Get the value, corresponding with the given TX Power,
 *          to be used to setup the radio accordingly.
 *
 * @param   txPower - TX Power in dBm.
 *
 * @return  The register value correspondign with txPower, if found.
 *          UBLE_TX_POWER_INVALID otherwise.
 */
uint16 urfi_getTxPowerVal(int8 txPower)
{
  uint8 i;

  for (i = 0; i < ubTxPowerTable.numTxPowerVal; i++)
  {
    if (ubTxPowerTable.pTxPowerVals[i].dBm == txPower)
    {
      return ubTxPowerTable.pTxPowerVals[i].txPowerVal;
    }
  }

  return UBLE_TX_POWER_INVALID;
}


/*********************************************************************
*********************************************************************/
