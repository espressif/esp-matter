/******************************************************************************

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2004-2022, Texas Instruments Incorporated
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

/**
 *  @file  comdef.h
 *  @brief      Common Defines
 */

#ifndef COMDEF_H
#define COMDEF_H

#ifdef __cplusplus
extern "C"
{
#endif


/*********************************************************************
 * INCLUDES
 */

/* HAL */
#include "hal_types.h"
#include "hal_defs.h"

/// @cond NODOC

/*********************************************************************
 * Lint Keywords
 */
#define VOID (void)

#define NULL_OK
#define INP
#define OUTP
#define UNUSED
#define ONLY
#define READONLY
#define SHARED
#define KEEP
#define RELAX

/*********************************************************************
 * CONSTANTS
 */

#ifndef false
  #define false 0
#endif

#ifndef true
  #define true 1
#endif

#ifndef CONST
  #define CONST const
#endif

#ifndef GENERIC
  #define GENERIC
#endif

/// @endcond // NODOC

/*** Generic Status Return Values ***/
#define SUCCESS                   0x00 //!< SUCCESS
#define FAILURE                   0x01 //!< Failure
#define INVALIDPARAMETER          0x02 //!< Invalid Parameter
#define INVALID_TASK              0x03 //!< Invalid Task
#define MSG_BUFFER_NOT_AVAIL      0x04 //!< No HCI Buffer is Available
#define INVALID_MSG_POINTER       0x05 //!< Invalid Message Pointer
#define INVALID_EVENT_ID          0x06 //!< Invalid Event ID
#define INVALID_INTERRUPT_ID      0x07 //!< Invalid Interupt ID
#define NO_TIMER_AVAIL            0x08 //!< No Timer Available
#define NV_ITEM_UNINIT            0x09 //!< NV Item Uninitialized
#define NV_OPER_FAILED            0x0A //!< NV Operation Failed
#define INVALID_MEM_SIZE          0x0B //!< Invalid Memory Size
#define NV_BAD_ITEM_LEN           0x0C //!< NV Bad Item Length

/*********************************************************************
 * TYPEDEFS
 */

/// @cond NODOC

// Generic Status return
typedef uint8 Status_t;

// Data types
typedef int32   int24;
typedef uint32  uint24;

/// @endcond // NODOC

/*********************************************************************
 * Global System Events
 */

#define SYS_EVENT_MSG               0x8000  //!< A message is waiting event

/*********************************************************************
 * Global Generic System Messages
 */

#define KEY_CHANGE                0xC0    //!< Key Events

// OSAL System Message IDs/Events Reserved for applications (user applications)
// 0xE0 - 0xFC

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* COMDEF_H */
