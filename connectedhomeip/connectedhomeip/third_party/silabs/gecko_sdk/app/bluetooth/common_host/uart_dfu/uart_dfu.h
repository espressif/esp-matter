/***************************************************************************//**
 * @file
 * @brief UART Dievice Firmware Update.
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

#ifndef UART_DFU_H
#define UART_DFU_H

#include <stddef.h>
#include "sl_status.h"

/***************************************************************************//**
 * Callback function prototype to indicate the update progress.
 *
 * @param[in] uploaded_size Number of bytes already transferred to the target.
 * @param[in] total_size Total size of the GBL file.
 ******************************************************************************/
typedef void (*uart_dfu_progress_t)(size_t uploaded_size, size_t total_size);

/***************************************************************************//**
 * Run UART Device Firmware Update.
 *
 * @param[in] filename Path to the GBL file to be flashed on the target device.
 * @param[in] progress Function pointer to indicate progress. Pass NULL to omit.
 *
 * @return Status of the DFU.
 * @retval SL_STATUS_OK The update finished successfully.
 * @retval SL_STATUS_IO Failure during file I/O operation, e.g. missing file.
 * @retval SL_STATUS_ABORT Update process aborted by user.
 *
 * @pre Call `sl_bt_user_reset_to_dfu()` to put the target device in DFU mode.
 *      The DFU mode is indicated by the `sl_bt_evt_dfu_boot_id` event.
 ******************************************************************************/
sl_status_t uart_dfu_run(const char *filename, uart_dfu_progress_t progress);

/***************************************************************************//**
 * Abort an ongoing DFU process.
 ******************************************************************************/
void uart_dfu_abort(void);

#endif // UART_DFU_H
