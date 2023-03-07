/***************************************************************************//**
 * @file
 * @brief EZSP-SPI communication veneer functions.
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

// -----------------------------------------------------------------------------
// Includes

#include "em_gpio.h"
#include "driver/btl_driver_delay.h"

// Configs
#include "btl_ezsp_gpio_activation_cfg.h"
#ifdef BTL_SPI_EUSART_ENABLE
#include "btl_spi_peripheral_eusart_driver_cfg.h"
#else
#include "btl_spi_peripheral_usart_driver_cfg.h"
#endif

// -----------------------------------------------------------------------------
// NSC functions

__attribute__((cmse_nonsecure_entry))
void bootloader_ezsp_init(void)
{
#if defined(_CMU_CLKEN0_MASK)
  CMU->CLKEN0_SET = CMU_CLKEN0_GPIO;
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

__attribute__((cmse_nonsecure_entry))
void bootloader_ezsp_shutdown(void)
{
#if defined(_CMU_CLKEN0_MASK)
  CMU->CLKEN0_SET = CMU_CLKEN0_GPIO;
#endif

  // Reset specific GPIO to Hi-Z
  GPIO_PinModeSet(SL_EZSPSPI_HOST_INT_PORT,
                  SL_EZSPSPI_HOST_INT_PIN,
                  gpioModeDisabled,
                  0);
  GPIO_PinModeSet(SL_EZSPSPI_WAKE_INT_PORT,
                  SL_EZSPSPI_WAKE_INT_PIN,
                  gpioModeDisabled,
                  0);

#if defined(_CMU_CLKEN0_MASK)
  CMU->CLKEN0_CLR = CMU_CLKEN0_GPIO;
#endif
}

__attribute__((cmse_nonsecure_entry))
void bootloader_ezsp_nhost_assert(void)
{
  GPIO_PinOutClear(SL_EZSPSPI_HOST_INT_PORT, SL_EZSPSPI_HOST_INT_PIN);
}

__attribute__((cmse_nonsecure_entry))
void bootloader_ezsp_nhost_deassert(void)
{
  GPIO_PinOutSet(SL_EZSPSPI_HOST_INT_PORT, SL_EZSPSPI_HOST_INT_PIN);
}

__attribute__((cmse_nonsecure_entry))
bool bootloader_ezsp_ncs_active(void)
{
  bool bootloader_ezsp_ncs_active = false;
#if defined(BTL_SPI_EUSART_ENABLE)
  bootloader_ezsp_ncs_active = (GPIO_PinInGet(SL_EUSART_SPINCP_CS_PORT, SL_EUSART_SPINCP_CS_PIN) == 0);
#else
  bootloader_ezsp_ncs_active = (GPIO_PinInGet(SL_USART_SPINCP_CS_PORT, SL_USART_SPINCP_CS_PIN) == 0);
#endif // BTL_SPI_EUSART_ENABLE
  return bootloader_ezsp_ncs_active;
}

__attribute__((cmse_nonsecure_entry))
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
    bootloader_ezsp_nhost_assert();
    while (GPIO_PinInGet(SL_EZSPSPI_WAKE_INT_PORT, SL_EZSPSPI_WAKE_INT_PIN)
           == 0) ;
    delay_microseconds(20);
    bootloader_ezsp_nhost_deassert();
    delay_microseconds(50);
  }
}