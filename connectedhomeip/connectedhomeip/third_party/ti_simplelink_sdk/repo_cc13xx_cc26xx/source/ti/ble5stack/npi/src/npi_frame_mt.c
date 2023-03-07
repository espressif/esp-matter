/******************************************************************************

 @file  npi_frame_mt.c

 @brief Network Processor Interface (NPI) data frame specific function
        implementations for the MT serial interface.

 Group: WCS LPC
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

/******************************************************************************
 Includes
 *****************************************************************************/

#include <string.h>
#include "mt_rpc.h"
#include "inc/npi_frame.h"
#include "inc/npi_rxbuf.h"

/******************************************************************************
 Constants and Definitions
 *****************************************************************************/

//! \name State values for MT protocol
//@{
#define NPIFRAMEMT_SOP_STATE 0x00
#define NPIFRAMEMT_CMD_STATE1 0x01
#define NPIFRAMEMT_CMD_STATE2 0x02
#define NPIFRAMEMT_LEN_STATE 0x03
#define NPIFRAMEMT_DATA_STATE 0x04
#define NPIFRAMEMT_FCS_STATE 0x05
//@}

//! \brief Start-of-frame delimiter for UART transport
//!
#define MT_SOF 0xFE

/******************************************************************************
 Local Variables
 *****************************************************************************/

//! \brief Call back function to call when a complete frame has been collected.
//!
static npiIncomingFrameCBack_t incomingFrameCBFunc = NULL;

//! \name MT serial collection globals
//@{
static uint8_t state = 0;
static uint8_t LEN_Token = 0;
static uint8_t FSC_Token = 0;
static uint8_t *pMsg = NULL;
static uint8_t tempDataLen;
//@}

/******************************************************************************
 Local Function Prototypes
 *****************************************************************************/

/*!----------------------------------------------------------------------------
 * \brief  Calculates FCS for MT Frame
 *
 * \param  msg_ptr   Pointer to message buffer.
 * \param  len       Length of message pointer pointed to by msg_ptr.
 *
 * \return     uint8_t   Calculated FCS.
 *---------------------------------------------------------------------------*/
static uint8_t npiframe_calcMTFCS(uint8_t *msg_ptr, uint8_t len);

/******************************************************************************
 Public Functions
 *****************************************************************************/

// ----------------------------------------------------------------------------
//! \brief      Initialize Frame module with NPI callbacks.
//!
//! \param  incomingFrameCB   Call back for complete inbound (from host)
//!                               messages
//!
//! \return     void
// ----------------------------------------------------------------------------
void NPIFrame_initialize(npiIncomingFrameCBack_t incomingFrameCB)
{
    incomingFrameCBFunc = incomingFrameCB;
}

// ----------------------------------------------------------------------------
//! \brief      Bundles message into Transport Layer frame and NPIMSG_msg_t
//!             container.  This is the MT specific version of this function.
//!
//!             This function frames the passed in buffer with an MT SOF and FCS
//!             bytes.  It then bundles the message in an NPIMSG_msg_t
//!             container.
//!
//!             Note: becauase the SOF and FCS are added, the passed in buffer
//!             is copied to a new buffer and then the passed in buffer is
//!             free'd.
//!
//! \param  pIncomingMsg     Pointer to message buffer.
//!
//! \return     void
// ----------------------------------------------------------------------------
NPIMSG_msg_t *NPIFrame_frameMsg(uint8_t *pIncomingMsg)
{
    uint8_t *payload;

    NPIMSG_msg_t *npiMsg = (NPIMSG_msg_t *)ICall_malloc(sizeof(NPIMSG_msg_t));

    if(npiMsg != NULL)
    {
        // extract the message length from the MT header bytes.
        uint8_t inMsgLen = pIncomingMsg[MTRPC_POS_LEN] + MTRPC_FRAME_HDR_SZ;

        // allocate a new buffer that is the incoming buffer length + 2
        // additional bytes for the SOF and FCS bytes.
        if((npiMsg->pBuf = (uint8_t *)ICall_allocMsg(inMsgLen + 2)) != NULL)
        {
            // mark the start of frame.
            npiMsg->pBuf[0] = MT_SOF;
            payload = npiMsg->pBuf + 1;

            // copy the incoming buffer into the newly created buffer
            memcpy(payload, pIncomingMsg, inMsgLen);

            // calculate and capture the FCS in the final byte.
            npiMsg->pBuf[inMsgLen + 1] = npiframe_calcMTFCS(npiMsg->pBuf + 1,
                                                            inMsgLen);
#if defined(NPI_SREQRSP)
            // document message type (SYNC or ASYNC) in the NPI container.
            if((pIncomingMsg[MTRPC_POS_CMD0] & MTRPC_CMD_TYPE_MASK) == MTRPC_CMD_SRSP)
            {
                npiMsg->msgType = NPIMSG_Type_SYNCRSP;
            }
            else
            {
                npiMsg->msgType = NPIMSG_Type_ASYNC;
            }
#else
            npiMsg->msgType = NPIMSG_Type_ASYNC;
#endif
            // capture the included buffer size in the NPI container.
            npiMsg->pBufSize = inMsgLen + 2;
        }
        else
        {
            // abort and free allocated memory.
            ICall_free(npiMsg);
            npiMsg = NULL;
        }
    }

    /* No matter what happened, give back incoming buffer */
    ICall_freeMsg(pIncomingMsg);

    return(npiMsg);
}

// ----------------------------------------------------------------------------
//! \brief      Collects MT message buffer.  Used during serial data receipt.
//!
//!             | SOP | Data Length  |   CMD   |   Data   |  FCS  |
//!             |  1  |     1        |    2    |  0-Len   |   1   |
//!
//! \return     void
// ----------------------------------------------------------------------------
void NPIFrame_collectFrameData(void)
{
    uint8_t ch;
    uint8_t uint8_tsInRxBuffer;

    while (NPIRxBuf_GetRxBufCount())
    {
        NPIRxBuf_ReadFromRxBuf(&ch, 1);

        switch (state)
        {
            case NPIFRAMEMT_SOP_STATE:
                if (ch == MT_SOF)
                {
                    state = NPIFRAMEMT_LEN_STATE;
                }
                break;

            case NPIFRAMEMT_LEN_STATE:
                LEN_Token = ch;

                tempDataLen = 0;

                /* Allocate memory for the data */
                pMsg = (uint8_t *) ICall_allocMsg(MTRPC_FRAME_HDR_SZ + LEN_Token);

                if (pMsg)
                {
                    pMsg[MTRPC_POS_LEN] = LEN_Token;
                    state = NPIFRAMEMT_CMD_STATE1;
                }
                else
                {
                    state = NPIFRAMEMT_SOP_STATE;
                    return;
                }
                break;

            case NPIFRAMEMT_CMD_STATE1:
                pMsg[MTRPC_POS_CMD0] = ch;
                state = NPIFRAMEMT_CMD_STATE2;
                break;

            case NPIFRAMEMT_CMD_STATE2:
                pMsg[MTRPC_POS_CMD1] = ch;
                /* If there is no data, skip to FCS state */
                if (LEN_Token)
                {
                    state = NPIFRAMEMT_DATA_STATE;
                }
                else
                {
                    state = NPIFRAMEMT_FCS_STATE;
                }
                break;

            case NPIFRAMEMT_DATA_STATE:

                /* Fill in the buffer the first uint8_t of the data */
                pMsg[MTRPC_FRAME_HDR_SZ + tempDataLen++] = ch;

                /* Check number of uint8_ts left in the Rx buffer */
                uint8_tsInRxBuffer = NPIRxBuf_GetRxBufCount();

                /* If the remain of the data is there, read them all, otherwise, just read enough */
                if (uint8_tsInRxBuffer <= LEN_Token - tempDataLen)
                {
                    NPIRxBuf_ReadFromRxBuf(&pMsg[MTRPC_FRAME_HDR_SZ + tempDataLen], uint8_tsInRxBuffer);
                    tempDataLen += uint8_tsInRxBuffer;
                }
                else
                {
                    NPIRxBuf_ReadFromRxBuf(&pMsg[MTRPC_FRAME_HDR_SZ + tempDataLen], LEN_Token - tempDataLen);
                    tempDataLen += (LEN_Token - tempDataLen);
                }

                /* If number of uint8_ts read is equal to data length, time to move on to FCS */
                if (tempDataLen == LEN_Token)
                {
                    state = NPIFRAMEMT_FCS_STATE;
                }

                break;

            case NPIFRAMEMT_FCS_STATE:

                FSC_Token = ch;

                /* Make sure it's correct */
                if ((npiframe_calcMTFCS((uint8_t *)&pMsg[0], MTRPC_FRAME_HDR_SZ + LEN_Token) == FSC_Token))
                {

                    /* Determine if it's a SYNC or ASYNC message */
                    NPIMSG_Type msgType;

#if defined(NPI_SREQRSP)
                    if ((pMsg[1] & MTRPC_CMD_TYPE_MASK) == MTRPC_CMD_SREQ)
                    {
                        msgType = NPIMSG_Type_SYNCREQ;
                    }
                    else
                    {
                        msgType = NPIMSG_Type_ASYNC;
                    }
#else
                    msgType = NPIMSG_Type_ASYNC;
#endif // NPI_SREQRSP

                    if ( incomingFrameCBFunc )
                    {
                        incomingFrameCBFunc(MTRPC_FRAME_HDR_SZ + LEN_Token, pMsg, msgType);
                    }

                }
                else
                {
                    /* deallocate the msg */
                    ICall_freeMsg(pMsg);
                }

                /* Reset the state, send or discard the buffers at this point */
                state = NPIFRAMEMT_SOP_STATE;

                break;

            default:
                break;
        }
    }
}


/******************************************************************************
 Local Functions
 *****************************************************************************/
// ----------------------------------------------------------------------------
//! \brief      Calculate the FCS of a message buffer by XOR'ing each uint8_t.
//!         Remember to exclude SOP and FCS fields, so start at the CMD field.
//!
//! \param  msg_ptr   message pointer
//! \param  len       ength (in uint8_ts) of message
//!
//! \return     uint8_t
// ----------------------------------------------------------------------------
static uint8_t npiframe_calcMTFCS(uint8_t *msg_ptr, uint8_t len)
{
    uint8_t x;
    uint8_t xorResult;

    xorResult = 0;

    for (x = 0; x < len; x++, msg_ptr++)
    {
        xorResult = xorResult ^ *msg_ptr;
    }

    return (xorResult);
}
