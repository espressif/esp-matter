/***************************************************************************//**
 * @brief APIs for the poll plugin.
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

#include "poll-config.h"
#include "stack/include/ember.h"
#include "hal/hal.h"

#include "poll.h"

static uint8_t shortPollIntervalQS = EMBER_AF_PLUGIN_POLL_SHORT_POLL_INTERVAL_QS;
static uint16_t longPollIntervalS = EMBER_AF_PLUGIN_POLL_LONG_POLL_INTERVAL_S;
static bool shortPolling = FALSE;
static uint32_t lastPollMs;

EmberEventControl emberAfPluginPollEventControl;

static void scheduleNextPoll(void)
{
  emberEventControlSetDelayQS(emberAfPluginPollEventControl,
                              ((shortPolling)
                               ? shortPollIntervalQS
                               : (longPollIntervalS << 2)));
}

void emberAfPluginPollEventHandler(void)
{
  uint32_t nowMs = halCommonGetInt32uMillisecondTick();
  emberEventControlSetInactive(emberAfPluginPollEventControl);

  if (emberStackIsUp()
      && (emberGetNodeType() == EMBER_STAR_END_DEVICE
          || emberGetNodeType() == EMBER_STAR_SLEEPY_END_DEVICE
          || emberGetNodeType() == EMBER_MAC_MODE_DEVICE
          || emberGetNodeType() == EMBER_MAC_MODE_SLEEPY_DEVICE)) {
    lastPollMs = nowMs;
    scheduleNextPoll();
    emberPollForData();
  }
}

void emberAfPluginPollStackStatusCallback(EmberStatus status)
{
  (void)status;

  if (emberStackIsUp()
      && (emberGetNodeType() == EMBER_STAR_END_DEVICE
          || emberGetNodeType() == EMBER_STAR_SLEEPY_END_DEVICE
          || emberGetNodeType() == EMBER_MAC_MODE_DEVICE
          || emberGetNodeType() == EMBER_MAC_MODE_SLEEPY_DEVICE)) {
    scheduleNextPoll();
  }
}

//------------------------------------------------------------------------------
// Public APIs

void emberAfPluginPollSetShortPollInterval(uint8_t intervalQS)
{
  shortPollIntervalQS = intervalQS;
  scheduleNextPoll();
}

void emberAfPluginPollSetLongPollInterval(uint16_t intervalS)
{
  longPollIntervalS = intervalS;
  scheduleNextPoll();
}

void emberAfPluginPollEnableShortPolling(bool enable)
{
  uint32_t nowMs = halCommonGetInt32uMillisecondTick();
  uint32_t pollIntervalMs;

  // Already short polling or already long polling, nothing to do.
  if ((enable && shortPolling)
      || (!enable && !shortPolling)) {
    return;
  }

  shortPolling = enable;

  if (emberEventControlGetActive(emberAfPluginPollEventControl)) {
    pollIntervalMs = ((shortPolling)
                      ? shortPollIntervalQS * MILLISECOND_TICKS_PER_QUARTERSECOND
                      : longPollIntervalS * MILLISECOND_TICKS_PER_SECOND);

    if (elapsedTimeInt32u(lastPollMs, nowMs) >= pollIntervalMs) {
      emberEventControlSetActive(emberAfPluginPollEventControl);
    } else {
      emberEventControlSetDelayMS(emberAfPluginPollEventControl,
                                  pollIntervalMs
                                  - elapsedTimeInt32u(lastPollMs, nowMs));
    }
  }
}
