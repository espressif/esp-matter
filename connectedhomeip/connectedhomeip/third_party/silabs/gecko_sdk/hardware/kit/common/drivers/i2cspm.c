/***************************************************************************//**
 * @file
 * @brief I2C simple poll-based master mode driver for the DK/STK.
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

#include <stddef.h>
#include "em_cmu.h"
#include "em_gpio.h"
#if defined(HAL_CONFIG)
#include "i2cspmhalconfig.h"
#else
#include "i2cspmconfig.h"
#endif
#include "i2cspm.h"
#include "em_assert.h"

/***************************************************************************//**
 * @addtogroup kitdrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup I2CSPM
 * @brief I2C Simple Polled Master driver
 *
 * @details
 *   This driver supports master mode, single bus-master only. It blocks
 *   while waiting for the transfer is complete, polling for completion in EM0.
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Initalize I2C peripheral
 *
 * @details
 *   This driver supports master mode only, single bus-master. In addition
 *   to configuring the I2C peripheral module, it also configures DK/STK
 *   specific setup in order to use the I2C bus.
 *
 * @param[in] init
 *   Pointer to I2C initialization structure
 ******************************************************************************/
void I2CSPM_Init(I2CSPM_Init_TypeDef *init)
{
  int i;
  CMU_Clock_TypeDef i2cClock;
  I2C_Init_TypeDef i2cInit;

  EFM_ASSERT(init != NULL);

#if defined(_CMU_HFPERCLKEN0_MASK)
  CMU_ClockEnable(cmuClock_HFPER, true);
#endif

  /* Select I2C peripheral clock */
  if (false) {
#if defined(I2C0)
  } else if (init->port == I2C0) {
    i2cClock = cmuClock_I2C0;
#endif
#if defined(I2C1)
  } else if (init->port == I2C1) {
    i2cClock = cmuClock_I2C1;
#endif
#if defined(I2C2)
  } else if (init->port == I2C2) {
    i2cClock = cmuClock_I2C2;
#endif
  } else {
    /* I2C clock is not defined */
    EFM_ASSERT(false);
    return;
  }
  CMU_ClockEnable(i2cClock, true);

  /* Output value must be set to 1 to not drive lines low. Set
     SCL first, to ensure it is high before changing SDA. */
  GPIO_PinModeSet(init->sclPort, init->sclPin, gpioModeWiredAndPullUp, 1);
  GPIO_PinModeSet(init->sdaPort, init->sdaPin, gpioModeWiredAndPullUp, 1);

  /* In some situations, after a reset during an I2C transfer, the slave
     device may be left in an unknown state. Send 9 clock pulses to
     set slave in a defined state. */
  for (i = 0; i < 9; i++) {
    GPIO_PinOutSet(init->sclPort, init->sclPin);
    GPIO_PinOutClear(init->sclPort, init->sclPin);
  }

  /* Enable pins and set location */
#if defined (_I2C_ROUTEPEN_MASK)
  init->port->ROUTEPEN  = I2C_ROUTEPEN_SDAPEN | I2C_ROUTEPEN_SCLPEN;
  init->port->ROUTELOC0 = (init->portLocationSda << _I2C_ROUTELOC0_SDALOC_SHIFT)
                          | (init->portLocationScl << _I2C_ROUTELOC0_SCLLOC_SHIFT);
#elif defined (_I2C_ROUTE_MASK)
  init->port->ROUTE = I2C_ROUTE_SDAPEN
                      | I2C_ROUTE_SCLPEN
                      | (init->portLocation << _I2C_ROUTE_LOCATION_SHIFT);
#else
#if defined(I2C0)
  if (init->port == I2C0) {
    GPIO->I2CROUTE[0].ROUTEEN = GPIO_I2C_ROUTEEN_SDAPEN | GPIO_I2C_ROUTEEN_SCLPEN;
    GPIO->I2CROUTE[0].SCLROUTE = (init->sclPin << _GPIO_I2C_SCLROUTE_PIN_SHIFT)
                                 | (init->sclPort << _GPIO_I2C_SCLROUTE_PORT_SHIFT);
    GPIO->I2CROUTE[0].SDAROUTE = (init->sdaPin << _GPIO_I2C_SDAROUTE_PIN_SHIFT)
                                 | (init->sdaPort << _GPIO_I2C_SDAROUTE_PORT_SHIFT);
  }
#endif
#if defined(I2C1)
  if (init->port == I2C1) {
    GPIO->I2CROUTE[1].ROUTEEN = GPIO_I2C_ROUTEEN_SDAPEN | GPIO_I2C_ROUTEEN_SCLPEN;
    GPIO->I2CROUTE[1].SCLROUTE = (init->sclPin << _GPIO_I2C_SCLROUTE_PIN_SHIFT)
                                 | (init->sclPort << _GPIO_I2C_SCLROUTE_PORT_SHIFT);
    GPIO->I2CROUTE[1].SDAROUTE = (init->sdaPin << _GPIO_I2C_SDAROUTE_PIN_SHIFT)
                                 | (init->sdaPort << _GPIO_I2C_SDAROUTE_PORT_SHIFT);
  }
#endif
#if defined(I2C2)
  if (init->port == I2C2) {
    GPIO->I2CROUTE[2].ROUTEEN = GPIO_I2C_ROUTEEN_SDAPEN | GPIO_I2C_ROUTEEN_SCLPEN;
    GPIO->I2CROUTE[2].SCLROUTE = (init->sclPin << _GPIO_I2C_SCLROUTE_PIN_SHIFT)
                                 | (init->sclPort << _GPIO_I2C_SCLROUTE_PORT_SHIFT);
    GPIO->I2CROUTE[2].SDAROUTE = (init->sdaPin << _GPIO_I2C_SDAROUTE_PIN_SHIFT)
                                 | (init->sdaPort << _GPIO_I2C_SDAROUTE_PORT_SHIFT);
  }
#endif
#endif

  /* Set emlib init parameters */
  i2cInit.enable = true;
  i2cInit.master = true; /* master mode only */
  i2cInit.freq = init->i2cMaxFreq;
  i2cInit.refFreq = init->i2cRefFreq;
  i2cInit.clhr = init->i2cClhr;

  I2C_Init(init->port, &i2cInit);
}

/***************************************************************************//**
 * @brief
 *   Perform I2C transfer
 *
 * @param[in] i2c
 *   Pointer to the peripheral port
 *
 * @param[in] seq
 *   Pointer to sequence structure defining the I2C transfer to take place. The
 *   referenced structure must exist until the transfer has fully completed.
 ******************************************************************************/
I2C_TransferReturn_TypeDef I2CSPM_Transfer(I2C_TypeDef *i2c, I2C_TransferSeq_TypeDef *seq)
{
  I2C_TransferReturn_TypeDef ret;
  uint32_t timeout = I2CSPM_TRANSFER_TIMEOUT;
  /* Do a polled transfer */
  ret = I2C_TransferInit(i2c, seq);
  while (ret == i2cTransferInProgress && timeout--) {
    ret = I2C_Transfer(i2c);
  }
  return ret;
}

/** @} (end group I2CSPM) */
/** @} (end addtogroup kitdrv) */
