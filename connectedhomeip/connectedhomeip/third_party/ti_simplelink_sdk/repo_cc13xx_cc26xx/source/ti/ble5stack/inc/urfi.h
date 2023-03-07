/******************************************************************************

 @file  urfi.h

 @brief This file contains the RF Interface.

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

#ifndef URFI_H
#define URFI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <driverlib/rf_ble_cmd.h>

/*********************************************************************
 * CONSTANTS
 */

/* Invalid RF command handle */
#define URFI_CMD_HANDLE_INVALID   RF_ALLOC_ERROR

/*********************************************************************
 * TYPEDEFS
 */


/* There are four different types of command struct depending on ADV type.
   However, they are actually all the same. We can use a common struct.
*/
typedef rfc_CMD_BLE_ADV_t  rfc_CMD_BLE_ADV_COMMON_t;


/*****************************************************************************
 * FUNCTION PROTOTYPES
 */

/**
 * @brief  Initialize RF Interface for Micro BLE Stack
 *
 * @param  none
 *
 * @return SUCCESS - RF driver opened successfully
 *         INVALIDPARAMETER - Failed to open RF driver
 */
bStatus_t urfi_init(void);

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
uint16 urfi_getTxPowerVal(int8 dBm);

/*********************************************************************
 *  EXTERNAL VARIABLES
 */

/* RF driver handle */
extern RF_Handle    urfiHandle;
#if defined(FEATURE_ADVERTISER)
/* ADV command handle */
extern RF_CmdHandle urfiAdvHandle;
/* ADV command parameter */
extern rfc_bleAdvPar_t urfiAdvParams;
#endif /* FEATURE_ADVERTISER */

#if defined(FEATURE_SCANNER)
/* Scan command handle */
extern RF_CmdHandle urfiScanHandle;
/* Scan command parameter */
extern rfc_bleScannerPar_t urfiScanParams;
#endif /* FEATURE_SCANNER */

#if defined(FEATURE_MONITOR)
/* Monitor command handle */
extern RF_CmdHandle urfiGenericRxHandle;
/* Monitor command parameter */
extern rfc_bleGenericRxPar_t urfiGenericRxParams;
#endif /* FEATURE_MONITOR */

#ifdef __cplusplus
}
#endif

#endif /* URFI_H */
