/***************************************************************************//**
 * @file
 * @brief Core functionality for Silicon Labs bootloader.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "config/btl_config.h"

#include "api/btl_interface.h"
#include "btl_core.h"
#include "em_device.h"

#ifdef BOOTLOADER_SUPPORT_STORAGE
#include "storage/btl_storage.h"
#endif

int32_t btl_init(void)
{
  int32_t retval = BOOTLOADER_OK;

#ifdef BOOTLOADER_SUPPORT_STORAGE
  retval = storage_init();
  if (retval != BOOTLOADER_OK) {
    return retval;
  }
#endif

  return retval;
}

int32_t btl_deinit(void)
{
  int32_t retval = BOOTLOADER_OK;

#ifdef BOOTLOADER_SUPPORT_STORAGE
  retval = storage_shutdown();
  if (retval != BOOTLOADER_OK) {
    return retval;
  }
#endif

  return retval;
}
