/***************************************************************************//**
 * @file
 * @brief Coexistence support for Bluetooth
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

#include <rail.h>
#include <string.h>
#include "coexistence-ble.h"
#include "coexistence-ble-ll.h"

// enum value is used as index in counter array. order of counter values has
// been documented in public documentation. please update API reference manual
// if you need to update the enum values or order
enum coexCounter {
  coexCounterLowPriorityRequested = 0,
  coexCounterHighPriorityRequested,
  coexCounterLowPriorityDenied,
  coexCounterHighPriorityDenied,
  coexCounterLowPriorityTxAborted,
  coexCounterHighPriorityTxAborted,
  coexCounterSize,
};

static struct {
  uint32_t counters[coexCounterSize];
  bool priority; // current priority
  bool requested;
} coex_counter;

static inline void incrementCounter(enum coexCounter counter)
{
  if (coex_counter.priority) {
    counter++;
  }
  coex_counter.counters[counter]++;
}

void sli_bt_coex_counter_request(bool request, bool priority)
{
  if (request) {
    coex_counter.priority = priority;
    if (!coex_counter.requested) {
      incrementCounter(coexCounterLowPriorityRequested);
    }
  }

  coex_counter.requested = request;
}

void sli_bt_coex_counter_increment_denied(void)
{
  incrementCounter(coexCounterLowPriorityDenied);
}

void sli_bt_coex_counter_grant_update(bool grant)
{
  if (!coex_counter.requested) {
    return;
  }

  if (!grant) {
    if (RAIL_GetRadioState(RAIL_EFR32_HANDLE) == RAIL_RF_STATE_TX_ACTIVE) {
      incrementCounter(coexCounterLowPriorityTxAborted);
    }
  }
}

bool sl_bt_get_coex_counters(const void **ptr, uint8_t *size)
{
  if (!ptr || !size) {
    // reset counters
    memset(coex_counter.counters, 0, sizeof(coex_counter.counters));
  } else {
    *ptr = coex_counter.counters;
    *size = sizeof(coex_counter.counters);
  }

  return true;
}
