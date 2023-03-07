/***************************************************************************//**
 * @file
 * @brief EZSP-SPI communication component for Silicon Labs Bootloader.
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

#include "btl_ezsp_spi.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "driver/btl_driver_delay.h"
#include "core/btl_bootload.h"

static const BootloaderParserCallbacks_t parseCb = {
  .context = NULL,
  .applicationCallback = bootload_applicationCallback,
  // Standalone bootloading doesn't care about metadata
  .metadataCallback = NULL,
  .bootloaderCallback = bootload_bootloaderCallback
};

static ImageProperties_t imageProps = {
  .contents = 0U,
  .instructions = 0xFFU,
  .imageCompleted = false,
  .imageVerified = false,
  .bootloaderVersion = 0,
  .application = { 0 }
};

static ParserContext_t parserContext;
static DecryptContext_t decryptContext;
static AuthContext_t authContext;

// -‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
// Functions

void bootloader_ezsp_init(void)
{
  // Setup EZSP-specific GPIO (nHOST_INT, nWAKE)
#if defined(CMU_CTRL_HFPERCLKEN)
  CMU->CTRL |= CMU_CTRL_HFPERCLKEN;
  CMU_ClockEnable(cmuClock_GPIO, true);
#endif

  GPIO_PinModeSet(SL_EZSPSPI_HOST_INT_PORT,
                  SL_EZSPSPI_HOST_INT_PIN,
                  gpioModePushPull,
                  1);
  GPIO_PinModeSet(SL_EZSPSPI_WAKE_INT_PORT,
                  SL_EZSPSPI_WAKE_INT_PIN,
                  gpioModeInputPull,
                  1);
}

// Checks the WAKE line and performs handshake if needed
void bootloader_ezsp_wakeHandshake(void)
{
  unsigned int hostIntState = GPIO_PinOutGet(SL_EZSPSPI_HOST_INT_PORT,
                                             SL_EZSPSPI_HOST_INT_PIN);

  if (hostIntState == 0) {
    // AN711 10: A wake handshake cannot be performed if nHOST_INT is already
    // asserted.
    return;
  }

  // check for wake up sequence
  if (GPIO_PinInGet(SL_EZSPSPI_WAKE_INT_PORT, SL_EZSPSPI_WAKE_INT_PIN) == 0) {
    // This is a handshake. Assert nHOST_INT until WAKE deasserts
    nHOST_ASSERT();
    while (GPIO_PinInGet(SL_EZSPSPI_WAKE_INT_PORT, SL_EZSPSPI_WAKE_INT_PIN)
           == 0) ;
    delay_microseconds(20);
    nHOST_DEASSERT();
    delay_microseconds(50);
  }
}

void bootloader_ezsp_shutdown(void)
{
  // Reset specific GPIO to Hi-Z
  GPIO_PinModeSet(SL_EZSPSPI_HOST_INT_PORT,
                  SL_EZSPSPI_HOST_INT_PIN,
                  gpioModeDisabled,
                  0);
  GPIO_PinModeSet(SL_EZSPSPI_WAKE_INT_PORT,
                  SL_EZSPSPI_WAKE_INT_PIN,
                  gpioModeDisabled,
                  0);
}

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
  parser_init(&parserContext,
              &decryptContext,
              &authContext,
              PARSER_FLAG_PARSE_CUSTOM_TAGS);
  ret = bootloader_ezsp_communication_main(&imageProps,
                                           &parserContext,
                                           &parseCb);
  return ret;
}

/***************************************************************************//**
 * Stop communication between the bootloader and external host.
 *
 * @return @ref BOOTLOADER_OK on success
 ******************************************************************************/
void communication_shutdown(void)
{
  bootloader_ezsp_communication_shutdown();
}
