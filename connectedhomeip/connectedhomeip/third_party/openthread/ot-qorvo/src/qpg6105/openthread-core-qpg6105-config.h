/*
 *  Copyright (c) 2019, The OpenThread Authors.
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
 *   This file includes qpg6105 compile-time configuration constants for OpenThread.
 */

#ifndef OPENTHREAD_CORE_QPG6105_CONFIG_H_
#define OPENTHREAD_CORE_QPG6105_CONFIG_H_

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_INFO
 *
 * The platform-specific string to insert into the OpenThread version string.
 *
 */
#define OPENTHREAD_CONFIG_PLATFORM_INFO "QPG6105"

/**
 * @def OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
 *
 * Define to 1 if you want to enable the csl receiver feature (for Thread >= 1.2 only)
 *
 */
#ifndef OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE
#define OPENTHREAD_CONFIG_MAC_CSL_TRANSMITTER_ENABLE (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
#endif

/**
 * @def OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
 *
 * Define to 1 if you want to enable the csl receiver feature (for Thread >= 1.2 only)
 *
 */
#ifndef OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
#define OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
#endif

#if defined(OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE)
#define OPENTHREAD_CONFIG_CSL_RECEIVE_TIME_AHEAD 1150
#endif

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_ASSERT_MANAGEMENT
 *
 * The assert is managed by platform defined logic when this flag is set.
 *
 */
#ifndef OPENTHREAD_CONFIG_PLATFORM_ASSERT_MANAGEMENT
#define OPENTHREAD_CONFIG_PLATFORM_ASSERT_MANAGEMENT 1
#endif

/**
 * @def OPENTHREAD_CONFIG_DIAG_ENABLE
 *
 * Define as 1 to enable the diag feature.
 *
 */
#ifndef OPENTHREAD_CONFIG_DIAG_ENABLE
#define OPENTHREAD_CONFIG_DIAG_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
 *
 * This is required for the CSL receiver feature
 *
 */
#ifndef OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
#define OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
#endif

#if defined(OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE) && defined(DEBUG)
#define OPENTHREAD_CONFIG_MAC_CSL_DEBUG_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
 *
 * Enable the Link Metrics subject feature (for Thread >= 1.2 only)
 * A Thread Router has to be able to be a Link Metrics Subject
 *
 */
#ifndef OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
#define OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
#endif

/**
 * @def OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE
 *
 * Enable the Link Metrics subject feature (for Thread >= 1.2 only)
 *
 */
#define OPENTHREAD_CONFIG_MLE_LINK_METRICS_INITIATOR_ENABLE (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

/**
 * @def OPENTHREAD_CONFIG_DUA_ENABLE
 *
 * Enable the DUA feature (for Thread >= 1.2 only)
 *
 */
#define OPENTHREAD_CONFIG_DUA_ENABLE (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

/**
 * @def OPENTHREAD_CONFIG_MLR_ENABLE
 *
 * Enable the Multicast Listener Registration feature (MLR) (for Thread >= 1.2 only)
 *
 */
#ifndef OPENTHREAD_CONFIG_MLR_ENABLE
#define OPENTHREAD_CONFIG_MLR_ENABLE (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
#endif

/**
 * @def OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE
 *
 * Enable the Service Registry Proxy client (for Thread >= 1.3 only)
 * It also requires support for ECDSA
 *
 */
#ifndef OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE
#define OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_3)
#endif

#if defined(OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE) || defined(OPENTHREAD_CONFIG_SRP_SERVER_ENABLE)
#define OPENTHREAD_CONFIG_ECDSA_ENABLE (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_3)
#endif

/**
 * @def OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_DEFAULT_MODE
 *
 * Define the default mode (enabled or disabled) of auto-start mode.
 *
 * This config is applicable only when `OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_API_ENABLE` is enabled.
 *
 */
#ifndef OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_DEFAULT_MODE
#define OPENTHREAD_CONFIG_SRP_CLIENT_AUTO_START_DEFAULT_MODE \
    ((OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_3) && OPENTHREAD_CONFIG_SRP_CLIENT_ENABLE)
#endif

/**
 * @def OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE
 *
 * Enable the Domain Name Service client (for Thread >= 1.3 only)
 *
 */
#ifndef OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE
#define OPENTHREAD_CONFIG_DNS_CLIENT_ENABLE (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_3)
#endif

#endif // OPENTHREAD_CORE_QPG6105_CONFIG_H_
