/*
 * Copyright (c) 2010-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * gpModule.h
 *   This file contains the MODULE ID of the components that uses the serial protocol.
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifndef _GP_MODULE_H_
#define _GP_MODULE_H_

#ifdef GP_COMP_SHMEM
#define GP_MODULE_ID_SHMEM                      23
#endif //GP_COMP_SHMEM
#ifdef GP_COMP_ASSERT
#define GP_MODULE_ID_ASSERTAPI                  29
#endif //GP_COMP_ASSERT
#ifdef GP_COMP_MACCORE
#define GP_MODULE_ID_MACCOREAPI                 109
#endif //GP_COMP_MACCORE

#ifdef GP_COMP_NVM
#define GP_MODULE_ID_NVM_API                    32
#endif // GP_COMP_NVM
#ifdef GP_COMP_RESET
#define GP_MODULE_ID_RESETAPI                    33
#endif // GP_COMP_RESET
#ifdef GP_COMP_BASECOMPS
#define GP_MODULE_ID_BASECOMPSAPI 35
#endif
#ifdef GP_COMP_GPHAL
#define GP_MODULE_ID_SNIFFERAPI                 68
#endif //GP_COMP_GPHAL
#ifdef GP_COMP_KEYSCAN
#define GP_MODULE_ID_KEYSCAN                    12
#endif //GP_COMP_KEYSCAN
#ifdef GP_COMP_RANDOM
#define GP_MODULE_ID_RANDOMAPI                  108
#endif //GP_COMP_RANDOM
#ifdef GP_COMP_RFCHANNEL
#define GP_MODULE_ID_RFCHANNELAPI               113
#endif //GP_COMP_RFCHANNEL
#ifdef GP_COMP_MACDISPATCHER
#define GP_MODULE_ID_MACDISPATCHERAPI           114
#endif //GP_COMP_MACDISPATCHER
#ifdef GP_COMP_TXMONITOR
#define GP_MODULE_ID_TXMONITOR                  122
#endif
#ifdef GP_COMP_ENCRYPTION
#define GP_MODULE_ID_ENCRYPTIONAPI              124
#endif
#ifdef GP_COMP_VERSION
#define GP_MODULE_ID_VERSIONAPI                 129
#endif
#ifdef GP_COMP_HCI
#define GP_MODULE_ID_HCIAPI                     156
#endif //GP_COMP_HCI
#ifdef GP_COMP_NVM
#define GP_MODULE_ID_NVMAPI                     181
#endif

#endif  // _GP_MODULE_H_

