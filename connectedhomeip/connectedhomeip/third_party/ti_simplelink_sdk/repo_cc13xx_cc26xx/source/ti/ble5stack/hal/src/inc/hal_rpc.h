/******************************************************************************

 @file  hal_rpc.h

 @brief This file contains information required by services that use the
        Remote Procedure Call (RPC) standard.

 Group: WCS, LPC, BTS
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

#ifndef HAL_RPC_H
#define HAL_RPC_H

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */

/* ------------------------------------------------------------------------------------------------
 *                                          Constants
 * ------------------------------------------------------------------------------------------------
 */

///////////////////////////////////////////////////////////////////////////////////////////////////
// Common
///////////////////////////////////////////////////////////////////////////////////////////////////

// RPC Command Field Type
#define RPC_CMD_POLL        0x00
#define RPC_CMD_SREQ        0x20
#define RPC_CMD_AREQ        0x40
#define RPC_CMD_SRSP        0x60
#define RPC_CMD_RES4        0x80
#define RPC_CMD_RES5        0xA0
#define RPC_CMD_RES6        0xC0
#define RPC_CMD_RES7        0xE0

// RPC Command Field Subsystem
#define RPC_SYS_RES0        0
#define RPC_SYS_SYS         1
#define RPC_SYS_MAC         2
#define RPC_SYS_NWK         3
#define RPC_SYS_AF          4
#define RPC_SYS_ZDO         5
#define RPC_SYS_SAPI        6    // Simple API
#define RPC_SYS_UTIL        7
#define RPC_SYS_DBG         8
#define RPC_SYS_APP         9
#define RPC_SYS_RCAF        10   // Remote Control Application Framework
#define RPC_SYS_RCN         11   // Remote Control Network Layer
#define RPC_SYS_RCN_CLIENT  12   // Remote Control Network Layer Client
#define RPC_SYS_BOOT        13   // Serial Bootloader
#define RPC_SYS_MAX         14   // Maximum value, must be last

/* 1st byte is the length of the data field, 2nd/3rd bytes are command field. */
#define RPC_FRAME_HDR_SZ    3

/* The 3 MSB's of the 1st command field byte are for command type. */
#define RPC_CMD_TYPE_MASK   0xE0

/* The 5 LSB's of the 1st command field byte are for the subsystem. */
#define RPC_SUBSYSTEM_MASK  0x1F

/* position of fields in the general format frame */
#define RPC_POS_LEN         0
#define RPC_POS_CMD0        1
#define RPC_POS_CMD1        2
#define RPC_POS_DAT0        3

/* Error codes */
#define RPC_SUCCESS         0     /* success */
#define RPC_ERR_SUBSYSTEM   1     /* invalid subsystem */
#define RPC_ERR_COMMAND_ID  2     /* invalid command ID */
#define RPC_ERR_PARAMETER   3     /* invalid parameter */
#define RPC_ERR_LENGTH      4     /* invalid length */

///////////////////////////////////////////////////////////////////////////////////////////////////
// UART Specific
///////////////////////////////////////////////////////////////////////////////////////////////////

#define RPC_UART_FRAME_OVHD 2

// Start of frame character value
#define RPC_UART_SOF        0xFE

///////////////////////////////////////////////////////////////////////////////////////////////////
// SPI Specific
///////////////////////////////////////////////////////////////////////////////////////////////////


/* ------------------------------------------------------------------------------------------------
 *                                           Typedefs
 * ------------------------------------------------------------------------------------------------
 */

/**************************************************************************************************
*/

#ifdef __cplusplus
};
#endif

#endif /* HAL_RPC_H */
