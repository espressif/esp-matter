/***************************************************************************//**
 * @file    iot_flash_drv_spi.c
 * @brief   SPI Flash Driver
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
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"

/* Gecko SDK emdrv includes */
#include "spidrv.h"

/* Flash driver layer */
#include "iot_flash_desc.h"
#include "iot_flash_drv_spi.h"

/* FreeRTOS kernel layer */
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/*******************************************************************************
 *                             STANDARD SIZES
 ******************************************************************************/

/* SPI flash block size */
#ifndef IOT_FLASH_DRV_SPI_BLOCK_SIZE
#define IOT_FLASH_DRV_SPI_BLOCK_SIZE   (64*1024)
#endif

/* SPI flash sector size */
#ifndef IOT_FLASH_DRV_SPI_SECTOR_SIZE
#define IOT_FLASH_DRV_SPI_SECTOR_SIZE  (4*1024)
#endif

/* SPI flash page size */
#ifndef IOT_FLASH_DRV_SPI_PAGE_SIZE
#define IOT_FLASH_DRV_SPI_PAGE_SIZE    (256)
#endif

/* default timeout value (in loops) */
#ifndef IOT_FLASH_DRV_SPI_TIMEOUT
#define IOT_FLASH_DRV_SPI_TIMEOUT      (1000000)
#endif

/*******************************************************************************
 *                                 MACROS
 ******************************************************************************/

/* SPI flash commands */
#define IOT_FLASH_DRV_SPI_COMMAND_PP       0x02
#define IOT_FLASH_DRV_SPI_COMMAND_READ     0x03
#define IOT_FLASH_DRV_SPI_COMMAND_RDSR     0x05
#define IOT_FLASH_DRV_SPI_COMMAND_WREN     0x06
#define IOT_FLASH_DRV_SPI_COMMAND_SE       0x20
#define IOT_FLASH_DRV_SPI_COMMAND_CE       0x60
#define IOT_FLASH_DRV_SPI_COMMAND_RDID     0x9F

/* bitmasks for status register fields */
#define IOT_FLASH_DRV_SPI_STATUS_WIP       0x01
#define IOT_FLASH_DRV_SPI_STATUS_WEL       0x02
#define IOT_FLASH_DRV_SPI_STATUS_BP0       0x04
#define IOT_FLASH_DRV_SPI_STATUS_BP1       0x08
#define IOT_FLASH_DRV_SPI_STATUS_BP2       0x10
#define IOT_FLASH_DRV_SPI_STATUS_BP3       0x20
#define IOT_FLASH_DRV_SPI_STATUS_QE        0x40
#define IOT_FLASH_DRV_SPI_STATUS_SRWD      0x80

/*******************************************************************************
 *                              DATA TYPES
 ******************************************************************************/

/* transfer descriptor */
typedef struct iot_flash_drv_spi_td {
  /* command buffer */
  void     *pvCmdBuf;
  uint32_t  ulCmdSize;
  /* address buffer */
  void     *pvAdrBuf;
  uint32_t  ulAdrSize;
  /* data to be sent */
  void     *pvTxBuf;
  uint32_t  ulTxSize;
  /* data to be received */
  void     *pvRxBuf;
  uint32_t  ulRxSize;
} iot_flash_drv_spi_td_t;

/*******************************************************************************
 *                            STATIC VARIABLES
 ******************************************************************************/

static SPIDRV_HandleData_t iot_flash_drv_spi_driver_handle;
SemaphoreHandle_t iot_flash_drv_spi_driver_sem;
StaticSemaphore_t iot_flash_drv_spi_driver_sembuf;

/*******************************************************************************
 *                  iot_flash_drv_spi_driver_cb()
 ******************************************************************************/

static void iot_flash_drv_spi_driver_cb(struct SPIDRV_HandleData *handle,
                                 Ecode_t transferStatus,
                                 int itemsTransferred)
{
  /* this is SPI callback; called one spi operation is finished */
  BaseType_t xHigherPriorityTaskWoken = pdTRUE;

  /* these parameters are not needed here */
  (void) handle;
  (void) transferStatus;
  (void) itemsTransferred;

  /* wake up SPI transfer thread to continue */
  xSemaphoreGiveFromISR(iot_flash_drv_spi_driver_sem, &xHigherPriorityTaskWoken);
}

/*******************************************************************************
 *                 iot_flash_drv_spi_driver_setup_tx()
 ******************************************************************************/

static void iot_flash_drv_spi_driver_setup_tx(void *pvHndl)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;

  /* set TX pin as output */
  GPIO_PinModeSet(pvDesc->xTxPort,
                  pvDesc->ucTxPin,
                  gpioModePushPull,
                  0);
}

/*******************************************************************************
*                  iot_flash_drv_spi_driver_setup_rx()
******************************************************************************/

static void iot_flash_drv_spi_driver_setup_rx(void *pvHndl)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;

  /* set RX pin as input */
  GPIO_PinModeSet(pvDesc->xRxPort,
                  pvDesc->ucRxPin,
                  gpioModeInput,
                  0);
}

/*******************************************************************************
*                  iot_flash_drv_spi_driver_setup_clk()
******************************************************************************/

static void iot_flash_drv_spi_driver_setup_clk(void *pvHndl)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;

  /* set CLK pin as output */
  GPIO_PinModeSet(pvDesc->xClkPort,
                  pvDesc->ucClkPin,
                  gpioModePushPull,
                  0);
}

/*******************************************************************************
 *                 iot_flash_drv_spi_driver_setup_cs()
 ******************************************************************************/

static void iot_flash_drv_spi_driver_setup_cs(void *pvHndl)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;

  /* set CS pin as output (active low) */
  GPIO_PinModeSet(pvDesc->xCsPort,
                  pvDesc->ucCsPin,
                  gpioModePushPull,
                  1);
}

/*******************************************************************************
 *                 iot_flash_drv_spi_driver_cs_enable()
 ******************************************************************************/

static void iot_flash_drv_spi_driver_cs_enable(void *pvHndl)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;

  /* enable CS (active low) */
  GPIO_PinOutClear(pvDesc->xCsPort, pvDesc->ucCsPin);
}

/*******************************************************************************
 *                 iot_flash_drv_spi_driver_cs_disable()
 ******************************************************************************/

static void iot_flash_drv_spi_driver_cs_disable(void *pvHndl)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;

  /* disable CS (active low) */
  GPIO_PinOutSet(pvDesc->xCsPort, pvDesc->ucCsPin);
}

/*******************************************************************************
 *                 iot_flash_drv_spi_driver_transfer_async()
 ******************************************************************************/

static uint32_t iot_flash_drv_spi_driver_transfer_async(void *pvHndl,
                                                  iot_flash_drv_spi_td_t *pxTd)
{
	
  IotFlashDescriptor_t *pvDesc = pvHndl;
  Ecode_t  xEcode   = ECODE_EMDRV_SPIDRV_OK;

  /* enable CS */
  iot_flash_drv_spi_driver_cs_enable(pvDesc);

  /* send command byte */
  if (xEcode == ECODE_EMDRV_SPIDRV_OK && pxTd->ulCmdSize > 0) {
    xEcode = SPIDRV_MTransmit(&iot_flash_drv_spi_driver_handle,
                              pxTd->pvCmdBuf,
                              pxTd->ulCmdSize,
                              iot_flash_drv_spi_driver_cb);
  }

  /* if SPI operation started, sleep until completed */
  if (xEcode == ECODE_EMDRV_SPIDRV_OK && pxTd->ulCmdSize > 0) {
    xSemaphoreTake(iot_flash_drv_spi_driver_sem, portMAX_DELAY);
  }

  /* send address bytes */
  if (xEcode == ECODE_EMDRV_SPIDRV_OK && pxTd->ulAdrSize > 0) {
    xEcode = SPIDRV_MTransmit(&iot_flash_drv_spi_driver_handle,
                              pxTd->pvAdrBuf,
                              pxTd->ulAdrSize,
                              iot_flash_drv_spi_driver_cb);
  }

  /* if SPI operation started, sleep until completed */
  if (xEcode == ECODE_EMDRV_SPIDRV_OK && pxTd->ulAdrSize > 0) {
    xSemaphoreTake(iot_flash_drv_spi_driver_sem, portMAX_DELAY);
  }

  /* send TX buffer (usually command) */
  if (xEcode == ECODE_EMDRV_SPIDRV_OK && pxTd->ulTxSize > 0) {
    xEcode = SPIDRV_MTransmit(&iot_flash_drv_spi_driver_handle,
                              pxTd->pvTxBuf,
                              pxTd->ulTxSize,
                              iot_flash_drv_spi_driver_cb);
  }

  /* if SPI operation started, sleep until completed */
  if (xEcode == ECODE_EMDRV_SPIDRV_OK && pxTd->ulTxSize > 0) {
    xSemaphoreTake(iot_flash_drv_spi_driver_sem, portMAX_DELAY);
  }

  /* receive RX buffer (usually data) */
  if (xEcode == ECODE_EMDRV_SPIDRV_OK && pxTd->ulRxSize > 0) {
    xEcode = SPIDRV_MReceive(&iot_flash_drv_spi_driver_handle,
                             pxTd->pvRxBuf,
                             pxTd->ulRxSize,
                             iot_flash_drv_spi_driver_cb);
  }

  /* if SPI operation started, sleep until completed */
  if (xEcode == ECODE_EMDRV_SPIDRV_OK && pxTd->ulRxSize > 0) {
    xSemaphoreTake(iot_flash_drv_spi_driver_sem, portMAX_DELAY);
  }

  /* disable CS */
  iot_flash_drv_spi_driver_cs_disable(pvDesc);

  /* done */
  return xEcode == ECODE_EMDRV_SPIDRV_OK;
}

/*******************************************************************************
 *                 iot_flash_drv_spi_driver_transfer_sync()
 ******************************************************************************/

static uint32_t iot_flash_drv_spi_driver_transfer_sync(void *pvHndl,
                                                  iot_flash_drv_spi_td_t *pxTd)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;

  /* transfer status */
  Ecode_t  xEcode   = ECODE_EMDRV_SPIDRV_OK;

  /* enable CS */
  iot_flash_drv_spi_driver_cs_enable(pvDesc);

  /* send command byte */
  if (xEcode == ECODE_EMDRV_SPIDRV_OK && pxTd->ulCmdSize > 0) {
    xEcode = SPIDRV_MTransmitB(&pvDesc->xHandleData,
                               pxTd->pvCmdBuf,
                               pxTd->ulCmdSize);
  }

  /* send address bytes */
  if (xEcode == ECODE_EMDRV_SPIDRV_OK && pxTd->ulAdrSize > 0) {
    xEcode = SPIDRV_MTransmitB(&pvDesc->xHandleData,
                               pxTd->pvAdrBuf,
                               pxTd->ulAdrSize);
  }

  /* send TX buffer (usually command) */
  if (xEcode == ECODE_EMDRV_SPIDRV_OK && pxTd->ulTxSize > 0) {
    xEcode = SPIDRV_MTransmitB(&pvDesc->xHandleData,
                               pxTd->pvTxBuf,
                               pxTd->ulTxSize);
  }

  /* receive RX buffer (usually data) */
  if (xEcode == ECODE_EMDRV_SPIDRV_OK && pxTd->ulRxSize > 0) {
    xEcode = SPIDRV_MReceiveB(&pvDesc->xHandleData,
                              pxTd->pvRxBuf,
                              pxTd->ulRxSize);
  }

  /* disable CS */
  iot_flash_drv_spi_driver_cs_disable(pvDesc);

  /* done */
  return xEcode == ECODE_EMDRV_SPIDRV_OK;
}

/*******************************************************************************
 *                 iot_flash_drv_spi_driver_transfer()
 ******************************************************************************/

static uint32_t iot_flash_drv_spi_driver_transfer(void *pvHndl,
                                                  iot_flash_drv_spi_td_t *pxTd)
{
  if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
    return iot_flash_drv_spi_driver_transfer_sync(pvHndl, pxTd);
  } else {
    return iot_flash_drv_spi_driver_transfer_async(pvHndl, pxTd);
  }
}

/*******************************************************************************
 *                    iot_flash_drv_spi_cmd_pp()
 ******************************************************************************/

static uint32_t iot_flash_drv_spi_cmd_pp(void *pvHndl,
                                         uint32_t ulAddress,
                                         void *pvTxBuf,
                                         uint32_t ulTxSize)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;

  /* transfer buffers */
  uint8_t pubCmdBuf[] = {IOT_FLASH_DRV_SPI_COMMAND_PP};
  uint8_t pubAdrBuf[] = {(ulAddress>>16) & 0xFF,
                         (ulAddress>> 8) & 0xFF,
                         (ulAddress>> 0) & 0xFF};

  /* transfer descriptor */
  iot_flash_drv_spi_td_t xTd = {0};

  /* status variable */
  uint32_t ulStatus = 1;

  /* construct transfer descriptor */
  xTd.pvCmdBuf  = pubCmdBuf;
  xTd.ulCmdSize = sizeof(pubCmdBuf);
  xTd.pvAdrBuf  = pubAdrBuf;
  xTd.ulAdrSize = sizeof(pubAdrBuf);
  xTd.pvTxBuf   = pvTxBuf;
  xTd.ulTxSize  = ulTxSize;
  xTd.pvRxBuf   = NULL;
  xTd.ulRxSize  = 0;

  /* issue PP (Page Program) command */
  ulStatus = iot_flash_drv_spi_driver_transfer(pvDesc, &xTd);

  /* done */
  return ulStatus;
}

/*******************************************************************************
 *                    iot_flash_drv_spi_cmd_read()
 ******************************************************************************/

static uint32_t iot_flash_drv_spi_cmd_read(void *pvHndl,
                                           uint32_t ulAddress,
                                           void *pvRxBuf,
                                           uint32_t ulRxSize)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;

  /* transfer buffers */
  uint8_t pubCmdBuf[] = {IOT_FLASH_DRV_SPI_COMMAND_READ};
  uint8_t pubAdrBuf[] = {(ulAddress>>16) & 0xFF,
                         (ulAddress>> 8) & 0xFF,
                         (ulAddress>> 0) & 0xFF};

  /* transfer descriptor */
  iot_flash_drv_spi_td_t xTd = {0};

  /* transfer status */
  uint32_t ulStatus = 1;

  /* construct transfer descriptor */
  xTd.pvCmdBuf  = pubCmdBuf;
  xTd.ulCmdSize = sizeof(pubCmdBuf);
  xTd.pvAdrBuf  = pubAdrBuf;
  xTd.ulAdrSize = sizeof(pubAdrBuf);
  xTd.pvTxBuf   = NULL;
  xTd.ulTxSize  = 0;
  xTd.pvRxBuf   = pvRxBuf;
  xTd.ulRxSize  = ulRxSize;

  /* issue READ command */
  ulStatus = iot_flash_drv_spi_driver_transfer(pvDesc, &xTd);

  /* done */
  return ulStatus;
}

/*******************************************************************************
 *                    iot_flash_drv_spi_cmd_rdsr()
 ******************************************************************************/

static uint32_t iot_flash_drv_spi_cmd_rdsr(void *pvHndl,
                                           void *pvRxBuf,
                                           uint32_t ulRxSize)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;

  /* transfer buffers */
  uint8_t  pubCmdBuf[] = {IOT_FLASH_DRV_SPI_COMMAND_RDSR};

  /* transfer descriptor */
  iot_flash_drv_spi_td_t xTd = {0};

  /* transfer status */
  uint32_t ulStatus = 1;

  /* construct transfer descriptor */
  xTd.pvCmdBuf  = pubCmdBuf;
  xTd.ulCmdSize = sizeof(pubCmdBuf);
  xTd.pvAdrBuf  = NULL;
  xTd.ulAdrSize = 0;
  xTd.pvTxBuf   = NULL;
  xTd.ulTxSize  = 0;
  xTd.pvRxBuf   = pvRxBuf;
  xTd.ulRxSize  = ulRxSize;

  /* issue RDSR (Read Status Register) command */
  ulStatus = iot_flash_drv_spi_driver_transfer(pvDesc, &xTd);

  /* done */
  return ulStatus;
}

/*******************************************************************************
 *                    iot_flash_drv_spi_cmd_wren()
 ******************************************************************************/

static uint32_t iot_flash_drv_spi_cmd_wren(void *pvHndl)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;

  /* transfer buffers */
  uint8_t  pubCmdBuf[] = {IOT_FLASH_DRV_SPI_COMMAND_WREN};

  /* transfer descriptor */
  iot_flash_drv_spi_td_t xTd = {0};

  /* transfer status */
  uint32_t ulStatus = 1;

  /* construct transfer descriptor */
  xTd.pvCmdBuf  = pubCmdBuf;
  xTd.ulCmdSize = sizeof(pubCmdBuf);
  xTd.pvAdrBuf  = NULL;
  xTd.ulAdrSize = 0;
  xTd.pvTxBuf   = NULL;
  xTd.ulTxSize  = 0;
  xTd.pvRxBuf   = NULL;
  xTd.ulRxSize  = 0;

  /* issue WREN (Write Enable) command */
  ulStatus = iot_flash_drv_spi_driver_transfer(pvDesc, &xTd);

  /* done */
  return ulStatus;
}

/*******************************************************************************
 *                     iot_flash_drv_spi_cmd_se()
 ******************************************************************************/

static uint32_t iot_flash_drv_spi_cmd_se(void *pvHndl,
                                         uint32_t ulAddress)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;

  /* transfer buffers */
  uint8_t pubCmdBuf[] = {IOT_FLASH_DRV_SPI_COMMAND_SE};
  uint8_t pubAdrBuf[] = {(ulAddress>>16) & 0xFF,
                         (ulAddress>> 8) & 0xFF,
                         (ulAddress>> 0) & 0xFF};

  /* transfer descriptor */
  iot_flash_drv_spi_td_t xTd = {0};

  /* transfer status */
  uint32_t ulStatus = 1;

  /* construct transfer descriptor */
  xTd.pvCmdBuf  = pubCmdBuf;
  xTd.ulCmdSize = sizeof(pubCmdBuf);
  xTd.pvAdrBuf  = pubAdrBuf;
  xTd.ulAdrSize = sizeof(pubAdrBuf);
  xTd.pvTxBuf   = NULL;
  xTd.ulTxSize  = 0;
  xTd.pvRxBuf   = NULL;
  xTd.ulRxSize  = 0;

  /* issue SE (Sector Erase) command */
  ulStatus = iot_flash_drv_spi_driver_transfer(pvDesc, &xTd);

  /* done */
  return ulStatus;
}

/*******************************************************************************
 *                     iot_flash_drv_spi_cmd_ce()
 ******************************************************************************/

static uint32_t iot_flash_drv_spi_cmd_ce(void *pvHndl)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;

  /* transfer buffers */
  uint8_t  pubCmdBuf[] = {IOT_FLASH_DRV_SPI_COMMAND_CE};

  /* transfer descriptor */
  iot_flash_drv_spi_td_t xTd = {0};

  /* transfer status */
  uint32_t ulStatus = 1;

  /* construct transfer descriptor */
  xTd.pvCmdBuf  = pubCmdBuf;
  xTd.ulCmdSize = sizeof(pubCmdBuf);
  xTd.pvAdrBuf  = NULL;
  xTd.ulAdrSize = 0;
  xTd.pvTxBuf   = NULL;
  xTd.ulTxSize  = 0;
  xTd.pvRxBuf   = NULL;
  xTd.ulRxSize  = 0;

  /* issue WREN (Write Enable) command */
  ulStatus = iot_flash_drv_spi_driver_transfer(pvDesc, &xTd);

  /* done */
  return ulStatus;
}

/*******************************************************************************
 *                    iot_flash_drv_spi_cmd_rdid()
 ******************************************************************************/

static uint32_t iot_flash_drv_spi_cmd_rdid(void *pvHndl,
                                           void *pvRxBuf,
                                           uint32_t ulRxSize)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;

  /* transfer buffers */
  uint8_t  pubCmdBuf[] = {IOT_FLASH_DRV_SPI_COMMAND_RDID};

  /* transfer descriptor */
  iot_flash_drv_spi_td_t xTd = {0};

  /* transfer status */
  uint32_t ulStatus = 1;

  /* construct transfer descriptor */
  xTd.pvCmdBuf  = pubCmdBuf;
  xTd.ulCmdSize = sizeof(pubCmdBuf);
  xTd.pvAdrBuf  = NULL;
  xTd.ulAdrSize = 0;
  xTd.pvTxBuf   = NULL;
  xTd.ulTxSize  = 0;
  xTd.pvRxBuf   = pvRxBuf;
  xTd.ulRxSize  = ulRxSize;

  /* issue RDID (Read ID) command */
  ulStatus = iot_flash_drv_spi_driver_transfer(pvDesc, &xTd);

  /* done */
  return ulStatus;
}

/*******************************************************************************
 *                     iot_flash_drv_spi_write_enable()
 ******************************************************************************/

static uint32_t iot_flash_drv_spi_write_enable(void *pvHndl)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;

  /* local variables */
  uint8_t  ubStatusReg   = 0x00;
  uint8_t  ubWelFlag     = 0;
  uint32_t ulTimeOut     = 0;
  uint32_t ulStatus      = 1;

  /* enter the WREN set and read loop */
  do {
    /* issue WREN command to set write latch */
    if (ulStatus == 1) {
      ulStatus = iot_flash_drv_spi_cmd_wren(pvDesc);
    }

    /* issue RDSR command to read status register */
    if (ulStatus == 1) {
      ulStatus = iot_flash_drv_spi_cmd_rdsr(pvDesc,
                                            &ubStatusReg,
                                            sizeof(ubStatusReg));
    }

    /* if status register was read successfully, read WEL flag */
    if (ulStatus == 1) {
      if ((ubStatusReg & IOT_FLASH_DRV_SPI_STATUS_WEL) != 0) {
        ubWelFlag = 1;
      }
    }

    /* update timeout counter and check for timeout condition */
    if (ulStatus == 1) {
      ulTimeOut++;
      if (ulTimeOut == IOT_FLASH_DRV_SPI_TIMEOUT) {
        ulStatus = 0;
      }
    }
  } while (ulStatus == 1 && ubWelFlag == 0);

  /* done */
  return ulStatus;
}

/*******************************************************************************
 *                     iot_flash_drv_spi_write_wait()
 ******************************************************************************/

static uint32_t iot_flash_drv_spi_write_wait(void *pvHndl)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;

  /* local variables */
  uint8_t  ubWipFlag     = 1;
  uint8_t  ubStatusReg   = 0x00;
  uint32_t ulTimeOut     = 0;
  uint32_t ulStatus      = 1;

  /* enter the WIP wait loop */
  do {
    /* issue RDSR command to read status register */
    if (ulStatus == 1) {
      ulStatus = iot_flash_drv_spi_cmd_rdsr(pvDesc,
                                            &ubStatusReg,
                                            sizeof(ubStatusReg));
    }

    /* if status register was read successfully, read WIP flag */
    if (ulStatus == 1) {
      if ((ubStatusReg & IOT_FLASH_DRV_SPI_STATUS_WIP) == 0) {
        ubWipFlag = 0;
      }
    }

    /* update timeout counter and check for timeout condition */
    if (ulStatus == 1) {
      ulTimeOut++;
      if (ulTimeOut == IOT_FLASH_DRV_SPI_TIMEOUT) {
        ulStatus = 0;
      }
    }
  } while (ulStatus == 1 && ubWipFlag == 1);

  /* done */
  return ulStatus;
}

/*******************************************************************************
 *                    iot_flash_drv_spi_driver_init()
 ******************************************************************************/

sl_status_t iot_flash_drv_spi_driver_init(void *pvHndl)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;

  /* local variables */
  Ecode_t xEcode = ECODE_EMDRV_SPIDRV_OK;

  /* setup GPIO pin modes */
  iot_flash_drv_spi_driver_setup_tx(pvDesc);
  iot_flash_drv_spi_driver_setup_rx(pvDesc);
  iot_flash_drv_spi_driver_setup_clk(pvDesc);
  iot_flash_drv_spi_driver_setup_cs(pvDesc);

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


  /* enable hardware */
  xEcode = SPIDRV_Init(&pvDesc->xHandleData, &pvDesc->xInit);
  
    /* initialize semaphore */
  iot_flash_drv_spi_driver_sem =
       xSemaphoreCreateBinaryStatic(&iot_flash_drv_spi_driver_sembuf);

  /* done */
  return xEcode == ECODE_EMDRV_SPIDRV_OK ? SL_STATUS_OK : SL_STATUS_FAIL;
}

/*******************************************************************************
 *                   iot_flash_drv_spi_driver_deinit()
 ******************************************************************************/

sl_status_t iot_flash_drv_spi_driver_deinit(void *pvHndl)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;

  /* local variables */
  Ecode_t xEcode = ECODE_EMDRV_SPIDRV_OK;

  /* disable hardware */
  xEcode = SPIDRV_DeInit(&pvDesc->xHandleData);

  /* done */
  return xEcode == ECODE_EMDRV_SPIDRV_OK ? SL_STATUS_OK : SL_STATUS_FAIL;
}

/*******************************************************************************
 *                   iot_flash_drv_spi_get_flash_size()
 ******************************************************************************/

sl_status_t iot_flash_drv_spi_get_flash_size(void *pvHndl,
                                             uint32_t *ulFlashSize)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;

  /* local variables */
  uint8_t pubDeviceId[3] = {0};
  Ecode_t ulStatus       = 0;

  /* issue RDID command */
  ulStatus = iot_flash_drv_spi_cmd_rdid(pvDesc,
                                        pubDeviceId,
                                        sizeof(pubDeviceId));

  /* if successful, return memory size in bytes */
  if (ulStatus == 1) {
    *ulFlashSize = 1<<pubDeviceId[2];
  }

  /* done */
  return ulStatus == 1 ? SL_STATUS_OK : SL_STATUS_FAIL;
}

/*******************************************************************************
 *                   iot_flash_drv_spi_get_block_size()
 ******************************************************************************/

sl_status_t iot_flash_drv_spi_get_block_size(void *pvHndl,
                                             uint32_t *ulBlockSize)
{
  /* flash descriptor unused here */
  (void) pvHndl;

  /* defaults to 64KB */
  *ulBlockSize = IOT_FLASH_DRV_SPI_BLOCK_SIZE;

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                   iot_flash_drv_spi_get_sector_size()
 ******************************************************************************/

sl_status_t iot_flash_drv_spi_get_sector_size(void *pvHndl,
                                              uint32_t *ulSectorSize)
{
  /* flash descriptor unused here */
  (void) pvHndl;

  /* defaults to 4KB */
  *ulSectorSize = IOT_FLASH_DRV_SPI_SECTOR_SIZE;

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                   iot_flash_drv_spi_get_page_size()
 ******************************************************************************/

sl_status_t iot_flash_drv_spi_get_page_size(void *pvHndl,
                                            uint32_t *ulPageSize)
{
  /* flash descriptor unused here */
  (void) pvHndl;

  /* defaults to 256 bytes */
  *ulPageSize  = IOT_FLASH_DRV_SPI_PAGE_SIZE;

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                   iot_flash_drv_spi_get_lock_size()
 ******************************************************************************/

sl_status_t iot_flash_drv_spi_get_lock_size(void *pvHndl,
                                            uint32_t *ulLockSize)
{
  /* flash descriptor unused here */
  (void) pvHndl;

  /* return lock size */
  *ulLockSize = 0;

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                  iot_flash_drv_spi_get_async_flag()
 ******************************************************************************/

sl_status_t iot_flash_drv_spi_get_async_flag(void *pvHndl,
                                             uint8_t *ubAsyncSupport)
{
  /* flash descriptor unused here */
  (void) pvHndl;

  /* return async support */
  *ubAsyncSupport = 0;

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                   iot_flash_drv_spi_get_device_id()
 ******************************************************************************/

sl_status_t iot_flash_drv_spi_get_device_id(void *pvHndl,
                                            uint32_t *ulDeviceId)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;

  /* local variables */
  uint8_t pubDeviceId[3] = {0};
  Ecode_t ulStatus       = 0;

  /* issue RDID command */
  ulStatus = iot_flash_drv_spi_cmd_rdid(pvDesc,
                                        pubDeviceId,
                                        sizeof(pubDeviceId));

  /* if successful, return device identifier */
  if (ulStatus == 1) {
    *ulDeviceId = (pubDeviceId[0]<<16) | /* manufacturer */
                  (pubDeviceId[1]<< 8) | /* memory type */
                  (pubDeviceId[2]<< 0);  /* memory density */
  }

  /* done */
  return ulStatus == 1 ? SL_STATUS_OK : SL_STATUS_FAIL;
}

/*******************************************************************************
 *                   iot_flash_drv_spi_erase_sector()
 ******************************************************************************/

sl_status_t iot_flash_drv_spi_erase_sector(void *pvHndl, uint32_t ulAddress)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;

  /* commands and status variables */
  uint32_t ulStatus = 1;

  /* set write latch */
  if (ulStatus == 1) {
    ulStatus = iot_flash_drv_spi_write_enable(pvDesc);
  }

  /* send SECTOR ERASE command */
  if (ulStatus == 1) {
    ulStatus = iot_flash_drv_spi_cmd_se(pvDesc, ulAddress);
  }

  /* wait until operation is finished  */
  if (ulStatus == 1) {
    ulStatus = iot_flash_drv_spi_write_wait(pvDesc);
  }

  /* done */
  return ulStatus == 1 ? SL_STATUS_OK : SL_STATUS_FAIL;
}

/*******************************************************************************
 *                    iot_flash_drv_spi_erase_chip()
 ******************************************************************************/

sl_status_t iot_flash_drv_spi_erase_chip(void *pvHndl)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;

  /* commands and status variables */
  uint32_t ulStatus = 1;

  /* set write latch */
  if (ulStatus == 1) {
    ulStatus = iot_flash_drv_spi_write_enable(pvDesc);
  }

  /* send PAGE PROGRAM command */
  if (ulStatus == 1) {
    ulStatus = iot_flash_drv_spi_cmd_ce(pvDesc);
  }

  /* wait until operation is finished  */
  if (ulStatus == 1) {
    ulStatus = iot_flash_drv_spi_write_wait(pvDesc);
  }

  /* done */
  return ulStatus == 1 ? SL_STATUS_OK : SL_STATUS_FAIL;
}

/*******************************************************************************
 *                    iot_flash_drv_spi_data_write()
 ******************************************************************************/

sl_status_t iot_flash_drv_spi_data_write(void *pvHndl,
                                         uint32_t ulAddress,
                                         uint8_t *pubBuffer,
                                         uint32_t ulSize)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;

  /* commands and status variables */
  uint32_t ulStatus    = 1;

  /* set write latch */
  if (ulStatus == 1) {
    ulStatus = iot_flash_drv_spi_write_enable(pvDesc);
  }

  /* send PAGE PROGRAM command */
  if (ulStatus == 1) {
    ulStatus = iot_flash_drv_spi_cmd_pp(pvDesc, ulAddress, pubBuffer, ulSize);
  }

  /* wait until operation is finished  */
  if (ulStatus == 1) {
    ulStatus = iot_flash_drv_spi_write_wait(pvDesc);
  }

  /* done */
  return ulStatus == 1 ? SL_STATUS_OK : SL_STATUS_FAIL;
}

/*******************************************************************************
 *                    iot_flash_drv_spi_data_read()
 ******************************************************************************/

sl_status_t iot_flash_drv_spi_data_read(void *pvHndl,
                                        uint32_t ulAddress,
                                        uint8_t *pubBuffer,
                                        uint32_t ulSize)
{
  IotFlashDescriptor_t *pvDesc = pvHndl;

  /* commands and status variables */
  uint32_t ulStatus = 1;

  /* perform read operation */
  ulStatus = iot_flash_drv_spi_cmd_read(pvDesc, ulAddress, pubBuffer, ulSize);

  /* done */
  return ulStatus == 1 ? SL_STATUS_OK : SL_STATUS_FAIL;
}
