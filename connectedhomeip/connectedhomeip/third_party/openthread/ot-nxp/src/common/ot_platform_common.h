/*
 *  Copyright (c) 2021, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file includes the platform-specific initializers.
 *
 */

#ifndef PLATFORM_RT_H_
#define PLATFORM_RT_H_

#include "openthread-system.h"
#include <openthread-core-config.h>
#include <stdint.h>
#include <openthread/config.h>
#include <openthread/instance.h>

#ifdef __cplusplus
extern "C" {
#endif

//#define OT_PLAT_DBG_LVL 4

#define OT_PLAT_DBG_LEVEL_NONE 0
#define OT_PLAT_DBG_LEVEL_ERR 1
#define OT_PLAT_DBG_LEVEL_WARNING 2
#define OT_PLAT_DBG_LEVEL_INFO 3
#define OT_PLAT_DBG_LEVEL_DEBUG 4

#ifndef OT_PLAT_DBG_LVL
#define OT_PLAT_DBG_LVL OT_PLAT_DBG_LEVEL_NONE
#endif

#if (OT_PLAT_DBG_LVL > OT_PLAT_DBG_LEVEL_NONE)
/* if OT_STACK_ENABLE_LOG the log module will be already enabled */
#ifndef OT_STACK_ENABLE_LOG
#define LOG_ENABLE 1
#define LOG_ENABLE_TIMESTAMP 1
#include "fsl_component_log.h"
#ifndef FREERTOS_LOG
#define LOG_MODULE_NAME ot_plat_log
LOG_MODULE_DEFINE(LOG_MODULE_NAME, kLOG_LevelDebug);
#endif
#endif
#endif

#if (OT_PLAT_DBG_LVL >= OT_PLAT_DBG_LEVEL_DEBUG)
#define OT_PLAT_DBG(fmt, ...) LOG_DBG("%s " fmt, __func__, ##__VA_ARGS__)
#define OT_PLAT_DBG_NO_FUNC(fmt, ...) LOG_DBG("%s " fmt, "", ##__VA_ARGS__)
#else
#define OT_PLAT_DBG(...)
#define OT_PLAT_DBG_NO_FUNC(...)
#endif
#if (OT_PLAT_DBG_LVL >= OT_PLAT_DBG_LEVEL_INFO)
#define OT_PLAT_INFO(fmt, ...) LOG_INF("%s " fmt, __func__, ##__VA_ARGS__)
#else
#define OT_PLAT_INFO(...)
#endif
#if (OT_PLAT_DBG_LVL >= OT_PLAT_DBG_LEVEL_WARNING)
#define OT_PLAT_WARN(fmt, ...) LOG_WRN("%s " fmt, __func__, ##__VA_ARGS__)
#else
#define OT_PLAT_WARN(...)
#endif
#if (OT_PLAT_DBG_LVL >= OT_PLAT_DBG_LEVEL_ERR)
#define OT_PLAT_ERR(fmt, ...) LOG_ERR("%s " fmt, __func__, ##__VA_ARGS__)
#else
#define OT_PLAT_ERR(...)
#endif

/**
 * This function initializes the alarm service used by OpenThread.
 *
 */
void otPlatAlarmInit(void);

/**
 * This function deinitialized the alarm service used by OpenThread.
 *
 */
void otPlatAlarmDeinit(void);

/**
 * This function performs alarm driver processing.
 *
 * @param[in]  aInstance  The OpenThread instance structure.
 *
 */
void otPlatAlarmProcess(otInstance *aInstance);

/**
 * This function initializes the radio service used by OpenThread.
 *
 */
void otPlatRadioInit(void);

/**
 * This function deinitializes the radio service used by OpenThread.
 *
 */
void otPlatRadioDeinit(void);

/**
 * This function process radio events.
 *
 */
void otPlatRadioProcess(const otInstance *aInstance);

/**
 * This function process cli event.
 */
void otPlatCliUartProcess(void);

/**
 * This function initializes the ramdom service used by OpenThread
 */
void otPlatRandomInit(void);

/**
 * This function deinitializes the ramdom service used by OpenThread
 */
void otPlatRandomDeinit(void);

#ifdef __cplusplus
} // end of extern "C"
#endif

#endif // PLATFORM_RT_H_
