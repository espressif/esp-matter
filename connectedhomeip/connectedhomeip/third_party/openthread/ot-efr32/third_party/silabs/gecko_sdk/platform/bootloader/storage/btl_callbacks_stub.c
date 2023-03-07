//
// This is a customizable callback functions file to be used when compiling the bootloader
// using UC. This file supplies dummy implementations of
// UC callbacks that the bootloader relies on.
//

#include "config/btl_config.h"
#include "em_gpio.h"
#ifdef BTL_SPI_EUSART_ENABLE
#include "btl_spi_controller_eusart_driver_cfg.h"
#else
#include "btl_spi_controller_usart_driver_cfg.h"
#endif

void storage_customInit(void)
{
#if defined(SL_EXTFLASH_HOLD_PORT)
  GPIO_PinModeSet(SL_EXTFLASH_HOLD_PORT, SL_EXTFLASH_HOLD_PIN, gpioModePushPull, 1);
#endif
#if defined(SL_EXTFLASH_WP_PORT)
  GPIO_PinModeSet(SL_EXTFLASH_WP_PORT, SL_EXTFLASH_WP_PIN, gpioModePushPull, 1);
#endif
}

void storage_customShutdown(void)
{
#if defined(SL_EXTFLASH_HOLD_PORT)
  GPIO_PinModeSet(SL_EXTFLASH_HOLD_PORT, SL_EXTFLASH_HOLD_PIN, gpioModeDisabled, 0);
#endif
#if defined(SL_EXTFLASH_WP_PORT)
  GPIO_PinModeSet(SL_EXTFLASH_WP_PORT, SL_EXTFLASH_WP_PIN, gpioModeDisabled, 0);
#endif
}
