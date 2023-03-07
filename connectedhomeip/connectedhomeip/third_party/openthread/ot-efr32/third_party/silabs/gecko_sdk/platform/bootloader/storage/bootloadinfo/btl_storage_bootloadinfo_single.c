/***************************************************************************//**
 * @file
 * @brief Bootload Info for Silicon Labs Bootloader.
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

#include "btl_storage_bootloadinfo.h"
#include "api/btl_errorcode.h"

int32_t storage_getBootloadList(int32_t slotIds[], size_t length)
{
  slotIds[0] = 0;
  for (size_t i = 1UL; i < length; i++) {
    slotIds[i] = -1;
  }
  return BOOTLOADER_OK;
}

int32_t storage_setBootloadList(int32_t slotIds[], size_t length)
{
  (void) length;
  // Allow setting slot 0, ignore setting -1
  if ((slotIds[0] != 0L) && (slotIds[0] != -1L)) {
    return BOOTLOADER_ERROR_BOOTLOAD_LIST_OVERFLOW;
  }

  // Return early
  return BOOTLOADER_OK;
}

int32_t storage_appendBootloadList(int32_t slotId)
{
  (void) slotId;
  return BOOTLOADER_ERROR_BOOTLOAD_LIST_FULL;
}
