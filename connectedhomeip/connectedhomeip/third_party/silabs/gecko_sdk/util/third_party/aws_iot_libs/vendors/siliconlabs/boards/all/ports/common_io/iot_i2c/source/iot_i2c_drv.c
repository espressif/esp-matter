/***************************************************************************//**
 * @file    iot_i2c_drv.c
 * @brief   I2C driver
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

/* SDK common headers */
#include "sl_status.h"

/* SDK emlib layer */
#include "em_core.h"
#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_i2c.h"

/* SDK service layer */
#include "sl_sleeptimer.h"
#include "sl_power_manager.h"

/* I2C driver layer */
#include "iot_i2c_cb.h"
#include "iot_i2c_desc.h"
#include "iot_i2c_drv.h"

/*******************************************************************************
 *                     iot_i2c_drv_callback_timeout
 ******************************************************************************/
static void iot_i2c_drv_timeout_cb(sl_sleeptimer_timer_handle_t *tmr,
                                   void *data)
{
  IotI2CDescriptor_t *pvDesc = data;

  /* sleep timer handle not used */
  (void) tmr;

  /* call the callback function */
  if (NVIC_GetEnableIRQ(pvDesc->xIRQn)) {
    IOT_I2C_CB(pvDesc->lInstNum);
  }
}

/*******************************************************************************
 *                       iot_i2c_drv_driver_init
 ******************************************************************************/
sl_status_t iot_i2c_drv_driver_init(void *pvHndl)
{
  IotI2CDescriptor_t *pvDesc = pvHndl;

  /* initialize the descriptor */
  pvDesc->xTransferRet = i2cTransferDone;

  /* initialize config using default values */
  pvDesc->ulTimeout = pvDesc->ulDefaultTimeout;
  pvDesc->ulFreq    = pvDesc->ulDefaultFreq;

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                      iot_i2c_drv_driver_deinit
 ******************************************************************************/
sl_status_t iot_i2c_drv_driver_deinit(void *pvHndl)
{
  IotI2CDescriptor_t *pvDesc = pvHndl;

  /* reset the descriptor */
  pvDesc->xTransferRet = i2cTransferDone;

  /* reset config */
  pvDesc->ulTimeout = 0;
  pvDesc->ulFreq    = 0;

  /* stop all timers */
  sl_sleeptimer_stop_timer(&(pvDesc->xTimeoutTimer));

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                        iot_i2c_drv_config_set
 ******************************************************************************/
sl_status_t iot_i2c_drv_config_set(void *pvHndl,
                                   uint32_t ulTimeout,
                                   uint32_t ulFreq)
{
  IotI2CDescriptor_t *pvDesc = pvHndl;

  /* replacing timeout value with default timeout, if zero */
  if (0 == ulTimeout) {
    ulTimeout = pvDesc->ulDefaultTimeout;
  }

  /* store config */
  pvDesc->ulTimeout = ulTimeout;
  pvDesc->ulFreq    = ulFreq;

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                        iot_i2c_drv_config_get
 ******************************************************************************/
sl_status_t iot_i2c_drv_config_get(void *pvHndl,
                                   uint32_t *pulTimeout,
                                   uint32_t *pulFreq)
{
  IotI2CDescriptor_t *pvDesc = pvHndl;

  /* restore config */
  *pulTimeout = pvDesc->ulTimeout;
  *pulFreq    = pvDesc->ulFreq;

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                        iot_i2c_drv_hw_enable
 ******************************************************************************/
sl_status_t iot_i2c_drv_hw_enable(void *pvHndl)
{
  IotI2CDescriptor_t *pvDesc = pvHndl;
  I2C_Init_TypeDef        xInit  = { 0 };

  /* enable HFPER */
#if defined(_CMU_HFPERCLKEN0_MASK)
  CMU_ClockEnable(cmuClock_HFPER, true);
#endif

  /* enable I2C clock */
  CMU_ClockEnable(pvDesc->xClock, true);

  /* set emlib init parameters */
  xInit.enable  = true;
  xInit.master  = true;
  xInit.refFreq = 0;

  /* set frequency parameters */
  switch (pvDesc->ulFreq) {
    case 100000:
      xInit.freq = I2C_FREQ_STANDARD_MAX;
      xInit.clhr = i2cClockHLRStandard;
      break;

    case 400000:
      xInit.freq = I2C_FREQ_FAST_MAX;
      xInit.clhr = i2cClockHLRAsymetric;
      break;

    case 1000000:
      xInit.freq = I2C_FREQ_FAST_MAX;
      xInit.clhr = i2cClockHLRFast;
      break;

    case 3400000:
      xInit.freq = I2C_FREQ_FAST_MAX;
      xInit.clhr = i2cClockHLRFast;
      break;

    default:
      xInit.freq = I2C_FREQ_STANDARD_MAX;
      xInit.clhr = i2cClockHLRStandard;
      break;
  }

  /* reset I2C hardware */
  I2C_Reset(pvDesc->pxPeripheral);

  /* initialize and enable hardware */
  I2C_Init(pvDesc->pxPeripheral, &xInit);

  /* setup SCL and SDA GPIO pins as output pull up */
  GPIO_PinModeSet(pvDesc->xSclPort, pvDesc->ucSclPin, gpioModeWiredAndPullUp, 1);
  GPIO_PinModeSet(pvDesc->xSdaPort, pvDesc->ucSdaPin, gpioModeWiredAndPullUp, 1);

  /* set enable signal */
  if (pvDesc->ucEnMode == 1) {
    GPIO_PinModeSet(pvDesc->xEnPort, pvDesc->ucEnPin, gpioModePushPull, 1);
  } else if (pvDesc->ucEnMode == 2) {
    GPIO_PinModeSet(pvDesc->xEnPort, pvDesc->ucEnPin, gpioModePushPull, 0);
  }

#if defined (_I2C_ROUTEPEN_MASK)
  /* enable SDA and SCL routing (series 1) */
  pvDesc->pxPeripheral->ROUTEPEN  = I2C_ROUTEPEN_SDAPEN
                                    | I2C_ROUTEPEN_SCLPEN;
  /* set up locations */
  pvDesc->pxPeripheral->ROUTELOC0 =
    (pvDesc->ucSdaLoc << _I2C_ROUTELOC0_SDALOC_SHIFT)
    | (pvDesc->ucSclLoc << _I2C_ROUTELOC0_SCLLOC_SHIFT);
#else
  /* enable SDA and SCL routing (series 2) */
  GPIO->I2CROUTE[pvDesc->ucPeripheralNo].ROUTEEN = GPIO_I2C_ROUTEEN_SDAPEN
                                                   | GPIO_I2C_ROUTEEN_SCLPEN;

  /* setup SCL routing */
  GPIO->I2CROUTE[pvDesc->ucPeripheralNo].SCLROUTE =
    (pvDesc->ucSclPin << _GPIO_I2C_SCLROUTE_PIN_SHIFT)
    | (pvDesc->xSclPort << _GPIO_I2C_SCLROUTE_PORT_SHIFT);

  /* setup SDA routing */
  GPIO->I2CROUTE[pvDesc->ucPeripheralNo].SDAROUTE =
    (pvDesc->ucSdaPin << _GPIO_I2C_SDAROUTE_PIN_SHIFT)
    | (pvDesc->xSdaPort << _GPIO_I2C_SDAROUTE_PORT_SHIFT);
#endif

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                        iot_i2c_drv_hw_disable
 ******************************************************************************/
sl_status_t iot_i2c_drv_hw_disable(void *pvHndl)
{
  IotI2CDescriptor_t *pvDesc = pvHndl;

  /* reset enable signal */
  if (pvDesc->ucEnMode == 1) {
    GPIO_PinModeSet(pvDesc->xEnPort, pvDesc->ucEnPin, gpioModePushPull, 0);
  } else if (pvDesc->ucEnMode == 2) {
    GPIO_PinModeSet(pvDesc->xEnPort, pvDesc->ucEnPin, gpioModePushPull, 1);
  }

  /* disable I2C hardware to save energy */
  I2C_Enable(pvDesc->pxPeripheral, false);

  /* disable I2C clock */
  CMU_ClockEnable(pvDesc->xClock, false);

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                       iot_i2c_drv_transfer_start
 ******************************************************************************/
sl_status_t iot_i2c_drv_transfer_start(void *pvHndl,
                                       uint16_t usAddress,
                                       uint8_t ucDirection,
                                       uint8_t *pucBuffer,
                                       uint32_t ulCount)
{
  IotI2CDescriptor_t  *pvDesc          = pvHndl;
  I2C_TransferSeq_TypeDef  xTransfer       = { 0 };
  uint32_t                 ulTimerFreq     = 0;
  uint32_t                 ulTimeoutTicks  = 0;

  /* Compute address */
  if (usAddress <= 0x007F) {
    /* 7 bits */
    xTransfer.addr  = usAddress << 1;
    xTransfer.flags = 0;
  } else {
    /* 10 bits */
    xTransfer.addr  = ((usAddress & 0x00FF) << 0)
                      | ((usAddress & 0x0300) << 1);
    xTransfer.flags = I2C_FLAG_10BIT_ADDR;
  }

  /* read or write? */
  if (ucDirection == 0) {
    xTransfer.flags |= I2C_FLAG_READ;
  } else {
    xTransfer.flags |= I2C_FLAG_WRITE;
  }

  /* setup buffer */
  xTransfer.buf[0].data = pucBuffer;
  xTransfer.buf[0].len  = ulCount;

  /* store transfer structure that we've just built */
  pvDesc->xTransfer = xTransfer;

  /* compute timeout ticks */
  ulTimerFreq    = sl_sleeptimer_get_timer_frequency();
  ulTimeoutTicks = ulTimerFreq * pvDesc->ulTimeout / 1000;

  /* start timeout timer */
  sl_sleeptimer_start_timer(&(pvDesc->xTimeoutTimer),
                            ulTimeoutTicks,
                            iot_i2c_drv_timeout_cb,
                            pvDesc,
                            0,
                            0);

  /* start the transfer */
  I2C_TransferInit(pvDesc->pxPeripheral, &pvDesc->xTransfer);

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                       iot_i2c_drv_transfer_step
 ******************************************************************************/
sl_status_t iot_i2c_drv_transfer_step(void *pvHndl)
{
  IotI2CDescriptor_t     *pvDesc       = pvHndl;
  bool                    bNoTimeout   = false;
  sl_status_t             xSlStatus    = SL_STATUS_OK;

  /* see if we timed out yet */
  sl_sleeptimer_is_timer_running(&(pvDesc->xTimeoutTimer), &bNoTimeout);

  /* Perform next step of the transfer */
  if (bNoTimeout == true) {
    /* started, no timeout */
    pvDesc->xTransferRet = I2C_Transfer(pvDesc->pxPeripheral);
    if (pvDesc->xTransferRet == i2cTransferInProgress) {
      xSlStatus = SL_STATUS_IN_PROGRESS;
    } else {
      xSlStatus = SL_STATUS_OK;
    }
  } else {
    /* timed out */
    pvDesc->xTransferRet = i2cTransferInProgress;
    xSlStatus = SL_STATUS_OK;
  }

  /* do not consider NAK as an error if requested (for testing) */
  if (pvDesc->ucAcceptNack == 1) {
    pvDesc->xTransferRet = i2cTransferDone;
    xSlStatus = SL_STATUS_OK;
  }

  /* return status */
  return xSlStatus;
}

/*******************************************************************************
 *                       iot_i2c_drv_transfer_finish
 ******************************************************************************/
sl_status_t iot_i2c_drv_transfer_finish(void *pvHndl)
{
  IotI2CDescriptor_t *pvDesc = pvHndl;

  /* make sure timeout timer is stopped */
  sl_sleeptimer_stop_timer(&(pvDesc->xTimeoutTimer));

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                       iot_i2c_drv_transfer_status
 ******************************************************************************/
sl_status_t iot_i2c_drv_transfer_status(void *pvHndl)
{
  IotI2CDescriptor_t *pvDesc    = pvHndl;
  sl_status_t             xSlStatus = SL_STATUS_OK;

  /* translate I2C return value into sl_status */
  switch (pvDesc->xTransferRet) {
    /* transfer complete */
    case i2cTransferDone:
      xSlStatus = SL_STATUS_OK;
      break;

    /* timeout */
    case i2cTransferInProgress:
      xSlStatus = SL_STATUS_TIMEOUT;
      break;

    /* NACK received */
    case i2cTransferNack:
      xSlStatus = SL_STATUS_NOT_READY;
      break;

    /* some other failure */
    default:
      xSlStatus = SL_STATUS_FAIL;
      break;
  }

  /* return status */
  return xSlStatus;
}

/*******************************************************************************
 *                        iot_i2c_drv_irq_enable
 ******************************************************************************/
sl_status_t iot_i2c_drv_irq_enable(void *pvHndl)
{
  IotI2CDescriptor_t *pvDesc = pvHndl;

  /* add EM1 requirement */
  //sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);

  /* enable all I2C interrupts */
  //I2C_IntEnable(pvDesc->pxPeripheral, _I2C_IEN_MASK);

  /* clear any pending I2C IRQ NVIC */
  NVIC_ClearPendingIRQ(pvDesc->xIRQn);

  /* enable I2C interrupt vector in NVIC */
  NVIC_EnableIRQ(pvDesc->xIRQn);

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                        iot_i2c_drv_irq_disable
 ******************************************************************************/
sl_status_t iot_i2c_drv_irq_disable(void *pvHndl)
{
  IotI2CDescriptor_t *pvDesc = pvHndl;

  /* disable I2C interrupt vector in NVIC */
  NVIC_DisableIRQ(pvDesc->xIRQn);

  /* clear any pending I2C IRQ NVIC */
  NVIC_ClearPendingIRQ(pvDesc->xIRQn);

  /* disable all I2C interrupts */
  //I2C_IntDisable(pvDesc->pxPeripheral, _I2C_IEN_MASK);

  /* remove EM1 requirement */
  //sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);

  /* done */
  return SL_STATUS_OK;
}

/*******************************************************************************
 *                             I2C0_IRQHandler
 ******************************************************************************/

#ifdef I2C0
void I2C0_IRQHandler(void)
{
  int32_t lInstNum = -1;

  /* search for matched descriptor */
  while (iot_i2c_desc_get(++lInstNum)->pxPeripheral != I2C0) ;

  /* call consolidated IRQ handler */
  IOT_I2C_CB(lInstNum);
}
#endif

/*******************************************************************************
 *                             I2C1_IRQHandler
 ******************************************************************************/

#ifdef I2C1
void I2C1_IRQHandler(void)
{
  int32_t lInstNum = -1;

  /* search for matched descriptor */
  while (iot_i2c_desc_get(++lInstNum)->pxPeripheral != I2C1) ;

  /* call consolidated IRQ handler */
  IOT_I2C_CB(lInstNum);
}
#endif

/*******************************************************************************
 *                             I2C2_IRQHandler
 ******************************************************************************/

#ifdef I2C2
void I2C2_IRQHandler(void)
{
  int32_t lInstNum = -1;

  /* search for matched descriptor */
  while (iot_i2c_desc_get(++lInstNum)->pxPeripheral != I2C2) ;

  /* call consolidated IRQ handler */
  IOT_I2C_CB(lInstNum);
}
#endif
