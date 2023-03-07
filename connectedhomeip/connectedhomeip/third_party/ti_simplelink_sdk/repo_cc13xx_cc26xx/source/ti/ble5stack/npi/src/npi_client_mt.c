/******************************************************************************

 @file  npi_client_mt.c

 @brief Utilities to redirect outbound NPI messages to NPI RTOS thread.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2015-2022, Texas Instruments Incorporated
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


// ****************************************************************************
// includes
// ****************************************************************************
#include "inc/npi_client.h"
#include "inc/npi_data.h"
#include <icall.h>
#include <stdint.h>
#include <string.h>
#include "MT_RPC.h"

// ****************************************************************************
// defines
// ****************************************************************************

// ****************************************************************************
// typedefs
// ****************************************************************************

// ****************************************************************************
// globals
// ****************************************************************************

static ICall_EntityID localEntityID;

// ****************************************************************************
// function prototypes
// ****************************************************************************

// ----------------------------------------------------------------------------
//! \brief      Overload the MT function to Build and Send ZTool Response.
//!             This function relays outgoing MT messages to the NPI task for
//!             further handling.
//!
//! \param[in]  cmdType -  MT Command field
//! \param[in]  cmdId - MT Command ID
//! \param[in]  datalen - lenght MT command
//! \param[in]  pData - pointer to MT commnad buffer
//!
//! \return     void
// ----------------------------------------------------------------------------
void MT_BuildAndSendZToolResponse(uint8_t cmdType, uint8_t cmdId,
                                  uint8_t dataLen, uint8_t *pData)
{
    // allocate an ICall message buffer to send message to NPI task.
    uint8_t *pRspMsg = ICall_allocMsg( dataLen + MTRPC_FRAME_HDR_SZ);

    if(pRspMsg != NULL)
    {
        // populuate the MT header fields.
        pRspMsg[MTRPC_POS_LEN] = dataLen;
        pRspMsg[MTRPC_POS_CMD0] = cmdType;
        pRspMsg[MTRPC_POS_CMD1] = cmdId;

        if(dataLen > 0)
        {
            memcpy(pRspMsg + MTRPC_FRAME_HDR_SZ, pData, dataLen);
        }

        // Send the message
        ICall_sendServiceMsg(localEntityID, ICALL_SERVICE_CLASS_NPI,
                             ICALL_MSG_FORMAT_KEEP, pRspMsg);
    }

    return;
}

// ----------------------------------------------------------------------------
//! \brief      Save local ICall Entity IDs
//!
//! \param[in]  localEntityID ICall entity ID of the local stack task.
//!
//! \return     void
// ----------------------------------------------------------------------------
void NPIClient_saveLocalTaskInfo(ICall_EntityID localTaskEntityID)
{
    localEntityID = localTaskEntityID;
}

