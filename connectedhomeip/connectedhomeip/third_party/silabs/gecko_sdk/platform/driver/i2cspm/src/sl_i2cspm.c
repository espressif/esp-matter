/***************************************************************************//**
 * @file
 * @brief I2C simple poll-based master mode driver
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <stddef.h>
#include "em_cmu.h"
#include "em_gpio.h"
#include "sl_assert.h"
#include "sl_i2cspm.h"
#include "sl_udelay.h"

/* transfer timeout (how many polls) */
#ifndef I2CSPM_TRANSFER_TIMEOUT
#define I2CSPM_TRANSFER_TIMEOUT 300000
#endif

/* SCL hold time (in initialization function) in microseconds */
#ifndef SL_I2CSPM_SCL_HOLD_TIME_US
#define SL_I2CSPM_SCL_HOLD_TIME_US 100
#endif

/*******************************************************************************
 *   Initalize I2C peripheral
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
    GPIO_PinOutClear(init->sclPort, init->sclPin);
    sl_udelay_wait(SL_I2CSPM_SCL_HOLD_TIME_US);
    GPIO_PinOutSet(init->sclPort, init->sclPin);
    sl_udelay_wait(SL_I2CSPM_SCL_HOLD_TIME_US);
  }

  /* Enable pins and set location */
#if defined (_I2C_ROUTEPEN_MASK)
  init->port->ROUTEPEN  = I2C_ROUTEPEN_SDAPEN | I2C_ROUTEPEN_SCLPEN;
  init->port->ROUTELOC0 = (uint32_t)((init->portLocationSda << _I2C_ROUTELOC0_SDALOC_SHIFT)
                                     | (init->portLocationScl << _I2C_ROUTELOC0_SCLLOC_SHIFT));
#elif defined (_I2C_ROUTE_MASK)
  init->port->ROUTE = I2C_ROUTE_SDAPEN
                      | I2C_ROUTE_SCLPEN
                      | (init->portLocation << _I2C_ROUTE_LOCATION_SHIFT);
#else
#if defined(I2C0)
  if (init->port == I2C0) {
    GPIO->I2CROUTE[0].ROUTEEN = GPIO_I2C_ROUTEEN_SDAPEN | GPIO_I2C_ROUTEEN_SCLPEN;
    GPIO->I2CROUTE[0].SCLROUTE = (uint32_t)((init->sclPin << _GPIO_I2C_SCLROUTE_PIN_SHIFT)
                                            | (init->sclPort << _GPIO_I2C_SCLROUTE_PORT_SHIFT));
    GPIO->I2CROUTE[0].SDAROUTE = (uint32_t)((init->sdaPin << _GPIO_I2C_SDAROUTE_PIN_SHIFT)
                                            | (init->sdaPort << _GPIO_I2C_SDAROUTE_PORT_SHIFT));
  }
#endif
#if defined(I2C1)
  if (init->port == I2C1) {
    GPIO->I2CROUTE[1].ROUTEEN = GPIO_I2C_ROUTEEN_SDAPEN | GPIO_I2C_ROUTEEN_SCLPEN;
    GPIO->I2CROUTE[1].SCLROUTE = (uint32_t)((init->sclPin << _GPIO_I2C_SCLROUTE_PIN_SHIFT)
                                            | (init->sclPort << _GPIO_I2C_SCLROUTE_PORT_SHIFT));
    GPIO->I2CROUTE[1].SDAROUTE = (uint32_t)((init->sdaPin << _GPIO_I2C_SDAROUTE_PIN_SHIFT)
                                            | (init->sdaPort << _GPIO_I2C_SDAROUTE_PORT_SHIFT));
  }
#endif
#if defined(I2C2)
  if (init->port == I2C2) {
    GPIO->I2CROUTE[2].ROUTEEN = GPIO_I2C_ROUTEEN_SDAPEN | GPIO_I2C_ROUTEEN_SCLPEN;
    GPIO->I2CROUTE[2].SCLROUTE = (uint32_t)((init->sclPin << _GPIO_I2C_SCLROUTE_PIN_SHIFT)
                                            | (init->sclPort << _GPIO_I2C_SCLROUTE_PORT_SHIFT));
    GPIO->I2CROUTE[2].SDAROUTE = (uint32_t)((init->sdaPin << _GPIO_I2C_SDAROUTE_PIN_SHIFT)
                                            | (init->sdaPort << _GPIO_I2C_SDAROUTE_PORT_SHIFT));
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

/*******************************************************************************
 *   Perform I2C transfer
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
