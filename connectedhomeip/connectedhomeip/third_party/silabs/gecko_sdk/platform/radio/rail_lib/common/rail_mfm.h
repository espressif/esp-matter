/***************************************************************************//**
 * @file
 * @brief The MFM specific header file for the RAIL library.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef __RAIL_MFM_H__
#define __RAIL_MFM_H__

#include "rail_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @addtogroup MFM Multi-Level Frequency Modulation
/// @ingroup Protocol_Specific
/// @brief MFM configuration routines
/// Note that this feature is only supported on EFR32xG23 devices.
///
/// This feature can be used to directly control the TX interpolation filter
/// input to allow for a more flexible frequency modulation scheme than the
/// standard MODEM. When doing this, the MFM buffer is treated as an array
/// of 8-bit signed data used as normalized frequency deviation to the SYNTH
/// frequency to directly control the interpolation filter input.
/// No support for frame handling, coding, nor shaping is supported.
/// Only compatible with FSK modulations.
///
/// The functions in this group configure RAIL Multi-Level Frequency Modulation (MFM)
/// hardware acceleration features.
///
/// To configure MFM functionality, the application must first set up
/// a RAIL instance with \ref RAIL_Init() and other setup functions.
/// Before enabling MFM, a ping-pong buffer (called buffer0 and buffer1
/// below) must be configured via \ref RAIL_SetMfmPingPongFifo() and
/// populated with the initial buffer content.
/// MFM is enabled by setting \ref RAIL_TxDataSource_t::TX_MFM_DATA using
/// \ref RAIL_ConfigData() and is activated when transmit is started by
/// \ref RAIL_StartTx(). Once transmitting the data in the ping-pong buffers,
/// RAIL will manage them so it looks like a continuous transmission to the
/// receiver. Every time one of the ping-ping buffers has been transmitted,
/// \ref RAIL_EVENT_MFM_TX_BUFFER_DONE is triggered so the application can
/// update the data in that buffer without the need to start/stop the
/// transmission. \ref RAIL_EVENT_MFM_TX_BUFFER_DONE can be enable with \ref
/// RAIL_ConfigEvents().
/// Use \ref RAIL_StopTx() to finish transmitting.
///
/// @code{.c}
///
/// uint8_t txCount = 0;
///
/// typedef struct RAIL_MFM_Config_App {
///   RAIL_MFM_PingPongBufferConfig_t buffer;
///   RAIL_StateTiming_t timings;
/// } RAIL_MFM_Config_App_t;
///
/// // Main RAIL_EVENT callback
/// static void RAILCb_Event(RAIL_Handle_t railHandle, RAIL_Events_t events)
/// {
///   // Increment TX counter
///   if (events & RAIL_EVENT_MFM_BUF_DONE) {
///       txCount++;
///       return;
///     }
///   }
/// }
///
/// static const RAIL_MFM_Config_App_t mfmConfig = {
///   .buffer = {
///     .pBuffer0 = (&channelHoppingBufferSpace[0]),
///     .pBuffer1 = (&channelHoppingBufferSpace[MFM_RAW_BUF_SZ_BYTES / 4]),
///     .bufferSizeWords = (MFM_RAW_BUF_SZ_BYTES / 4)
///   },
///   .timings = {
///     .idleToTx = 100,
///     .idleToRx = 0,
///     .rxToTx = 0,
///     .txToRx = 0,
///     .rxSearchTimeout = 0,
///     .txToRxSearchTimeout = 0
/// };
///
/// RAIL_Status_t mfmInit(void)
/// {
///   // initialize MFM
///   uint32_t idx;
///   uint32_t *pDst0 = mfmConfig.pBuffer0;
///   uint32_t *pDst1 = mfmConfig.pBuffer1;
///   RAIL_Status_t status;
///   for (idx = 0; idx < (MFM_RAW_BUF_SZ_BYTES / 16); idx++) {
///     pDst0[4 * idx + 0] = 0x755A3100;
///     pDst1[4 * idx + 0] = 0x755A3100;
///     pDst0[4 * idx + 1] = 0x315A757F;
///     pDst1[4 * idx + 1] = 0x315A757F;
///     pDst0[4 * idx + 2] = 0x8BA6CF00;
///     pDst1[4 * idx + 2] = 0x8BA6CF00;
///     pDst0[4 * idx + 3] = 0xCFA68B81;
///     pDst1[4 * idx + 3] = 0xCFA68B81;
///   }
///
///   RAIL_Status_t status;
///   railDataConfig.txSource = TX_MFM_DATA;
///   status = RAIL_SetMfmPingPongFifo(railHandle,
///                                    &(config->buffer));
///   if (status != RAIL_STATUS_NO_ERROR) {
///     return (status);
///   }
///
///
///   status = RAIL_ConfigData(railHandle, &railDataConfig);
///   if (status != RAIL_STATUS_NO_ERROR) {
///     return (status);
///   }
///
///   status = RAIL_SetStateTiming(railHandle, &(config->timings));
///   if (status != RAIL_STATUS_NO_ERROR) {
///     return (status);
///   }
///
///   // start transmitting
///   return (RAIL_StartTx(railHandle, 0, 0, &schedulerInfo));
/// }
///
/// RAIL_Status_t mfmDeInit(void)
/// {
///   RAIL_Status_t status;
///   status = RAIL_StopTx(railHandle, RAIL_STOP_MODES_ALL);
///   if (status != RAIL_STATUS_NO_ERROR) {
///     return (status);
///   }
///
///   railDataConfig.txSource = TX_PACKET_DATA;
///   return (RAIL_ConfigData(railHandle, &railDataConfig));
/// }
/// @endcode
///
/// @{

/**
 * @struct RAIL_MFM_PingPongBufferConfig_t
 * @brief A configuration structure for MFM Ping-pong buffer in RAIL.
 */
typedef struct RAIL_MFM_PingPongBufferConfig {
  /** pointer to buffer0. Must be 32-bit aligned. */
  uint32_t *pBuffer0;
  /** pointer to buffer1. Must be 32-bit aligned. */
  uint32_t *pBuffer1;
  /** size of each buffer A and B in 32-bit words. */
  uint32_t bufferSizeWords;
} RAIL_MFM_PingPongBufferConfig_t;

/**
 * Set MFM ping-pong buffer.
 *
 * @param[in] railHandle A handle of RAIL instance.
 * @param[in] config A MFM ping-pong buffer configuration structure.
 * @return A status code indicating success of the function call.
 *
 */
RAIL_Status_t RAIL_SetMfmPingPongFifo(RAIL_Handle_t railHandle,
                                      const RAIL_MFM_PingPongBufferConfig_t *config);

/** @} */ // end of MFM

#ifdef __cplusplus
}
#endif

#endif // __RAIL_MFM_H__
