/******************************************************************************

 @file  npi_data.h

 @brief NPI Data structures

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

#ifndef NPI_DATA_H
#define NPI_DATA_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
// Includes
//*****************************************************************************

#include <stdint.h>

#include <ti/drivers/UART.h>
#include <ti/drivers/SPI.h>

//*****************************************************************************
// Configuration
//*****************************************************************************

#ifndef NPI_FLOW_CTRL
#  ifdef POWER_SAVING
#    define NPI_FLOW_CTRL       1
#  else
#    define NPI_FLOW_CTRL       0
#  endif
#elif !(NPI_FLOW_CTRL == 0) && !(NPI_FLOW_CTRL == 1)
#  error "NPI ERROR: NPI_FLOW_CTRL can only be assigned 0 (disabled) or 1 (enabled)"
#endif

//*****************************************************************************
// Defines
//*****************************************************************************

//! \brief NPI Constants
//!
#define NPI_MSG_SOF_VAL                         0xFE

//! \brief NPI Command Type.
#define NPI_MSG_TYPE_POLL                       0x00
#define NPI_MSG_TYPE_SYNCREQ                    0x01
#define NPI_MSG_TYPE_ASYNC                      0x02
#define NPI_MSG_TYPE_SYNCRSP                    0x03

//! \brief NPI Message Lengths
#define NPI_MSG_CMD_LENGTH                      2
#define NPI_MSG_LEN_LENGTH                      2
#define NPI_MSG_HDR_LENGTH                      NPI_MSG_CMD_LENGTH + \
                                                NPI_MSG_LEN_LENGTH
//! \brief NPI Subsystem IDs
//!
#define RPC_SYS_RES0                            0
#define RPC_SYS_SYS                             1
#define RPC_SYS_MAC                             2
#define RPC_SYS_NWK                             3
#define RPC_SYS_AF                              4
#define RPC_SYS_ZDO                             5
#define RPC_SYS_SAPI                            6
#define RPC_SYS_UTIL                            7
#define RPC_SYS_DBG                             8
#define RPC_SYS_APP                             9
#define RPC_SYS_RCAF                            10
#define RPC_SYS_RCN                             11
#define RPC_SYS_RCN_CLIENT                      12
#define RPC_SYS_BOOT                            13
#define RPC_SYS_ZIPTEST                         14
#define RPC_SYS_DEBUG                           15
#define RPC_SYS_PERIPHERALS                     16
#define RPC_SYS_NFC                             17
#define RPC_SYS_PB_NWK_MGR                      18
#define RPC_SYS_PB_GW                           19
#define RPC_SYS_PB_OTA_MGR                      20
#define RPC_SYS_BLE_SNP                         21
#define RPC_SYS_BLE_HCI                         22
#define RPC_SYS_CM_CMD                          23
#define RPC_SYS_UNDEF2                          24
#define RPC_SYS_RTLS_CTRL                       25
#define RPC_SYS_UNDEF4                          26
#define RPC_SYS_UNDEF5                          27
#define RPC_SYS_UNDEF6                          28
#define RPC_SYS_UNDEF7                          29
#define RPC_SYS_UNDEF8                          30
#define RPC_SYS_SRV_CTRL                        31

//! \brief NPI Return Codes
#define NPI_SUCCESS                             0
#define NPI_ROUTING_FULL                        1
#define NPI_SS_NOT_FOUND                        2
#define NPI_INCOMPLETE_PKT                      3
#define NPI_INVALID_PKT                         4
#define NPI_BUSY                                5
#define NPI_TX_MSG_OVERSIZE                     6
#define NPI_TASK_FAILURE                        7
#define NPI_TASK_INVALID_PARAMS                 8

//! \brief Reserved Subsystem ID
#define NPI_SS_RESERVED_ID                      0x00

//! \brief Reserved ICall ID
#define NPI_ICALL_RESERVED_ID                   0x00

//! \brief Masks for cmd0 bits of NPI message
#define NPI_CMD0_TYPE_MASK                      0xE0
#define NPI_CMD0_TYPE_MASK_CLR                  0x1F
#define NPI_CMD0_SS_MASK                        0x1F
#define NPI_CMD0_SS_MASK_CLR                    0xE0

#define NPI_SERIAL_TYPE_UART                    0
#define NPI_SERIAL_TYPE_SPI                     1  // Not supported

//! \brief Returns the message type of an NPI message
#define NPI_GET_MSG_TYPE(pMsg)          ((pMsg->cmd0 & NPI_CMD0_TYPE_MASK)>> 5)

//! \brief Returns the subsystem ID of an NPI message
#define NPI_GET_SS_ID(pMsg)             ((pMsg->cmd0) & NPI_CMD0_SS_MASK)

//! \brief Sets the message type of an NPI message
#define NPI_SET_MSG_TYPE(pMsg,TYPE)     pMsg->cmd0 &= NPI_CMD0_TYPE_MASK_CLR; \
                                        pMsg->cmd0 |= ( (TYPE & 0x3) << 5 );

//! \brief Sets the subsystem ID of an NPI message
#define NPI_SET_SS_ID(pMsg,SSID)        pMsg->cmd0 &= NPI_CMD0_SS_MASK_CLR; \
                                        pMsg->cmd0 |= ( (SSID & 0x1F) );

//! \brief Sets values for default assert NPI response
#define NPI_ASSERT_PAYLOAD_LEN                  1
#define NPI_ASSERT_CMD1_ID                      0x11

//*****************************************************************************
// Typedefs
//*****************************************************************************

//! \brief Generic message NPI Frame. All messages sent over NPI Transport Layer
//         will follow this structure. Any messages passed between NPI Task and
//         subsystems will be of this type.
typedef struct _npiFrame_t
{
    uint16_t              dataLen;
    uint8_t               cmd0;
    uint8_t               cmd1;
    uint8_t               *pData;
} _npiFrame_t;

typedef union
{
  UART_Params uartParams;
  SPI_Params spiParams;                 // Not supported
} npiInterfaceParams;

//*****************************************************************************
// Globals
//*****************************************************************************

//*****************************************************************************
// Function Prototypes
//*****************************************************************************
extern void NPIData_postAssertNpiMsgEvent(uint8_t assertType);

#ifdef __cplusplus
}
#endif

#endif /* NPI_DATA_H */

