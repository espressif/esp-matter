/******************************************************************************

 @file  npi_task.h

 @brief NPI is a TI RTOS Application Thread that provides a common
        Network Processor Interface framework.

 Group: CMCU, LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2021-2022, Texas Instruments Incorporated
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

#ifndef NPI_TASK_H
#define NPI_TASK_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
// Includes
//*****************************************************************************
#include "npi_data.h"

//*****************************************************************************
// Defines
//*****************************************************************************

//*****************************************************************************
// Typedefs
//*****************************************************************************

//! \brief Call back function that a subsystem must register with NPI Task to
//         receive messages from the Host Processor
typedef void (*npiFromHostCBack_t)(_npiFrame_t *pNPIMsg);

//! \brief Call back function that a subsystem must register with NPI Task to
//         receive forwarded messages from ICall
typedef void (*npiFromICallCBack_t)(uint8_t *pGenMsg);

typedef struct
{
  uint16_t              stackSize;      //!< Configurable size of stack for NPI Task
  uint16_t              bufSize;        //!< Buffer size of Tx/Rx Transport layer buffers
  uint32_t              mrdyPinID;      //!< Pin ID Mrdy (only with Power Saving enabled)
  uint32_t              srdyPinID;      //!< Pin ID Srdy (only with Power Saving enabled)
  uint8_t               portType;       //!< NPI_SERIAL_TYPE_[UART,SPI]
  uint8_t               portBoardID;    //!< Board ID for HW, i.e. CC2650_UART0
  npiInterfaceParams    portParams;     //!< Params to initialize NPI port
} NPI_Params;

//*****************************************************************************
// Globals
//*****************************************************************************

//*****************************************************************************
// Function Prototypes
//*****************************************************************************

// -----------------------------------------------------------------------------
//! \brief      Initialize a NPI_Params struct with default values
//!
//! \param[in]  portType  NPI_SERIAL_TYPE_[UART,SPI]
//! \param[in]  params    Pointer to NPI params to be initialized
//!
//! \return     uint8_t   Status NPI_SUCCESS, NPI_TASK_INVALID_PARAMS
// -----------------------------------------------------------------------------
extern uint8_t NPITask_Params_init(uint8_t portType, NPI_Params *params);

// -----------------------------------------------------------------------------
//! \brief      Task creation function for NPI
//!
//! \param[in]  params    Pointer to NPI params which will be used to
//!                       initialize the NPI Task
//!
//! \return     uint8_t   Status NPI_SUCCESS, or NPI_TASK_FAILURE
// -----------------------------------------------------------------------------
extern uint8_t NPITask_open(NPI_Params *params);

// -----------------------------------------------------------------------------
//! \brief      NPI Task close and tear down. Cannot be used with ICall because
//!             ICall service cannot be un-enrolled
//!
//! \return     uint8_t   Status NPI_SUCCESS, or NPI_TASK_FAILURE
// -----------------------------------------------------------------------------
extern uint8_t NPITask_close(void);

// -----------------------------------------------------------------------------
//! \brief      API for application task to send a message to the Host.
//!             NOTE: It's assumed all message traffic to the stack will use
//!             other (ICALL) APIs/Interfaces.
//!
//! \param[in]  pMsg    Pointer to "unframed" message buffer.
//!
//! \return     uint8_t Status NPI_SUCCESS, or NPI_SS_NOT_FOUND
// -----------------------------------------------------------------------------
extern uint8_t NPITask_sendToHost(_npiFrame_t *pMsg);

// -----------------------------------------------------------------------------
//! \brief      API for subsystems to register for NPI messages received with
//!             the specific ssID. All NPI messages will be passed to callback
//!             provided
//!
//! \param[in]  ssID    The subsystem ID of NPI messages that should be routed
//!                     to pCB
//! \param[in]  pCB     The call back function that will receive NPI messages
//!
//! \return     uint8_t Status NPI_SUCCESS, or NPI_ROUTING_FULL
// -----------------------------------------------------------------------------
extern uint8_t NPITask_regSSFromHostCB(uint8_t ssID, npiFromHostCBack_t pCB);

// -----------------------------------------------------------------------------
//! \brief      API for subsystems to register for ICall messages received from
//!             the specific source entity ID. All ICall messages will be passed
//!             to the callback provided
//!
//! \param[in]  icallID Source entity ID whose messages should be sent to pCB
//!             pCB     The call back function that will receive ICall messages
//!
//! \return     uint8_t Status NPI_SUCCESS, or NPI_ROUTING_FULL
// -----------------------------------------------------------------------------
extern uint8_t NPITask_regSSFromICallCB(uint8_t icallID, npiFromICallCBack_t pCB);

// -----------------------------------------------------------------------------
//! \brief      API to allocate an NPI frame of a given data length
//!
//! \param[in]  len             Length of data field of frame
//!
//! \return     _npiFrame_t *   Pointer to newly allocated frame
// -----------------------------------------------------------------------------
extern _npiFrame_t * NPITask_mallocFrame(uint16_t len);

// -----------------------------------------------------------------------------
//! \brief      API to de-allocate an NPI frame
//!
//! \param[in]  frame   Pointer to NPI frame to be de-allocated
//!
//! \return     void
// -----------------------------------------------------------------------------
extern void NPITask_freeFrame(_npiFrame_t *frame);

// -----------------------------------------------------------------------------
//! \brief   change the NPI header of the pre-defined Assert NPI message
//!          currently hardcoded set to {RPC_SYS_BLE_SNP, NPI_ASSERT_CMD1_ID}.
//!
//! \param		npi_cmd0 	NPI header cmd0 = subsystemId and command type
//! \param		npi_cmd1 	NPI header cmd1 = the command
// -----------------------------------------------------------------------------
void NPITask_chgAssertHdr(uint8_t npi_cmd0, uint8_t npi_cmd1);

// -----------------------------------------------------------------------------
//! \brief   Send a final out of memory message if there is an allocation
//!          failure to Host. Spinlock afterwards to avoid further corrupt
//!          opperation.
//!
//! \param		assertMsg 	Send assert type value
// -----------------------------------------------------------------------------
extern void NPITask_sendAssertMsg(uint8_t assertMsg);

#ifdef __cplusplus
}
#endif // extern "C"

#endif // end of NPI_TASK_H definition
