/***************************************************************************//**
 * @file
 * @brief sl_flex_util_802154_init.c
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
// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "rail.h"
#include "sli_rail_util_callbacks.h" // for internal-only callback signatures
#include "sl_flex_util_802154_init.h"
#include "sl_flex_util_802154_protocol.h"
#include "app_assert.h"
#include "app_log.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// RAIL handle for RAIL context
static RAIL_Handle_t sl_flex_handle = RAIL_EFR32_HANDLE;
/// RAIL schedule configuration
static RAILSched_Config_t sl_flex_sched_config;
/// RAIL configuration
static RAIL_Config_t sl_flex_config = {
  .eventsCallback = &sli_rail_util_on_event,
  .scheduler = &sl_flex_sched_config,
};

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
RAIL_Handle_t sl_flex_util_get_handle(void)
{
  return sl_flex_handle;
}

void sl_flex_util_init(void)
{
  RAIL_Status_t status;

  // initializes the RAIL core
  sl_flex_handle = RAIL_Init(&sl_flex_config,
                             &sli_rail_util_on_rf_ready
                             );
  app_assert((NULL != sl_flex_handle),
             "RAIL_Init failed, return value: NULL");

  // configures the data management
  RAIL_DataConfig_t data_config = {
    .txSource = TX_PACKET_DATA,
    .rxSource = RX_PACKET_DATA,
    .txMethod = PACKET_MODE,
    .rxMethod = PACKET_MODE,
  };
  status = RAIL_ConfigData(sl_flex_handle, &data_config);
  app_assert((RAIL_STATUS_NO_ERROR == status),
             "RAIL_ConfigData failed, return value: %d",
             status);

  // configures the channels
  const RAIL_ChannelConfig_t *channel_config = NULL;
  (void) RAIL_ConfigChannels(sl_flex_handle,
                             channel_config,
                             &sli_rail_util_on_channel_config_change);

  // configures the IEEE 802.15.4 protocol based on the chosen protocol by user
  status = sl_flex_util_802154_protocol_config(sl_flex_handle,
                                               SL_FLEX_UTIL_INIT_PROTOCOL_INSTANCE_DEFAULT);
  app_assert((RAIL_STATUS_NO_ERROR == status),
             "sl_rail_util_protocol_config failed, return value: %d",
             status);

  // configures RAIL calibration
  status = RAIL_ConfigCal(sl_flex_handle,
                          0U
                          | (0
                             ? RAIL_CAL_TEMP : 0U)
                          | (0
                             ? RAIL_CAL_ONETIME : 0U));
  app_assert((RAIL_STATUS_NO_ERROR == status),
             "RAIL_ConfigCal failed, return value: %d",
             status);

  // configures the using RAIL events
  status = RAIL_ConfigEvents(sl_flex_handle,
                             RAIL_EVENTS_ALL,
                             RAIL_EVENT_RX_PACKET_RECEIVED
                             | RAIL_EVENT_TX_PACKET_SENT
                             | RAIL_EVENT_CAL_NEEDED
                             | RAIL_EVENT_TXACK_PACKET_SENT
                             | RAIL_EVENT_RX_PACKET_ABORTED
                             | RAIL_EVENT_RX_FRAME_ERROR
                             | RAIL_EVENT_RX_FIFO_OVERFLOW
                             | RAIL_EVENT_RX_ADDRESS_FILTERED
                             | RAIL_EVENT_RX_SCHEDULED_RX_MISSED
                             | RAIL_EVENT_TX_ABORTED
                             | RAIL_EVENT_TX_BLOCKED
                             | RAIL_EVENT_TX_UNDERFLOW
                             | RAIL_EVENT_TX_CHANNEL_BUSY
                             | RAIL_EVENT_TX_SCHEDULED_TX_MISSED
                             | RAIL_EVENT_TXACK_ABORTED
                             | RAIL_EVENT_TXACK_BLOCKED
                             | RAIL_EVENT_TXACK_UNDERFLOW
                             );
  app_assert((RAIL_STATUS_NO_ERROR == status),
             "RAIL_ConfigEvents failed, return value: %d",
             status);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
