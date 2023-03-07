/******************************************************************************

 @file  ll_user_config.h

 @brief This file contains user configurable variables for the BLE
        Controller.

        To change the default value of configurable variable:
          - Include "ll_userConfig.h" in your OSAL_ICallBle.c file.
          - Set the variables at the start of stack_main. Actually,
            it is okay to set the variables anywhere in stack_main
            as long as it is BEFORE osal_init_system, but best to
            set at the very start of stack_main.

        Note: User configurable variables are only used during the
              initialization of the Controller. Changing the values
              of these variables after this will have no effect.

        For example:
          int stack_main()
          {
            // user reconfiguration of Controller variables
            llUserConfig.maxNumConns  = 1;
            llUserConfig.numTxEntries = 10;
                       :

        Default values:
          maxNumConns  : 3
          numTxEntries : 6

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2014-2022, Texas Instruments Incorporated

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
 *
 * WARNING!!!
 *
 * THE API'S FOUND IN THIS FILE ARE FOR INTERNAL STACK USE ONLY!
 * FUNCTIONS SHOULD NOT BE CALLED DIRECTLY FROM APPLICATIONS, AND ANY
 * CALLS TO THESE FUNCTIONS FROM OUTSIDE OF THE STACK MAY RESULT IN
 * UNEXPECTED BEHAVIOR.
 *
 */

#ifndef LL_USER_CONFIG_H
#define LL_USER_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */
#include "ble_user_config.h"

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * TYPEDEFS
 */

typedef struct
{
  uint8                 maxNumConns;           // Max number of BLE connections
  uint8                 numTxEntries;          // Max number of BLE connection Tx buffers
  uint8                 maxPduSize;            // Max PDU data size
  uint8                 rfFeModeBias;          // RF Front End Mode and Bias
  regOverride_t         *rfRegPtr;             // RF Common Override Registers
  regOverride_t         *rfReg1MPtr;           // RF 1M Override Register Table
#if defined(BLE_V50_FEATURES) && (BLE_V50_FEATURES & (PHY_2MBPS_CFG | PHY_LR_CFG))
  regOverride_t         *rfReg2MPtr;            // RF 2M Override Register Table
  regOverride_t         *rfRegCodedPtr;         // RF Coded Override Register Table
#endif // PHY_2MBPS_CFG | PHY_LR_CFG
  txPwrTbl_t            *txPwrTblPtr;           // Tx Power Table
  rfDrvTblPtr_t         *rfDrvTblPtr;           // Table of Rf Driver API
  eccDrvTblPtr_t        *eccDrvTblPtr;          // Table of ECC Driver API
  cryptoDrvTblPtr_t     *cryptoDrvTblPtr;       // Table of Crypto Driver API
  trngDrvTblPtr_t       *trngDrvTblPtr;         // Table of TRNG Driver API
  rtosApiTblPtr_t       *rtosApiTblPtr;         // Table of RTOS API
  uint32                startupMarginUsecs;     // Startup Margin in us
  uint32                inactivityTimeout;      // Inactivity timeout in us
  uint32                powerUpDuration;        // Powerup time in us
  RF_Callback           *pErrCb;                // RF Driver Error Callback
  uint8                 maxWlElems;             // Max elements in the white list
  uint8                 maxRlElems;             // Max elements in the resolving list
  ECCParams_CurveParams *eccCurveParams;        // ECC curve parameters
  pfnFastStateUpdate_t  fastStateUpdateCb;      // Fast state update callback
  uint32                bleStackType;           // BLE Stack Type
  uint32                extStackSettings;       // BLE misc stack settings
#if defined(CC13X2P)
  txPwrBackoffTbl_t     *txPwrBackoffTblPtr;    // Tx Power Table
  regOverride_t         *rfRegOverrideTx20Ptr;  // High gain overrides
  regOverride_t         *rfRegOverrideTxStdPtr; // Default PA overrides
#endif //CC13X2P
  regOverride_t         *rfRegOverrideCtePtr;   // CTE overrides
  cteAntProp_t          *cteAntProp;            // CTE antenna properties
  uint8                 privOverrideOffset;    // Privacy Override Offset
  coexUseCaseConfig_t   *coexUseCaseConfig;     // CoEx priority and RX request configuration
  uint8                 maxNumCteBufs;         // num of CTE samples buffers (each ~2.5KB) used for RF auto copy
} llUserCfg_t;

/*******************************************************************************
 * LOCAL VARIABLES
 */


/*******************************************************************************
 * GLOBAL VARIABLES
 */

extern llUserCfg_t llUserConfig;
extern uint16      llUserConfig_maxPduSize;

#ifdef __cplusplus
}
#endif

#endif /* LL_USER_CONFIG_H */
