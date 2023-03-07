/*
 * Copyright (C) 2016-2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//*****************************************************************************
// Includes
//*****************************************************************************
// Standard includes
#ifndef WIFI_IF_H
#define WIFI_IF_H

#include "wifi_settings.h"

//*****************************************************************************
//                 WIFI IF INTRODUCTION
//*****************************************************************************
/* This module enables an easy integration of Wi-Fi to a SimpleLink Networking
 * framework.
 * It was designed for applications that use the Wi-Fi Station role only.
 * The simple API and settings enables the user to initiate the Wi-Fi and
 * configure the provisioning method that will be used upon first connection
 * attempt.
 * Upon successful init (WIFI_IF_init()), the system enables the NWP for any
 * SL commands.
 * The NWP will be in a low power state (AUTO-CONNECT will be disabled) waiting
 * for connection request (SlNetConn_Start()).
 * User should not call sl_Start/sl_Stop when using this module. Please use
 * WIFI_IF_restart() (for reseting the NWP) or WIFI_IF_deinit() instead.
 */

//*****************************************************************************
//                 WIFI IF DEFINITIONS
//*****************************************************************************
typedef void (*ExtProv_start_f)(void*);
typedef void (*ExtProv_stop_f)(void*);

//*****************************************************************************
//                WIFI IF USER SETTINGS
//*****************************************************************************
/*
 * Defines task priority and stack size for each of the supported 3 threads:
 * 1 - SL Spawn Thread
 * 2 - SlWifiConn Thread
 * 3 - External Provisioning Thread (if needed)
 */
#define SL_SPAWN_TASK_PRIORITY      (9)
#define SL_SPAWN_STACK_SIZE         (2048)
#define WIFI_CONN_TASK_PRIORITY     (9)
#define WIFI_CONN_STACK_SIZE        (1200)
#define EXT_PROV_TASK_PRIORITY      (9)
#define EXT_PROV_STACK_SIZE         (2048)



//*****************************************************************************
//                 WIFI_IF Function Prototypes
//*****************************************************************************
extern bool  gIsProvsioning;

/*!

    \brief Allocate resources and initialize Wi-Fi, including Simpleink driver
           (SL Spawn) and SlWifiConn (WiFi Connection and provisioning thread).
           This sets the Wi-Fi in Station role, ready to connect with the SlNetConn
           API.
           According to the user settings above, a provisioning method may be enabled or
           static network configuration will be set.

    \param[in] none

    \return                 0 upon success or a negative error code

    \note  Should be called before any access to Wi-Fi (through SlNetConn).
           sl_Start will be called in this context and should not be used elsewhere
           (If NWP reset is needed, please use WIFI_IF_reset()).
           The device will stay in a low-power (idle) mode until an connection
           (to an Access Point) will be requested using the SlNetConn_Start() API.
           Any SL command can be used after a successful completion as the NWP is enabled.
           It is recommended to use this only once when the application starts and control
           the Wi-Fi connection through the SlNetConn_Start(), SlNetConn_Stop().

    \sa    WIFI_IF_deinit(), WIFI_IF_reset(), SlNetConn_Start(), SlNetConn_Stop()

*/
int WIFI_IF_init();

/*!

    \brief     Register Start and Stop callbacks and application handle that will
               be invoked whenever external provisioning is enabled

    \param[in] fStart      Callback that will start the external provisioning
    \param[in] fStop       Callback that will stop the external provisioning
    \param[in] pHandle     Application context to be passed with both callbacks

    \return    0 upon success or a negative error code
*/
int WIFI_IF_registerExtProvCallbacks(ExtProv_start_f fStart, ExtProv_stop_f fStop, void *pHandle);

/*!

    \brief      A request to reset the NWP (sl_Stop + sl_Start + update SlWifiConnSM).

    \param[in]  none

    \return     0 upon success or a negative error code

    \note  This should be called only when no connection is active i.e. when Wi-Fi is IDLE

    \sa    WIFI_IF_init(), WIFI_IF_init()
*/
int WIFI_IF_reset();

/*!

    \brief Frees Wi-Fi resources (including threads other then the SL Spawn).

    \param[in] none

    \return                 0 upon success or a negative error code

    \note  sl_Stop will be called in this context and should not be used elsewhere
           (If NWP reset is needed, please use WIFI_IF_reset().
           This API can be called whenever there is no active connections request.
           It is suggested to use this only once when the application exits.

    \sa    WIFI_IF_init(), WIFI_IF_reset(), SlNetConn_Start(), SlNetConn_Stop()
*/
int WIFI_IF_deinit();

#endif // WIFI_IF_H
