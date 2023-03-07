/***************************************************************************//**
 * @file
 * @brief Board support package device initialization
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
#include "bsp_init.h"
#include "hal-config.h"

#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"

#if BSP_LED_PRESENT
#include "bsp.h"
#endif
#if HAL_IOEXP_ENABLE
#include "bsp.h"
#include "bsp_stk_ioexp.h"
#endif
#if HAL_I2CSENSOR_ENABLE
#include "i2cspm.h"
#endif
#if defined(BSP_SERIAL_APP_PORT)
#include "retargetserial.h"
#endif

// Fetch CTUNE value from USERDATA page as a manufacturing token
#define MFG_CTUNE_EN   1
#define MFG_CTUNE_ADDR 0x0FE00100UL
#define MFG_CTUNE_VAL  (*((uint16_t *) (MFG_CTUNE_ADDR)))

void BSP_initDevice(void)
{
  // Device errata
  CHIP_Init();

#if defined(_EMU_DCDCCTRL_MASK) || defined(_DCDC_CTRL_MASK)
  // Set up DC-DC converter
  BSP_initDcdc();
#endif // defined(_EMU_DCDCCTRL_MASK) || defined(_DCDC_CTRL_MASK)

  // Set up clocks
  BSP_initClocks();

  // Set up energy mode configuration
  BSP_initEmu();

#if defined(RMU_PRESENT)
  // Set reset mode for sysreset back to DEFAULT (extended), this might have
  // been changed by the bootloader to FULL reset.
  RMU->CTRL = (RMU->CTRL & ~_RMU_CTRL_SYSRMODE_MASK) | RMU_CTRL_SYSRMODE_DEFAULT;
#endif
}

#if defined(_EMU_DCDCCTRL_MASK) || defined(_DCDC_CTRL_MASK)
void BSP_initDcdc(void)
{
#if BSP_DCDC_PRESENT
  EMU_DCDCInit_TypeDef dcdcInit = BSP_DCDC_INIT;
  #if HAL_DCDC_BYPASS
  #if defined(_DCDC_CTRL_MASK)
  dcdcInit.mode = emuDcdcMode_Bypass;
  #else
  dcdcInit.dcdcMode = emuDcdcMode_Bypass;
  #endif
  #endif
  EMU_DCDCInit(&dcdcInit);
#else
  EMU_DCDCPowerOff();
#endif
}
#endif // defined(_EMU_DCDCCTRL_MASK) || defined(_DCDC_CTRL_MASK)

void BSP_initEmu(void)
{
#if HAL_EMU_ENABLE
#if defined(EMU_VSCALE_PRESENT)
  EMU_EM01Init_TypeDef em01Init = EMU_EM01INIT_DEFAULT;
  em01Init.vScaleEM01LowPowerVoltageEnable = HAL_EMU_EM01_VSCALE;
  EMU_EM01Init(&em01Init);
#endif

  EMU_EM23Init_TypeDef em23init = EMU_EM23INIT_DEFAULT;
#if HAL_EMU_EM23_VREG
  em23init.em23VregFullEn = true;
#endif
#if defined(EMU_VSCALE_PRESENT)
  #if HAL_EMU_EM23_VSCALE == HAL_EMU_EM23_VSCALE_FASTWAKEUP
  em23init.vScaleEM23Voltage = emuVScaleEM23_FastWakeup;
  #elif HAL_EMU_EM23_VSCALE == HAL_EMU_EM23_VSCALE_LOWPOWER
  em23init.vScaleEM23Voltage = emuVScaleEM23_LowPower;
  #endif
#endif
  EMU_EM23Init(&em23init);
#endif //HAL_EMU_ENABLE
}

void BSP_initClocks(void)
{
  // --------------------------------
  // Initialize HFXO if present

#if BSP_CLK_HFXO_PRESENT
  // HFXO
  CMU_HFXOInit_TypeDef hfxoInit = BSP_CLK_HFXO_INIT;
  int ctune = -1;

#if defined(_DEVINFO_MODXOCAL_HFXOCTUNE_MASK) // Series 1
  if ((DEVINFO->MODULEINFO & _DEVINFO_MODULEINFO_HFXOCALVAL_MASK) == 0) {
    ctune = DEVINFO->MODXOCAL & _DEVINFO_MODXOCAL_HFXOCTUNE_MASK;
  }
#elif defined(_DEVINFO_MODXOCAL_HFXOCTUNEXIANA_MASK) // Series 2
  if ((DEVINFO->MODULEINFO & _DEVINFO_MODULEINFO_HFXOCALVAL_MASK) == 0) {
    ctune = DEVINFO->MODXOCAL & _DEVINFO_MODXOCAL_HFXOCTUNEXIANA_MASK;
  }
#endif

  if ((ctune == -1) && (MFG_CTUNE_EN == 1) && (MFG_CTUNE_VAL != 0xFFFF)) {
    ctune = MFG_CTUNE_VAL;
  }

#if defined(BSP_CLK_HFXO_CTUNE) && BSP_CLK_HFXO_CTUNE >= 0
  if (ctune == -1) {
    ctune = BSP_CLK_HFXO_CTUNE;
  }
#endif

  if (ctune != -1) {
#if defined(_SILICON_LABS_32B_SERIES_1)
    hfxoInit.ctuneSteadyState = ctune;
#elif defined(_SILICON_LABS_32B_SERIES_2)
    hfxoInit.ctuneXoAna = ctune;
    hfxoInit.ctuneXiAna = ctune;
#endif
  }
  CMU_HFXOInit(&hfxoInit);
  SystemHFXOClockSet(BSP_CLK_HFXO_FREQ);
#endif // BSP_CLK_HFXO_PRESENT

  // --------------------------------
  // Initialize LFXO if present

#if BSP_CLK_LFXO_PRESENT
  // LFXO
  CMU_LFXOInit_TypeDef lfxoInit = BSP_CLK_LFXO_INIT;

#if defined(BSP_CLK_LFXO_CTUNE) && BSP_CLK_LFXO_CTUNE > 0

#if defined(_SILICON_LABS_32B_SERIES_2)
  lfxoInit.capTune = BSP_CLK_LFXO_CTUNE;
#elif defined(_CMU_LFXOCTRL_MASK)
  lfxoInit.ctune = BSP_CLK_LFXO_CTUNE;
#endif

#endif // BSP_CLK_LFXO_CTUNE > 0

  CMU_LFXOInit(&lfxoInit);

  // Set system LFXO frequency
  SystemLFXOClockSet(BSP_CLK_LFXO_FREQ);
#endif // BSP_CLK_LFXO_PRESENT

  // --------------------------------
  // Enable HFXO if selected as HFCLK

#if (HAL_CLK_HFCLK_SOURCE == HAL_CLK_HFCLK_SOURCE_HFXO)
  // Enable HFXO oscillator, and wait for it to be stable
  CMU_OscillatorEnable(cmuOsc_HFXO, true, true);

#if defined(HAL_CLK_HFXO_AUTOSTART) && HAL_CLK_HFXO_AUTOSTART == HAL_CLK_HFXO_AUTOSTART_SELECT
  // Automatically start and select HFXO
  CMU_HFXOAutostartEnable(0, true, true);
#else
#if defined(HAL_CLK_HFXO_AUTOSTART) && HAL_CLK_HFXO_AUTOSTART == HAL_CLK_HFXO_AUTOSTART_START
  // Automatically start HFXO
  CMU_HFXOAutostartEnable(0, true, false);
#endif // HAL_CLK_HFXO_AUTOSTART
#if defined(CMU_HF_CLK_BRANCH)
  // Use HFXO as high frequency clock
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
#else
  // Use HFXO as system clock
  CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_HFXO);
#endif
#endif // HAL_CLK_HFXO_AUTOSTART

  // HFRCO not needed when using HFXO
#if defined(_CMU_OSCENCMD_HFRCOEN_MASK)
  CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);
#endif

#elif (HAL_CLK_HFCLK_SOURCE == HAL_CLK_HFCLK_SOURCE_HFRCODPLL)
#if defined(HAL_CLK_PLL_CONFIGURATION)
  #if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
  #if HAL_CLK_PLL_CONFIGURATION == HAL_CLK_PLL_CONFIGURATION_40MHZ
    #if !BSP_CLK_LFXO_PRESENT
    #error "PLL reference set to LFXO, but no LFXO present"
    #endif
  CMU_DPLLInit_TypeDef dpllInit = CMU_DPLL_LFXO_TO_40MHZ;
  #elif HAL_CLK_PLL_CONFIGURATION == HAL_CLK_PLL_CONFIGURATION_80MHZ
    #if !BSP_CLK_HFXO_PRESENT
    #error "PLL reference set to HFXO, but no HFXO present"
    #endif
  CMU_DPLLInit_TypeDef dpllInit = CMU_DPLL_HFXO_TO_80MHZ;
  #else
    #error "Invalid DPLL configuration"
  #endif

  #elif defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2)
  #if HAL_CLK_PLL_CONFIGURATION == HAL_CLK_PLL_CONFIGURATION_76_8MHZ
    #if !BSP_CLK_HFXO_PRESENT
    #error "PLL reference set to HFXO, but no HFXO present"
    #endif
  CMU_DPLLInit_TypeDef dpllInit = CMU_DPLL_HFXO_TO_76_8MHZ;
  #else
    #error "Invalid DPLL configuration"
  #endif
  #endif // CONFIG_2

  bool dpllLock = false;
  while (!dpllLock) {
    dpllLock = CMU_DPLLLock(&dpllInit);
  }
#endif // HAL_CLK_PLL_CONFIGURATION
  CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_HFRCODPLL);

#elif (HAL_CLK_HFCLK_SOURCE == HAL_CLK_HFCLK_SOURCE_HFRCO)
  #if defined(CMU_HF_CLK_BRANCH)
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);
  #endif // CMU_HF_CLK_BRANCH
#else
  #error "Must define HAL_CLK_HFCLK_SOURCE"
#endif // HAL_CLK_HFCLK_SOURCE

  // --------------------------------
  // Enable LFXO if selected as LFCLK

#if (HAL_CLK_LFACLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO)   \
  || (HAL_CLK_LFBCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO)  \
  || (HAL_CLK_LFCCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO)  \
  || (HAL_CLK_LFECLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO)  \
  || (HAL_CLK_EM23CLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO) \
  || (HAL_CLK_EM4CLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO)  \
  || (HAL_CLK_RTCCCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO) \
  || (HAL_CLK_WDOGCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO)
  #if !BSP_CLK_LFXO_PRESENT
    #error "Cannot select LFXO when LFXO is not present"
  #endif
#endif

  // ------------------------
  // Series 0/1

  // LFA
#if defined(HAL_CLK_LFACLK_SOURCE)
#if (HAL_CLK_LFACLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO)
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
#elif (HAL_CLK_LFACLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFRCO)
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
#elif (HAL_CLK_LFACLK_SOURCE == HAL_CLK_LFCLK_SOURCE_ULFRCO)
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);
#elif (HAL_CLK_LFACLK_SOURCE == HAL_CLK_LFCLK_SOURCE_HFLE)
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_HFCLKLE);
#elif (HAL_CLK_LFACLK_SOURCE == HAL_CLK_LFCLK_SOURCE_PLFRCO)
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_PLFRCO);
#endif
#endif

  // LFB
#if defined(HAL_CLK_LFBCLK_SOURCE)
#if (HAL_CLK_LFBCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO)
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);
#elif (HAL_CLK_LFBCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFRCO)
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);
#elif (HAL_CLK_LFBCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_ULFRCO)
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_ULFRCO);
#elif (HAL_CLK_LFBCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_HFLE)
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_HFCLKLE);
#elif (HAL_CLK_LFBCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_PLFRCO)
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_PLFRCO);
#endif
#endif

  // LFC
#if defined(HAL_CLK_LFCCLK_SOURCE)
#if (HAL_CLK_LFCCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO)
  CMU_ClockSelectSet(cmuClock_LFC, cmuSelect_LFXO);
#elif (HAL_CLK_LFCCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFRCO)
  CMU_ClockSelectSet(cmuClock_LFC, cmuSelect_LFRCO);
#endif
#endif

  // LFE
#if defined(HAL_CLK_LFECLK_SOURCE)
#if (HAL_CLK_LFECLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO)
  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFXO);
#elif (HAL_CLK_LFECLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFRCO)
  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFRCO);
#elif (HAL_CLK_LFECLK_SOURCE == HAL_CLK_LFCLK_SOURCE_ULFRCO)
  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_ULFRCO);
#elif (HAL_CLK_LFECLK_SOURCE == HAL_CLK_LFCLK_SOURCE_PLFRCO)
  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_PLFRCO);
#endif
#endif

  // ------------------------
  // Series 2

#if defined(_SILICON_LABS_32B_SERIES_2) && HAL_CLK_LFRCO_PRECISION
  CMU_LFRCOSetPrecision(cmuPrecisionHigh);
#endif

  // EM23
#if defined(HAL_CLK_EM23CLK_SOURCE)
#if (HAL_CLK_EM23CLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO)
  CMU_ClockSelectSet(cmuClock_EM23GRPACLK, cmuSelect_LFXO);
#elif (HAL_CLK_EM23CLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFRCO)
  CMU_ClockSelectSet(cmuClock_EM23GRPACLK, cmuSelect_LFRCO);
#elif (HAL_CLK_EM23CLK_SOURCE == HAL_CLK_LFCLK_SOURCE_ULFRCO)
  CMU_ClockSelectSet(cmuClock_EM23GRPACLK, cmuSelect_ULFRCO);
#endif
#endif

  // EM4
#if defined(HAL_CLK_EM4CLK_SOURCE)
#if (HAL_CLK_EM4CLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO)
  CMU_ClockSelectSet(cmuClock_EM4GRPACLK, cmuSelect_LFXO);
#elif (HAL_CLK_EM4CLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFRCO)
  CMU_ClockSelectSet(cmuClock_EM4GRPACLK, cmuSelect_LFRCO);
#elif (HAL_CLK_EM4CLK_SOURCE == HAL_CLK_LFCLK_SOURCE_ULFRCO)
  CMU_ClockSelectSet(cmuClock_EM4GRPACLK, cmuSelect_ULFRCO);
#endif
#endif

  // RTCC
#if defined(HAL_CLK_RTCCCLK_SOURCE)
#if (HAL_CLK_RTCCCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO)
  CMU_ClockSelectSet(cmuClock_RTCC, cmuSelect_LFXO);
#elif (HAL_CLK_RTCCCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFRCO)
  CMU_ClockSelectSet(cmuClock_RTCC, cmuSelect_LFRCO);
#elif (HAL_CLK_RTCCCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_ULFRCO)
  CMU_ClockSelectSet(cmuClock_RTCC, cmuSelect_ULFRCO);
#endif
#endif

  // WDOG
#if defined(HAL_CLK_WDOGCLK_SOURCE)
#if (HAL_CLK_WDOGCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO)
  CMU_ClockSelectSet(cmuClock_WDOG0, cmuSelect_LFXO);
#if WDOG_COUNT > 1
  CMU_ClockSelectSet(cmuClock_WDOG1, cmuSelect_LFXO);
#endif
#elif (HAL_CLK_WDOGCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFRCO)
  CMU_ClockSelectSet(cmuClock_WDOG0, cmuSelect_LFRCO);
#if WDOG_COUNT > 1
  CMU_ClockSelectSet(cmuClock_WDOG1, cmuSelect_LFRCO);
#endif
#elif (HAL_CLK_WDOGCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_ULFRCO)
  CMU_ClockSelectSet(cmuClock_WDOG0, cmuSelect_ULFRCO);
#if WDOG_COUNT > 1
  CMU_ClockSelectSet(cmuClock_WDOG1, cmuSelect_ULFRCO);
#endif
#endif
#endif
}

void BSP_initBoard(void)
{
  // Board functionality necessarily needs GPIO
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Initialize IO expander
#if HAL_IOEXP_ENABLE
  BSP_Init(BSP_INIT_IOEXP);
#endif

  // Initialize LEDs
#if BSP_LED_PRESENT
  #if HAL_IOEXP_ENABLE
  BSP_PeripheralAccess(BSP_IOEXP_LEDS, true);
  #else
  BSP_LedsInit();
  #endif
#endif

  // Initialize sensors
#if HAL_I2CSENSOR_ENABLE
  #if HAL_IOEXP_ENABLE
  BSP_PeripheralAccess(BSP_IOEXP_SENSORS, true);
  #endif
  #if defined(BSP_I2CSENSOR_ENABLE_PORT)
  GPIO_PinModeSet(BSP_I2CSENSOR_ENABLE_PORT, BSP_I2CSENSOR_ENABLE_PIN, gpioModePushPull, 1);
  #endif

  I2CSPM_Init_TypeDef i2cInit = I2CSPM_INIT_SENSOR;
  I2CSPM_Init(&i2cInit);
#endif

  // Configure VCOM enable
#if HAL_VCOM_ENABLE || HAL_IOEXP_VCOM_ENABLE
  #if HAL_IOEXP_VCOM_ENABLE
  BSP_PeripheralAccess(BSP_IOEXP_VCOM, true);
  #endif
  #if defined(BSP_VCOM_ENABLE_PORT)
  GPIO_PinModeSet(BSP_VCOM_ENABLE_PORT, BSP_VCOM_ENABLE_PIN, gpioModePushPull, 1);
  #endif
#endif

  // Initialize serial port
#if defined(BSP_SERIAL_APP_PORT)
  RETARGET_SerialInit();
#endif
}
