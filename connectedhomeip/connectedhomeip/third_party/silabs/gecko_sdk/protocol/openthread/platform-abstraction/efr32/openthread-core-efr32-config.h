/*
 *  Copyright (c) 2020, The OpenThread Authors.
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
 *   This file includes all compile-time configuration constants used by
 *   efr32 applications for OpenThread.
 */

#ifndef OPENTHREAD_CORE_EFR32_CONFIG_H_
#define OPENTHREAD_CORE_EFR32_CONFIG_H_

// Use (user defined) application config file to define OpenThread configurations
#ifdef   SL_OPENTHREAD_APPLICATION_CONFIG_FILE
#include SL_OPENTHREAD_APPLICATION_CONFIG_FILE
#endif

// Use (pre-defined) stack features config file available for applications built
// with Simplicity Studio
#ifdef   SL_OPENTHREAD_STACK_FEATURES_CONFIG_FILE
#include SL_OPENTHREAD_STACK_FEATURES_CONFIG_FILE
#endif

#include "board_config.h"
#include "em_msc.h"

/**
 * @def OPENTHREAD_CONFIG_CLI_MAX_LINE_LENGTH
 *
 * The maximum size of the CLI line in bytes including the null terminator.
 *
 */
#ifndef OPENTHREAD_CONFIG_CLI_MAX_LINE_LENGTH
#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
#define OPENTHREAD_CONFIG_CLI_MAX_LINE_LENGTH 640 // Default for Test Harness certification
#else
#define OPENTHREAD_CONFIG_CLI_MAX_LINE_LENGTH 384 // Stack default
#endif
#endif

/*
 * @def OPENTHREAD_CONFIG_RADIO_915MHZ_OQPSK_SUPPORT
 *
 * Define to 1 if you want to enable physical layer to support OQPSK modulation in 915MHz band.
 * (currently not supported).
 *
 */
#if RADIO_CONFIG_915MHZ_OQPSK_SUPPORT
#define OPENTHREAD_CONFIG_RADIO_915MHZ_OQPSK_SUPPORT 1
#else
#define OPENTHREAD_CONFIG_RADIO_915MHZ_OQPSK_SUPPORT 0
#endif

/*
 * @def OPENTHREAD_CONFIG_RADIO_2P4GHZ_OQPSK_SUPPORT
 *
 * Define to 1 if you want to enable physical layer to support OQPSK modulation in 2.4GHz band.
 *
 */
#if RADIO_CONFIG_2P4GHZ_OQPSK_SUPPORT
#define OPENTHREAD_CONFIG_RADIO_2P4GHZ_OQPSK_SUPPORT 1
#else
#define OPENTHREAD_CONFIG_RADIO_2P4GHZ_OQPSK_SUPPORT 0
#endif

/*
 * @def OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_SUPPORT
 *
 * Define to 1 if you want to enable physical layer to support proprietary radio configurations.
 *
 * This configuration option is used by the Sub-GHz feature to specify proprietary radio parameters,
 * currently not defined by the Thread spec.
 */
#if RADIO_CONFIG_SUBGHZ_SUPPORT
#define OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_SUPPORT 1
#else
#define OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_SUPPORT 0
#endif

#if RADIO_CONFIG_SUBGHZ_SUPPORT
/**
 * @def OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_PAGE
 *
 * Channel Page value for (proprietary) Sub-GHz PHY using 2GFSK modulation in 915MHz band.
 *
 */
#ifndef OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_PAGE
#define OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_PAGE 	23
#endif

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MIN
 *
 * Minimum Channel number supported with (proprietary) Sub-GHz PHY using 2GFSK modulation in 915MHz band.
 *
 */
#ifndef OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MIN
#define OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MIN 	0
#endif

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MAX
 *
 * Maximum Channel number supported with (proprietary) Sub-GHz PHY using 2GFSK modulation in 915MHz band.
 *
 */
#ifndef OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MAX
#define OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MAX 	24
#endif

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MASK
 *
 * Default Channel Mask for (proprietary) Sub-GHz PHY using 2GFSK modulation in 915MHz band.
 *
 */
#ifndef OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MASK
#define OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MASK 	0x1ffffff
#endif

/**
 * @def OPENTHREAD_CONFIG_DEFAULT_CHANNEL
 *
 * Default channel to use when working with proprietary radio configurations.
 *
 */
#ifndef OPENTHREAD_CONFIG_DEFAULT_CHANNEL
#define OPENTHREAD_CONFIG_DEFAULT_CHANNEL OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MIN
#endif
#endif // RADIO_CONFIG_SUBGHZ_SUPPORT

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_INFO
 *
 * The platform-specific string to insert into the OpenThread version string.
 *
 */
#ifndef OPENTHREAD_CONFIG_PLATFORM_INFO
#define OPENTHREAD_CONFIG_PLATFORM_INFO "EFR32"
#endif

/**
  * @def OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_MAX_SERVICES
  *
  * Specifies number of service entries in the SRP client service pool.
  *
  * This config is applicable only when `OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_ENABLE` is enabled.
  *
  */
#ifndef OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_MAX_SERVICES
#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
#define OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_MAX_SERVICES 10
#else
#define OPENTHREAD_CONFIG_SRP_CLIENT_BUFFERS_MAX_SERVICES 2
#endif
#endif

/**
 * @def OPENTHREAD_CONFIG_MAC_CSL_AUTO_SYNC_ENABLE
 *
 * This setting configures CSL auto synchronization based on data poll mechanism in Thread 1.2.
 *
 */
#ifndef OPENTHREAD_CONFIG_MAC_CSL_AUTO_SYNC_ENABLE
#if OPENTHREAD_CONFIG_REFERENCE_DEVICE_ENABLE
#define OPENTHREAD_CONFIG_MAC_CSL_AUTO_SYNC_ENABLE 0
#else
#define OPENTHREAD_CONFIG_MAC_CSL_AUTO_SYNC_ENABLE 1
#endif
#endif

/**
 * @def OPENTHREAD_CONFIG_MAC_CSL_REQUEST_AHEAD_US
 *
 * Define how many microseconds ahead should MAC deliver CSL frame to SubMac.
 *
 */
#ifndef OPENTHREAD_CONFIG_MAC_CSL_REQUEST_AHEAD_US
#define OPENTHREAD_CONFIG_MAC_CSL_REQUEST_AHEAD_US 2000
#endif

/**
 * @def OPENTHREAD_CONFIG_CSL_RECEIVE_TIME_AHEAD
 *
 * Reception scheduling and ramp up time needed for the CSL receiver to be ready, in units of microseconds.
 *
 */
#ifndef OPENTHREAD_CONFIG_CSL_RECEIVE_TIME_AHEAD
#define OPENTHREAD_CONFIG_CSL_RECEIVE_TIME_AHEAD 600
#endif

/**
 * @def OPENTHREAD_CONFIG_CSL_MIN_RECEIVE_ON
 *
 * The minimum CSL receive window (in microseconds) required to receive an IEEE 802.15.4 frame.
 * - Maximum frame size with preamble: 6*2+127*2 symbols
 * - AIFS: 12 symbols
 * - Maximum ACK size with preamble: 6*2+33*2 symbols
 * - Additional frame window: 6*2+127*2 symbols
 */
#ifndef OPENTHREAD_CONFIG_CSL_MIN_RECEIVE_ON
#define OPENTHREAD_CONFIG_CSL_MIN_RECEIVE_ON 622 * 16
#endif

/*
 * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_RETRANSMIT_ENABLE
 *
 * Define to 1 if you want to enable software retransmission logic.
 *
 */
#ifndef OPENTHREAD_CONFIG_MAC_SOFTWARE_RETRANSMIT_ENABLE
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_RETRANSMIT_ENABLE OPENTHREAD_RADIO
#endif

/**
 * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_CSMA_BACKOFF_ENABLE
 *
 * Define to 1 if you want to enable software CSMA-CA backoff logic.
 * RCPs only.
 *
 */
#ifndef OPENTHREAD_CONFIG_MAC_SOFTWARE_CSMA_BACKOFF_ENABLE
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_CSMA_BACKOFF_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_SECURITY_ENABLE
 *
 * Define to 1 if you want to enable software transmission security logic.
 * RCPs only.
 *
 */
#ifndef OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_SECURITY_ENABLE
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_SECURITY_ENABLE OPENTHREAD_RADIO && (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
#endif

/**
 * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_TIMING_ENABLE
 *
 * Define to 1 to enable software transmission target time logic.
 * RCPs only.
 *
 */
#ifndef OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_TIMING_ENABLE
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_TIMING_ENABLE OPENTHREAD_RADIO && (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
#endif

/**
  * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_RX_TIMING_ENABLE
  *
  * Define to 1 to enable software reception target time logic.
  * RCPs only.
  *
  */
#ifndef OPENTHREAD_CONFIG_MAC_SOFTWARE_RX_TIMING_ENABLE
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_RX_TIMING_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_MAC_SOFTWARE_ENERGY_SCAN_ENABLE
 *
 * Define to 1 if you want to enable software energy scanning logic.
 * RCPs only.
 *
 */
#ifndef OPENTHREAD_CONFIG_MAC_SOFTWARE_ENERGY_SCAN_ENABLE
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_ENERGY_SCAN_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
 *
 * Define to 1 if you want to support microsecond timer in platform.
 *
 */
#ifndef OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
#define OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
#endif

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE
 *
 * Define to 1 to enable otPlatFlash* APIs to support non-volatile storage.
 *
 * When defined to 1, the platform MUST implement the otPlatFlash* APIs instead of the otPlatSettings* APIs.
 *
 */
#ifndef OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE
#define OPENTHREAD_CONFIG_PLATFORM_FLASH_API_ENABLE 0
#endif

/**
  * @def OPENTHREAD_CONFIG_TCP_ENABLE
  *
  * Define as 1 to enable TCPlp (low power TCP defined in Thread spec).
  *
  */
#ifndef OPENTHREAD_CONFIG_TCP_ENABLE
#define OPENTHREAD_CONFIG_TCP_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_NCP_HDLC_ENABLE
 *
 * Define to 1 to enable the NCP HDLC interface.
 *
 */
#ifndef OPENTHREAD_CONFIG_NCP_HDLC_ENABLE
#define OPENTHREAD_CONFIG_NCP_HDLC_ENABLE 1
#endif

/**
 * @def OPENTHREAD_CONFIG_NCP_CPC_ENABLE
 *
 * Define to 1 to enable NCP CPC support.
 *
 */
#ifndef OPENTHREAD_CONFIG_NCP_CPC_ENABLE
#define OPENTHREAD_CONFIG_NCP_CPC_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_NCP_SPI_ENABLE
 *
 * Define to 1 to enable NCP SPI support.
 *
 */
#ifndef OPENTHREAD_CONFIG_NCP_SPI_ENABLE
#define OPENTHREAD_CONFIG_NCP_SPI_ENABLE 0
#endif

/**
 * @def OPENTHREAD_CONFIG_MIN_SLEEP_DURATION_MS
 *
 * Minimum duration in ms below which the platform will not
 * enter a deep sleep (EM2) mode.
 *
 */
#ifndef OPENTHREAD_CONFIG_MIN_SLEEP_DURATION_MS
#define OPENTHREAD_CONFIG_MIN_SLEEP_DURATION_MS 5
#endif

/**
 * @def OPENTHREAD_CONFIG_EFR32_UART_TX_FLUSH_TIMEOUT_MS
 *
 * Maximum time to wait for a flush to complete in otPlatUartFlush().
 *
 * Value is in milliseconds
 *
 */
#ifndef OPENTHREAD_CONFIG_EFR32_UART_TX_FLUSH_TIMEOUT_MS
#define OPENTHREAD_CONFIG_EFR32_UART_TX_FLUSH_TIMEOUT_MS 500
#endif

/**
* @def OPENTHREAD_CONFIG_PSA_ITS_NVM_OFFSET
*
* This is the offset in ITS where the persistent keys are stored.
* For Silabs OT applications, this needs to be in the range of 
* 0x20000 to 0x2ffff.
*
*/
#define OPENTHREAD_CONFIG_PSA_ITS_NVM_OFFSET  0x20000

/**
 * @def OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
 *
 * This config enables key references to be used in Openthread stack instead of
 * literal keys.
 *
 * Platform needs to support PSA Crypto to enable this option.
 *
 */
#ifndef OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
#define OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE 1
#endif

/**
  * @def OPENTHREAD_CONFIG_CRYPTO_LIB
  *
  * Selects the crypto backend library for OpenThread.
  *
  * There are several options available, but we enable PSA if key references are
  * available.  Otherwise, mbedTLS is used as default (see src/core/config/crypto.h)
  *
  * - @sa OPENTHREAD_CONFIG_CRYPTO_LIB_MBEDTLS
  * - @sa OPENTHREAD_CONFIG_CRYPTO_LIB_PSA
  * - @sa OPENTHREAD_CONFIG_CRYPTO_LIB_PLATFORM
  *
  */
#if OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE
#define OPENTHREAD_CONFIG_CRYPTO_LIB OPENTHREAD_CONFIG_CRYPTO_LIB_PSA
#endif

/**
 * @def SL_OPENTHREAD_RADIO_CCA_MODE
 *
 * Defines the CCA mode to be used by the platform.
 *
 */
#ifndef SL_OPENTHREAD_RADIO_CCA_MODE
#define SL_OPENTHREAD_RADIO_CCA_MODE RAIL_IEEE802154_CCA_MODE_RSSI
#endif

#endif // OPENTHREAD_CORE_EFR32_CONFIG_H_
