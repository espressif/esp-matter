/******************************************************************************
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 * Puts the eeprom into Deep Power state
 *
 *****************************************************************************/

// Create a stub entry for this if we are in simulation, or if we are not using
// and EFR based micro
#if defined(EMBER_TEST) || !defined(CORTEXM3_EFR32_MICRO)

void emberEepromPowerDownInitCallback(void)
{
}

#else

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER

#if 0
  #include EMBER_AF_API_STACK

  #include EMBER_AF_API_HAL
#else
  #include "stack/include/ember-types.h"
  #include "event_control/event.h"
  #include "hal/hal.h"
#endif

#include "em_gpio.h"

#define MX25_PORT_MOSI         BSP_EXTFLASH_MOSI_PORT
#define MX25_PIN_MOSI          BSP_EXTFLASH_MOSI_PIN
#define MX25_RXLOC             (BSP_EXTFLASH_MOSI_LOC << _USART_ROUTELOC0_RXLOC_SHIFT)

#define MX25_PORT_MISO         BSP_EXTFLASH_MISO_PORT
#define MX25_PIN_MISO          BSP_EXTFLASH_MISO_PIN
#define MX25_TXLOC             (BSP_EXTFLASH_MISO_LOC << _USART_ROUTELOC0_TXLOC_SHIFT)

#define MX25_PORT_SCLK         BSP_EXTFLASH_CLK_PORT
#define MX25_PIN_SCLK          BSP_EXTFLASH_CLK_PIN
#define MX25_CLKLOC            (BSP_EXTFLASH_CLK_LOC << _USART_ROUTELOC0_CLKLOC_SHIFT)

#define MX25_PORT_CS           BSP_EXTFLASH_CS_PORT
#define MX25_PIN_CS            BSP_EXTFLASH_CS_PIN

#define MX25_USART             USART1
#define MX25_USART_CLK         cmuClock_USART1

// The amount of time specified to wait after sending a power command
#define MX25_POWER_UP_TIME_MS 10

#ifdef EMBER_AF_API_DEBUG_PRINT
  #include EMBER_AF_API_DEBUG_PRINT
#endif

// Because the EEPROM is typically used for OTA, which requires a bootloader,
// standard interaction with the EEPROM occurs in the bootloader code. However,
// if firware and a bootloader do not both fit on the on-board flash, but the
// user needs to disable the EEPROM to save power, the following plugin may be
// utilized instead, which places the MX25 EEPROM in a power-saving mode
// without requiring a bootloader.

// Configures MX25_USART to enable communication with the EEPROM
static void eepromInit(void)
{
  USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;

  // Enable clocks
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(MX25_USART_CLK, true);

  // Configure GPIO pins
  GPIO_PinModeSet(MX25_PORT_MOSI, MX25_PIN_MOSI, gpioModePushPull, 1);
  GPIO_PinModeSet(MX25_PORT_MISO, MX25_PIN_MISO, gpioModeInput, 0);
  GPIO_PinModeSet(MX25_PORT_SCLK, MX25_PIN_SCLK, gpioModePushPull, 1);
  GPIO_PinModeSet(MX25_PORT_CS, MX25_PIN_CS, gpioModePushPull, 1);

  // Configure USART
  init.msbf = true;
  USART_InitSync(MX25_USART, &init);

  MX25_USART->ROUTELOC0 = (MX25_RXLOC | MX25_TXLOC | MX25_CLKLOC);
  MX25_USART->ROUTEPEN  = (USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_CLKPEN);

  // Wait for flash to power up
  uint32_t delay = MX25_POWER_UP_TIME_MS;
  EmberStatus status = { 0 };
  // Sleep until the flash chip is powered up...
  do {
    if ((status = halCommonIdleForMilliseconds(&delay)) != EMBER_SUCCESS) {
      halCommonDelayMilliseconds(delay);
      break;
    }
  } while (delay != 0);

  // Set EM4 pin retention so chip select stays high if we enter EM4
  EMU->EM4CTRL |= EMU_EM4CTRL_EM4IORETMODE_EM4EXIT;

  return;
}

// Sends the "Deep Power" command on MX25_USART
static void deepPowerDown(void)
{
  // Assert chip select ( low ) to begin a command
  GPIO_PinOutClear(MX25_PORT_CS, MX25_PIN_CS);

  // Send Deep Power-Down command ( 0xB9 )
  USART_SpiTransfer(MX25_USART, 0xB9);

  // Deassert chip select ( high ) to complete command
  GPIO_PinOutSet(MX25_PORT_CS, MX25_PIN_CS);

  // Disable SPI communication
  USART_Reset(MX25_USART);

  // Disable the UART pins connected to the EEPROM
  GPIO_PinModeSet(MX25_PORT_MOSI, MX25_PIN_MOSI, gpioModeDisabled, 1);
  GPIO_PinModeSet(MX25_PORT_MISO, MX25_PIN_MISO, gpioModeDisabled, 0);
  GPIO_PinModeSet(MX25_PORT_SCLK, MX25_PIN_SCLK, gpioModeDisabled, 1);

  return;
}

void emberEepromPowerDownInitCallback(void)
{
// This might conflict with applications with a bootloader that uses external
// flash, the macro here is to prevent such conflict by limit it to local
// storage bootloader.
#if defined(LOCAL_STORAGE_BTL) || defined(LOCAL_STORAGE_GECKO_INFO_PAGE_BTL)
  // Ensure the eeprom is powered up to receive commands
  eepromInit();
  // place the eeprom into Deep Power mode
  deepPowerDown();
#endif
}

#endif
