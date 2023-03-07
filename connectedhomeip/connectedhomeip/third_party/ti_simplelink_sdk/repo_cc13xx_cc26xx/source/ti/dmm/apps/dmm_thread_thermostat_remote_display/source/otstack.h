/******************************************************************************

 @file otstack.h

 @brief OpenThread network parameter definitions.

 Group: CMCU, LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2021, Texas Instruments Incorporated
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

#ifndef _OTSTACK_H_
#define _OTSTACK_H_

#ifdef __cplusplus
extern "C"
{
#endif
/******************************************************************************
 Includes
 *****************************************************************************/
#include <openthread/config.h>

/******************************************************************************
 Typedefs
 *****************************************************************************/

/* OT Stack event Callback function typedef */
typedef void (*OtStack_EventsCallback_t)(uint8_t event, void *aContext);

/******************************************************************************
 Constants and definitions
 *****************************************************************************/

/**
 * Events reported to the registered application
 */
#define OT_STACK_EVENT_NWK_NOT_JOINED           0x01
#define OT_STACK_EVENT_NWK_JOIN_IN_PROGRESS     0x02
#define OT_STACK_EVENT_NWK_JOINED               0x03
#define OT_STACK_EVENT_NWK_JOINED_FAILURE       0x04
#define OT_STACK_EVENT_NWK_DATA_CHANGED         0x05
#define OT_STACK_EVENT_DEV_ROLE_CHANGED         0x06
#ifdef USE_DMM
#define OT_STACK_EVENT_NWK_PARAMS_CHANGED       0x07
#endif

/******************************************************************************
 External functions
 *****************************************************************************/

#if defined (USE_DMM)
extern void OtStack_setupStateChange(void);
#endif
/**
 * @brief Callback function registered with the netif.
 *
 * @param aFlags    A bit-field indicating specific state that
 *                  has changed.
 * @param aContext  A pointer to application-specific context.
 * @return None
 */
extern void handleNetifStateChanged(uint32_t aFlags, void *aContext);

/**
 * @brief Allows the application to register callback for ot stack events.
 *
 * @param appCB application callback function.
 * @return None
 */
extern void OtStack_registerCallback( OtStack_EventsCallback_t appCB);

/**
 * @brief Returns the current network join event of the
 *        OpenThread stack.
 *
 * @return uint8_t event value.
 */
extern uint8_t OtStack_joinState(void);

/**
 * @brief Allows the application to start the network join
 *        process via the commissioner.
 *
 * @param pskd Pre-shared key of the device.
 * @return None
 */
extern void OtStack_joinNetwork(const char* pskd);

/**
 * @brief Allows the application to start the network join process
 *        via the commissioner using the pre-configured PSKd.
 *
 * @return None
 */
extern void OtStack_joinConfiguredNetwork(void);

/**
 * @brief Allows the application to setup the thread
 *        network(thread protocol operation) after the device
 *        has successfully joined the network.
 *
 * @return bool true if successful in starting the thread
 *         protocol operation.
 */
extern bool OtStack_setupNetwork(void);

/**
 * @brief Allows the application to attempt to bring up the interface
 *        and setup the thread network (thread protocol operation)
 *        using the current device dataset.
 *
 * @return bool true if successful in starting the thread
 *         protocol operation.
 */
extern bool OtStack_setupInterfaceAndNetwork(void);

#ifdef __cplusplus
}
#endif

#endif /* _OTSTACK_H_ */
