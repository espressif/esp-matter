/***************************************************************************//**
 * @file: multirail-demo.c
 * @brief Routines to utilise a second instance of RAIL.
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

#include PLATFORM_HEADER

#include "stack/include/ember-types.h"
#include "stack/include/stack-info.h"

#include "rail_ieee802154.h"

#include "multirail-demo.h"

// Local callback prototype, used in railCfgLocal
static void multirailCbGeneric(RAIL_Handle_t handle, RAIL_Events_t events);

// Plugin local data
static RAIL_Handle_t rail2Handle = NULL;
static bool rfReady = false;

// Default RAIL configuration, used if the caller has not provided their own.
// It must be module global (i.e. 'static', not 'auto') and not const.
static RAILSched_Config_t railSchedStateLocal;
static RAIL_Config_t railCfgLocal = {
  .eventsCallback = &multirailCbGeneric,
  .protocol = NULL, // For BLE, pointer to a RAIL_BLE_State_t
  .scheduler = &railSchedStateLocal
};

// The default FIFO, used unless the caller provides their own
// This buffer is required to be RAIL_FIFO_ALIGNMENT aligned,
// which is 4 bytes for Series 2 and 1 byte for Series 1
ALIGNMENT(RAIL_FIFO_ALIGNMENT)
static uint8_t railTxFifoLocal[128];

// Local functions
static void multirailCbGeneric(RAIL_Handle_t handle, RAIL_Events_t events)
{
  // Alert the user...
  emberAfPluginMultirailDemoRailEventCallback(handle, events);
  // ...and yield.
  if (events & (RAIL_EVENT_TX_PACKET_SENT
                | RAIL_EVENT_TX_ABORTED
                | RAIL_EVENT_TX_UNDERFLOW
                | RAIL_EVENT_SCHEDULER_STATUS)) {
    RAIL_YieldRadio(handle);
  }
}

// Called from RAIL_Init to indicate a new RAIL instance is ready
static void multirailCbRfReady(RAIL_Handle_t handle)
{
  rfReady = true;
}

/** Return the second RAIL handle. Null if not initialized.
 */
RAIL_Handle_t emberAfPluginMultirailDemoGetHandle(void)
{
  return rail2Handle;
}

/** Send a packet using the second RAIL instance.
 *
 * buff .... Pointer to the data (make sue it is properly formatted)
 * size .... Size of the data
 * channel . Channel (11-26, or 0 to use the current Zigbee channel)
 * scheduledTxConfig (optional)
 * schedulerInfo (optional) .. Scheduler configuration (NULL to use defaults)
 *
 * Returns a RAIL status value.
 */
RAIL_Status_t emberAfPluginMultirailDemoSend(const uint8_t *buff,
                                             uint32_t size,
                                             uint8_t channel,
                                             RAIL_ScheduleTxConfig_t *scheduledTxConfig,
                                             RAIL_SchedulerInfo_t *schedulerInfo)
{
  RAIL_ScheduleTxConfig_t scheduledTxConfigLocal;
  RAIL_SchedulerInfo_t schedulerInfoLocal;

  // Sanity check
  if (!rail2Handle) {
    return RAIL_STATUS_INVALID_STATE;
  }

  if (!scheduledTxConfig) {
    // Use the default schedule to send it as soon as possible.
    scheduledTxConfigLocal.when = 0;
    scheduledTxConfigLocal.mode = RAIL_TIME_DELAY;
    scheduledTxConfig = &scheduledTxConfigLocal;
  }

  if (!schedulerInfo) {
    // This assumes the Tx time is around 10 ms but should be tweaked based
    // on the specific PHY configuration.
    schedulerInfoLocal.priority = 100;
    schedulerInfoLocal.slipTime = 50000;
    schedulerInfoLocal.transactionTime = 10000;
    schedulerInfo = &schedulerInfoLocal;
  }

  if (channel == 0) {
    channel = emberGetRadioChannel();
  }

  // Load the transmit buffer with something to send.
  // If success, transmit this packet at the specified time.
  if (RAIL_WriteTxFifo(rail2Handle, buff, size, true) != size) {
    return RAIL_STATUS_INVALID_CALL;
  } else {
    return RAIL_StartScheduledTx(rail2Handle,
                                 channel,
                                 RAIL_TX_OPTIONS_DEFAULT,
                                 scheduledTxConfig,
                                 schedulerInfo);
  }
}

/** Initialize a new RAIL handle.
 *
 * railCfg ........ RAIL init options (NULL to use the current Zigbee channel config)
 * txPowerConfig .. RAIL power options (NULL to use defaults)
 * paAutoMode ..... Whether an automatic PA mode is enabled
 * defaultTxPower . Default TX power in deci-dBm
 * txFifo ......... Pointer to a 32 bit aligned TX FIFO buffer (NULL to use the default)
 * txFifoSize ..... Size of the TX buffer
 * panId .......... PAN ID (use 0xFFFF if not needed)
 * ieeeAddr ....... Long IEEE address (use NULL if not needed)
 *
 * Returns a new RAIL handle, NULL if not successfully initialized.
 */
RAIL_Handle_t emberAfPluginMultirailDemoInit(RAIL_Config_t *railCfg,
                                             RAIL_TxPowerConfig_t *txPowerConfig,
                                             bool paAutoMode,
                                             RAIL_TxPower_t defaultTxPower,
                                             uint8_t *txFifo,
                                             uint16_t txFifoSize,
                                             uint16_t panId,
                                             const uint8_t *ieeeAddr)
{
  RAIL_TxPowerConfig_t txPowerConfigLocal;

  // Sanity check
  if (rail2Handle) {
    return rail2Handle;
  }

  RAIL_Handle_t handle = RAIL_Init(railCfg ? railCfg : &railCfgLocal,
                                   multirailCbRfReady);
  if (!handle) {
    return NULL;
  }

  if (!txPowerConfig) {
    // Read the current power config from RAIL to reuse in the second instance
    if (RAIL_GetTxPowerConfig(emberGetRailHandle(), &txPowerConfigLocal) != RAIL_STATUS_NO_ERROR) {
      // The new RAIL instance has been initialized by this point and we merely
      // failed to set up parameters (power, Tx FIFO, radio calibrations...).
      // Simply returning NULL will undoubtedly result in a resource leak, but
      // there is no API to de-initialize RAIL. Thus this code is only useful
      // as a demo and/or during debugging. The production code needs a bit more
      // sophisticated error handling.
      return NULL;
    }
    txPowerConfig = &txPowerConfigLocal;
  }

  // Initialise the PA now that the HFXO is up and the timing is correct
  if (RAIL_ConfigTxPower(handle, txPowerConfig) != RAIL_STATUS_NO_ERROR) {
    // Error: The PA could not be initialised due to an improper configuration.
    // Please ensure your configuration is valid for the selected part.
    return NULL;
  }

  // Enable Auto PA
  if (RAIL_EnablePaAutoMode(handle, paAutoMode) != RAIL_STATUS_NO_ERROR) {
    return NULL;
  }

  // Set TX FIFO, and verify that the size is correct
  if (!txFifo) {
    txFifo = railTxFifoLocal;
    txFifoSize = sizeof railTxFifoLocal;
  }
  if (RAIL_SetTxFifo(handle, txFifo, 0, txFifoSize) != txFifoSize) {
    return NULL;
  }

  // Initialise Radio Calibrations
  if (RAIL_ConfigCal(handle, RAIL_CAL_ALL) != RAIL_STATUS_NO_ERROR) {
    return NULL;
  }

  // Configure all RAIL events with appended info
  RAIL_Events_t events = RAIL_EVENT_TX_PACKET_SENT
                         | RAIL_EVENT_TX_ABORTED
                         | RAIL_EVENT_TX_UNDERFLOW
                         | RAIL_EVENT_SCHEDULER_STATUS
                         | RAIL_EVENT_RX_PACKET_RECEIVED
                         | RAIL_EVENT_RX_FRAME_ERROR // invalid CRC
                         | RAIL_EVENT_RX_ADDRESS_FILTERED;
  if (RAIL_ConfigEvents(handle, RAIL_EVENTS_ALL, events) != RAIL_STATUS_NO_ERROR) {
    return NULL;
  }

  RAIL_StateTransitions_t transitions = {
    .success = RAIL_RF_STATE_IDLE,
    .error = RAIL_RF_STATE_IDLE
  };
  if (RAIL_SetRxTransitions(handle, &transitions) != RAIL_STATUS_NO_ERROR) {
    return NULL;
  }

  // Initialise IEEE
  if (!RAIL_IEEE802154_IsEnabled(handle)) {
    //This initialises the radio for 2.4GHz operation.
    if (RAIL_IEEE802154_Config2p4GHzRadio(handle) != RAIL_STATUS_NO_ERROR) {
      return NULL;
    }

    RAIL_StateTiming_t timings = {
      .idleToTx = 100,
      .idleToRx = 100,
      .rxToTx = 192,
      // Make txToRx slightly lower than desired to make sure we get to
      // RX in time
      .txToRx = 192 - 10,
      .rxSearchTimeout = 0,
      .txToRxSearchTimeout = 0
    };

    RAIL_IEEE802154_Config_t rail802154_init = {
      .addresses = NULL,
      .ackConfig = {
        .enable = false,
        .ackTimeout = 0,
        .rxTransitions = {
          .success = RAIL_RF_STATE_IDLE,
          .error = RAIL_RF_STATE_IDLE // ignored
        },
        .txTransitions = {
          .success = RAIL_RF_STATE_IDLE,
          .error = RAIL_RF_STATE_IDLE // ignored
        }
      },
      .timings = timings,
      .framesMask = RAIL_IEEE802154_ACCEPT_STANDARD_FRAMES
                    | RAIL_IEEE802154_ACCEPT_ACK_FRAMES
                    | RAIL_IEEE802154_ACCEPT_DATA_FRAMES
                    | RAIL_IEEE802154_ACCEPT_COMMAND_FRAMES
                    | RAIL_IEEE802154_ACCEPT_BEACON_FRAMES,
      .promiscuousMode = true,
      .isPanCoordinator = false
    };

    if (RAIL_IEEE802154_Init(handle, &rail802154_init) != RAIL_STATUS_NO_ERROR) {
      return NULL;
    }

    // Set the PAN ID
    if (RAIL_IEEE802154_SetPanId(handle, panId, 0x00) != RAIL_STATUS_NO_ERROR) {
      return NULL;
    }

    // Set Long address, if the address is provided.
    if (ieeeAddr
        && RAIL_IEEE802154_SetLongAddress(handle, ieeeAddr, 0x00) != RAIL_STATUS_NO_ERROR) {
      return NULL;
    }
  }

  // Wait until the intialisation is complete (flag set by a RAIL_Init callback)
  while (!rfReady) ;

  // Set default TX power in deci-dBm
  if (RAIL_SetTxPowerDbm(handle, defaultTxPower) != RAIL_STATUS_NO_ERROR) {
    return NULL;
  }

  // Cache and return the new RAIL handle
  rail2Handle = handle;
  return handle;
}
