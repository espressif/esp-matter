/***************************************************************************//**
 * @file
 * @brief
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

#include "rail.h"
#include "coexistence.h"

static COEX_ReqState_t pwmReq;
static RAIL_MultiTimer_t pwmRequestTimer;
static COEX_ReqCb_t pwmReqCb;
static bool pwmIsAsserted;
static COEX_PwmArgs_t pwmArgs;
static uint32_t pwmOnUs;
static uint32_t pwmOffUs;

const COEX_PwmArgs_t *COEX_GetPwmRequest(void)
{
  return &pwmArgs;
}

static void pwmRequestTimerCb(RAIL_MultiTimer_t *tmr,
                              RAIL_Time_t expectedTimeOfEvent,
                              void *cbArg)
{
  (void)expectedTimeOfEvent;
  (void)cbArg;
  RAIL_Time_t delay;
  COEX_Req_t coexReq;

  if (pwmIsAsserted) {
    coexReq = COEX_REQ_OFF;
    delay = pwmOffUs;
  } else {
    coexReq = pwmArgs.req;
    delay = pwmOnUs;
  }
  COEX_SetRequest(&pwmReq, coexReq, pwmReqCb);
  pwmIsAsserted = !pwmIsAsserted;
  RAIL_SetMultiTimer(tmr,
                     delay,
                     RAIL_TIME_DELAY,
                     &pwmRequestTimerCb,
                     NULL);
}

// (dutyCycle/100{percent})*(periodHalfMs/2)*1000{milliseconds/microseconds}
#define DUTY_CYCLE_TO_US (1000 / 100 / 2)

bool COEX_SetPwmRequest(COEX_Req_t coexReq,
                        COEX_ReqCb_t cb,
                        uint8_t dutyCycle,
                        uint8_t periodHalfMs)
{
  if (dutyCycle > 100) {
    return false;
  }
  pwmArgs.dutyCycle = dutyCycle;
  pwmArgs.periodHalfMs = periodHalfMs;
  pwmArgs.req = coexReq;
  if (periodHalfMs == 0 || dutyCycle == 0 || coexReq == COEX_REQ_OFF) {
    RAIL_CancelMultiTimer(&pwmRequestTimer);
    COEX_SetRequest(&pwmReq, COEX_REQ_OFF, cb);
  } else if (dutyCycle == 100) {
    RAIL_CancelMultiTimer(&pwmRequestTimer);
    COEX_SetRequest(&pwmReq, pwmArgs.req, cb);
  } else {
    pwmReqCb = cb;
    pwmIsAsserted = false;
    pwmOnUs = dutyCycle * periodHalfMs * DUTY_CYCLE_TO_US;
    pwmOffUs = (100 - dutyCycle) * periodHalfMs * DUTY_CYCLE_TO_US;
    pwmRequestTimerCb(&pwmRequestTimer, 0, NULL);
  }
  return true;
}
