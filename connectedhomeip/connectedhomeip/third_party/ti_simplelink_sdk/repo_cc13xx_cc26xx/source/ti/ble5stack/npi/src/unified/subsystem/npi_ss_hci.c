/******************************************************************************

 @file  npi_ss_hci.c

 @brief NPI BLE HCI Subsystem

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

#include <inc/npi_data.h>
#include <inc/npi_task.h>
#include <inc/npi_ble.h>
#include <subsystem/npi_ss_ble_hci.h>

// ****************************************************************************
// defines
// ****************************************************************************

// ****************************************************************************
// typedefs
// ****************************************************************************

//*****************************************************************************
// globals
//*****************************************************************************

extern ICall_EntityID npiAppEntityID;

//*****************************************************************************
// function prototypes
//*****************************************************************************

void NPISS_BLE_HCI_msgFromHost(_npiFrame_t *pNPIMsg);

void NPISS_BLE_HCI_msgFromICall(uint8_t *pGenMsg);

// -----------------------------------------------------------------------------
//! \brief      NPI BLE Subsystem initialization function
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPISS_BLE_HCI_init(void)
{
    // Send BLE Stack the NPI Task Entity ID
    NPI_RegisterTask(npiAppEntityID);

    // Register for messages from Host with RPC_SYS_BLE ssID
    NPITask_regSSFromHostCB(RPC_SYS_BLE_HCI,NPISS_BLE_HCI_msgFromHost);

    // Register for messages from ICall
    NPITask_regSSFromICallCB(ICALL_SERVICE_CLASS_BLE,
                             NPISS_BLE_HCI_msgFromICall);
}

// -----------------------------------------------------------------------------
//! \brief      Call back function provided to NPI Task. All incoming NPI
//!             received by NPI Task with the subsystem ID of this subsystem
//!             will be sent to this call back through the NPI routing system
//!
//!             *** This function MUST free pNPIMsg
//!
//! \param[in]  pNPIMsg    Pointer to a "framed" NPI message
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPISS_BLE_HCI_msgFromHost(_npiFrame_t *pNPIMsg)
{
    // Free NPI Frame after use
    NPITask_freeFrame(pNPIMsg);
}

// -----------------------------------------------------------------------------
//! \brief      Call back function provided to NPI Task. All incoming ICall
//!             messages received by NPI Task from the src ID provided to NPI
//!             will be sent to this call back through the NPI routing system
//!
//!             *** This function MUST free pGenMsg
//!
//! \param[in]  pGenMsg    Pointer to a generic Icall msg
//!
//! \return     void
// -----------------------------------------------------------------------------
void NPISS_BLE_HCI_msgFromICall(uint8_t *pGenMsg)
{
    // Free ICall Msg after use
    ICall_free(pGenMsg);
}
