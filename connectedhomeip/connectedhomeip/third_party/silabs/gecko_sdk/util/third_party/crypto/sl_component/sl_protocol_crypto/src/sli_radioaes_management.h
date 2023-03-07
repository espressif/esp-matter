/**************************************************************************/ /**
 * @file
 * @brief Threadsafe utilities for RADIOAES peripheral.
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
#ifndef SLI_RADIOAES_MANAGEMENT_H
#define SLI_RADIOAES_MANAGEMENT_H

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

#include <stdint.h>
#include "sl_status.h"

#if defined(RADIOAES_PRESENT)

/***************************************************************************//**
 * @addtogroup sli_protocol_crypto
 * @{
 ******************************************************************************/

///
/// @brief Structure that represents the state of the RADIOAES peripheral
/// (in scatter-gather mode).
///
typedef struct {
  uint32_t FETCHADDR;   ///< Fetcher Address
  uint32_t PUSHADDR;    ///< Pusher Address
} sli_radioaes_state_t;

#if defined(SLI_RADIOAES_REQUIRES_MASKING)
/// Static variable containing the masking value for the RADIOAES
extern uint32_t sli_radioaes_mask;
#endif // SLI_RADIOAES_REQUIRES_MASKING

/***************************************************************************//**
 * @brief          Acquire RADIOAES access
 *
 * @return         SL_STATUS_OK if successful and resource is idle,
 *                 SL_STATUS_ISR if successful but resource was preempted, in
 *                   which case the caller is responsible for saving state,
 *                 relevant status code on error
 ******************************************************************************/
sl_status_t sli_radioaes_acquire(void);

/***************************************************************************//**
 * @brief          Release RADIOAES access
 *
 * @return         SL_STATUS_OK if successful, relevant status code on error
 ******************************************************************************/
sl_status_t sli_radioaes_release(void);

/***************************************************************************//**
 * @brief          Save RADIOAES register state to RAM
 *
 * @param ctx      Context struct to save register state into
 *
 * @return         SL_STATUS_OK if successful, relevant status code on error
 ******************************************************************************/
sl_status_t sli_radioaes_save_state(sli_radioaes_state_t *ctx);

/***************************************************************************//**
 * @brief          Restore RADIOAES register state from RAM
 *
 * @param ctx      Context struct to restore register state from
 *
 * @return         SL_STATUS_OK if successful, relevant status code on error
 ******************************************************************************/
sl_status_t sli_radioaes_restore_state(sli_radioaes_state_t *ctx);

#ifdef __cplusplus
}
#endif

/// @} (end addtogroup sli_protocol_crypto)
#endif // RADIOAES_PRESENT
/// @endcond
#endif // SLI_RADIOAES_MANAGEMENT_H
