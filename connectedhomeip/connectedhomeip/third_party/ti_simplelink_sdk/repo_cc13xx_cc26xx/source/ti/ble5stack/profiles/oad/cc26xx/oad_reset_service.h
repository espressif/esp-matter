/******************************************************************************

 @file  oad_reset_service.h

 @brief This file contains OAD Reset Service header file.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2022, Texas Instruments Incorporated
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
#ifndef OAD_RESET_SERVICE_H
#define OAD_RESET_SERVICE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "hal_types.h"

/*********************************************************************
 * CONSTANTS
 */

#define OAD_RESET_SERVICE_UUID          0xFFD0
#define OAD_RESET_CHAR_UUID             0xFFD1

#define OAD_RESET_CMD_RESET_TGT         0x00
#define OAD_RESET_CMD_START_OAD         0x01

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */


typedef struct
{
  uint16_t connHandle;
  uint16_t bim_var;
} oadResetWrite_t;


/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * Profile Callbacks
 */

// Callback when a characteristic value has changed
typedef void (*oadResetWriteCB_t)(uint16_t connHandle,
                                  uint16_t bim_var);

typedef struct
{
  oadResetWriteCB_t       pfnOadWrite; // Called when characteristic value changes.
} oadUsrAppCBs_t;

/*********************************************************************
 * FUNCTIONS
 */

/*********************************************************************
 * @fn      Reset_addService
 *
 * @brief   Initializes the Reset Service by registering GATT attributes
 *          with the GATT server. Only call this function once.
 *
 * @param   None.
 *
 * @return  Success or Failure
 */
extern uint8_t Reset_addService(oadUsrAppCBs_t *pfnOadCBs);

#ifdef __cplusplus
}
#endif

#endif /* OAD_RESET_SERVICE_H */
