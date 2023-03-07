/***************************************************************************//**
 * @file
 * @brief Implemented routines for managing prepayment tick.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "app/framework/include/af.h"
#include "prepayment-tick.h"
#include "prepayment-debt-schedule.h"
#include "prepayment-modes-table.h"

#ifdef UC_BUILD
#include "prepayment-server-config.h"
#endif // UC_BUILD

static uint16_t PendingPrepaymentEvents;

void emberAfPrepaymentClusterClearPendingEvent(uint16_t event)
{
  PendingPrepaymentEvents &= ~(event);
}

void emberAfPrepaymentClusterScheduleTickCallback(uint8_t endpoint, uint16_t event)
{
  PendingPrepaymentEvents |= event;

  // This call will execute any ready events and schedule the tick for the nearest future event.
  emberAfPrepaymentClusterServerTickCallback(endpoint);
}

void emberAfPrepaymentClusterServerTickCallback(uint8_t endpoint)
{
  uint32_t timeNowUtc;
  uint32_t minEventDelaySec = 0xFFFFFFFF;
  uint32_t eventDelaySec;
  uint16_t minTimeEvent = 0;

  uint8_t ep = emberAfFindClusterServerEndpointIndex(endpoint, ZCL_PREPAYMENT_CLUSTER_ID);
  emberAfPrepaymentClusterPrintln("Prepayment Tick Callback, epx=%d", ep);
  if ( ep == 0xFF ) {
    return;
  }

  timeNowUtc = emberAfGetCurrentTime();

  // Look at all currently pending events and determine the min delay time for each.
  // If any are less than nextEventTimeSec, update nextEventTimeSec.

  if ( PendingPrepaymentEvents & PREPAYMENT_TICK_CHANGE_DEBT_EVENT ) {
    eventDelaySec = emberAfPrepaymentServerSecondsUntilDebtCollectionEvent(timeNowUtc);
    if ( eventDelaySec == 0 ) {
      // Execute now
      emberAfPrepaymentClusterPrintln("Prepayment Tick:  Handle Debt Collection");
      emberAfPrepaymentServerSetDebtMode(endpoint, timeNowUtc);
      // Recalculate next delay time
      eventDelaySec = emberAfPrepaymentServerSecondsUntilDebtCollectionEvent(timeNowUtc);
    }
    if ( eventDelaySec < minEventDelaySec ) {
      minEventDelaySec = eventDelaySec;
      minTimeEvent = PREPAYMENT_TICK_CHANGE_PAYMENT_MODE_EVENT;
    }
  }

  if ( PendingPrepaymentEvents & PREPAYMENT_TICK_CHANGE_PAYMENT_MODE_EVENT ) {
    eventDelaySec = emberAfPrepaymentServerSecondsUntilPaymentModeEvent(timeNowUtc);
    if ( eventDelaySec == 0 ) {
      // Execute now
      emberAfPrepaymentClusterPrintln("Prepayment Tick:  Handle Payment Mode");
      emberAfPrepaymentServerSetPaymentMode(endpoint);
      // Recalculate next delay time
      eventDelaySec = emberAfPrepaymentServerSecondsUntilPaymentModeEvent(timeNowUtc);
    }
    if ( eventDelaySec < minEventDelaySec ) {
      minEventDelaySec = eventDelaySec;
      minTimeEvent = PREPAYMENT_TICK_CHANGE_PAYMENT_MODE_EVENT;
    }
  }
  emberAfPrepaymentClusterPrintln("Scheduling Tick Callback in %d sec, eventBitFlag=%d, pendingEvents=%d", minEventDelaySec, minTimeEvent, PendingPrepaymentEvents);
  slxu_zigbee_zcl_schedule_server_tick(endpoint, ZCL_PREPAYMENT_CLUSTER_ID, (minEventDelaySec * MILLISECOND_TICKS_PER_SECOND) );
}
