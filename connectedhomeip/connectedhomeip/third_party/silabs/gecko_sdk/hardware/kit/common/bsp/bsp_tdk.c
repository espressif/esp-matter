/***************************************************************************//**
 * @file
 * @brief Board support package for Touch Display Kit
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "bsp_tdk.h"

#include "bspconfig.h"

#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_ebi.h"

/******************************************************************************/
/*                                                                            */
/* Local function declarations                                                */
/*                                                                            */
/******************************************************************************/

static void initEbiCommon  (void);
static void initGpio       (void);

/******************************************************************************/
/*                                                                            */
/* Local variable declarations                                                */
/*                                                                            */
/******************************************************************************/

static bool BSP_ebiInitialized    = false;
static bool BSP_sramInitialized   = false;
static bool BSP_tftInitialized    = false;
static bool BSP_flashInitialized  = false;

/******************************************************************************/
/*                                                                            */
/* Global function definitions                                                */
/*                                                                            */
/******************************************************************************/

/***************************************************************************//**
 *
 * @brief
 *    Initialize the flash
 *
 ******************************************************************************/
void  BSP_initEbiFlash(void)
{
  EBI_Init_TypeDef flashInit;

  if ( BSP_flashInitialized ) {
    return;
  }

  /* Initialize common EBI configuration */
  initEbiCommon();

  /* Init Flash */
  flashInit = BSP_CONFIG_EBI_FLASHINIT_DEFAULT;
  EBI_Init(&flashInit);

  BSP_flashInitialized = true;

  return;
}

/***************************************************************************//**
 *
 * @brief
 *    Initialize the SRAM.
 *
 ******************************************************************************/
void  BSP_initEbiSram(void)
{
  EBI_Init_TypeDef sramInit;

  if ( BSP_sramInitialized ) {
    return;
  }

  /* Initialize common EBI configuration */
  initEbiCommon();

  /* Init EBI SRAM */
  sramInit = BSP_CONFIG_EBI_SRAMINIT_DEFAULT;
  EBI_Init(&sramInit);

  BSP_sramInitialized = true;

  return;
}

/***************************************************************************//**
 *
 * @brief
 *    Initialize the TFT display
 *
 ******************************************************************************/
void  BSP_initEbiTftDirectDrive(void)
{
  EBI_TFTInit_TypeDef tftInit;

  if ( BSP_tftInitialized ) {
    return;
  }

  /* Initialize common EBI configuration */
  initEbiCommon();

  /* Init EBI to TFT Direct Drive mode*/
  tftInit = BSP_CONFIG_EBI_TFTINIT_DEFAULT;
  EBI_TFTInit(&tftInit);
  EBI->ROUTEPEN |= EBI_ROUTEPEN_DATAENPEN;

  BSP_tftInitialized = true;

  return;
}

/***************************************************************************//**
 *
 * @brief
 *    Get the TFT SRAM base address
 *
 * @return
 *    Pointer to current TFT base address
 *
 ******************************************************************************/
void *BSP_getTftSRAMDirectDriveBaseAddress(void)
{
  uint32_t baseAddress;

  baseAddress  = EBI_BankAddress(BSP_CONFIG_SRAM_EBI_BANK);
  baseAddress += EBI->TFTFRAMEBASE;

  return (void *)baseAddress;
}

/***************************************************************************//**
 *
 * @brief
 *    Set the TFT SRAM base address.
 *    The TFT address is updated according to the EBI_TFTCTRL_FBCTRIG bit.
 *    Default is every VSYNC.
 *
 * @param baseAddressIn
 *    Start address for the TFT display. This has to be in the memory space
 *    corresponding to BSP_CONFIG_SRAM_EBI_BANK.
 *
 ******************************************************************************/
void BSP_setTftSRAMDirectDriveBaseAddress(void *baseAddressIn)
{
  uint32_t baseAddress;

  baseAddress       = (uint32_t)baseAddressIn;
  baseAddress      -= EBI_BankAddress(BSP_CONFIG_SRAM_EBI_BANK);

  EBI->TFTFRAMEBASE = baseAddress & _EBI_TFTFRAMEBASE_MASK;

  return;
}

/******************************************************************************/
/*                                                                            */
/* Local function definitions                                                 */
/*                                                                            */
/******************************************************************************/

/***************************************************************************//**
 *
 * @brief
 *    Initialize common EBI settings (GPIO, power and pin routing)
 *
 ******************************************************************************/
static void initEbiCommon(void)
{
  volatile uint32_t i;

  if ( BSP_ebiInitialized ) {
    return;
  }

  /* Enable GPIO */
  initGpio();

  /* Enable clocks */
  CMU_ClockEnable(cmuClock_EBI, true);

  /* Set pin routing */
  EBI->ROUTELOC0 = BSP_CONFIG_EBI_TFT_LOC << _EBI_ROUTELOC0_TFTLOC_SHIFT
                   | BSP_CONFIG_EBI_CS_LOC  << _EBI_ROUTELOC0_CSLOC_SHIFT
                   | BSP_CONFIG_EBI_EBI_LOC << _EBI_ROUTELOC0_EBILOC_SHIFT;
  EBI->ROUTELOC1 = BSP_CONFIG_EBI_A_LOC   << _EBI_ROUTELOC1_ALOC_SHIFT
                   | BSP_CONFIG_EBI_AD_LOC  << _EBI_ROUTELOC1_ADLOC_SHIFT;

  /* Enable EBI power */
  GPIO_PinOutSet(BSP_CONFIG_EBI_PWR_PORT, BSP_CONFIG_EBI_PWR_PIN);

  BSP_ebiInitialized = true;

  /* Wait for all devices to power up! */
  for ( i = 0; i < 1000; ++i ) {
    ;
  }

  return;
}

/***************************************************************************//**
 *
 * @brief
 *    Initialize GPIO according to bsp_config.h
 *
 ******************************************************************************/
static void initGpio(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* EBI Power domain pin */
  GPIO_PinModeSet(BSP_CONFIG_EBI_PWR_PORT, BSP_CONFIG_EBI_PWR_PIN, gpioModePushPull, 0);

  /* Addressing pins */
  GPIO_PinModeSet(BSP_CONFIG_EBI_A0_PORT, BSP_CONFIG_EBI_A0_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_A1_PORT, BSP_CONFIG_EBI_A1_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_A2_PORT, BSP_CONFIG_EBI_A2_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_A3_PORT, BSP_CONFIG_EBI_A3_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_A4_PORT, BSP_CONFIG_EBI_A4_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_A5_PORT, BSP_CONFIG_EBI_A5_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_A6_PORT, BSP_CONFIG_EBI_A6_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_A7_PORT, BSP_CONFIG_EBI_A7_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_A8_PORT, BSP_CONFIG_EBI_A8_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_A9_PORT, BSP_CONFIG_EBI_A9_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_A10_PORT, BSP_CONFIG_EBI_A10_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_A11_PORT, BSP_CONFIG_EBI_A11_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_A12_PORT, BSP_CONFIG_EBI_A12_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_A13_PORT, BSP_CONFIG_EBI_A13_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_A14_PORT, BSP_CONFIG_EBI_A14_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_A15_PORT, BSP_CONFIG_EBI_A15_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_A16_PORT, BSP_CONFIG_EBI_A16_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_A17_PORT, BSP_CONFIG_EBI_A17_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_A18_PORT, BSP_CONFIG_EBI_A18_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_A19_PORT, BSP_CONFIG_EBI_A19_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_A20_PORT, BSP_CONFIG_EBI_A20_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_A21_PORT, BSP_CONFIG_EBI_A21_PIN, gpioModePushPull, 0);

  /* Data pins */
  /* Blue */
  GPIO_PinModeSet(BSP_CONFIG_EBI_AD0_PORT, BSP_CONFIG_EBI_AD0_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_AD1_PORT, BSP_CONFIG_EBI_AD1_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_AD2_PORT, BSP_CONFIG_EBI_AD2_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_AD3_PORT, BSP_CONFIG_EBI_AD3_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_AD4_PORT, BSP_CONFIG_EBI_AD4_PIN, gpioModePushPull, 0);
  /* Green */
  GPIO_PinModeSet(BSP_CONFIG_EBI_AD5_PORT, BSP_CONFIG_EBI_AD5_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_AD6_PORT, BSP_CONFIG_EBI_AD6_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_AD7_PORT, BSP_CONFIG_EBI_AD7_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_AD8_PORT, BSP_CONFIG_EBI_AD8_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_AD9_PORT, BSP_CONFIG_EBI_AD9_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_AD10_PORT, BSP_CONFIG_EBI_AD10_PIN, gpioModePushPull, 0);
  /* Red */
  GPIO_PinModeSet(BSP_CONFIG_EBI_AD11_PORT, BSP_CONFIG_EBI_AD11_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_AD12_PORT, BSP_CONFIG_EBI_AD12_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_AD13_PORT, BSP_CONFIG_EBI_AD13_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_AD14_PORT, BSP_CONFIG_EBI_AD14_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_AD15_PORT, BSP_CONFIG_EBI_AD15_PIN, gpioModePushPull, 0);

  /* Chip Select pins */
  GPIO_PinModeSet(BSP_CONFIG_EBI_CS0_PORT, BSP_CONFIG_EBI_CS0_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_CS2_PORT, BSP_CONFIG_EBI_CS2_PIN, gpioModePushPull, 0);

  /* Byte lane pins */
  GPIO_PinModeSet(BSP_CONFIG_EBI_BL0_PORT, BSP_CONFIG_EBI_BL0_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_BL1_PORT, BSP_CONFIG_EBI_BL1_PIN, gpioModePushPull, 0);

  /* Write and read enable */
  GPIO_PinModeSet(BSP_CONFIG_EBI_RE_PORT, BSP_CONFIG_EBI_RE_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_WE_PORT, BSP_CONFIG_EBI_WE_PIN, gpioModePushPull, 0);

  /* Display control pins */
  GPIO_PinModeSet(BSP_CONFIG_EBI_DCLK_PORT, BSP_CONFIG_EBI_DCLK_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_DTEN_PORT, BSP_CONFIG_EBI_DTEN_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_VSNC_PORT, BSP_CONFIG_EBI_VSNC_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_CONFIG_EBI_HSNC_PORT, BSP_CONFIG_EBI_HSNC_PIN, gpioModePushPull, 0);

  return;
}
