/***************************************************************************//**
 * @file
 * @brief TrustZone Non-secure main for Gecko Bootloader.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "core/btl_reset_ns.h"
#include "api/btl_reset_info.h"
#include "communication/btl_communication.h"

// Main Bootloader implementation
int main(void)
{
  int32_t ret = BOOTLOADER_ERROR_COMMUNICATION_BASE;

  communication_init();

  ret = communication_start();
  if (ret != BOOTLOADER_OK) {
    reset_resetWithReason(BOOTLOADER_RESET_REASON_FATAL);
  }

  ret = communication_main();

  communication_shutdown();

  if ((ret == BOOTLOADER_OK)
      || (ret == BOOTLOADER_ERROR_COMMUNICATION_DONE)) {
    reset_resetWithReason(BOOTLOADER_RESET_REASON_GO);
  }

  // An error occurred in storage or communication, and a firmware upgrade
  // was not performed
  if ((ret == BOOTLOADER_ERROR_COMMUNICATION_IMAGE_ERROR)
      || (ret == BOOTLOADER_ERROR_COMMUNICATION_TIMEOUT)
      ) {
    reset_resetWithReason(BOOTLOADER_RESET_REASON_BADIMAGE);
  } else {
    reset_resetWithReason(BOOTLOADER_RESET_REASON_FATAL);
  }

  return 0;
}
