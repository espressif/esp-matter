/***************************************************************************//**
 * @file
 * @brief Application interface provided to main().
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

#ifndef APP_H
#define APP_H

/// Status indication timer cycletimes in millisec.
#define DOWNLOAD_TIMER_CYCLE 1000u
#define VERIFICATION_TIMER_CYCLE 2500u

/// Returns a data blocks percentage based on a byte position.
#define GET_DATA_PERCENTAGE(storage_size, actual_byte_pos) \
  (uint8_t) (actual_byte_pos / (storage_size / 100U))      \

/// Estimate the actual transfer speed in Kbps based on the current byte
/// position in the used storage and the elapsed seconds.
#define GET_TRANSFER_SPEED_KBPS(actual_byte_pos, elapsed_sec) \
  (uint32_t) (actual_byte_pos * 8U / (1024U * elapsed_sec))   \

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
void app_init(void);

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
void app_process_action(void);

#endif // APP_H
