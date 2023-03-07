/***************************************************************************//**
 * @file
 * @brief EZSP-SPI communication component for Silicon Labs Bootloader.
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

#include "config/btl_config.h"
#include "btl_ezsp_spi.h"

// ‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
// Local variables

static ImageProperties_t imageProps = {
  .contents = 0U,
  .imageCompleted = false,
  .imageVerified = false,
  .bootloaderVersion = 0,
#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
  .seUpgradeVersion = 0
#endif
};

// -‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
// Functions

void communication_init(void)
{
  bootloader_ezsp_communication_init();
}

int32_t communication_start(void)
{
  return bootloader_ezsp_communication_start();
}

int32_t communication_main(void)
{
  int32_t ret = -1;
  parser_init(PARSER_FLAG_PARSE_CUSTOM_TAGS);
  ret = bootloader_ezsp_communication_main(&imageProps,
                                           NULL,
                                           NULL);
  return ret;
}

void communication_shutdown(void)
{
  bootloader_ezsp_communication_shutdown();
}


