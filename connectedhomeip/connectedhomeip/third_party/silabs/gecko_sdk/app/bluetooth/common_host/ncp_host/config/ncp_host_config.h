/***************************************************************************//**
 * @file
 * @brief NCP host application module config.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef NCP_HOST_CONFIG_H
#define NCP_HOST_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Peek Delay Configuration

// <q PEEK_US_SLEEP> 1
// <i> Microsec delay used between peek function calls.
#ifndef PEEK_US_SLEEP
#define PEEK_US_SLEEP            1
#endif

// </h> End Peek Delay Configuration

// <h> Peek Timeout Configuration

// <q MSG_RECV_TIMEOUT_MS> 1
// <i> Millisec timeout for peek function calls.
#ifndef MSG_RECV_TIMEOUT_MS
#define MSG_RECV_TIMEOUT_MS      10
#endif

// </h> End Peek Delay Configuration

// <<< end of configuration section >>>

#endif // NCP_HOST_CONFIG_H
