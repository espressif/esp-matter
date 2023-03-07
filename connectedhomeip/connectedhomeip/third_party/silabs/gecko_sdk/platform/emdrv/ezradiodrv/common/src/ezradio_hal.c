/***************************************************************************//**
 * @file
 * @brief This file contains EZRadio HAL.
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

#include <stdlib.h>
#include <stdint.h>

#include "em_gpio.h"
#include "em_prs.h"

#include "spidrv.h"
#include "gpiointerrupt.h"

#include "ezradiodrv_config.h"
#include "ezradio_hal.h"

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

static SPIDRV_HandleData_t  ezradioSpiHandle;
static SPIDRV_Handle_t      ezradioSpiHandlePtr = &ezradioSpiHandle;

#if defined(_EZR32_HAPPY_FAMILY)
static SPIDRV_Init_t        ezradioSpiInitData = SPIDRV_MASTER_USARTRF1;
#elif (defined(_EZR32_LEOPARD_FAMILY) || defined(_EZR32_WONDER_FAMILY))
static SPIDRV_Init_t        ezradioSpiInitData = SPIDRV_MASTER_USARTRF0;
#else
static SPIDRV_Init_t        ezradioSpiInitData = SPIDRV_MASTER_USART1;
#endif

/// @endcond

/**
 * Configures the EZRadio GPIO port and pins
 *
 * @param[in] ezradioIrqCallback EZRadio interrupt callback configuration
 * @param[in] enablePTI If true enables the radio PTI bridge in the controller.
 */
void ezradio_hal_GpioInit(GPIOINT_IrqCallbackPtr_t ezradioIrqCallback, bool enablePTI)
{
#if defined(_EZR32_HAPPY_FAMILY)
  (void)enablePTI;
#endif

#if !defined(EZRADIODRV_SPI_4WIRE_MODE)
  GPIO_PinModeSet( (GPIO_Port_TypeDef) RF_USARTRF_CS_PORT, RF_USARTRF_CS_PIN, gpioModePushPull, 1);
#endif

  /* Setup enable and interrupt pins to radio */
  GPIO_PinModeSet( (GPIO_Port_TypeDef) RF_SDN_PORT, RF_SDN_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet( (GPIO_Port_TypeDef) RF_INT_PORT, RF_INT_PIN, gpioModeInputPull, 1);

  /* EZR32HG family uses hard wired PTI interface from the radio to the board controller */
#if (!defined(_EZR32_HAPPY_FAMILY) && !defined(EZRADIODRV_DISABLE_PTI) )
  if (enablePTI) {
    /* Setup PRS for PTI pins */
    CMU_ClockEnable(cmuClock_PRS, true);

    /* Configure RF_GPIO0 and RF_GPIO1 to inputs. */
    GPIO_PinModeSet((GPIO_Port_TypeDef)RF_GPIO0_PORT, RF_GPIO0_PIN, gpioModeInput, 0);
    GPIO_PinModeSet((GPIO_Port_TypeDef)RF_GPIO1_PORT, RF_GPIO1_PIN, gpioModeInput, 0);

    /* Pin PA0 and PA1 output the GPIO0 and GPIO1 via PRS to PTI */
    GPIO_PinModeSet(gpioPortA, 0, gpioModePushPull, 0);
    GPIO_PinModeSet(gpioPortA, 1, gpioModePushPull, 0);

    /* Disable INT for PRS channels */
    GPIO_ExtIntConfig((GPIO_Port_TypeDef)RF_GPIO0_PORT, RF_GPIO0_PIN, RF_GPIO0_PIN, false, false, false);
    GPIO_ExtIntConfig((GPIO_Port_TypeDef)RF_GPIO1_PORT, RF_GPIO1_PIN, RF_GPIO1_PIN, false, false, false);

    /* Setup PRS for RF GPIO pins  */
    PRS_SourceAsyncSignalSet(0, PRS_CH_CTRL_SOURCESEL_GPIOH, PRS_CH_CTRL_SIGSEL_GPIOPIN15);
    PRS_SourceAsyncSignalSet(1, PRS_CH_CTRL_SOURCESEL_GPIOH, PRS_CH_CTRL_SIGSEL_GPIOPIN14);
    PRS->ROUTE = (PRS_ROUTE_CH0PEN | PRS_ROUTE_CH1PEN);

    /* Make sure PRS sensing is enabled (should be by default) */
    GPIO_InputSenseSet(GPIO_INSENSE_PRS, GPIO_INSENSE_PRS);
  }
#endif //#if !defined(_EZR32_HAPPY_FAMILY)

#if defined(EZRADIODRV_DISABLE_PTI) && defined(EZRADIODRV_COMM_USE_GPIO1_FOR_CTS)
  //Enable GPIO1 for CTS input
  GPIO_PinModeSet((GPIO_Port_TypeDef)RF_GPIO1_PORT, RF_GPIO1_PIN, gpioModeInput, 0);
#endif

  if (NULL != ezradioIrqCallback) {
    /* Register callback and enable interrupt */
    GPIOINT_CallbackRegister(RF_INT_PIN, ezradioIrqCallback);
    GPIO_ExtIntConfig( (GPIO_Port_TypeDef) RF_INT_PORT, RF_INT_PIN, RF_INT_PIN, false, true, true);
  }
}

/**
 * Initializes SPI driver for the EZRadio device.
 */
void ezradio_hal_SpiInit(void)
{
  ezradioSpiInitData.bitRate   = 8E6;

#if !defined(EZRADIODRV_SPI_4WIRE_MODE)
  ezradioSpiInitData.csControl = spidrvCsControlApplication;
#endif

  SPIDRV_Init(ezradioSpiHandlePtr, &ezradioSpiInitData);
}

/**
 * Asserts SDN pin of the EZRadio device.
 */
void ezradio_hal_AssertShutdown(void)
{
  GPIO_PinOutSet( (GPIO_Port_TypeDef) RF_SDN_PORT, RF_SDN_PIN);
}

/**
 * Deasserts SDN pin of the EZRadio device.
 */
void ezradio_hal_DeassertShutdown(void)
{
  GPIO_PinOutClear( (GPIO_Port_TypeDef) RF_SDN_PORT, RF_SDN_PIN);
}

/**
 * Clears nSEL pin of the EZRadio device.
 */
void ezradio_hal_ClearNsel(void)
{
  GPIO_PinOutClear( (GPIO_Port_TypeDef) RF_USARTRF_CS_PORT, RF_USARTRF_CS_PIN);
}

/**
 * Sets nSEL pin of the EZRadio device.
 */
void ezradio_hal_SetNsel(void)
{
  GPIO_PinOutSet( (GPIO_Port_TypeDef) RF_USARTRF_CS_PORT, RF_USARTRF_CS_PIN);
}

/**
 * Reads nIRQ pin of the EZRadio device.
 *
 * @return Value of nIRQ pin.
 */
uint8_t ezradio_hal_NirqLevel(void)
{
  return GPIO_PinInGet( (GPIO_Port_TypeDef) RF_INT_PORT, RF_INT_PIN);
}

#if defined(EZRADIODRV_DISABLE_PTI) && defined(EZRADIODRV_COMM_USE_GPIO1_FOR_CTS)
/**
 * Reads GPIO1 pin of the EZRadio device.
 *
 * @return Value of GPIO1 pin.
 */
uint8_t ezradio_hal_Gpio1Level(void)
{
  return GPIO_PinInGet( (GPIO_Port_TypeDef) RF_GPIO1_PORT, RF_GPIO1_PIN);
}
#endif

/**
 * Writes a single byte to the EZRadio SPI port.
 *
 * @param byteToWrite Byte to write.
 */
void ezradio_hal_SpiWriteByte(uint8_t byteToWrite)
{
  SPIDRV_MTransmitB(ezradioSpiHandlePtr, &byteToWrite, 1);
}

/**
 * Reads a single byte from the EZRadio SPI port.
 *
 * @param readByte Read byte.
 */
void ezradio_hal_SpiReadByte(uint8_t* readByte)
{
  SPIDRV_MReceiveB(ezradioSpiHandlePtr, readByte, 1);
}

/**
 * Writes byteCount number of bytes to the EZRadio SPI port.
 *
 * @param byteCount Number of bytes to write.
 * @param pData Pointer to the byte array.
 */
void ezradio_hal_SpiWriteData(uint8_t byteCount, uint8_t* pData)
{
  SPIDRV_MTransmitB(ezradioSpiHandlePtr, pData, byteCount);
}

/**
 * Reads byteCount number of bytes from the EZRadio SPI port.
 *
 * @param byteCount Number of bytes to write.
 * @param pData Pointer to the byte array.
 */
void ezradio_hal_SpiReadData(uint8_t byteCount, uint8_t* pData)
{
  SPIDRV_MReceiveB(ezradioSpiHandlePtr, pData, byteCount);
}

/**
 * Reads byteCount number of bytes from the EZRadio SPI port.
 *
 * @param byteCount Number of bytes to write.
 * @param pData Pointer to the byte array.
 */
void ezradio_hal_SpiWriteReadData(uint8_t byteCount, uint8_t* txData, uint8_t* rxData)
{
  SPIDRV_MTransferB(ezradioSpiHandlePtr, txData, rxData, byteCount);
}
