/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "stack/include/ember.h"
#include "coexistence-802154.h"

bool halPtaIsEnabled(void)
{
  return false;
}

HalPtaOptions halPtaGetOptions(void)
{
  return 0;
}

uint8_t halPtaGetPhySelectTimeout(void)
{
  return 0;
}

const HalPtaPwmArgs_t *halPtaGetRequestPwmArgs(void)
{
  return NULL;
}

uint8_t halPtaGetDirectionalPriorityPulseWidth(void)
{
  return 0;
}

EmberStatus halPtaSetPhySelectTimeout(uint8_t timeoutMs)
{
  (void)timeoutMs;
  return EMBER_ERR_FATAL;
}

EmberStatus halPtaSetRequestPwm(halPtaReq_t ptaReq,
                                halPtaCb_t ptaCb,
                                uint8_t dutyCycle,
                                uint8_t periodHalfMs)
{
  (void)ptaReq;
  (void)ptaCb;
  (void)dutyCycle;
  (void)periodHalfMs;
  return EMBER_ERR_FATAL;
}

EmberStatus halPtaSetEnable(bool enabled)
{
  (void)enabled;
  return EMBER_ERR_FATAL;
}

EmberStatus halPtaSetDirectionalPriorityPulseWidth(uint8_t pulseWidth)
{
  (void)pulseWidth;
  return EMBER_ERR_FATAL;
}

EmberStatus halPtaSetOptions(HalPtaOptions options)
{
  (void)options;
  return EMBER_ERR_FATAL;
}
