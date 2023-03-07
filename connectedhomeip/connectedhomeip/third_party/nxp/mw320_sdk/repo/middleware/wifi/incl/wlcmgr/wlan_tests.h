/*  Copyright 2008-2020 NXP
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

/*! \file wlan_tests.h
 *  \brief WLAN Connection Manager Tests
 */

#ifndef __WLAN_TESTS_H__
#define __WLAN_TESTS_H__

/** Print the TX PWR Limit table received from Wi-Fi firmware
 *
 * \param[in] txpwrlimit A \ref wlan_txpwrlimit_t struct holding the
 * 		the TX PWR Limit table received from Wi-Fi firmware.
 *
 */
void print_txpwrlimit(wlan_txpwrlimit_t txpwrlimit);
#endif /* WLAN_TESTS_H */
