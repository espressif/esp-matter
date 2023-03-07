/******************************************************************************

 @file  npi_task.h

 @brief NPI is a TI RTOS Application Thread that provides a common
        Network Processor Interface framework.

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
#ifndef _NPITASK_H
#define _NPITASK_H

#ifdef __cplusplus
extern "C"
{
#endif

// ****************************************************************************
// includes
// ****************************************************************************
#include "npi_data.h"

// ****************************************************************************
// defines
// ****************************************************************************


// ****************************************************************************
// typedefs
// ****************************************************************************

// -----------------------------------------------------------------------------
//! \brief      Typedef for call back function mechanism to reroute incoming NPI
//!             messages.
//!             NOTE: Definer MUST copy contents to local buffer.  NPI task will
//!             free this memory.
//!             NOTE: The contained message buffer does NOT include any "framing"
//!             bytes, ie. SOF, FCS etc.
//! \param[in]  pMsg   Pointer to "unframed" message buffer.
//!
//! \return     void
// -----------------------------------------------------------------------------
typedef void (*npiIncomingEventCBack_t)(uint8_t *pMsg);

//! \brief      numeration used to determine the type of NPI Rerouting requested
//!             by the application.
//!
//!             NONE: no rerouting despite callback non-null registered callback
//!                   func.
//!             ECHO: Sends a copy of the NPI message to both the registered
//!                   application callback as well as to the stack task.
//!             INTERCEPT: Sends the NPI message to the via the registered callback
//!                        and NOT to the stack task.
//!
typedef enum NPIEventRerouteType { NONE,
                                   ECHO,
                                   INTERCEPT } NPI_IncomingNPIEventRerouteType;



//*****************************************************************************
// globals
//*****************************************************************************

//*****************************************************************************
// function prototypes
//*****************************************************************************

// -----------------------------------------------------------------------------
//! \brief      NPI task creation function
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPITask_createTask(uint32_t stackID);

// -----------------------------------------------------------------------------
//! \brief      Register callback function to reroute incoming (from host)
//!             NPI messages.
//!
//! \param[in]  appRxCB   Callback function.
//! \param[in]  reRouteType Type of re-routing requested
//!
//! \return     void
// -----------------------------------------------------------------------------
extern void NPITask_registerIncomingRXEventAppCB(npiIncomingEventCBack_t appRxCB,
                                                 NPI_IncomingNPIEventRerouteType reRouteType);

// -----------------------------------------------------------------------------
//! \brief      Register callback function to reroute outgoing (from stack)
//!             NPI messages.
//!
//! \param[in]  appTxCB   Callback function.
//! \param[in]  reRouteType Type of re-routing requested
//!
//! \return     void
// -----------------------------------------------------------------------------
extern void NPITask_registerIncomingTXEventAppCB(npiIncomingEventCBack_t appTxCB,
                                                 NPI_IncomingNPIEventRerouteType reRouteType);

// -----------------------------------------------------------------------------
//! \brief      API for application task to send a message to the Host.
//!             NOTE: It's assumed all message traffic to the stack will use
//!             other (ICALL) APIs/Interfaces.
//!
//! \param[in]  pMsg    Pointer to "unframed" message buffer.
//!
//! \return     void
// -----------------------------------------------------------------------------
extern void NPITask_sendToHost(uint8_t *pMsg);


#ifdef __cplusplus
}
#endif // extern "C"

#endif // end of _NPITASK_H definition
