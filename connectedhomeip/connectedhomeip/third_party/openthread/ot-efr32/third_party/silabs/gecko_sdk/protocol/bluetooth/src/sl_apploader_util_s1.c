/***************************************************************************//**
 * @file sl_apploader_util.h
 * @brief Bluetooth AppLoader Utility
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

#include "sl_apploader_util.h"
#include "em_device.h"

// The memory place in SRAM where the reset reason will be written to
#define SL_APPLOADER_RESET_REASON_COMMAND    (*((uint32_t*)(SRAM_BASE)))

// Reset reason 2 for entering DFU mode
#define SL_APPLOADER_RESET_REASON_DFU        2

// Reset the device to OTA DFU mode.
void sl_apploader_util_reset_to_ota_dfu()
{
  SL_APPLOADER_RESET_REASON_COMMAND = SL_APPLOADER_RESET_REASON_DFU;
  NVIC_SystemReset();
}
