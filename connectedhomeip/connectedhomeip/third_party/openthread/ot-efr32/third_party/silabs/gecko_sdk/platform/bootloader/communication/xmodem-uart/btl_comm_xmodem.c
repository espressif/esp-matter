/***************************************************************************//**
 * @file
 * @brief Communication component implementing XMODEM over UART
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

#include "btl_comm_xmodem.h"
#include "communication/btl_communication.h"
#include "core/btl_bootload.h"

// -----------------------------------------------------------------------------
// Functions

void communication_init(void)
{
  bootloader_xmodem_communication_init();
}

int32_t communication_start(void)
{
  return bootloader_xmodem_communication_start();
}

int32_t communication_main(void)
{
  int32_t ret = BOOTLOADER_OK;

static ImageProperties_t imageProps = {
  .contents = 0U,
  .instructions = 0U,
  .imageCompleted = false,
  .imageVerified = false,
  .bootloaderVersion = 0,
  .application = { 0 },
#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
  .seUpgradeVersion = 0
#endif
};

static const BootloaderParserCallbacks_t parseCb = {
  .context = NULL,
  .applicationCallback = bootload_applicationCallback,
  .metadataCallback = NULL,
  .bootloaderCallback = bootload_bootloaderCallback
};

  ret = bootloader_xmodem_communication_main(&imageProps,
                                             &parseCb);
  return ret;
}

void communication_shutdown(void)
{
  // Do nothing
}
