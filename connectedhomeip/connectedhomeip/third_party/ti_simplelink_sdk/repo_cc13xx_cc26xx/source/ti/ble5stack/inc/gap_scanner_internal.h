/******************************************************************************

 @file  gap_scanner_internal.h

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

#ifndef GAP_SCANNER_INTERNAL_H
#define GAP_SCANNER_INTERNAL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "bcomdef.h"
#include "ll.h"
#include "gap_scanner.h"

/*********************************************************************
 * MACROS
 */

#ifndef status_t
  #define status_t bStatus_t
#endif

// Masks to split type and status in evtType field of @ref GapScan_Evt_AdvRpt_t
#define ADV_RPT_EVT_TYPE_MASK   (ADV_RPT_EVT_TYPE_CONNECTABLE | \
                                 ADV_RPT_EVT_TYPE_SCANNABLE |   \
                                 ADV_RPT_EVT_TYPE_DIRECTED |    \
                                 ADV_RPT_EVT_TYPE_SCAN_RSP |    \
                                 ADV_RPT_EVT_TYPE_LEGACY)
#define ADV_RPT_EVT_STATUS_MASK (ADV_RPT_EVT_STATUS_MORE_DATA | \
                                 ADV_RPT_EVT_STATUS_TRUNCATED)

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/// Advertising Report Fragment
typedef struct GapScan_AdvRptFrag_s GapScan_AdvRptFrag_t;

/// Advertising report fragment
struct GapScan_AdvRptFrag_s {
  /// Pointer to next  advertising report fragment
  GapScan_AdvRptFrag_t* pNext;
  uint8_t               dataLen;  //!< data length
  uint8_t*              pData;    //!< pointer to data
};

/// Advertising Report Session
typedef struct {
  GapScan_AdvRptFrag_t* pFragHead;  //!< pointer to fragment head
  GapScan_AdvRptFrag_t* pFragCurr;  //!< pointer to current fragment
  GapScan_Evt_AdvRpt_t* pDefrag;    //!< pointer to defragmented data
} GapScan_AdvRptSession_t;

/// Scan Session End Event
typedef struct {
  osal_event_hdr_t         hdr;      //!< GAP_MSG_EVENT and status
  uint8_t                  opcode;   //!< GAP_SCAN_SESSION_END_EVENT
  GapScan_AdvRptSession_t* pSession; //!< Advertising report session to complete
} GapScan_SessionEndEvent_t;
  
// Lookup for a GAP Scanner parameter.
// note: Min/max work only for uint8-type parameters
typedef struct {
  uint8_t  offset;
  uint8_t  len;
  uint8_t  min;
  uint8_t  max;
} gapScan_ParamLookup_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * GAP Scanner Functions
 */
extern bStatus_t gapScan_init(void);

/**
 * gapScan_periodicAdvCmdCompleteCBs
 *
 * Process an incoming OSAL HCI Command Complete Event for
 * periodic advertising.
 *
 * @param   pMsg - message to process
 *
 * @return  TRUE if processed and safe to deallocate, FALSE if passed
 *          off to another task.
 */
 uint8_t gapScan_periodicAdvCmdCompleteCBs( hciEvt_CmdComplete_t *pMsg );

/*********************************************************************
 * gapScan_processBLEPeriodicAdvCBs
 *
 * Process an incoming OSAL HCI BLE specific events for
 * periodic advertising.
 *
 * @param   pMsg - message to process
 *
 * @return  TRUE if processed and safe to deallocate, FALSE if not processed.
 */
uint8_t gapScan_processBLEPeriodicAdvCBs( osal_event_hdr_t *pMsg );

/**
 * gapScan_periodicAdvCmdStatusCBs
 *
 * Process an incoming OSAL HCI Command Status Event for
 * periodic advertising.
 *
 * @param   pMsg - message to process
 *
 * @return  TRUE if processed and safe to deallocate, FALSE if passed
 *          off to another task.
 */
uint8_t gapScan_periodicAdvCmdStatusCBs( hciEvt_CommandStatus_t *pMsg );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* GAP_SCANNER_INTERNAL_H */
