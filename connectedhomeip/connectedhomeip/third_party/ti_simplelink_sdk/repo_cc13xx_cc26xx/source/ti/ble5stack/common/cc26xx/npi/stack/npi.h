/******************************************************************************

 @file  npi.h

 @brief This file contains the Network Processor Interface (NPI), which
        abstracts the physical link between the Application
        Processor (AP) and the Network Processor (NP). The NPI serves as the
        HAL's client for the SPI and UART drivers, and provides
        API and callback services for its client.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2008-2022, Texas Instruments Incorporated
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

#ifndef NPI_H
#define NPI_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */
#include "hal_types.h"
#include "osal.h"

/*******************************************************************************
 * CONSTANTS
 */

#define NPI_UART_RX_BUF_SIZE           270//  This should be common to app/stack
#define NPI_UART_TX_BUF_SIZE           270//  This should be common to app/stack

/*******************************************************************************
 * TYPEDEFS
 */

typedef void (*npiCBack_t)(uint8 port, uint8 event);

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * FUNCTIONS
 */

//
// Network Processor Interface APIs
//

extern void   NPI_InitTransport(npiCBack_t npiCallback);
extern uint16 NPI_ReadTransport(uint8 *buf, uint16 len);
extern uint16 NPI_WriteTransport(uint8 *, uint16);
extern uint16 NPI_RxBufLen(void);
extern uint16 NPI_GetMaxRxBufSize(void);
extern uint16 NPI_GetMaxTxBufSize(void);
extern void NPI_RegisterTask(uint8_t taskId);

/*******************************************************************************
*/

#ifdef __cplusplus
}
#endif

#endif /* NPI_H */
