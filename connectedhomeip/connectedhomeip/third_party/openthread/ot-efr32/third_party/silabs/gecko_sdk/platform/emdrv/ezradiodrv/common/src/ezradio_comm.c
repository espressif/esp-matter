/***************************************************************************//**
 * @file
 * @brief This file contains the EZRadio communication layer.
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
#include <stdarg.h>
#include "em_gpio.h"
#include "gpiointerrupt.h"
#include "ustimer.h"

#include "ezradiodrv_config.h"
#include "ezradio_hal.h"
#include "ezradio_comm.h"

/** Can be used to prevent CTS check before any communication command. */
uint8_t ezradio_comm_CtsWentHigh = 0;

/**
 * Gets a command response from the radio chip
 *
 * @param byteCount     Number of bytes to get from the radio chip
 * @param pData         Pointer to where to put the data
 *
 * @return CTS value
 */
uint8_t ezradio_comm_GetResp(uint8_t byteCount, uint8_t* pData)
{
  uint16_t errCnt = EZRADIODRV_COMM_CTS_RETRY;
  uint8_t ret = 0;
#if !defined(EZRADIODRV_SPI_4WIRE_MODE)
  uint8_t rxCtsVal;
#else
  uint8_t txCtsBuf[EZRADIODRV_MAX_CTS_BUFF_SIZE];
  uint8_t rxCtsBuf[EZRADIODRV_MAX_CTS_BUFF_SIZE];
  uint8_t cnt;

  txCtsBuf[0] = 0x44;
  for (cnt = 1; cnt < byteCount + 2; cnt++) {
    txCtsBuf[cnt] = 0xFF;
  }
#endif

  while (errCnt != 0) {    //wait until radio IC is ready with the data
#if defined(EZRADIODRV_SPI_4WIRE_MODE)
    ezradio_hal_SpiWriteReadData(2, txCtsBuf, rxCtsBuf);
#else
    ezradio_hal_ClearNsel();
    ezradio_hal_SpiWriteByte(0x44);    //read CMD buffer
    ezradio_hal_SpiReadByte(&rxCtsVal);
#endif

#if defined(EZRADIODRV_SPI_4WIRE_MODE)
    if (rxCtsBuf[1] == 0xFF)
#else
    if (rxCtsVal == 0xFF)
#endif
    {
      if (byteCount) {
#if defined(EZRADIODRV_SPI_4WIRE_MODE)
        ezradio_hal_SpiWriteReadData(byteCount + 2, txCtsBuf, rxCtsBuf);
#else
        ezradio_hal_SpiReadData(byteCount, pData);
#endif
      }
#if !defined(EZRADIODRV_SPI_4WIRE_MODE)
      ezradio_hal_SetNsel();
#endif
      break;
    }
#if !defined(EZRADIODRV_SPI_4WIRE_MODE)
    ezradio_hal_SetNsel();
#endif
    errCnt--;
  }

#if defined(ezradio_comm_ERROR_CALLBACK)
  if (errCnt == 0) {
    ezradio_comm_ERROR_CALLBACK();
  }
#endif

#if defined(EZRADIODRV_SPI_4WIRE_MODE)
  if (rxCtsBuf[1] == 0xFF)
#else
  if (rxCtsVal == 0xFF)
#endif
  {
    ezradio_comm_CtsWentHigh = 1;
    ret = rxCtsVal;
  }

#if defined(EZRADIODRV_SPI_4WIRE_MODE)
  for (cnt = 0; cnt < byteCount; cnt++) {
    pData[cnt] = rxCtsBuf[cnt + 2];
  }
#endif

  return ret;
}

/**
 * Sends a command to the radio chip
 *
 * @param byteCount     Number of bytes in the command to send to the radio device
 * @param pData         Pointer to the command to send.
 */
void ezradio_comm_SendCmd(uint8_t byteCount, uint8_t* pData)
{
  while (!ezradio_comm_CtsWentHigh) {
    ezradio_comm_PollCTS();
  }
#if !defined(EZRADIODRV_SPI_4WIRE_MODE)
  ezradio_hal_ClearNsel();
#endif
  ezradio_hal_SpiWriteData(byteCount, pData);
#if !defined(EZRADIODRV_SPI_4WIRE_MODE)
  ezradio_hal_SetNsel();
#endif
  ezradio_comm_CtsWentHigh = 0;
}

/**
 * Gets a command response from the radio chip
 *
 * @param cmd           Command ID
 * @param pollCts       Set to poll CTS
 * @param byteCount     Number of bytes to get from the radio chip.
 * @param pData         Pointer to where to put the data.
 */
void ezradio_comm_ReadData(uint8_t cmd, uint8_t pollCts, uint8_t byteCount, uint8_t* pData)
{
#if defined(EZRADIODRV_SPI_4WIRE_MODE)
  uint8_t txBuf[] = { cmd, 0xFF };
  uint8_t rxBuf[EZRADIODRV_MAX_CTS_BUFF_SIZE];
  uint8_t cnt;
#endif

  if (pollCts) {
    while (!ezradio_comm_CtsWentHigh) {
      ezradio_comm_PollCTS();
    }
  }

#if defined(EZRADIODRV_SPI_4WIRE_MODE)
  ezradio_hal_SpiWriteReadData(byteCount + 1, txBuf, rxBuf);

  for (cnt = 0; cnt < byteCount; cnt++) {
    pData[cnt] = rxBuf[cnt + 1];
  }

#else
  ezradio_hal_ClearNsel();
  ezradio_hal_SpiWriteByte(cmd);
  ezradio_hal_SpiReadData(byteCount, pData);
  ezradio_hal_SetNsel();
#endif

  ezradio_comm_CtsWentHigh = 0;
}

/**
 * Gets a command response from the radio chip
 *
 * @param cmd           Command ID
 * @param pollCts       Set to poll CTS
 * @param byteCount     Number of bytes to get from the radio chip
 * @param pData         Pointer to where to put the data
 */
void ezradio_comm_WriteData(uint8_t cmd, uint8_t pollCts, uint8_t byteCount, uint8_t* pData)
{
#if defined(EZRADIODRV_SPI_4WIRE_MODE)
  uint8_t txBuff[EZRADIODRV_MAX_CTS_BUFF_SIZE];
  uint8_t cnt;
#endif

  if (pollCts) {
    while (!ezradio_comm_CtsWentHigh) {
      ezradio_comm_PollCTS();
    }
  }

#if defined(EZRADIODRV_SPI_4WIRE_MODE)
  txBuff[0] = cmd;

  for (cnt = 0; cnt < byteCount; cnt++) {
    txBuff[cnt + 1] = pData[cnt];
  }

  ezradio_hal_SpiWriteData(byteCount + 1, txBuff);

#else
  ezradio_hal_ClearNsel();
  ezradio_hal_SpiWriteByte(cmd);
  ezradio_hal_SpiWriteData(byteCount, pData);
  ezradio_hal_SetNsel();
#endif

  ezradio_comm_CtsWentHigh = 0;
}

/**
 * Waits for CTS to be high
 *
 * @return CTS value
 */
uint8_t ezradio_comm_PollCTS(void)
{
#ifdef EZRADIODRV_COMM_USE_GPIO1_FOR_CTS
  uint8_t ret = 0;
  uint16_t errCnt = EZRADIODRV_COMM_CTS_RETRY;

  USTIMER_Init();
  while (!ezradio_hal_Gpio1Level()) {
    /* Wait 10us before retry */
    USTIMER_Delay(10u);
    errCnt--;
  }
  USTIMER_DeInit();

  /* CTS arrived */
  if (errCnt) {
    ezradio_comm_CtsWentHigh = 1;
    ret = 0xFF;
  }

  return ret;
#else
  return ezradio_comm_GetResp(0, 0);
#endif
}

/**
 * Clears the CTS state variable.
 */
void ezradio_comm_ClearCTS()
{
  ezradio_comm_CtsWentHigh = 0;
}

/**
 * Sends a command to the radio chip and gets a response
 *
 * @param cmdByteCount  Number of bytes in the command to send to the radio device
 * @param pCmdData      Pointer to the command data
 * @param respByteCount Number of bytes in the response to fetch
 * @param pRespData     Pointer to where to put the response data
 *
 * @return CTS value
 */
uint8_t ezradio_comm_SendCmdGetResp(uint8_t cmdByteCount, uint8_t* pCmdData, uint8_t respByteCount, uint8_t* pRespData)
{
  ezradio_comm_SendCmd(cmdByteCount, pCmdData);
  return ezradio_comm_GetResp(respByteCount, pRespData);
}
