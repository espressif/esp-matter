/******************************************************************************

 @file  gap_advertiser_internal.h

 @brief This file contains internal interfaces for the GAP.

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
 *
 * WARNING!!!
 *
 * THE API'S FOUND IN THIS FILE ARE FOR INTERNAL STACK USE ONLY!
 * FUNCTIONS SHOULD NOT BE CALLED DIRECTLY FROM APPLICATIONS, AND ANY
 * CALLS TO THESE FUNCTIONS FROM OUTSIDE OF THE STACK MAY RESULT IN
 * UNEXPECTED BEHAVIOR
 *
 * These API's shall begin with a lower-case letter
 */

#ifndef GAP_ADV_INTERNAL_H
#define GAP_ADV_INTERNAL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "bcomdef.h"
#include "ll.h"
#include "gap.h"

/*********************************************************************
 * MACROS
 */

#ifndef status_t
  #define status_t bStatus_t
#endif

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */
/** Advertising and Scan Response Data **/
typedef struct
{
  uint8   dataLen;                  // Number of bytes used in "dataField"
  uint8   dataField[B_MAX_ADV_LEN]; // Data field of the advertisement or SCAN_RSP
} gapAdvertisingData_t;

typedef struct
{
  uint8   dataLen;       // length (in bytes) of "dataField"
  uint8   dataField[1];  // This is just a place holder size
                         // The dataField will be allocated bigger
} gapAdvertRecData_t;

// Temporary advertising record
typedef struct
{
  uint8  eventType;               // Avertisement or SCAN_RSP
  uint8  addrType;                // Advertiser's address type
  uint8  addr[B_ADDR_LEN];        // Advertiser's address
  gapAdvertRecData_t *pAdData;    // Advertising data field. This space is allocated.
  gapAdvertRecData_t *pScanData;  // SCAN_RSP data field. This space is allocated.
} gapAdvertRec_t;

typedef enum
{
  GAP_ADSTATE_SET_PARAMS,     // Setting the advertisement parameters
  GAP_ADSTATE_SET_MODE,       // Turning on advertising
  GAP_ADSTATE_ADVERTISING,    // Currently Advertising
  GAP_ADSTATE_ENDING          // Turning off advertising
} gapAdvertStatesIDs_t;

// Lookup for a GAP Scanner parameter.
// Note: Offset is offset into controller aeSetParamCmd_t structure
// There is no need for range checking in the host as it is done in the
// controller
typedef struct {
  uint8_t  offset;
  uint8_t  len;
} gapAdv_ParamLookup_t;

/// Advertiser remove set Event
typedef struct {
  osal_event_hdr_t hdr;       //!< GAP_MSG_EVENT and status
  uint8_t          opcode;    //!< GAP_ADV_REMOVE_SET_EVENT
  uint8_t          handle;    //!< Handle of the adv set to remove
  aeSetParamCmd_t* pAdvParam; //!< pointer of the parameter to free
} gapAdv_removeAdvEvent_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * GAP Advertiser Functions
 */
extern bStatus_t gapAdv_init(void);
extern void      gapAdv_controllerCb(uint8 cBackID, void *pParams);
extern bStatus_t gapAdv_searchForBufferUse(uint8 *pBuf, advSet_t *pAdvSet);
extern void      gapAdv_processRemoveSetEvt(uint8_t handle, 
                                            aeSetParamCmd_t* pAdvParam);

/**
 * gapAdv_periodicAdvCmdCompleteCBs
 *
 * Process an incoming OSAL HCI Command Complete Event for
 * periodic advertising.
 *
 * @param   pMsg - message to process
 *
 * @return  TRUE if processed and safe to deallocate, FALSE if passed
 *          off to another task.
 */
uint8_t gapAdv_periodicAdvCmdCompleteCBs( hciEvt_CmdComplete_t *pMsg );

uint8_t gapAdv_handleAdvHciCmdComplete( hciEvt_CmdComplete_t *pMsg );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* GAP_INTERNAL_H */
