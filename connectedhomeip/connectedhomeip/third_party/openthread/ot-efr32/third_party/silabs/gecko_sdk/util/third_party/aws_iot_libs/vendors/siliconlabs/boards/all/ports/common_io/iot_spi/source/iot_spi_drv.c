/***************************************************************************//**
 * @file    iot_spi_drv.c
 * @brief   SPI driver
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

/*******************************************************************************
 *                               INCLUDES
 ******************************************************************************/

/* SDK emlib layer */
#include "em_core.h"
#include "em_device.h"
#include "em_usart.h"

/* Gecko SDK emdrv includes */
#include "spidrv.h"

/* SPI driver layer */
#include "iot_spi_cb.h"
#include "iot_spi_desc.h"
#include "iot_spi_drv.h"

/*******************************************************************************
 *                        iot_spi_drv_cb_read
 ******************************************************************************/

static void iot_spi_drv_cb_read(SPIDRV_HandleData_t *pxHandle,
                                Ecode_t xEcode,
                                int lCount)
{
  int32_t     lInstNum  = -1;
  sl_status_t xStatus   = 0;

  /* lCount is not used */
  (void) lCount;

  /* search for matched descriptor */
  while (&iot_spi_desc_get(++lInstNum)->xHandleData != pxHandle);

  /* transfer was successful? */
  xStatus = xEcode == ECODE_EMDRV_SPIDRV_OK ? SL_STATUS_OK : SL_STATUS_FAIL;

  /* issue callback */
  IOT_SPI_CB_RX(lInstNum, xStatus);
}

/*******************************************************************************
 *                        iot_spi_drv_cb_write
 ******************************************************************************/

static void iot_spi_drv_cb_write(SPIDRV_HandleData_t *pxHandle,
                                 Ecode_t xEcode,
                                 int lCount)
{
  int32_t     lInstNum  = -1;
  sl_status_t xStatus   = 0;

  /* lCount is not used */
  (void) lCount;

  /* search for matched descriptor */
  while (&iot_spi_desc_get(++lInstNum)->xHandleData != pxHandle);

  /* transfer was successful? */
  xStatus = xEcode == ECODE_EMDRV_SPIDRV_OK ? SL_STATUS_OK : SL_STATUS_FAIL;

  /* issue callback */
  IOT_SPI_CB_TX(lInstNum, xStatus);
}

/*******************************************************************************
 *                       iot_spi_drv_cb_transfer
 ******************************************************************************/

static void iot_spi_drv_cb_transfer(SPIDRV_HandleData_t *pxHandle,
                                    Ecode_t xEcode,
                                    int lCount)
{
  int32_t     lInstNum  = -1;
  sl_status_t xStatus   = 0;

  /* lCount is not used */
  (void) lCount;

  /* search for matched descriptor */
  while (&iot_spi_desc_get(++lInstNum)->xHandleData != pxHandle);

  /* transfer was successful? */
  xStatus = xEcode == ECODE_EMDRV_SPIDRV_OK ? SL_STATUS_OK : SL_STATUS_FAIL;

  /* issue callback */
  IOT_SPI_CB_XX(lInstNum, xStatus);
}

/*******************************************************************************
 *                       iot_spi_drv_driver_init
 ******************************************************************************/

sl_status_t iot_spi_drv_driver_init(void *pvHndl)
{
  IotSPIDescriptor_t *pvDesc = pvHndl;
  SPIDRV_Init_t           xInit  = {0};

  /* re-initialize init struct */
  pvDesc->xInit = xInit;

  /* store peripheral pointer into xInit */
  pvDesc->xInit.port            = pvDesc->pxPeripheral;

  /* update xInit with port/pin and loc information */
#if (_SILICON_LABS_32B_SERIES == 0)
  pvDesc->xInit.portLocation    = pvDesc->ucTxLoc,
#elif (_SILICON_LABS_32B_SERIES == 1)
  pvDesc->xInit.portLocationTx  = pvDesc->ucTxLoc;
  pvDesc->xInit.portLocationRx  = pvDesc->ucRxLoc;
  pvDesc->xInit.portLocationClk = pvDesc->ucClkLoc;
  pvDesc->xInit.portLocationCs  = pvDesc->ucCsLoc;
#else
  pvDesc->xInit.portTx          = pvDesc->xTxPort;
  pvDesc->xInit.portRx          = pvDesc->xRxPort;
  pvDesc->xInit.portClk         = pvDesc->xClkPort;
  pvDesc->xInit.portCs          = pvDesc->xCsPort;
  pvDesc->xInit.pinTx           = pvDesc->ucTxPin;
  pvDesc->xInit.pinRx           = pvDesc->ucRxPin;
  pvDesc->xInit.pinClk          = pvDesc->ucClkPin;
  pvDesc->xInit.pinCs           = pvDesc->ucCsPin;
#endif

  /* apply default configs to xInit */
  pvDesc->xInit.bitRate         = pvDesc->ulDefaultBitrate;
  pvDesc->xInit.frameLength     = pvDesc->ulDefaultFrameLen;
  pvDesc->xInit.dummyTxValue    = pvDesc->ulDefaultTXValue;
  pvDesc->xInit.type            = pvDesc->xDefaultType;
  pvDesc->xInit.bitOrder        = pvDesc->xDefaultBitOrder;
  pvDesc->xInit.clockMode       = pvDesc->xDefaultClockMode;
  pvDesc->xInit.csControl       = pvDesc->xDefaultCsControl;
  pvDesc->xInit.slaveStartMode  = pvDesc->xDefaultSlaveStart;

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                      iot_spi_drv_driver_deinit
 ******************************************************************************/

sl_status_t iot_spi_drv_driver_deinit(void *pvHndl)
{
  IotSPIDescriptor_t *pvDesc = pvHndl;
  SPIDRV_Init_t           xInit  = {0};

  /* reset init struct */
  pvDesc->xInit = xInit;

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                        iot_spi_drv_config_set
 ******************************************************************************/

sl_status_t iot_spi_drv_config_set(void *pvHndl,
                                      uint32_t ulFreq,
                                      uint8_t ucMode,
                                      uint8_t ucBitOrder,
                                      uint8_t ucDummyValue)
{
  IotSPIDescriptor_t *pvDesc = pvHndl;

  /* set frequency */
  pvDesc->xInit.bitRate = ulFreq;

  /* set spi mode */
  switch(ucMode) {
    case 0:
      pvDesc->xInit.clockMode = spidrvClockMode0;
      break;
    case 1:
      pvDesc->xInit.clockMode = spidrvClockMode1;
      break;
    case 2:
      pvDesc->xInit.clockMode = spidrvClockMode2;
      break;
    case 3:
      pvDesc->xInit.clockMode = spidrvClockMode3;
      break;
    default:
      break;
  }

  /* set bit order */
  switch(ucBitOrder) {
    case 0:
      pvDesc->xInit.bitOrder = spidrvBitOrderMsbFirst;
      break;
    case 1:
      pvDesc->xInit.bitOrder = spidrvBitOrderLsbFirst;
      break;
    default:
      break;
  }

  /* set dummy tx value */
  pvDesc->xInit.dummyTxValue = ucDummyValue;

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                        iot_spi_drv_config_get
 ******************************************************************************/

sl_status_t iot_spi_drv_config_get(void *pvHndl,
                                      uint32_t *pulFreq,
                                      uint8_t *pucMode,
                                      uint8_t *pucBitOrder,
                                      uint8_t *pucDummyValue)
{
  IotSPIDescriptor_t *pvDesc = pvHndl;

  /* get frequency */
  *pulFreq = pvDesc->xInit.bitRate;

  /* get spi mode */
  switch(pvDesc->xInit.clockMode) {
    case spidrvClockMode0:
      *pucMode = 0;
      break;
    case spidrvClockMode1:
      *pucMode = 1;
      break;
    case spidrvClockMode2:
      *pucMode = 2;
      break;
    case spidrvClockMode3:
      *pucMode = 3;
      break;
    default:
      break;
  }

  /* get bit order */
  switch(pvDesc->xInit.bitOrder) {
    case spidrvBitOrderMsbFirst:
      *pucBitOrder = 0;
      break;
    case spidrvBitOrderLsbFirst:
      *pucBitOrder = 1;
      break;
    default:
      break;
  }

  /* get dummy tx value */
  *pucDummyValue = pvDesc->xInit.dummyTxValue;

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                        iot_spi_drv_hw_enable
 ******************************************************************************/

sl_status_t iot_spi_drv_hw_enable(void *pvHndl)
{
  IotSPIDescriptor_t  *pvDesc   = pvHndl;
  SPIDRV_HandleData_t     *pxHandle = &pvDesc->xHandleData;
  Ecode_t                  xEcode   = ECODE_EMDRV_SPIDRV_OK;

  /* initialize spidrv */
  xEcode = SPIDRV_Init(pxHandle, &pvDesc->xInit);

  /* set pin modes for SPI pins */
  GPIO_PinModeSet(pvDesc->xRxPort,  pvDesc->ucRxPin,  gpioModeInput,    0);
  GPIO_PinModeSet(pvDesc->xTxPort,  pvDesc->ucTxPin,  gpioModePushPull, 1);
  GPIO_PinModeSet(pvDesc->xClkPort, pvDesc->ucClkPin, gpioModePushPull, 1);
  GPIO_PinModeSet(pvDesc->xCsPort,  pvDesc->ucCsPin,  gpioModePushPull, 1);

  /* done */
  return xEcode == ECODE_EMDRV_SPIDRV_OK ? SL_STATUS_OK : SL_STATUS_FAIL;
}

/*******************************************************************************
 *                       iot_spi_drv_hw_disable
 ******************************************************************************/

sl_status_t iot_spi_drv_hw_disable(void *pvHndl)
{
  IotSPIDescriptor_t  *pvDesc   = pvHndl;
  SPIDRV_HandleData_t     *pxHandle = &pvDesc->xHandleData;
  Ecode_t                  xEcode   = ECODE_EMDRV_SPIDRV_OK;

  /* de-initialize spidrv */
  xEcode = SPIDRV_DeInit(pxHandle);

  /* done */
  return xEcode == ECODE_EMDRV_SPIDRV_OK ? SL_STATUS_OK : SL_STATUS_FAIL;
}

/*******************************************************************************
 *                       iot_spi_drv_read_sync
 ******************************************************************************/

sl_status_t iot_spi_drv_read_sync(void *pvHndl,
                                     uint8_t *pvBuf,
                                     uint32_t ulCount)
{
  IotSPIDescriptor_t  *pvDesc   = pvHndl;
  SPIDRV_HandleData_t *pxHandle = &pvDesc->xHandleData;
  Ecode_t              xEcode   = ECODE_EMDRV_SPIDRV_OK;

  /* perform SPI operation */
  xEcode = SPIDRV_MReceiveB(pxHandle,
                            pvBuf,
                            ulCount);

  /* done */
  return xEcode == ECODE_EMDRV_SPIDRV_OK ? SL_STATUS_OK : SL_STATUS_FAIL;
}

/*******************************************************************************
 *                      iot_spi_drv_read_async
 ******************************************************************************/

sl_status_t iot_spi_drv_read_async(void *pvHndl,
                                      uint8_t *pvBuf,
                                      uint32_t ulCount)
{
  IotSPIDescriptor_t  *pvDesc   = pvHndl;
  SPIDRV_HandleData_t *pxHandle = &pvDesc->xHandleData;
  Ecode_t              xEcode   = ECODE_EMDRV_SPIDRV_OK;

  /* perform SPI operation */
  xEcode = SPIDRV_MReceive(pxHandle,
                           pvBuf,
                           ulCount,
                           iot_spi_drv_cb_read);

  /* done */
  return xEcode == ECODE_EMDRV_SPIDRV_OK ? SL_STATUS_OK : SL_STATUS_FAIL;
}

/*******************************************************************************
 *                       iot_spi_drv_read_abort
 ******************************************************************************/

sl_status_t iot_spi_drv_read_abort(void *pvHndl)
{
  IotSPIDescriptor_t  *pvDesc   = pvHndl;
  SPIDRV_HandleData_t     *pxHandle = &pvDesc->xHandleData;
  Ecode_t                  xEcode   = ECODE_EMDRV_SPIDRV_OK;

  /* perform SPI operation */
  xEcode = SPIDRV_AbortTransfer(pxHandle);

  /* done */
  return xEcode == ECODE_EMDRV_SPIDRV_OK ? SL_STATUS_OK : SL_STATUS_FAIL;
}

/*******************************************************************************
 *                       iot_spi_drv_write_sync
 ******************************************************************************/

sl_status_t iot_spi_drv_write_sync(void *pvHndl,
                                      uint8_t *pvBuf,
                                      uint32_t ulCount)
{
  IotSPIDescriptor_t  *pvDesc   = pvHndl;
  SPIDRV_HandleData_t     *pxHandle = &pvDesc->xHandleData;
  Ecode_t                  xEcode   = ECODE_EMDRV_SPIDRV_OK;

  /* perform SPI operation */
  xEcode = SPIDRV_MTransmitB(pxHandle,
                             pvBuf,
                             ulCount);

  /* done */
  return xEcode == ECODE_EMDRV_SPIDRV_OK ? SL_STATUS_OK : SL_STATUS_FAIL;
}

/*******************************************************************************
 *                       iot_spi_drv_write_async
 ******************************************************************************/

sl_status_t iot_spi_drv_write_async(void *pvHndl,
                                       uint8_t *pvBuf,
                                       uint32_t ulCount)
{
  IotSPIDescriptor_t  *pvDesc   = pvHndl;
  SPIDRV_HandleData_t     *pxHandle = &pvDesc->xHandleData;
  Ecode_t                  xEcode   = ECODE_EMDRV_SPIDRV_OK;

  /* perform SPI operation */
  xEcode = SPIDRV_MTransmit(pxHandle,
                            pvBuf,
                            ulCount,
                            iot_spi_drv_cb_write);

  /* done */
  return xEcode == ECODE_EMDRV_SPIDRV_OK ? SL_STATUS_OK : SL_STATUS_FAIL;
}

/*******************************************************************************
 *                        iot_spi_drv_write_abort
 ******************************************************************************/

sl_status_t iot_spi_drv_write_abort(void *pvHndl)
{
  IotSPIDescriptor_t  *pvDesc   = pvHndl;
  SPIDRV_HandleData_t     *pxHandle = &pvDesc->xHandleData;
  Ecode_t                  xEcode   = ECODE_EMDRV_SPIDRV_OK;

  /* perform SPI operation */
  xEcode = SPIDRV_AbortTransfer(pxHandle);

  /* done */
  return xEcode == ECODE_EMDRV_SPIDRV_OK ? SL_STATUS_OK : SL_STATUS_FAIL;
}

/*******************************************************************************
 *                      iot_spi_drv_transfer_sync
 ******************************************************************************/

sl_status_t iot_spi_drv_transfer_sync(void *pvHndl,
                                         uint8_t *pvTxBuf,
                                         uint8_t *pvRxBuf,
                                         uint32_t ulCount)
{
  IotSPIDescriptor_t  *pvDesc   = pvHndl;
  SPIDRV_HandleData_t     *pxHandle = &pvDesc->xHandleData;
  Ecode_t                  xEcode   = ECODE_EMDRV_SPIDRV_OK;

  /* perform SPI operation */
  xEcode = SPIDRV_MTransferB(pxHandle, pvTxBuf, pvRxBuf, ulCount);

  /* done */
  return xEcode == ECODE_EMDRV_SPIDRV_OK ? SL_STATUS_OK : SL_STATUS_FAIL;
}

/*******************************************************************************
 *                      iot_spi_drv_transfer_async
 ******************************************************************************/

sl_status_t iot_spi_drv_transfer_async(void *pvHndl,
                                          uint8_t *pvTxBuf,
                                          uint8_t *pvRxBuf,
                                          uint32_t ulCount)
{
  IotSPIDescriptor_t  *pvDesc   = pvHndl;
  SPIDRV_HandleData_t     *pxHandle = &pvDesc->xHandleData;
  Ecode_t                  xEcode   = ECODE_EMDRV_SPIDRV_OK;

  /* perform SPI operation */
  xEcode = SPIDRV_MTransfer(pxHandle,
                            pvTxBuf,
                            pvRxBuf,
                            ulCount,
                            iot_spi_drv_cb_transfer);

  /* done */
  return xEcode == ECODE_EMDRV_SPIDRV_OK ? SL_STATUS_OK : SL_STATUS_FAIL;
}

/*******************************************************************************
 *                     iot_spi_drv_transfer_abort
 ******************************************************************************/

sl_status_t iot_spi_drv_transfer_abort(void *pvHndl)
{
  IotSPIDescriptor_t  *pvDesc   = pvHndl;
  SPIDRV_HandleData_t     *pxHandle = &pvDesc->xHandleData;
  Ecode_t                  xEcode   = ECODE_EMDRV_SPIDRV_OK;

  /* perform SPI operation */
  xEcode = SPIDRV_AbortTransfer(pxHandle);

  /* done */
  return xEcode == ECODE_EMDRV_SPIDRV_OK ? SL_STATUS_OK : SL_STATUS_FAIL;
}

/*******************************************************************************
 *                       iot_spi_drv_status_tx
 ******************************************************************************/

sl_status_t iot_spi_drv_status_tx(void *pvHndl, uint32_t *pulCount) {
  IotSPIDescriptor_t  *pvDesc   = pvHndl;
  SPIDRV_HandleData_t     *pxHandle = &pvDesc->xHandleData;
  Ecode_t                  xEcode   = ECODE_EMDRV_SPIDRV_OK;
  int                      lRemain  = 0;

  /* get SPI staus */
  xEcode = SPIDRV_GetTransferStatus(pxHandle, (int *) pulCount, &lRemain);

  /* done */
  return xEcode == ECODE_EMDRV_SPIDRV_OK ? SL_STATUS_OK : SL_STATUS_FAIL;
}

/*******************************************************************************
 *                       iot_spi_drv_status_rx
 ******************************************************************************/

sl_status_t iot_spi_drv_status_rx(void *pvHndl, uint32_t *pulCount) {
  IotSPIDescriptor_t  *pvDesc   = pvHndl;
  SPIDRV_HandleData_t     *pxHandle = &pvDesc->xHandleData;
  Ecode_t                  xEcode   = ECODE_EMDRV_SPIDRV_OK;
  int                      lRemain  = 0;

  /* get SPI staus */
  xEcode = SPIDRV_GetTransferStatus(pxHandle, (int *) pulCount, &lRemain);

  /* done */
  return xEcode == ECODE_EMDRV_SPIDRV_OK ? SL_STATUS_OK : SL_STATUS_FAIL;
}
