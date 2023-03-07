/******************************************************************************

 @file  npi_frame.h

 @brief This file contains the Network Processor Interface (NPI) data frame
        specific functions definitions.

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
#ifndef NPIFRAME_H
#define NPIFRAME_H

#ifdef __cplusplus
extern "C"
{
#endif

// ****************************************************************************
// includes
// ****************************************************************************
#include <inc/hw_types.h>
#include "npi_data.h"

// ****************************************************************************
// defines
// ****************************************************************************

// ****************************************************************************
// typedefs
// ****************************************************************************

//! \brief typedef for call back function to return a complete NPI message.
//!        The npiFrame module encapsulates the collecting/parsing of the
//!        complete message and returns via this callback the received message.
//!        NOTE: the message buffer does NOT include the framing elements
//!        (i.e. Start of Frame, FCS/CRC or similar).
typedef void (*npiIncomingFrameCBack_t)( uint16_t frameSize, uint8_t *pFrame,
                                         NPIMSG_Type msgType );


//*****************************************************************************
// globals
//*****************************************************************************

//*****************************************************************************
// function prototypes
//*****************************************************************************
// ----------------------------------------------------------------------------
//! \brief      Initialize Frame module with NPI callbacks.
//!
//! \param[in]  incomingFrameCB   Call back for complete inbound (from host)
//!                               messages
//!
//! \return     void
// ----------------------------------------------------------------------------
extern void NPIFrame_initialize(npiIncomingFrameCBack_t incomingFrameCB);


// ----------------------------------------------------------------------------
//! \brief      Bundles message into Transport Layer frame and NPIMSG_msg_t
//!             container.  A transport layer specific version of this function
//!             must be implemented.
//!
//! \param[in]  pData     Pointer to message buffer.
//!
//! \return     void
// ----------------------------------------------------------------------------
extern NPIMSG_msg_t * NPIFrame_frameMsg(uint8_t *pIncomingMsg);

// ----------------------------------------------------------------------------
//! \brief      Collects serial message buffer.  Called based on events
//!             received from the transport layer.  When an entire message has
//!             been successfully received, it is passed back to NPI task via
//!             the callback function above: npiIncomingFrameCBack_t.
//!
//! \return     void
// -----------------------------------------------------------------------------
extern void NPIFrame_collectFrameData(void);

#ifdef __cplusplus
}
#endif

#endif /* NPIFRAME_H */
