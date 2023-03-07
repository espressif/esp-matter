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

#include <stdio.h>
#include <stdbool.h>
#include "sl_bt_api.h"
#include "uart_dfu.h"

// The start address in the flash memory has to be 0.
#define DFU_START_ADDRESS  0

// Maximum length of the data that is flashed to the target device at once.
// It must be a multiple of 4 bytes.
#define DFU_BLOCK_SIZE    48

// Execution status flag.
static bool run_dfu = false;

/***************************************************************************//**
 * Run UART Device Firmware Update.
 ******************************************************************************/
sl_status_t uart_dfu_run(const char *filename, uart_dfu_progress_t progress)
{
  FILE *dfu_file = NULL;
  size_t dfu_file_size = 0;
  size_t uploaded_size = 0;
  size_t block_size;
  uint8_t data_block[DFU_BLOCK_SIZE];
  sl_status_t sc = SL_STATUS_OK;

  if (filename == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  // Open file.
  dfu_file = fopen(filename, "rb");
  if (dfu_file == NULL) {
    return SL_STATUS_IO;
  }

  // Get file size.
  if (fseek(dfu_file, 0, SEEK_END) != 0) {
    sc = SL_STATUS_IO;
    goto cleanup;
  }
  dfu_file_size = ftell(dfu_file);
  if (fseek(dfu_file, 0, SEEK_SET) != 0) {
    sc = SL_STATUS_IO;
    goto cleanup;
  }

  // Start file transfer.
  sc = sl_bt_dfu_flash_set_address(DFU_START_ADDRESS);
  if (sc != SL_STATUS_OK) {
    goto cleanup;
  }
  if (progress != NULL) {
    progress(uploaded_size, dfu_file_size);
  }

  run_dfu = true;

  while (uploaded_size < dfu_file_size) {
    if (!run_dfu) {
      sc = SL_STATUS_ABORT;
      goto cleanup;
    }

    block_size = dfu_file_size - uploaded_size;
    if (block_size > DFU_BLOCK_SIZE) {
      // Limit the data block size.
      block_size = DFU_BLOCK_SIZE;
    }
    if (fread(data_block, 1, block_size, dfu_file) != block_size) {
      sc = SL_STATUS_IO;
      goto cleanup;
    }

    sc = sl_bt_dfu_flash_upload(block_size, data_block);
    if (sc != SL_STATUS_OK) {
      goto cleanup;
    }
    uploaded_size += block_size;
    if (progress != NULL) {
      progress(uploaded_size, dfu_file_size);
    }
  }

  sc = sl_bt_dfu_flash_upload_finish();

  cleanup:
  run_dfu = false;
  // Close file.
  fclose(dfu_file);

  return sc;
}

void uart_dfu_abort(void)
{
  run_dfu = false;
}
