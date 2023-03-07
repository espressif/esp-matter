/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef __SL_IPERF_H__
#define __SL_IPERF_H__

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <stddef.h>
#include <stdint.h>

#include "sl_iperf_types.h"
#include "sl_iperf_log.h"
#include "sl_iperf_config.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @addtogroup SL_IPERF iPerf
 * @{
 *****************************************************************************/

/**************************************************************************//**
 * @defgroup SL_IPERF_TYPES iPerf type definitions
 * @ingroup SL_IPERF
 * @{
 *****************************************************************************/

/// iPerf UDP server final ack size
#define SL_IPERF_UDP_SERVER_FIN_ACK_SIZE      (128U)

/// iPerf header version1 mask
#define SL_IPERF_HEADER_VERSION1              (0x80000000UL)

/// iPerf header version2 mask
#define SL_IPERF_HEADER_VERSION2              (0x04000000UL)

/// iPerf header extended mask
#define SL_IPERF_HEADER_EXTEND                (0x40000000UL)

/// iPerf header seqno 64bit mask
#define SL_IPERF_HEADER_SEQNO64B              (0x08000000UL)

/// iPerf header UDP test mask
#define SL_IPERF_HEADER_UDPTEST               (0x20000000UL)

/// iPerf header epoch start mask
#define SL_IPERF_HEADER_EPOCH_START           (0x00001000UL)

/// iPerf header triptime mask
#define SL_IPERF_HEADER_TRIPTIME              (0x00000010UL)

/// iPerf header time mode mask
#define SL_IPERF_HEADER_TIME_MODE             (0x80000000UL)

/// iPerf Server TX FINACK max count to retry
#define SL_IPERF_SERVER_UDP_TX_FINACK_COUNT   (10U)

/** @} (end SL_IPERF_TYPES) */

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Initialize the iPerf service.
 * @details Init OS objects and default contents
 *****************************************************************************/
void sl_iperf_service_init(void);

/**************************************************************************//**
 * @brief Initialize the iPerf test.
 * @details Initialize a test descriptor with default content
 * @param[in, out] test Test descriptor
 * @param[in] mode Mode
 * @param[in] protocol Protocol
 *****************************************************************************/
void sl_iperf_test_init(sl_iperf_test_t * const test,
                        sl_iperf_mode_t mode,
                        sl_iperf_protocol_t protocol);

/**************************************************************************//**
 * @brief Set the default internal logger for the test descriptor.
 * @details Helper function
 * @param[out] test Test descriptor
 *****************************************************************************/
void sl_iperf_test_set_default_logger(sl_iperf_test_t * const test);

/**************************************************************************//**
 * @brief Set the default internal test buffer.
 * @details Helper function
 * @param[out] test Test descriptor
 *****************************************************************************/
void sl_iperf_test_set_default_buff(sl_iperf_test_t * const test);

#if !defined(SL_IPERF_CMSIS_RTOS_DISABLED)
/**************************************************************************//**
 * @brief Add the iPerf test to the execution queue.
 * @details Add test to the input messagequeue.
 * @param[in] test Test descriptor
 * @return true On Success
 * @return false On Failure
 *****************************************************************************/
bool sl_iperf_test_add(sl_iperf_test_t * const test);

/**************************************************************************//**
 * @brief Get the iPerf test from the result queue.
 * @details Get the test from the output messagequeue.
 * @param[out] test Destination test descriptor
 * @return true On Success
 * @return false On Failure
 *****************************************************************************/
bool sl_iperf_test_get(sl_iperf_test_t * const test);
#endif

/** @}*/

#ifdef __cplusplus
}
#endif
#endif
