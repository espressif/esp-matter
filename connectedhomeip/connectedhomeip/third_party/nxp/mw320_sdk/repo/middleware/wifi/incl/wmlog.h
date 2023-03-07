/** @file wmlog.h
 *
 *  @brief This file contains macros to print logs
 *
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

#ifndef __WMLOG_H__
#define __WMLOG_H__

#include "fsl_debug_console.h"

#ifdef CONFIG_ENABLE_ERROR_LOGS
#define wmlog_e(_mod_name_, _fmt_, ...) PRINTF("[%s]%s" _fmt_ "\n\r", _mod_name_, " Error: ", ##__VA_ARGS__)
#else
#define wmlog_e(...)
#endif /* CONFIG_ENABLE_ERROR_LOGS */

#ifdef CONFIG_ENABLE_WARNING_LOGS
#define wmlog_w(_mod_name_, _fmt_, ...) PRINTF("[%s]%s" _fmt_ "\n\r", _mod_name_, " Warn: ", ##__VA_ARGS__)
#else
#define wmlog_w(...)
#endif /* CONFIG_ENABLE_WARNING_LOGS */

/* General debug function. User can map his own debug functions to this
ne */
#define wmlog(_mod_name_, _fmt_, ...) PRINTF("[%s] " _fmt_ "\n\r", _mod_name_, ##__VA_ARGS__)

/* Function entry */
#define wmlog_entry(_fmt_, ...) PRINTF("> %s (" _fmt_ ")\n\r", __func__, ##__VA_ARGS__)

/* function exit */
#define wmlog_exit(_fmt_, ...) PRINTF("< %s" _fmt_ "\n\r", __func__, ##__VA_ARGS__)

#endif /* __WMLOG_H__ */
