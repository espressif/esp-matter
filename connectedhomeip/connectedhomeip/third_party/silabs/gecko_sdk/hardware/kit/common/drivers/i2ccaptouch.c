/***************************************************************************//**
 * @file
 * @brief I2C touch support on Touch Display
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

#include "i2ccaptouch.h"
#include "captouchconfig.h"
#include "i2cspmconfig.h"
#include "i2cspm.h"

/* Standard library includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* EMLIB includes */
#include "em_device.h"
#include "em_cmu.h"
#include "em_core.h"
#include "em_gpio.h"
#include "em_i2c.h"

/* Other */
#include "emstatus.h"
#include "ustimer.h"

/******************************************************************************/
/*                                                                            */
/* Local function declarations                                                */
/*                                                                            */
/******************************************************************************/

static uint32_t gpioInit         (void);
static uint32_t i2cInit          (void);
static uint32_t parseData        (CAPT_Touch *buffer,
                                  uint8_t len,
                                  uint8_t *nTouches);

static I2C_TransferReturn_TypeDef controllerRead (void);

/******************************************************************************/
/*                                                                            */
/* Local variable declarations                                                */
/*                                                                            */
/******************************************************************************/

static uint8_t          i2cReadBuffer[CAPT_I2C_BUFFER_SIZE];
static volatile uint8_t samplesSinceLastUpdate = 0;
static I2C_TypeDef      *i2c                   = CAPT_CONFIG_I2C;

/******************************************************************************/
/*                                                                            */
/* Interrupt handlers                                                         */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Global function definitions                                                */
/*                                                                            */
/******************************************************************************/

/***************************************************************************//**
 *
 * @brief
 *    Initialize capacitive touch controller
 *
 * @return
 *    CAPT_OK on success
 *
 ******************************************************************************/
uint32_t CAPT_init(void)
{
  uint32_t stat;

  stat = CAPT_OK;

  gpioInit();

  stat = i2cInit();

  return stat;
}

/***************************************************************************//**
 *
 * @brief
 *    Enable touch controller.
 *    When enabling this function will wait until the controller is
 *    started before returning.
 *
 * @param enable
 *    If true the controller will be enabled, otherwise it will be disabled.
 *
 * @param wait
 *    If true when enabling controller, wait for the touch controller
 *    to boot after enabling it.
 *    Has no effect when disabling the touch controller.
 *
 * @return
 *    CAPT_OK on success.
 *
 ******************************************************************************/
uint32_t CAPT_enable(bool enable, bool wait)
{
  uint32_t     stat;

  stat = CAPT_OK;

  if ( enable ) {
    /* Set reset pin high */
    GPIO_PinOutSet(CAPT_CONFIG_RESET_PORT, CAPT_CONFIG_RESET_PIN);

    /* Wait for controller to be ready */
    if ( wait ) {
      USTIMER_Init();
      USTIMER_DelayIntSafe(CAPT_CONFIG_BOOT_TIME_US);
      USTIMER_DeInit();
    }
  } else {
    /* Set reset pin low */
    GPIO_PinOutClear(CAPT_CONFIG_RESET_PORT, CAPT_CONFIG_RESET_PIN);
  }

  return stat;
}

/***************************************************************************//**
 *
 * @brief
 *    Update the list of registered touches
 *    This function also updates the list of new touches since last time
 *    CAPT_update was called.
 *
 * @param *buffer
 *    Pointer to a buffer in which to store the CAPT_Touch structs
 *
 * @param len
 *    Length (in number of CAPT_Touch elements) of the buffer
 *
 * @param *nTouches
 *    Number of touches read out
 *
 * @return
 *    CAPT_OK on success.
 *    CAPT_ERROR_PARSE_ERROR if data is invalid.
 *    CAPT_ERROR_BUFFER_TOO_SMALL if the buffer is too short. In this case
 *    as much data as possible is stored in the buffer.
 *    CAPT_ERROR_I2C_ERROR on I2C transfer error
 *
 ******************************************************************************/
uint32_t CAPT_getTouches(CAPT_Touch *touchBuffer,
                         uint8_t len,
                         uint8_t *nTouches)
{
  uint32_t stat;

  stat = CAPT_OK;

  /* Get new data from touch controller */
  stat = controllerRead();
  if ( stat != CAPT_OK ) {
    goto returnStatus;
  }

  /* Parse data received from touch controller into user buffer*/
  stat = parseData(touchBuffer, len, nTouches);
  if ( stat != CAPT_OK ) {
    goto returnStatus;
  }

  returnStatus:

  return stat;
}

/******************************************************************************/
/*                                                                            */
/* Local function definitions                                                 */
/*                                                                            */
/******************************************************************************/

/***************************************************************************//**
 *
 * @brief
 *    Initialize all GPIO pins
 *
 * @return
 *    CAPT_OK on success
 *
 ******************************************************************************/
static uint32_t gpioInit(void)
{
  uint32_t stat;

  stat = CAPT_OK;

  /* Enable GPIO Clock */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Setup I2C Pins */
  GPIO_PinModeSet(CAPT_CONFIG_I2C_SDA_PORT, CAPT_CONFIG_I2C_SDA_PIN,
                  gpioModeWiredAnd, 1);
  GPIO_PinModeSet(CAPT_CONFIG_I2C_SCL_PORT, CAPT_CONFIG_I2C_SCL_PIN,
                  gpioModeWiredAnd, 1);

  /* Reset Pin */
  GPIO_PinModeSet(CAPT_CONFIG_RESET_PORT, CAPT_CONFIG_RESET_PIN,
                  gpioModePushPull, 0);

  GPIO_PinModeSet(CAPT_CONFIG_INT_PORT, CAPT_CONFIG_INT_PIN,
                  gpioModeInput, 0);

  return stat;
}

/***************************************************************************//**
 *
 * @brief
 *    Initialize I2C peripheral
 *
 * @return
 *    CAPT_OK on success
 *
 ******************************************************************************/
static uint32_t i2cInit(void)
{
  uint32_t            stat;
  I2CSPM_Init_TypeDef i2cInit = I2CSPM_INIT_CTP;

  stat = CAPT_OK;

  I2CSPM_Init(&i2cInit);

  return stat;
}

/***************************************************************************//**
 *
 * @brief
 *    Read contents of all registers in the touch controller
 *
 * @return
 *    i2cTransferDone on success.
 *
 ******************************************************************************/
static I2C_TransferReturn_TypeDef controllerRead(void)
{
  uint8_t                    readStartAddress;
  I2C_TransferSeq_TypeDef    transfer;
  I2C_TransferReturn_TypeDef stat;
  stat             = i2cTransferDone;
  readStartAddress = 0x02;

  /* First setup a write specifying which register to start
   * reading from
   */
  transfer.addr        = CAPT_CONFIG_CONTROLLER_ADDRESS << 1;
  transfer.flags       = I2C_FLAG_WRITE;
  transfer.buf[0].data = &readStartAddress;
  transfer.buf[0].len  = 1;

  /* Perform transfer */
  stat = I2CSPM_Transfer(i2c, &transfer);
  if ( stat != i2cTransferDone ) {
    goto returnStatus;
  }

  /* Setup reading from readStartAddress */
  transfer.flags       = I2C_FLAG_READ;
  transfer.buf[0].data = i2cReadBuffer;
  transfer.buf[0].len  = CAPT_I2C_BUFFER_SIZE;

  /* Perform read */
  stat = I2CSPM_Transfer(i2c, &transfer);
  if ( stat != i2cTransferDone ) {
    goto returnStatus;
  }

  stat = i2cTransferDone;

  returnStatus:

  return stat;
}

/***************************************************************************//**
 *
 * @brief
 *    Parse data from i2cReadBuffer into buffer
 *
 * @param *buffer
 *    Pointer to a buffer in which to store the CAPT_Touch structs
 *
 * @param len
 *    Length (in number of CAPT_Touch elements) of the buffer
 *
 * @param *nTouches
 *    Number of touches read out
 *
 * @return
 *    CAPT_OK on success. CAPT_ERROR_PARSE_ERROR if data is invalid.
 *    CAPT_ERROR_BUFFER_TOO_SMALL if the buffer is too short. In this case
 *    as much data as possible is stored in the buffer.
 *
 ******************************************************************************/
static uint32_t parseData(CAPT_Touch *buffer, uint8_t len, uint8_t *nTouches)
{
  uint32_t  stat;
  uint32_t  i;
  uint8_t   base;
  uint8_t   status;
  uint8_t   id;
  uint8_t   numTouches;
  uint16_t  controllerX;
  uint16_t  controllerY;
  float     dispX;
  float     dispY;

  stat = CAPT_OK;
  base = 1;

  /* Get number of active touches */
  numTouches = i2cReadBuffer[0] & 0xF;
  if ( numTouches > CAPT_NUM_TOUCHES_MAX ) {
    stat = CAPT_ERROR_PARSE_ERROR;
    goto returnStatus;
  }

  if ( numTouches > len ) {
    numTouches = len;
    stat = CAPT_ERROR_BUFFER_TOO_SMALL;
  }

  for ( i = 0; i < numTouches; ++i ) {
    /* Extract data */
    status      = (uint8_t) (i2cReadBuffer[base + 0] & 0xC0) >> 6;
    id          = (uint8_t) (i2cReadBuffer[base + 2] & 0xF0) >> 4;
    controllerX = (uint16_t)(i2cReadBuffer[base + 0] & 0x0F) << 8
                  | (uint16_t)(i2cReadBuffer[base + 1]);
    controllerY = (uint16_t)(i2cReadBuffer[base + 2] & 0x0F) << 8
                  | (uint16_t)(i2cReadBuffer[base + 3]);

    if ( status == 0x3 ) {
      /* If the status flag is reserved it means this touch is
       * inactive. Should not happen during normal use */
      status = CAPT_STATUS_INACTIVE;
    }

    /* Convert touch controller coordinates to relative coordinates */
    dispX  = (float)controllerX;
    dispX /= (float)CAPT_CONFIG_TC_X_MAX;
    dispY  = (float)controllerY;
    dispY /= (float)CAPT_CONFIG_TC_Y_MAX;

    /* Store to parseBuffer */
    buffer[i].id     = id;
    buffer[i].status = status;
    buffer[i].x      = dispX;
    buffer[i].y      = dispY;

    /* Increment base */
    base += 6;
  }

  *nTouches = numTouches;

  returnStatus:

  return stat;
}
