/***************************************************************************//**
 * @brief CMSIS RTOS support code.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef _CMSIS_RTOS_SUPPORT_H_
#define _CMSIS_RTOS_SUPPORT_H_

#include <cmsis_os2.h>
#include "cmsis-rtos-support-gen.h"

#define FLAG_STACK_ACTION_PENDING                       0x01
#define FLAG_STACK_CALLBACK_PENDING                     0x02
#define FLAG_IPC_COMMAND_PENDING                        0x04
#define FLAG_IPC_RESPONSE_PENDING                       0x08

#define CMSIS_RTOS_ERROR_MASK                           0x80000000

//------------------------------------------------------------------------------
// Public APIs

void emberAfPluginCmsisRtosIpcInit(void);

void emberAfPluginCmsisRtosAcquireBufferSystemMutex(void);

void emberAfPluginCmsisRtosReleaseBufferSystemMutex(void);

//------------------------------------------------------------------------------
// Internal APIs - generic OS

void emAfPluginCmsisRtosInitTasks(void);

void emAfPluginCmsisRtosStackTask(void *p_arg);

void emAfPluginCmsisRtosAppFrameworkTask(void *p_arg);

bool emAfPluginCmsisRtosIsCurrentTaskStackTask(void);

osThreadId_t emAfPluginCmsisRtosGetStackTcb(void);

void emAfPluginCmsisRtosWakeUpConnectStackTask(void);

void emAfPluginCmsisRtosWakeUpAppFrameworkTask(void);

//------------------------------------------------------------------------------
// Internal APIs - IPC

void emAfPluginCmsisRtosIpcInit(void);

void emAfPluginCmsisRtosSendBlockingCommand(uint16_t identifier,
                                            const char *format,
                                            ...);

void emAfPluginCmsisRtosSendResponse(uint16_t identifier,
                                     const char *format,
                                     ...);

void emAfPluginCmsisRtosSendCallbackCommand(uint16_t identifier,
                                            const char *format,
                                            ...);

void emAfPluginCmsisRtosProcessIncomingApiCommand(void);

void emAfPluginCmsisRtosHandleIncomingApiCommand(uint16_t commandId);

bool emAfPluginCmsisRtosProcessIncomingCallbackCommand(void);

void emAfPluginCmsisRtosHandleIncomingCallbackCommand(uint16_t commandId,
                                                      uint8_t *callbackParams);

void emAfPluginCmsisRtosAcquireCommandMutex(void);

void emAfPluginCmsisRtosReleaseCommandMutex(void);

void emAfPluginCmsisRtosFetchApiParams(PGM_P format, ...);

void emAfPluginCmsisRtosFetchCallbackParams(uint8_t *callbackParams,
                                            PGM_P format,
                                            ...);

#endif // _CMSIS_RTOS_SUPPORT_H_
