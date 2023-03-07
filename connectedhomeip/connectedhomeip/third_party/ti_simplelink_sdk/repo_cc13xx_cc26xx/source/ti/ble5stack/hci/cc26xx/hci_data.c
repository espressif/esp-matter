/******************************************************************************

 @file  hci_data.c

 @brief This file handles HCI data for the controller

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

/*******************************************************************************
 * INCLUDES
 */

#include "bcomdef.h"
#include "hal_mcu.h"
#include "osal_bufmgr.h"
#include "ll_common.h"
#include "hci_tl.h"
#include "hci_data.h"

#if defined( CC26XX ) || defined( CC13XX )
#include "rom_jt.h"
#endif // CC26XX/CC13XX

#include "rom_jt.h"

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */


/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 */


/*******************************************************************************
 * EXTERNS
 */

extern uint8  ctrlToHostEnable;
extern uint16 numHostBufs;

/*
** Data API
*/

/*******************************************************************************
 * This function is used to reverse the order of the bytes in an array in place.
 *
 * Public function defined in hci_c_data.h.
 */
void HCI_ReverseBytes( uint8 *buf,
                       uint8 len )
{
  uint8 temp;
  uint8 index = (uint8)(len - 1);
  uint8 i;

  // adjust length as only half the operations are needed
  len >>= 1;

  // reverse the order of the bytes
  for (i=0; i<len; i++)
  {
    temp           = buf[i];
    buf[i]         = buf[index - i];
    buf[index - i] = temp;
  }

  return;
}


/*
** LL Callbacks
*/

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
/*******************************************************************************
 * This Callback is used by the LL to indicate to the HCI that data has been
 * received and placed in the buffer provided by the HCI.
 *
 * Public function defined in hci_c_data.h.
 */
void LL_RxDataCompleteCback( uint16 connHandle,
                             uint8  *pBuf,
                             uint16 len,
                             uint8  fragFlag,
                             int8   rssi )
{
  // unused input parameter; PC-Lint error 715.
  (void)rssi;

  // check if Controller to Host flow control is enabled
  if ( ctrlToHostEnable == TRUE )
  {
#ifdef DEBUG
    // Sanity Check:
    // The number of available Host buffers should be one or more!
    HCI_ASSERT( numHostBufs != 0 );
#endif // DEBUG

    // decrement the number of available Host buffers, and check if exhausted
    if ( --numHostBufs == 0 )
    {
      // enable LL Rx flow control
      MAP_LL_CtrlToHostFlowControl( LL_ENABLE_RX_FLOW_CONTROL );
    }
  }

  // check if this is for the Host
  if ( hciL2capTaskID != 0 )
  {
    hciDataEvent_t *msg;

    // allocated space for OSAL message
    msg = (hciDataEvent_t *)MAP_osal_msg_allocate( sizeof ( hciDataEvent_t ) );

    if ( msg )
    {
      // message header
      msg->hdr.event  = HCI_DATA_EVENT;
      msg->hdr.status = 0xFF;

      // message data
      msg->connHandle = connHandle;
      msg->pbFlag     = FIRST_PKT_CTRL_TO_HOST;
      msg->len        = len;
      msg->pData      = pBuf;

      // send message
      (void)MAP_osal_msg_send( hciL2capTaskID, (uint8 *)msg );
    }
    else // can't queue this packet
    {
      // so free the packet buffer
      MAP_osal_bm_free( pBuf );
    }
  }
  else // not Host, so route to HCI transport layer
  {
    hciPacket_t *msg;

    // allocate an OSAL HCI message
    msg = (hciPacket_t *)MAP_osal_msg_allocate( sizeof(hciPacket_t) );

    if ( msg )
    {
      // message header
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;
      msg->pData      = pBuf-HCI_DATA_MIN_LENGTH;

      // create HCI packet
      // Note: The payload offset starting after the packet header was given to
      //       the LL for receiving data.
      msg->pData[0] = HCI_ACL_DATA_PACKET;
      msg->pData[1] = LO_UINT16(connHandle);
      msg->pData[2] = HI_UINT16(connHandle) | (fragFlag << 4); // packet boundary flags
      msg->pData[3] = LO_UINT16(len);
      msg->pData[4] = HI_UINT16(len);

      // send message
      (void)MAP_osal_msg_send( hciTaskID, (uint8 *)msg );
    }
    else // can't send this packet
    {
      // so free the packet buffer
      MAP_osal_bm_free( pBuf );
    }
  }
}
#endif // ADV_CONN_CFG | INIT_CFG


/*******************************************************************************
 */
