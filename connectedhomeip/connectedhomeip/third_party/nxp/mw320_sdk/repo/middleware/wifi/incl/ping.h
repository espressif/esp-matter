/** @file ping.h
 *
 *  @brief  This file provides the support for network utility ping
 */
/*
 *  Copyright 2008-2020 NXP
 *
 *  NXP CONFIDENTIAL
 *  The source code contained or described herein and all documents related to
 *  the source code ("Materials") are owned by NXP, its
 *  suppliers and/or its licensors. Title to the Materials remains with NXP,
 *  its suppliers and/or its licensors. The Materials contain
 *  trade secrets and proprietary and confidential information of NXP, its
 *  suppliers and/or its licensors. The Materials are protected by worldwide copyright
 *  and trade secret laws and treaty provisions. No part of the Materials may be
 *  used, copied, reproduced, modified, published, uploaded, posted,
 *  transmitted, distributed, or disclosed in any way without NXP's prior
 *  express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual
 *  property right is granted to or conferred upon you by disclosure or delivery
 *  of the Materials, either expressly, by implication, inducement, estoppel or
 *  otherwise. Any license under such intellectual property rights must be
 *  express and approved by NXP in writing.
 *
 */

#ifndef _PING_H_
#define _PING_H_

#include <wmlog.h>

#define ping_e(...) wmlog_e("ping", ##__VA_ARGS__)
#define ping_w(...) wmlog_w("ping", ##__VA_ARGS__)

#define PING_ID                  0xAFAF
#define PING_INTERVAL            1000
#define PING_DEFAULT_TIMEOUT_SEC 2
#define PING_DEFAULT_COUNT       10
#define PING_DEFAULT_SIZE        56
#define PING_MAX_SIZE            65507

/** Register Network Utility CLI commands.
 *
 *  Register the Network Utility CLI commands. Currently, only ping command is
 *  supported.
 *
 *  \note This function can only be called by the application after \ref
 *  wlan_init() called.
 *
 *  \return WM_SUCCESS if the CLI commands are registered
 *  \return -WM_FAIL otherwise (for example if this function
 *          was called while the CLI commands were already registered)
 */

int ping_cli_init();

/** Unregister Network Utility CLI commands.
 *
 *  Unregister the Network Utility CLI commands.
 *
 *  \return WM_SUCCESS if the CLI commands are unregistered
 *  \return -WM_FAIL otherwise
 */

int ping_cli_deinit();
#endif /*_PING_H_ */
