/***************************************************************************//**
 * @file
 * @brief Event APIs and defines for the Prepayment Server plugin.
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

#ifndef SILABS_PREPAYMENT_TICK_H
#define SILABS_PREPAYMENT_TICK_H

//
enum {
  PREPAYMENT_TICK_CHANGE_DEBT_EVENT         = (1 << 0),
  PREPAYMENT_TICK_EMERGENCY_CREDIT_EVENT    = (1 << 1),
  PREPAYMENT_TICK_CREDIT_ADJUSTMENT_EVENT   = (1 << 2),
  PREPAYMENT_TICK_CHANGE_PAYMENT_MODE_EVENT = (1 << 3),
  PREPAYMENT_TICK_MAX_CREDIT_LIMIT_EVENT    = (1 << 4),
  PREPAYMENT_TICK_DEBT_CAP_EVENT            = (1 << 5),
};

#define EVENT_TIME_NO_PENDING_EVENTS  0xFFFFFFFF

/** @brief Prepayment Cluster Server Tick
 *
 * Server Tick
 *
 * @param endpoint Endpoint that is being served  Ver.: always
 */
void emberAfPrepaymentClusterServerTickCallback(uint8_t endpoint);

/**
 * @brief Sets the event flag and schedules the prepayment tick based on the next expiring pending event.
 * @param endpoint The endpoint of the device that supports the prepayment server.
 * @event The event bit that should be set when scheduling the next tick.
 *
 **/
void emberAfPrepaymentClusterScheduleTickCallback(uint8_t endpoint, uint16_t event);

/**
 * @brief This function clears an event flag from the pending events bitmask.
 * @event The event bit that should be cleared.
 *
 **/
void emberAfPrepaymentClusterClearPendingEvent(uint16_t event);

#endif  // #ifndef _PREPAYMENT_TICK_H_
