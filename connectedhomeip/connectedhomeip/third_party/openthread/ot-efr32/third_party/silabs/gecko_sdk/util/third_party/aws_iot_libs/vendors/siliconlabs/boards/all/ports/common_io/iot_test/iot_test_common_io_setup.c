/***************************************************************************//**
 * # License
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is Third Party Software licensed by Silicon Labs from a third party
 * and is governed by the sections of the MSLA applicable to Third Party
 * Software and the additional terms set forth below.
 *
 ******************************************************************************/

/*
 * FreeRTOS Common IO V0.1.2
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/*******************************************************************************
 * @file iot_test_common_io_setup.c
 * @brief Setup functions for test suite configurable global variables
 *******************************************************************************
 */

#include "iot_test_common_io_internal.h"
#include "iot_test_common_io_config.h"

#include "em_core.h"
#include "em_device.h"

#include "sl_power_manager.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "string.h"

#include "iot_adc_pinout_drv.h"

/*-----------------------------------------------------------*/

// IRQ Name depending on devices
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
#define HFXO_IRQ_NUMBER  HFXO00_IRQn
#else
#define HFXO_IRQ_NUMBER  HFXO0_IRQn
#endif


#if (IOT_TEST_COMMON_IO_ADC_SUPPORTED >= 1)
void SET_TEST_IOT_ADC_CONFIG(int testSet)
{
  /* channel list in .data section */
  static uint8_t ucAdcChList[2];

  /* unused parameter */
  (void) testSet;

  /* set invalid channel number */
  uctestInvalidChannelNo = 0;

  /* select channel for the print test */
  ucAssistedTestIotAdcChannel = slGpioPinB1;

  /* select ADC channels for loopback tests */
  ucAdcChList[0] = slGpioPinB1;
  ucAdcChList[1] = slGpioPinB2;

  /* setup channel list */
  puctestIotAdcChList   = ucAdcChList;
  uctestIotAdcChListLen = 2;

  /* setup pins as input pull-up */
  GPIO_PinModeSet(gpioPortB, 1, gpioModePushPull, 1);
  GPIO_PinModeSet(gpioPortB, 2, gpioModePushPull, 1);

  /* wait until the GPIO pin is set to high */
  vTaskDelay(10);
}
#endif

/*-----------------------------------------------------------*/

#if (IOT_TEST_COMMON_IO_BATTERY_SUPPORTED >= 1)
void SET_TEST_IOT_BATTERY_CONFIG(int testSet)
{
  /* nothing to do here */
  (void) testSet;
  return;
}
#endif

/*-----------------------------------------------------------*/

#if (IOT_TEST_COMMON_IO_EFUSE_SUPPORTED >= 1)
void SET_TEST_IOT_EFUSE_CONFIG(int testSet)
{
  /* choose where to store values */
  ultestIotEfuse16BitWordValidIdx = 1; /* stored at addr 2, and 3 */
  ultestIotEfuse32BitWordValidIdx = 1; /* stored at addr 4, 5, 6, and 7 */

  /* invalid addresses */
  ultestIotEfuse32BitWordInvalidIdx = 0xFFFF;
  ultestIotEfuse16BitWordInvalidIdx = 0xFFFF;

  /* testSet not used */
  (void) testSet;
}
#endif

/*-----------------------------------------------------------*/

#if (IOT_TEST_COMMON_IO_FLASH_SUPPORTED >= 1)
void SET_TEST_IOT_FLASH_CONFIG(int testSet)
{
  /* set flash instance */
  ltestIotFlashInstance = testSet;

  /* set flash test address */
  ultestIotFlashStartOffset = testSet ? 0 : FLASH_SIZE-2*FLASH_PAGE_SIZE;
}
#endif

/*-----------------------------------------------------------*/

#if (IOT_TEST_COMMON_IO_GPIO_SUPPORTED >= 2)
void SET_TEST_IOT_GPIO_CONFIG(int testSet)
{
  /* unused parameter */
  (void) testSet;

  /* single test config */
  ustestIotGpioConfig   = 0x0500;
  ultestIotGpioWaitTime = 10;

  /* loopback test config */
  ltestIotGpioPortA = 0;
  ltestIotGpioPortB = 1;
}
#endif

/*-----------------------------------------------------------*/

#if (IOT_TEST_COMMON_IO_I2C_SUPPORTED >= 1)
void SET_TEST_IOT_I2C_CONFIG(int testSet)
{
  /* unused parameter */
  (void) testSet;

  /* I2C test parameters */
  uctestIotI2CSlaveAddr        = 0x40;
  uctestIotI2CDeviceRegister   = 0x51;
  uctestIotI2CWriteVal         = 0x00;
  uctestIotI2CInstanceIdx      = 0;
  uctestIotI2CInstanceNum      = 5;
}
#endif

/*-----------------------------------------------------------*/

#if (IOT_TEST_COMMON_IO_I2S_SUPPORTED >= 1)
void SET_TEST_IOT_I2S_CONFIG(int testSet)
{
  /* nothing to do here */
  (void) testSet;
  return;
}
#endif

/*-----------------------------------------------------------*/

#if (IOT_TEST_COMMON_IO_PERFCOUNTER_SUPPORTED >= 1)
void SET_TEST_IOT_PERFCOUNTER_CONFIG(int testSet)
{
  /* nothing to do here */
  (void) testSet;
  return;
}
#endif

/*-----------------------------------------------------------*/

#if (IOT_TEST_COMMON_IO_POWER_SUPPORTED >= 1)
void SET_TEST_IOT_POWER_CONFIG(int testSet)
{
  uint64_t irqMask = 0;

#ifdef RTCC_PRESENT
  irqMask |= ((uint64_t)1u << RTCC_IRQn);
#endif

#if _SILICON_LABS_32B_SERIES_2_CONFIG >= 2
  irqMask |= ((uint64_t)1u << HFXO_IRQ_NUMBER);
#endif

  ultestIotPowerInterruptConfig1 = irqMask & 0xFFFFFFFF;
  ultestIotPowerInterruptConfig2 = (irqMask >> 32) & 0xFFFFFFFF;

  /* testSet is not used */
  (void) testSet;
}
#endif

/*-----------------------------------------------------------*/

#if (IOT_TEST_COMMON_IO_PWM_SUPPORTED >= 1)
void SET_TEST_IOT_PWM_CONFIG(int testSet)
{
  /* testSet is not used */
  (void) testSet;

  /* PWM instance and channel */
  ultestIotPwmInstance = 0;
  ultestIotPwmChannel  = 0;

  /* PWM test parameters */
  ultestIotPwmFrequency = 2000UL;
  ultestIotPwmDutyCycle = 20;

  /* GPIO Pin to test */
  ultestIotPwmGpioInputPin = 0;
}
#endif

/*-----------------------------------------------------------*/

#if (IOT_TEST_COMMON_IO_RESET_SUPPORTED >= 1)
void SET_TEST_IOT_RESET_CONFIG(int testSet)
{
  /* nothing to do here */
  (void) testSet;
  return;
}
#endif

/*-----------------------------------------------------------*/

#if (IOT_TEST_COMMON_IO_RTC_SUPPORTED >= 1)
void SET_TEST_IOT_RTC_CONFIG(int testSet)
{
  /* nothing to do here */
  (void) testSet;
  return;
}
#endif

/*-----------------------------------------------------------*/

#if (IOT_TEST_COMMON_IO_SDIO_SUPPORTED >= 1)
void SET_TEST_IOT_SDIO_CONFIG(int testSet)
{
  /* nothing to do here */
  (void) testSet;
  return;
}
#endif

/*-----------------------------------------------------------*/

#if (IOT_TEST_COMMON_IO_SPI_SUPPORTED >= 1)
void SET_TEST_IOT_SPI_CONFIG(int testSet)
{
  /* unused parameter */
  (void) testSet;

  /* default SPI for testing */
  ultestIotSpiInstance = 0;
}
#endif

/*-----------------------------------------------------------*/

#if (IOT_TEST_COMMON_IO_TEMP_SENSOR_SUPPORTED >= 1)
void SET_TEST_IOT_TEMP_SENSOR_CONFIG(int testSet)
{
  /* nothing to do here */
  (void) testSet;
  return;
}
#endif

/*-----------------------------------------------------------*/

#if (IOT_TEST_COMMON_IO_TIMER_SUPPORTED >= 1)
void SET_TEST_IOT_TIMER_CONFIG(int testSet)
{
  /* nothing to do here */
  (void) testSet;
  return;
}
#endif

/*-----------------------------------------------------------*/

#if (IOT_TEST_COMMON_IO_UART_SUPPORTED >= 1)
void SET_TEST_IOT_UART_CONFIG(int testSet)
{
  /* unused parameter */
  (void) testSet;

  /* default UART for testing */
  uctestIotUartPort = 0;
}
#endif

/*-----------------------------------------------------------*/

#if (IOT_TEST_COMMON_IO_USB_DEVICE_SUPPORTED >= 1)
void SET_TEST_IOT_USB_DEVICE_CONFIG(int testSet)
{
  /* nothing to do here */
  (void) testSet;
  return;
}
#endif

/*-----------------------------------------------------------*/

#if (IOT_TEST_COMMON_IO_USB_HOST_SUPPORTED >= 1)
void SET_TEST_IOT_USB_HOST_CONFIG(int testSet)
{
  /* nothing to do here */
  (void) testSet;
  return;
}
#endif

/*-----------------------------------------------------------*/

#if (IOT_TEST_COMMON_IO_WATCHDOG_SUPPORTED >= 1)
void SET_TEST_IOT_WATCHDOG_CONFIG(int testSet)
{
  /* nothing to do here */
  (void) testSet;
  return;
}
#endif
