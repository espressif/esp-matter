/***************************************************************************//**
 * @file sl_rail_mux.c
 * @brief RAIL Multiplexer implementation
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SLI_COEX_SUPPORTED
#define SLI_COEX_SUPPORTED 1
#endif // SLI_COEX_SUPPORTED

#ifndef SLI_ANTDIV_SUPPORTED
#define SLI_ANTDIV_SUPPORTED 1
#endif // SLI_ANTDIV_SUPPORTED

#define SLI_DMP_TUNING_SUPPORTED SLI_COEX_SUPPORTED // DMP_TUNING requires COEX

#include "rail.h"
#include "rail_ieee802154.h"
#include "rail_module.h"
#include "rail_util_ieee802154/sl_rail_util_ieee802154_phy_select.h"
#ifdef SL_RAIL_UTIL_PA_CONFIG_HEADER
#include SL_RAIL_UTIL_PA_CONFIG_HEADER
#endif
#include "em_common.h" // for SL_WEAK

#include "sl_rail_mux.h"

#include "buffer_manager/buffer-management.h"
#include "buffer_manager/buffer-queue.h"

//------------------------------------------------------------------------------
// Forward declarations

static void fn_update_current_tx_power(void);
static void fn_set_global_flag(uint16_t flag, bool val);
static bool fn_get_global_flag(uint16_t flag);
static uint8_t fn_get_context_index(RAIL_Handle_t handle);
static void fn_set_context_flag_by_index(uint8_t index, uint16_t flag, bool val);
static bool fn_get_context_flag_by_index(uint8_t index, uint16_t flag);
static uint8_t fn_get_context_index(RAIL_Handle_t handle);
static void fn_init_802154_address_config(RAIL_IEEE802154_AddrConfig_t *addr_config);
static void fn_update_802154_address_filtering_table(void);
static RAIL_Status_t fn_start_pending_tx(void);
static bool fn_operation_in_progress(uint16_t operation_flags);
static uint8_t fn_get_active_tx_context_index(void);

HIDDEN void fn_mux_rail_init_callback(RAIL_Handle_t railHandle);
HIDDEN void fn_mux_rail_events_callback(RAIL_Handle_t railHandle, RAIL_Events_t events);
HIDDEN void fn_timer_callback(struct RAIL_MultiTimer *tmr,
                              RAIL_Time_t expectedTimeOfEvent,
                              void *cbArg);

#define tx_in_progress()                                          \
  fn_operation_in_progress(RAIL_MUX_PROTOCOL_FLAGS_TX_IN_PROGRESS \
                           | RAIL_MUX_PROTOCOL_FLAGS_WAIT_FOR_ACK)

// (If no lock is active || lock acquired by current context_index )
#define check_lock_permissions(context_index)                     \
  (!fn_operation_in_progress(RAIL_MUX_PROTOCOL_FLAGS_LOCK_ACTIVE) \
   || fn_get_context_flag_by_index(context_index, RAIL_MUX_PROTOCOL_FLAGS_LOCK_ACTIVE) )
//------------------------------------------------------------------------------
// Globals

RAIL_Handle_t emPhyRailHandle = NULL;
#define mux_rail_handle emPhyRailHandle

static volatile uint16_t internal_flags = 0;
static RAIL_TxPower_t current_tx_power = RAIL_TX_POWER_MIN;
static uint16_t rx_channel = INVALID_CHANNEL;
static RAIL_IEEE802154_AddrConfig_t rail_addresses_802154;
static RAILSched_Config_t mux_scheduler_config;

HIDDEN sl_rail_mux_context_t protocol_context[SUPPORTED_PROTOCOL_COUNT];

HIDDEN RAIL_Config_t mux_rail_config = {
  .eventsCallback = fn_mux_rail_events_callback,
  .protocol = NULL, // only used by Bluetooth stack
  .scheduler = &mux_scheduler_config,
};

HIDDEN const RAIL_IEEE802154_Config_t ieee_802154_config = {
  NULL, // addresses
  {
    // ackConfig
    true, // ackConfig.enable
    672,  // ackConfig.ackTimeout
    {
      // ackConfig.rxTransitions
      RAIL_RF_STATE_RX,   // ackConfig.rxTransitions.success
      RAIL_RF_STATE_RX,   // ackConfig.rxTransitions.error
    },
    {
      // ackConfig.txTransitions
      RAIL_RF_STATE_RX,   // ackConfig.txTransitions.success
      RAIL_RF_STATE_RX,   // ackConfig.txTransitions.error
    },
  },
  {
    // timings
    100,      // timings.idleToRx
    192 - 10, // timings.txToRx
    100,      // timings.idleToTx
    256,      // timings.rxToTx - This is set to 256 to accommodate enhanced
              // ACKs (part of OT >= 1.2), regular value would 192.
    0,        // timings.rxSearchTimeout
    0,        // timings.txToRxSearchTimeout
    0,        // timings.txToTx
  },
  RAIL_IEEE802154_ACCEPT_STANDARD_FRAMES, // framesMask
  false,                                  // promiscuousMode
  false,                                  // isPanCoordinator
  false,                                  // defaultFramePendingInOutgoingAcks
};

//------------------------------------------------------------------------------
// Internal APIs

void sli_rail_mux_local_init(void)
{
  uint8_t i;

  // We use the RAIL config pointer to indicate whether an entry is already in
  // use or not.
  for (i = 0; i < SUPPORTED_PROTOCOL_COUNT; i++) {
    protocol_context[i].rail_config = NULL;
    protocol_context[i].events = 0;
    protocol_context[i].flags = 0;
    protocol_context[i].fifo_tx_info.tx_init_length = 0;
    protocol_context[i].fifo_tx_info.tx_size = 0;

    protocol_context[i].channel = INVALID_CHANNEL;
    protocol_context[i].tx_repeat_config.iterations = 0;
    protocol_context[i].tx_power = RAIL_TX_POWER_MIN;
    // 802.15.4 specific fields
    fn_init_802154_address_config(&protocol_context[i].addr_802154);
    protocol_context[i].is_pan_coordinator_802154 = false;
    // Initialize to address broadcast and PAN broadcast
    protocol_context[i].addr_filter_mask_802154 = RAIL_FILTERING_MASK_BROADCAST_ENABLED;
  }

  internal_flags = 0;
  rx_channel = INVALID_CHANNEL;
  fn_init_802154_address_config(&rail_addresses_802154);

  // TODO: for now we assume all protocols to be 802.15.4 and use the 2.4 OQPSK
  // standard PHY. In order to support SubGHz PHY, we will need to modify the
  // rail_config accordingly.
}

//------------------------------------------------------------------------------
// Public APIs

RAIL_Handle_t sl_rail_mux_Init(RAIL_Config_t *railCfg,
                               RAIL_InitCompleteCallbackPtr_t cb)
{
  RAIL_MUX_DECLARE_IRQ_STATE;
  uint8_t i;

  RAIL_MUX_ENTER_CRITICAL();

  // Find an unused protocol context entry. Return a NULL handler if an entry
  // is not available. If we find an entry containing the same railCfg, it means
  // that the protocol called RAIL_Init() multiple times, in this case we just
  // return the RAIL handle.
  for (i = 0; i < SUPPORTED_PROTOCOL_COUNT; i++) {
    if (protocol_context[i].rail_config == railCfg) {
      return &(protocol_context[i]);
    }

    if (protocol_context[i].rail_config == NULL) {
      break;
    }
  }

  if (i >= SUPPORTED_PROTOCOL_COUNT) {
    return NULL;
  }

  protocol_context[i].rail_config = railCfg;
  protocol_context[i].init_callback = cb;
  fn_set_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_INIT_CB_PENDING, true);

  // TODO: confirm with the RAIL team that the init callback *always* gets
  // called before RAIL_Init() returns. If that is the case, this logic can be
  // much simpler.

  // First call to sl_rail_mux_Init(), we need to call RAIL_Init().
  if (!fn_get_global_flag(RAIL_MUX_FLAGS_RAIL_INIT_STARTED)) {
    fn_set_global_flag(RAIL_MUX_FLAGS_RAIL_INIT_STARTED, true);
    mux_rail_handle = RAIL_Init(&mux_rail_config, fn_mux_rail_init_callback);
    assert(mux_rail_handle != NULL);
  } else if (fn_get_global_flag(RAIL_MUX_FLAGS_RAIL_INIT_COMPLETED)) {
    // RAIL Init already completed, we can just call the protocol
    // init_complete callback here.

    fn_set_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_INIT_CB_PENDING, false);
    if (cb != NULL) {
      cb(&(protocol_context[i]));
    }
  }

  // If the RAIL initialization was started by another protocol, but not
  // completed yet. We will call the protocol init_completed callback when
  // RAIL calls it.

  RAIL_MUX_EXIT_CRITICAL();

  // Enable use of RAIL multi-timer.
  RAIL_ConfigMultiTimer(true);

  // The RAIL handler we return to the protocol layers is a pointer to our own
  // internal protocol context structure.
  return &(protocol_context[i]);
}

RAIL_Status_t sl_rail_mux_IEEE802154_Init(RAIL_Handle_t railHandle,
                                          const RAIL_IEEE802154_Config_t *config)
{
  RAIL_MUX_DECLARE_IRQ_STATE;
  (void)railHandle;
  (void)config;

  RAIL_MUX_ENTER_CRITICAL();

  if (!fn_get_global_flag(RAIL_MUX_FLAGS_IEEE802154_INIT_COMPLETED)) {
    assert(RAIL_IEEE802154_Init(mux_rail_handle, &ieee_802154_config)
           == RAIL_STATUS_NO_ERROR);
    fn_set_global_flag(RAIL_MUX_FLAGS_IEEE802154_INIT_COMPLETED, true);
  }

  RAIL_MUX_EXIT_CRITICAL();

  return RAIL_STATUS_NO_ERROR;
}

sl_status_t sli_rail_mux_lock_radio(RAIL_Handle_t railHandle)
{
  RAIL_MUX_DECLARE_IRQ_STATE;

  uint8_t context_index = fn_get_context_index(railHandle);
  assert(context_index < SUPPORTED_PROTOCOL_COUNT);
  sl_status_t ret_val = SL_STATUS_OK;

  RAIL_MUX_ENTER_CRITICAL();
  // Is lock currently active, but for a different protocol
  if ( !check_lock_permissions(context_index)) {
    ret_val = SL_STATUS_FAIL;
  } else {
    // LOCK CAN BE GRANTED
    // Some cleanup to be performed before granting the lock

    for (uint8_t i = 0; (i < SUPPORTED_PROTOCOL_COUNT) && (i != context_index); i++) {
      // 1. Cancel any previously scheduled RX operations on the other protocol
      if (fn_get_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_RX_SCHEDULED)) {
        // Call to RAIL Idle will cancel any previously scheduled Rx on the other protocol
        RAIL_Idle(mux_rail_handle, RAIL_IDLE_ABORT, true);

        //restore rx state since we were previously in rx
        if ( rx_channel != INVALID_CHANNEL) {
          RAIL_StartRx(mux_rail_handle, rx_channel, NULL);
        }
        fn_mux_rail_events_callback(mux_rail_handle, RAIL_EVENT_RX_SCHEDULED_RX_END);
      }

      // 2. Check if there are any TX scheduled events on the other protocol
      if (fn_get_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_TX_SCHEDULED)) {
        // Stop all pending transmit operations
        RAIL_StopTx(mux_rail_handle, RAIL_STOP_MODE_PENDING_SHIFT);

        // First call tx started, this will unblock statemachine and clear the flag
        fn_mux_rail_events_callback(mux_rail_handle, RAIL_EVENT_SCHEDULED_TX_STARTED);

        // Now post TX Blocked to inform the application that the tx did not go through
        fn_mux_rail_events_callback(mux_rail_handle, RAIL_EVENT_TX_BLOCKED);
      }
    }
    fn_set_context_flag_by_index(context_index, RAIL_MUX_PROTOCOL_FLAGS_LOCK_ACTIVE, true);
  }
  RAIL_MUX_EXIT_CRITICAL();
  return ret_val;
}

sl_status_t sli_rail_mux_unlock_radio(RAIL_Handle_t railHandle)
{
  RAIL_MUX_DECLARE_IRQ_STATE;

  uint8_t context_index = fn_get_context_index(railHandle);
  assert(context_index < SUPPORTED_PROTOCOL_COUNT);
  sl_status_t ret_val = SL_STATUS_OK;

  RAIL_MUX_ENTER_CRITICAL();
  // Is lock currently active, but for a different protocol
  if ( !check_lock_permissions(context_index) ) {
    ret_val = SL_STATUS_FAIL;
  } else {
    fn_set_context_flag_by_index(context_index, RAIL_MUX_PROTOCOL_FLAGS_LOCK_ACTIVE, false);
  }
  RAIL_MUX_EXIT_CRITICAL();

  return ret_val;
}

RAIL_Status_t sl_rail_mux_ConfigEvents(RAIL_Handle_t railHandle,
                                       RAIL_Events_t mask,
                                       RAIL_Events_t events)
{
  RAIL_MUX_DECLARE_IRQ_STATE;

  RAIL_Events_t all_protocols_events = RAIL_EVENTS_NONE;
  RAIL_Events_t updated_protocol_events;
  RAIL_Status_t status;
  uint8_t i;

  uint8_t context_index = fn_get_context_index(railHandle);
  assert(context_index < SUPPORTED_PROTOCOL_COUNT);

  updated_protocol_events = (protocol_context[context_index].events & ~mask);
  updated_protocol_events |= (mask & events);

  RAIL_MUX_ENTER_CRITICAL();

  for (i = 0; i < SUPPORTED_PROTOCOL_COUNT; i++) {
    if (i == context_index) {
      // Use the updated events for the protocol that is requesting a change.
      all_protocols_events |= updated_protocol_events;
    } else {
      all_protocols_events |= protocol_context[i].events;
    }
  }

  status = RAIL_ConfigEvents(mux_rail_handle,
                             RAIL_EVENTS_ALL,
                             all_protocols_events);

  // Update protocol events only if the actual RAIL_ConfigEvents() call was
  // successful.
  if (status == RAIL_STATUS_NO_ERROR) {
    protocol_context[context_index].events = updated_protocol_events;
  }

  RAIL_MUX_EXIT_CRITICAL();

  return status;
}

RAIL_Status_t sl_rail_mux_SetPtiProtocol(RAIL_Handle_t railHandle,
                                         RAIL_PtiProtocol_t protocol)
{
  (void)railHandle;
  (void)protocol;

  if (!fn_get_global_flag(RAIL_MUX_FLAGS_RAIL_SET_PTI_DONE)) {
    fn_set_global_flag(RAIL_MUX_FLAGS_RAIL_SET_PTI_DONE, true);
    // TODO: for now we simply set it to 802154, which means that Network
    // Analyzer as is, would only decode the MAC portion of the Zigbee/OT
    // frames. We might want to refine this, either by using a custom protocol
    // and do some work on the network analyzer side of things, or at least
    // change the PTI protocol at runtime so that least for outgoing packets
    // with get proper decoding.
    RAIL_SetPtiProtocol(mux_rail_handle, RAIL_PTI_PROTOCOL_802154);
  }

  return RAIL_STATUS_NO_ERROR;
}

RAIL_Status_t sl_rail_mux_IEEE802154_SetShortAddress(RAIL_Handle_t railHandle,
                                                     uint16_t shortAddr,
                                                     uint8_t index)
{
  RAIL_MUX_DECLARE_IRQ_STATE;

  uint8_t context_index = fn_get_context_index(railHandle);
  assert(context_index < SUPPORTED_PROTOCOL_COUNT);

  if (index >= RAIL_IEEE802154_MAX_ADDRESSES) {
    return RAIL_STATUS_INVALID_PARAMETER;
  }

  RAIL_MUX_ENTER_CRITICAL();

  protocol_context[context_index].addr_802154.shortAddr[index] = shortAddr;

  fn_update_802154_address_filtering_table();

  RAIL_MUX_EXIT_CRITICAL();

  return RAIL_STATUS_NO_ERROR;
}

RAIL_Status_t sl_rail_mux_IEEE802154_SetLongAddress(RAIL_Handle_t railHandle,
                                                    const uint8_t *longAddr,
                                                    uint8_t index)
{
  RAIL_MUX_DECLARE_IRQ_STATE;

  uint8_t context_index = fn_get_context_index(railHandle);
  assert(context_index < SUPPORTED_PROTOCOL_COUNT);

  if (index >= RAIL_IEEE802154_MAX_ADDRESSES) {
    return RAIL_STATUS_INVALID_PARAMETER;
  }

  RAIL_MUX_ENTER_CRITICAL();

  memcpy(protocol_context[context_index].addr_802154.longAddr[index], longAddr, 8);

  fn_update_802154_address_filtering_table();

  RAIL_MUX_EXIT_CRITICAL();

  return RAIL_STATUS_NO_ERROR;
}

RAIL_Status_t sl_rail_mux_IEEE802154_SetPanId(RAIL_Handle_t railHandle,
                                              uint16_t panId,
                                              uint8_t index)
{
  RAIL_MUX_DECLARE_IRQ_STATE;

  uint8_t context_index = fn_get_context_index(railHandle);
  assert(context_index < SUPPORTED_PROTOCOL_COUNT);

  if (index >= RAIL_IEEE802154_MAX_ADDRESSES) {
    return RAIL_STATUS_INVALID_PARAMETER;
  }

  RAIL_MUX_ENTER_CRITICAL();

  protocol_context[context_index].addr_802154.panId[index] = panId;

  fn_update_802154_address_filtering_table();

  RAIL_MUX_EXIT_CRITICAL();

  return RAIL_STATUS_NO_ERROR;
}

RAIL_Status_t sl_rail_mux_IEEE802154_SetAddresses(RAIL_Handle_t railHandle,
                                                  const RAIL_IEEE802154_AddrConfig_t *addresses)
{
  RAIL_MUX_DECLARE_IRQ_STATE;

  uint8_t context_index = fn_get_context_index(railHandle);
  assert(context_index < SUPPORTED_PROTOCOL_COUNT);

  RAIL_MUX_ENTER_CRITICAL();

  memcpy(&protocol_context[context_index].addr_802154,
         addresses,
         sizeof(RAIL_IEEE802154_AddrConfig_t));

  fn_update_802154_address_filtering_table();

  RAIL_MUX_EXIT_CRITICAL();

  return RAIL_STATUS_NO_ERROR;
}

RAIL_Status_t sl_rail_mux_IEEE802154_SetPanCoordinator(RAIL_Handle_t railHandle,
                                                       bool isPanCoordinator)
{
  RAIL_MUX_DECLARE_IRQ_STATE;
  uint8_t i;

  uint8_t context_index = fn_get_context_index(railHandle);
  assert(context_index < SUPPORTED_PROTOCOL_COUNT);

  RAIL_MUX_ENTER_CRITICAL();

  protocol_context[context_index].is_pan_coordinator_802154 = isPanCoordinator;

  for (i = 0; i < SUPPORTED_PROTOCOL_COUNT; i++) {
    if (protocol_context[i].is_pan_coordinator_802154) {
      break;
    }
  }
  // We push down to RAIL a 'true' coordinator flag if at least one protocol is
  // configured as 802.15.4 coordinator.
  RAIL_IEEE802154_SetPanCoordinator(mux_rail_handle,
                                    (i < SUPPORTED_PROTOCOL_COUNT));

  RAIL_MUX_EXIT_CRITICAL();

  return RAIL_STATUS_NO_ERROR;
}

RAIL_Status_t sl_rail_mux_StartRx(RAIL_Handle_t railHandle,
                                  uint16_t channel,
                                  const RAIL_SchedulerInfo_t *schedulerInfo)
{
  RAIL_MUX_DECLARE_IRQ_STATE;

  uint8_t context_index = fn_get_context_index(railHandle);
  assert(context_index < SUPPORTED_PROTOCOL_COUNT);
  RAIL_Status_t ret_status = RAIL_STATUS_NO_ERROR;

  RAIL_MUX_ENTER_CRITICAL();

  // Store channel in the protocol context
  // This is done regardless of the lock because a lock will
  // prevent start rx operation and channel change from happening
  // and rx events will use the channel to determine whether to
  // pass the event up or not
  protocol_context[context_index].channel = channel;

  // Check to ensure lock is not active before acting on startRx
  if ( check_lock_permissions(context_index) ) {
    rx_channel = channel;
    fn_update_802154_address_filtering_table();
    // TODO: for now we pass down the schedulerInfo "as is".
    ret_status = RAIL_StartRx(mux_rail_handle, channel, schedulerInfo);
  }
  RAIL_MUX_EXIT_CRITICAL();

  return ret_status;
}

void sl_rail_mux_Idle(RAIL_Handle_t railHandle,
                      RAIL_IdleMode_t mode,
                      bool wait)
{
  uint8_t i;

  uint8_t context_index = fn_get_context_index(railHandle);
  assert(context_index < SUPPORTED_PROTOCOL_COUNT);

  protocol_context[context_index].channel = INVALID_CHANNEL;

  // If another protocol is RXing, we stay in RX, otherwise we idle the radio.
  for (i = 0; i < SUPPORTED_PROTOCOL_COUNT; i++) {
    if (check_lock_permissions(i) && protocol_context[i].channel != INVALID_CHANNEL) {
      rx_channel = protocol_context[i].channel;
      RAIL_StartRx(mux_rail_handle, protocol_context[i].channel, NULL);
      return;
    }
  }

  rx_channel = INVALID_CHANNEL;
  RAIL_Idle(mux_rail_handle, mode, wait);
}

RAIL_Status_t sl_rail_mux_ConfigRxOptions(RAIL_Handle_t railHandle,
                                          RAIL_RxOptions_t mask,
                                          RAIL_RxOptions_t options)
{
  (void)railHandle;

  // TODO: Maintain separate options per protocol? Complain if there is some
  // conflict among the protocols' options?
  // For now we can skip this since both MAC layers configure the same RX
  // options.

  return RAIL_ConfigRxOptions(mux_rail_handle, mask, options);
}

RAIL_Status_t sl_rail_mux_SetTaskPriority(RAIL_Handle_t railHandle,
                                          uint8_t priority,
                                          RAIL_TaskType_t taskType)
{
  (void)railHandle;

  return RAIL_SetTaskPriority(mux_rail_handle, priority, taskType);
}

RAIL_Status_t sl_rail_mux_IEEE802154_Config2p4GHzRadio(RAIL_Handle_t railHandle)
{
  (void)railHandle;

  return RAIL_IEEE802154_Config2p4GHzRadio(mux_rail_handle);
}

uint16_t sl_rail_mux_ConfigChannels(RAIL_Handle_t railHandle,
                                    const RAIL_ChannelConfig_t *config,
                                    RAIL_RadioConfigChangedCallback_t cb)
{
  (void)railHandle;

  // TODO: this currently only called from Zigbee in simulation.

  return RAIL_ConfigChannels(mux_rail_handle, config, cb);
}

RAIL_Status_t sl_rail_mux_ConvertLqi(RAIL_Handle_t railHandle,
                                     RAIL_ConvertLqiCallback_t cb)
{
  (void)railHandle;

  // The callback does not pass up a RAIL_Handle_t reference, so we can just
  // pass the callback from the upper layer down to RAIL.

  return RAIL_ConvertLqi(mux_rail_handle, cb);
}

RAIL_Status_t sl_rail_mux_Calibrate(RAIL_Handle_t railHandle,
                                    RAIL_CalValues_t *calValues,
                                    RAIL_CalMask_t calForce)
{
  (void)railHandle;

  return RAIL_Calibrate(mux_rail_handle, calValues, calForce);
}

RAIL_Status_t sl_rail_mux_IEEE802154_CalibrateIr2p4Ghz(RAIL_Handle_t railHandle,
                                                       uint32_t *imageRejection)
{
  (void)railHandle;

  return RAIL_IEEE802154_CalibrateIr2p4Ghz(mux_rail_handle, imageRejection);
}

uint16_t sl_rail_mux_GetRadioEntropy(RAIL_Handle_t railHandle,
                                     uint8_t *buffer,
                                     uint16_t bytes)
{
  (void)railHandle;

  return RAIL_GetRadioEntropy(mux_rail_handle, buffer, bytes);
}

RAIL_Status_t sl_rail_mux_IsValidChannel(RAIL_Handle_t railHandle, uint16_t channel)
{
  (void)railHandle;

  return RAIL_IsValidChannel(mux_rail_handle, channel);
}

int16_t sl_rail_mux_GetRssi(RAIL_Handle_t railHandle, bool wait)
{
  (void)railHandle;

  return RAIL_GetRssi(mux_rail_handle, wait);
}

uint32_t sl_rail_mux_GetSymbolRate(RAIL_Handle_t railHandle)
{
  (void)railHandle;

  return RAIL_GetSymbolRate(mux_rail_handle);
}

uint32_t sl_rail_mux_GetBitRate(RAIL_Handle_t railHandle)
{
  (void)railHandle;

  return RAIL_GetBitRate(mux_rail_handle);
}

RAIL_Status_t sl_rail_mux_SetFreqOffset(RAIL_Handle_t railHandle,
                                        RAIL_FrequencyOffset_t freqOffset)
{
  (void)railHandle;

  return RAIL_SetFreqOffset(mux_rail_handle, freqOffset);
}

RAIL_Status_t sl_rail_mux_SetTimer(RAIL_Handle_t railHandle,
                                   RAIL_Time_t time,
                                   RAIL_TimeMode_t mode,
                                   RAIL_TimerCallback_t cb)
{
  uint8_t context_index = fn_get_context_index(railHandle);
  assert(context_index < SUPPORTED_PROTOCOL_COUNT);

  protocol_context[context_index].timer_callback = cb;

  return RAIL_SetMultiTimer(&protocol_context[context_index].timer,
                            time,
                            mode,
                            (cb == NULL) ? NULL : fn_timer_callback,
                            railHandle);
}

void sl_rail_mux_CancelTimer(RAIL_Handle_t railHandle)
{
  uint8_t context_index = fn_get_context_index(railHandle);
  assert(context_index < SUPPORTED_PROTOCOL_COUNT);

  RAIL_CancelMultiTimer(&protocol_context[context_index].timer);
}

RAIL_CalMask_t sl_rail_mux_GetPendingCal(RAIL_Handle_t railHandle)
{
  (void)railHandle;

  return RAIL_GetPendingCal(mux_rail_handle);
}

RAIL_Status_t sl_rail_mux_CalibrateTemp(RAIL_Handle_t railHandle)
{
  (void)railHandle;

  return RAIL_CalibrateTemp(mux_rail_handle);
}

RAIL_RxPacketHandle_t sl_rail_mux_GetRxPacketInfo(RAIL_Handle_t railHandle,
                                                  RAIL_RxPacketHandle_t packetHandle,
                                                  RAIL_RxPacketInfo_t *pPacketInfo)
{
  (void)railHandle;

  return RAIL_GetRxPacketInfo(mux_rail_handle, packetHandle, pPacketInfo);
}

RAIL_Status_t sl_rail_mux_GetRxPacketDetailsAlt(RAIL_Handle_t railHandle,
                                                RAIL_RxPacketHandle_t packetHandle,
                                                RAIL_RxPacketDetails_t *pPacketDetails)
{
  (void)railHandle;

  return RAIL_GetRxPacketDetailsAlt(mux_rail_handle,
                                    packetHandle,
                                    pPacketDetails);
}

RAIL_Status_t sl_rail_mux_GetRxTimeSyncWordEnd(RAIL_Handle_t railHandle,
                                               uint16_t totalPacketBytes,
                                               RAIL_Time_t *pPacketTime)
{
  (void)railHandle;

  return RAIL_GetRxTimeSyncWordEnd(mux_rail_handle,
                                   totalPacketBytes,
                                   pPacketTime);
}

uint16_t sl_rail_mux_PeekRxPacket(RAIL_Handle_t railHandle,
                                  RAIL_RxPacketHandle_t packetHandle,
                                  uint8_t *pDst,
                                  uint16_t len,
                                  uint16_t offset)
{
  (void)railHandle;

  return RAIL_PeekRxPacket(mux_rail_handle,
                           packetHandle,
                           pDst,
                           len,
                           offset);
}

RAIL_Status_t sl_rail_mux_IEEE802154_GetAddress(RAIL_Handle_t railHandle,
                                                RAIL_IEEE802154_Address_t *pAddress)
{
  (void)railHandle;

  return RAIL_IEEE802154_GetAddress(mux_rail_handle, pAddress);
}

void sl_rail_mux_GetRxIncomingPacketInfo(RAIL_Handle_t railHandle,
                                         RAIL_RxPacketInfo_t *pPacketInfo)
{
  (void)railHandle;

  RAIL_GetRxIncomingPacketInfo(mux_rail_handle, pPacketInfo);
  return;
}

int8_t sl_rail_mux_GetRssiOffset(RAIL_Handle_t railHandle)
{
  (void)railHandle;

  return RAIL_GetRssiOffset(mux_rail_handle);
}

RAIL_Status_t sl_rail_mux_IEEE802154_SetFramePending(RAIL_Handle_t railHandle)
{
  (void)railHandle;

  return RAIL_IEEE802154_SetFramePending(mux_rail_handle);
}

RAIL_SchedulerStatus_t sl_rail_mux_GetSchedulerStatus(RAIL_Handle_t railHandle)
{
  (void)railHandle;

  return RAIL_GetSchedulerStatus(mux_rail_handle);
}

bool sl_rail_mux_IsRxAutoAckPaused(RAIL_Handle_t railHandle)
{
  (void)railHandle;

  return RAIL_IsRxAutoAckPaused(mux_rail_handle);
}

RAIL_Status_t sl_rail_mux_GetTxPowerConfig(RAIL_Handle_t railHandle,
                                           RAIL_TxPowerConfig_t *config)
{
  (void)railHandle;

  return RAIL_GetTxPowerConfig(mux_rail_handle, config);
}

RAIL_TxPower_t sl_rail_mux_GetTxPowerDbm(RAIL_Handle_t railHandle)
{
  (void)railHandle;

  return RAIL_GetTxPowerDbm(mux_rail_handle);
}

RAIL_Status_t sl_rail_mux_GetChannel(RAIL_Handle_t railHandle, uint16_t *channel)
{
  (void)railHandle;

  return RAIL_GetChannel(mux_rail_handle, channel);
}

RAIL_TxPower_t sl_rail_mux_GetTxPower(RAIL_Handle_t railHandle)
{
  (void) railHandle;
  return RAIL_GetTxPower(mux_rail_handle);
}

RAIL_Status_t sl_rail_mux_ConfigTxPower(RAIL_Handle_t railHandle,
                                        const RAIL_TxPowerConfig_t *config)
{
  RAIL_MUX_DECLARE_IRQ_STATE;
  uint16_t status = RAIL_STATUS_NO_ERROR;
  (void)railHandle;

  RAIL_MUX_ENTER_CRITICAL();

  if (!fn_get_global_flag(RAIL_MUX_FLAGS_RAIL_CONFIG_TX_POWER_DONE)) {
#ifdef SL_RAIL_UTIL_PA_CONFIG_HEADER
    (void)config;
    RAIL_TxPowerConfig_t txPowerConfig = { SL_RAIL_UTIL_PA_SELECTION_2P4GHZ,
                                           SL_RAIL_UTIL_PA_VOLTAGE_MV,
                                           SL_RAIL_UTIL_PA_RAMP_TIME_US };
    status = RAIL_ConfigTxPower(mux_rail_handle, &txPowerConfig);
#else // !SL_RAIL_UTIL_PA_CONFIG_HEADER
    status = RAIL_ConfigTxPower(mux_rail_handle, config);
#endif // SL_RAIL_UTIL_PA_CONFIG_HEADER

    fn_set_global_flag(RAIL_MUX_FLAGS_RAIL_CONFIG_TX_POWER_DONE, true);
  }

  RAIL_MUX_EXIT_CRITICAL();

  return status;
}

RAIL_Status_t sl_rail_mux_SetTxPowerDbm(RAIL_Handle_t railHandle,
                                        RAIL_TxPower_t power)
{
  RAIL_MUX_DECLARE_IRQ_STATE;

  uint8_t context_index = fn_get_context_index(railHandle);
  assert(context_index < SUPPORTED_PROTOCOL_COUNT);

  RAIL_MUX_ENTER_CRITICAL();
  protocol_context[context_index].tx_power = power;
  fn_update_current_tx_power();
  RAIL_MUX_EXIT_CRITICAL();

  return RAIL_STATUS_NO_ERROR;
}

RAIL_Status_t sl_rail_mux_SetTxPower(RAIL_Handle_t railHandle,
                                     RAIL_TxPowerLevel_t powerLevel)
{
  RAIL_TxPowerConfig_t txPowerConfig;
  RAIL_GetTxPowerConfig(mux_rail_handle, &txPowerConfig);

  RAIL_TxPower_t power = RAIL_ConvertRawToDbm(mux_rail_handle,
                                              txPowerConfig.mode,
                                              powerLevel);
  return sl_rail_mux_SetTxPowerDbm(railHandle, power);
}

void sl_rail_mux_YieldRadio(RAIL_Handle_t railHandle)
{
  (void)railHandle;

  RAIL_YieldRadio(mux_rail_handle);
  return;
}

RAIL_RadioState_t sl_rail_mux_GetRadioState(RAIL_Handle_t railHandle)
{
  (void)railHandle;

  return RAIL_GetRadioState(mux_rail_handle);
}

uint16_t sl_rail_mux_SetTxFifo(RAIL_Handle_t railHandle,
                               uint8_t *addr,
                               uint16_t initLength,
                               uint16_t size)
{
  RAIL_MUX_DECLARE_IRQ_STATE;
  uint16_t buf_len;

  uint8_t context_index = fn_get_context_index(railHandle);
  assert(context_index < SUPPORTED_PROTOCOL_COUNT);

  if (size > RAIL_MUX_MAX_BUFFER_SIZE) {
    buf_len = RAIL_MUX_MAX_BUFFER_SIZE;
  } else if (size > 0) {
    buf_len = size;
  } else if (initLength > RAIL_MUX_MAX_BUFFER_SIZE) {
    buf_len = RAIL_MUX_MAX_BUFFER_SIZE;
  } else {
    buf_len = initLength;
  }

  RAIL_MUX_ENTER_CRITICAL();

  fn_set_context_flag_by_index(context_index, RAIL_MUX_PROTOCOL_FLAGS_SETUP_TX_FIFO, true);
  protocol_context[context_index].fifo_tx_info.data_ptr = addr;
  protocol_context[context_index].fifo_tx_info.tx_init_length = initLength;
  protocol_context[context_index].fifo_tx_info.tx_size = size;

  RAIL_MUX_EXIT_CRITICAL();

  return buf_len;
}

RAIL_Status_t sl_rail_mux_StartCcaCsmaTx(RAIL_Handle_t railHandle,
                                         uint16_t channel,
                                         RAIL_TxOptions_t options,
                                         const RAIL_CsmaConfig_t *csmaConfig,
                                         const RAIL_SchedulerInfo_t *schedulerInfo)
{
  RAIL_MUX_DECLARE_IRQ_STATE;
  uint16_t ret_status = RAIL_STATUS_INVALID_STATE;

  uint8_t context_index = fn_get_context_index(railHandle);
  assert(context_index < SUPPORTED_PROTOCOL_COUNT);

  // Reject operation if other protocol holds lock
  RAIL_MUX_ENTER_CRITICAL();
  if ( check_lock_permissions(context_index) ) {
    if (!fn_get_context_flag_by_index(context_index, RAIL_MUX_PROTOCOL_FLAGS_START_TX_PENDING)) {
      fn_set_context_flag_by_index(context_index, RAIL_MUX_PROTOCOL_FLAGS_START_TX_PENDING, true);
      protocol_context[context_index].csma_tx_info.tx_type = SINGLE_TX_CCA_CSMA_REQUEST;
      protocol_context[context_index].csma_tx_info.channel = channel;
      protocol_context[context_index].csma_tx_info.options = options;

      fn_set_context_flag_by_index(context_index,
                                   RAIL_MUX_PROTOCOL_FLAGS_CSMA_CONFIG_VALID,
                                   (csmaConfig != NULL));
      if (csmaConfig != NULL) {
        protocol_context[context_index].csma_tx_info.csmaConfig = *csmaConfig;
      }

      fn_set_context_flag_by_index(context_index,
                                   RAIL_MUX_PROTOCOL_FLAGS_SCHEDULER_INFO_VALID,
                                   (schedulerInfo != NULL));
      if (schedulerInfo != NULL) {
        protocol_context[context_index].csma_tx_info.schedulerInfo = *schedulerInfo;
      }
    }
    ret_status = fn_start_pending_tx();
  }
  RAIL_MUX_EXIT_CRITICAL();
  return ret_status;
}

RAIL_Status_t sl_rail_mux_StartTx(RAIL_Handle_t railHandle,
                                  uint16_t channel,
                                  RAIL_TxOptions_t options,
                                  const RAIL_SchedulerInfo_t *schedulerInfo)
{
  RAIL_MUX_DECLARE_IRQ_STATE;
  RAIL_Status_t ret_status = RAIL_STATUS_INVALID_STATE;

  uint8_t context_index = fn_get_context_index(railHandle);
  assert(context_index < SUPPORTED_PROTOCOL_COUNT);

  RAIL_MUX_ENTER_CRITICAL();
  if ( check_lock_permissions(context_index) ) {
    if (!fn_get_context_flag_by_index(context_index, RAIL_MUX_PROTOCOL_FLAGS_START_TX_PENDING)) {
      fn_set_context_flag_by_index(context_index, RAIL_MUX_PROTOCOL_FLAGS_START_TX_PENDING, true);
      protocol_context[context_index].csma_tx_info.tx_type = SINGLE_TX_REQUEST;
      protocol_context[context_index].csma_tx_info.channel = channel;
      protocol_context[context_index].csma_tx_info.options = options;

      fn_set_context_flag_by_index(context_index,
                                   RAIL_MUX_PROTOCOL_FLAGS_SCHEDULER_INFO_VALID,
                                   (schedulerInfo != NULL));
      if (schedulerInfo != NULL) {
        protocol_context[context_index].csma_tx_info.schedulerInfo = *schedulerInfo;
      }
    }
    ret_status = fn_start_pending_tx();
  }
  RAIL_MUX_EXIT_CRITICAL();

  return ret_status;
}

RAIL_Status_t sl_rail_mux_StartTxStream(RAIL_Handle_t railHandle,
                                        uint16_t channel,
                                        RAIL_StreamMode_t mode)
{
  RAIL_MUX_DECLARE_IRQ_STATE;
  uint8_t context_index = fn_get_context_index(railHandle);
  assert(context_index < SUPPORTED_PROTOCOL_COUNT);
  RAIL_Status_t ret_status = RAIL_STATUS_INVALID_STATE;

  RAIL_MUX_ENTER_CRITICAL();
  bool is_ok_to_proceed = check_lock_permissions(context_index);
  RAIL_MUX_EXIT_CRITICAL();

  if ( is_ok_to_proceed ) {
    ret_status = RAIL_StartTxStream(mux_rail_handle, channel, mode);
  }

  return ret_status;
}

RAIL_Status_t sl_rail_mux_StopTxStream(RAIL_Handle_t railHandle)
{
  (void)railHandle;

  return RAIL_StopTxStream(mux_rail_handle);
}

RAIL_Status_t sl_rail_mux_IEEE802154_SetPromiscuousMode(RAIL_Handle_t railHandle,
                                                        bool enable)
{
  (void)railHandle;

  return RAIL_IEEE802154_SetPromiscuousMode(mux_rail_handle, enable);
}

void sl_rail_mux_PauseRxAutoAck(RAIL_Handle_t railHandle,
                                bool pause)
{
  (void)railHandle;

  RAIL_PauseRxAutoAck(mux_rail_handle, pause);
  return;
}

RAIL_Status_t sl_rail_mux_SetCcaThreshold(RAIL_Handle_t railHandle,
                                          int8_t ccaThresholdDbm)
{
  (void)railHandle;

  return RAIL_SetCcaThreshold(mux_rail_handle, ccaThresholdDbm);
}

bool sl_rail_mux_IEEE802154_IsEnabled(RAIL_Handle_t railHandle)
{
  (void)railHandle;

  return RAIL_IEEE802154_IsEnabled(mux_rail_handle);
}

RAIL_Status_t sl_rail_mux_SetRxTransitions(RAIL_Handle_t railHandle,
                                           const RAIL_StateTransitions_t *transitions)
{
  (void)railHandle;

  return RAIL_SetRxTransitions(mux_rail_handle, transitions);
}

RAIL_Status_t sl_rail_mux_ConfigCal(RAIL_Handle_t railHandle,
                                    RAIL_CalMask_t calEnable)
{
  (void)railHandle;

  return RAIL_ConfigCal(mux_rail_handle, calEnable);
}

RAIL_Status_t sl_rail_mux_EnablePaAutoMode(RAIL_Handle_t railHandle, bool enable)
{
  (void)railHandle;

  return RAIL_EnablePaAutoMode(mux_rail_handle, enable);
}

uint16_t sl_rail_mux_ReadRxFifo(RAIL_Handle_t railHandle,
                                uint8_t *dataPtr,
                                uint16_t readLength)
{
  (void)railHandle;

  return RAIL_ReadRxFifo(mux_rail_handle, dataPtr, readLength);
}

uint16_t sl_rail_mux_GetRxFifoBytesAvailable(RAIL_Handle_t railHandle)
{
  (void)railHandle;

  return RAIL_GetRxFifoBytesAvailable(mux_rail_handle);
}

RAIL_Status_t sl_rail_mux_StartScheduledCcaCsmaTx(RAIL_Handle_t railHandle,
                                                  uint16_t channel,
                                                  RAIL_TxOptions_t options,
                                                  const RAIL_ScheduleTxConfig_t *scheduleTxConfig,
                                                  const RAIL_CsmaConfig_t *csmaConfig,
                                                  const RAIL_SchedulerInfo_t *schedulerInfo)
{
  RAIL_MUX_DECLARE_IRQ_STATE;
  RAIL_Status_t ret_status = RAIL_STATUS_INVALID_STATE;

  uint8_t context_index = fn_get_context_index(railHandle);
  assert(context_index < SUPPORTED_PROTOCOL_COUNT);

  // ToDo: we might need to consider protocol priorities here

  RAIL_MUX_ENTER_CRITICAL();
  if ( check_lock_permissions(context_index) ) {
    // Even if there was a pending tx here, we are gonna re-write it, but we have
    // to make sure tx FIFO is also written.
    fn_set_context_flag_by_index(context_index, RAIL_MUX_PROTOCOL_FLAGS_SCHEDULED_TX_PENDING, true);
    protocol_context[context_index].csma_tx_info.tx_type = SCHEDULED_TX_CCA_CSMA_REQUEST;
    protocol_context[context_index].csma_tx_info.channel = channel;
    protocol_context[context_index].csma_tx_info.options = options;

    fn_set_context_flag_by_index(context_index,
                                 RAIL_MUX_PROTOCOL_FLAGS_SCHEDULE_TX_CONFIG_VALID,
                                 (scheduleTxConfig != NULL));
    if (scheduleTxConfig != NULL) {
      protocol_context[context_index].csma_tx_info.scheduler_config = *scheduleTxConfig;
    }

    fn_set_context_flag_by_index(context_index,
                                 RAIL_MUX_PROTOCOL_FLAGS_CSMA_CONFIG_VALID,
                                 (csmaConfig != NULL));
    if (csmaConfig != NULL) {
      protocol_context[context_index].csma_tx_info.csmaConfig = *csmaConfig;
    }

    fn_set_context_flag_by_index(context_index,
                                 RAIL_MUX_PROTOCOL_FLAGS_SCHEDULER_INFO_VALID,
                                 (schedulerInfo != NULL));
    if (schedulerInfo != NULL) {
      protocol_context[context_index].csma_tx_info.schedulerInfo = *schedulerInfo;
    }

    ret_status = fn_start_pending_tx();
  }
  RAIL_MUX_EXIT_CRITICAL();
  return ret_status;
}

RAIL_Status_t sl_rail_mux_StartScheduledTx(RAIL_Handle_t railHandle,
                                           uint16_t channel,
                                           RAIL_TxOptions_t options,
                                           const RAIL_ScheduleTxConfig_t *config,
                                           const RAIL_SchedulerInfo_t *schedulerInfo)
{
  RAIL_MUX_DECLARE_IRQ_STATE;
  RAIL_Status_t ret_status = RAIL_STATUS_INVALID_STATE;

  uint8_t context_index = fn_get_context_index(railHandle);
  assert(context_index < SUPPORTED_PROTOCOL_COUNT);

  // ToDo: we might need to consider protocol priorities here

  RAIL_MUX_ENTER_CRITICAL();
  if ( check_lock_permissions(context_index) ) {
    // Even if there was a pending tx here, we are gonna re-write it, but we have
    // to make sure tx FIFO is also written.
    fn_set_context_flag_by_index(context_index, RAIL_MUX_PROTOCOL_FLAGS_SCHEDULED_TX_PENDING, true);
    protocol_context[context_index].csma_tx_info.tx_type = SCHEDULED_TX_REQUEST;
    protocol_context[context_index].csma_tx_info.channel = channel;
    protocol_context[context_index].csma_tx_info.options = options;

    fn_set_context_flag_by_index(context_index,
                                 RAIL_MUX_PROTOCOL_FLAGS_SCHEDULE_TX_CONFIG_VALID,
                                 (config != NULL));
    if (config != NULL) {
      protocol_context[context_index].csma_tx_info.scheduler_config = *config;
    }

    fn_set_context_flag_by_index(context_index,
                                 RAIL_MUX_PROTOCOL_FLAGS_SCHEDULER_INFO_VALID,
                                 (schedulerInfo != NULL));
    if (schedulerInfo != NULL) {
      protocol_context[context_index].csma_tx_info.schedulerInfo = *schedulerInfo;
    }
    ret_status = fn_start_pending_tx();
  }
  RAIL_MUX_EXIT_CRITICAL();

  return ret_status;
}

RAIL_Status_t sl_rail_mux_ConfigSleep(RAIL_Handle_t railHandle,
                                      RAIL_SleepConfig_t sleepConfig)
{
  (void)railHandle;

  return RAIL_ConfigSleep(mux_rail_handle, sleepConfig);
}

RAIL_Status_t sl_rail_mux_StartAverageRssi(RAIL_Handle_t railHandle,
                                           uint16_t channel,
                                           RAIL_Time_t averagingTimeUs,
                                           const RAIL_SchedulerInfo_t *schedulerInfo)
{
  RAIL_MUX_DECLARE_IRQ_STATE;

  uint8_t context_index = fn_get_context_index(railHandle);
  assert(context_index < SUPPORTED_PROTOCOL_COUNT);
  RAIL_Status_t ret_status = RAIL_STATUS_INVALID_STATE;

  RAIL_MUX_ENTER_CRITICAL();
  bool is_ok_to_proceed = check_lock_permissions(context_index);
  RAIL_MUX_EXIT_CRITICAL();

  if ( is_ok_to_proceed ) {
    ret_status = RAIL_StartAverageRssi(mux_rail_handle,
                                       channel,
                                       averagingTimeUs,
                                       schedulerInfo);
  }
  return ret_status;
}

uint16_t sl_rail_mux_WriteTxFifo(RAIL_Handle_t railHandle,
                                 const uint8_t *dataPtr,
                                 uint16_t writeLength,
                                 bool reset)
{
  RAIL_MUX_DECLARE_IRQ_STATE;
  uint8_t active_tx_context_index = fn_get_active_tx_context_index();
  uint16_t ret_len = 0;

  uint8_t context_index = fn_get_context_index(railHandle);
  assert(context_index < SUPPORTED_PROTOCOL_COUNT);

  assert(fn_get_context_flag_by_index(context_index, RAIL_MUX_PROTOCOL_FLAGS_SETUP_TX_FIFO));

  RAIL_MUX_ENTER_CRITICAL();
  // ToDo:
  // verify This call does not ever result to a call to RAIL_WriteTxFifo
  // veify we do not need to worry about details such as bytewarps etc?
  if (active_tx_context_index != context_index) {
    if (!reset) {
      memcpy(protocol_context[context_index].fifo_tx_info.data_ptr + protocol_context[context_index].fifo_tx_info.tx_init_length, dataPtr, writeLength);
      protocol_context[context_index].fifo_tx_info.tx_init_length +=  writeLength;
    } else {
      memcpy(protocol_context[context_index].fifo_tx_info.data_ptr, dataPtr, writeLength);
      protocol_context[context_index].fifo_tx_info.tx_init_length = writeLength;
    }
    ret_len = writeLength;
  } else {
    ret_len = RAIL_WriteTxFifo(mux_rail_handle,
                               dataPtr,
                               writeLength,
                               reset);

    if (!reset) {
      protocol_context[context_index].fifo_tx_info.tx_init_length += ret_len;
    } else {
      protocol_context[context_index].fifo_tx_info.tx_init_length = ret_len;
    }
  }
  RAIL_MUX_EXIT_CRITICAL();

  return ret_len;
}

RAIL_Status_t sl_rail_mux_IEEE802154_WriteEnhAck(RAIL_Handle_t railHandle,
                                                 const uint8_t *ackData,
                                                 uint8_t ackDataLen)
{
  (void)railHandle;

  return RAIL_IEEE802154_WriteEnhAck(mux_rail_handle, ackData, ackDataLen);
}

RAIL_Status_t sl_rail_mux_ReleaseRxPacket(RAIL_Handle_t railHandle,
                                          RAIL_RxPacketHandle_t packetHandle)
{
  (void)railHandle;

  return RAIL_ReleaseRxPacket(mux_rail_handle, packetHandle);
}

RAIL_RxPacketHandle_t sl_rail_mux_HoldRxPacket(RAIL_Handle_t railHandle)
{
  (void)railHandle;

  return RAIL_HoldRxPacket(mux_rail_handle);
}

int16_t sl_rail_mux_GetAverageRssi(RAIL_Handle_t railHandle)
{
  (void)railHandle;

  return RAIL_GetAverageRssi(mux_rail_handle);
}

RAIL_Status_t sl_rail_mux_GetRxTimeSyncWordEndAlt(RAIL_Handle_t railHandle,
                                                  RAIL_RxPacketDetails_t *pPacketDetails)
{
  (void)railHandle;

  return RAIL_GetRxTimeSyncWordEndAlt(mux_rail_handle, pPacketDetails);
}

RAIL_IEEE802154_PtiRadioConfig_t sl_rail_mux_IEEE802154_GetPtiRadioConfig(RAIL_Handle_t railHandle)
{
  (void)railHandle;

  return RAIL_IEEE802154_GetPtiRadioConfig(mux_rail_handle);
}

RAIL_Status_t sl_rail_mux_IEEE802154_EnableEarlyFramePending(RAIL_Handle_t railHandle,
                                                             bool enable)
{
  (void)railHandle;

  return RAIL_IEEE802154_EnableEarlyFramePending(mux_rail_handle, enable);
}

uint16_t sl_rail_mux_SetTxFifoThreshold(RAIL_Handle_t railHandle, uint16_t txThreshold)
{
  (void)railHandle;

  return RAIL_SetTxFifoThreshold(mux_rail_handle, txThreshold);
}

RAIL_Status_t sl_rail_mux_SetNextTxRepeat(RAIL_Handle_t railHandle,
                                          const RAIL_TxRepeatConfig_t *repeatConfig)
{
  RAIL_Status_t status = RAIL_STATUS_INVALID_CALL;

  if (RAIL_SupportsTxToTx(mux_rail_handle)) {
    uint8_t context_index = fn_get_context_index(railHandle);
    assert(context_index < SUPPORTED_PROTOCOL_COUNT);

    status = RAIL_STATUS_NO_ERROR;
    protocol_context[context_index].tx_repeat_config = *repeatConfig;
    fn_set_context_flag_by_index(context_index,
                                 RAIL_MUX_PROTOCOL_FLAGS_CONFIG_REPEATED_TX,
                                 true);
  }

  return status;
}

RAIL_Status_t sl_rail_mux_RAIL_ScheduleRx(RAIL_Handle_t railHandle,
                                          uint16_t channel,
                                          const RAIL_ScheduleRxConfig_t *cfg,
                                          const RAIL_SchedulerInfo_t *schedulerInfo)
{
  RAIL_MUX_DECLARE_IRQ_STATE;
  RAIL_Status_t ret_status = RAIL_STATUS_INVALID_CALL;

  uint8_t context_index = fn_get_context_index(railHandle);
  assert(context_index < SUPPORTED_PROTOCOL_COUNT);

  RAIL_MUX_ENTER_CRITICAL();

  // If there are any other scheduled RX
  if (!fn_operation_in_progress(RAIL_MUX_PROTOCOL_FLAGS_RX_SCHEDULED)) {
    fn_set_context_flag_by_index(context_index, RAIL_MUX_PROTOCOL_FLAGS_RX_SCHEDULED, true);

    protocol_context[context_index].channel = channel;

    if ( check_lock_permissions(context_index) ) {
      rx_channel = channel;
      fn_update_802154_address_filtering_table();
      ret_status = RAIL_ScheduleRx(mux_rail_handle, channel, cfg, schedulerInfo);
    } else {
      ret_status = RAIL_STATUS_INVALID_STATE;
    }
  }

  RAIL_MUX_EXIT_CRITICAL();
  return ret_status;
}

//------------------------------------------------------------------------------
// Static functions

static void fn_set_global_flag(uint16_t flag, bool val)
{
  RAIL_MUX_DECLARE_IRQ_STATE;

  RAIL_MUX_ENTER_CRITICAL();
  if (val) {
    internal_flags |= flag;
  } else {
    internal_flags &= ~flag;
  }
  RAIL_MUX_EXIT_CRITICAL();
}

static bool fn_get_global_flag(uint16_t flag)
{
  return ((internal_flags & flag) != 0);
}

static uint8_t fn_get_context_index(RAIL_Handle_t handle)
{
  uint8_t i;

  for (i = 0; i < SUPPORTED_PROTOCOL_COUNT; i++) {
    if (handle == &protocol_context[i]) {
      return i;
    }
  }

  return INVALID_CONTEXT_INDEX;
}

static bool fn_get_context_flag_by_index(uint8_t index, uint16_t flag)
{
  return ((protocol_context[index].flags & flag) != 0);
}

static void fn_set_context_flag_by_index(uint8_t index, uint16_t flag, bool val)
{
  RAIL_MUX_DECLARE_IRQ_STATE;

  RAIL_MUX_ENTER_CRITICAL();
  if (val) {
    protocol_context[index].flags |= flag;
  } else {
    protocol_context[index].flags &= ~flag;
  }
  RAIL_MUX_EXIT_CRITICAL();
}

static void fn_init_802154_address_config(RAIL_IEEE802154_AddrConfig_t *addr_config)
{
  uint8_t i;

  for (i = 0; i < RAIL_IEEE802154_MAX_ADDRESSES; i++) {
    addr_config->panId[i] = 0xFFFF;
    addr_config->shortAddr[i] = 0xFFFF;
    memset(addr_config->longAddr[i], 0x00, 8);
  }
}

// It updates the RAIL 802.15.4 addressing filtering table, according to the
// current RX channel.
static void fn_update_802154_address_filtering_table(void)
{
  RAIL_IEEE802154_AddrConfig_t temp_addr_config;
  uint8_t temp_addr_filter_mask[SUPPORTED_PROTOCOL_COUNT];
  bool addr_filter_mask_changed = false;
  uint8_t i, j;
  uint8_t entry_index = 0;
  const uint8_t zeroed_long_addr[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

  // Nothing to do, the node is not RXing.
  if (rx_channel == INVALID_CHANNEL) {
    return;
  }

  fn_init_802154_address_config(&temp_addr_config);
  // Set the broadcast address bit and the broadcast PAN bit for each protocol.
  memset(temp_addr_filter_mask, RAIL_FILTERING_MASK_BROADCAST_ENABLED, SUPPORTED_PROTOCOL_COUNT);

  for (i = 0; i < SUPPORTED_PROTOCOL_COUNT; i++) {
    for (j = 0; j < RAIL_IEEE802154_MAX_ADDRESSES; j++) {
      // If either PAN ID or short Address is a value other than 0xFFFF or
      // the long ID is a value other than 0x00s, we have some non-default
      // address filtering that needs to be pushed down to RAIL.
      if (protocol_context[i].channel == rx_channel
          && (protocol_context[i].addr_802154.panId[j] != 0xFFFF
              || protocol_context[i].addr_802154.shortAddr[j] != 0xFFFF
              || memcmp(protocol_context[i].addr_802154.longAddr[j],
                        zeroed_long_addr,
                        8) != 0)) {
        temp_addr_config.panId[entry_index] = protocol_context[i].addr_802154.panId[j];
        temp_addr_config.shortAddr[entry_index] = protocol_context[i].addr_802154.shortAddr[j];
        memcpy(temp_addr_config.longAddr[entry_index],
               protocol_context[i].addr_802154.longAddr[j],
               8);
        // Set the panID bit and address bit corresponding to the rail addressing entry.
        temp_addr_filter_mask[i] |= ((1 << (entry_index + 1)) | (1 << (entry_index + 5)));
        entry_index++;

        // We maxed out the RAIL address filtering entries.
        // TODO: for now we just go in arbitrary order, we might revisit this
        // later on and use some sort of protocol priority to decide which
        // address filtering entry should go first.
        if (entry_index == RAIL_IEEE802154_MAX_ADDRESSES) {
          goto done;
        }
      }
    }
  }

  done:
  // Determine if the filtering mask changed for any of the protocols
  for (i = 0; i < SUPPORTED_PROTOCOL_COUNT; i++) {
    if (temp_addr_filter_mask[i] != protocol_context[i].addr_filter_mask_802154) {
      addr_filter_mask_changed = true;
      break;
    }
  }
  // We only update the address table if it changed or the filter mask changed.
  if (addr_filter_mask_changed
      || memcmp(&rail_addresses_802154,
                &temp_addr_config,
                sizeof(RAIL_IEEE802154_AddrConfig_t)) != 0) {
    for (i = 0; i < SUPPORTED_PROTOCOL_COUNT; i++) {
      protocol_context[i].addr_filter_mask_802154 = temp_addr_filter_mask[i];
    }
    memcpy(&rail_addresses_802154, &temp_addr_config, sizeof(RAIL_IEEE802154_AddrConfig_t));
    RAIL_IEEE802154_SetAddresses(mux_rail_handle, &rail_addresses_802154);
  }
}

HIDDEN void fn_mux_rail_init_callback(RAIL_Handle_t railHandle)
{
  uint8_t i;

  assert(fn_get_global_flag(RAIL_MUX_FLAGS_RAIL_INIT_STARTED));
  assert(!fn_get_global_flag(RAIL_MUX_FLAGS_RAIL_INIT_COMPLETED));

  fn_set_global_flag(RAIL_MUX_FLAGS_RAIL_INIT_COMPLETED, true);

  for (i = 0; i < SUPPORTED_PROTOCOL_COUNT; i++) {
    if (fn_get_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_INIT_CB_PENDING)) {
      if (protocol_context[i].init_callback != NULL) {
        protocol_context[i].init_callback(&protocol_context[i]);
      }
    }
  }
}

HIDDEN void fn_timer_callback(struct RAIL_MultiTimer *tmr,
                              RAIL_Time_t expectedTimeOfEvent,
                              void *cbArg)
{
  (void)tmr;
  (void)expectedTimeOfEvent;

  RAIL_Handle_t railHandle = (RAIL_Handle_t)cbArg;
  uint8_t context_index = fn_get_context_index(railHandle);

  assert(context_index < SUPPORTED_PROTOCOL_COUNT);

  if (protocol_context[context_index].timer_callback != NULL) {
    protocol_context[context_index].timer_callback(railHandle);
  }
}

static bool fn_operation_in_progress(uint16_t operation_flags)
{
  uint8_t i;

  for (i = 0; i < SUPPORTED_PROTOCOL_COUNT; i++) {
    if (fn_get_context_flag_by_index(i, operation_flags)) {
      return true;
    }
  }

  return false;
}

static uint8_t fn_get_active_tx_context_index(void)
{
  uint8_t active_tx_context_index = INVALID_CONTEXT_INDEX;
  uint8_t i;

  for (i = 0; i < SUPPORTED_PROTOCOL_COUNT; i++) {
    // Protocol context not in use
    if (protocol_context[i].rail_config == NULL) {
      continue;
    }

    if (fn_get_context_flag_by_index(i, RAIL_MUX_PROTOCOL_ACTIVE_TX_FLAGS)) {
      // Enforce that only one protocol could be in "active TX" state
      assert(active_tx_context_index == INVALID_CONTEXT_INDEX);
      active_tx_context_index = i;
    }
  }

  return active_tx_context_index;
}

HIDDEN void fn_mux_rail_events_callback(RAIL_Handle_t railHandle, RAIL_Events_t events)
{
  (void)railHandle;
  RAIL_RxPacketInfo_t rx_info, data_req_Info;
  RAIL_RxPacketHandle_t rx_packet_handle;
  RAIL_RxPacketDetails_t packet_details;
  bool start_pending_tx = false;
  uint8_t active_tx_protocol_index = fn_get_active_tx_context_index();
  uint8_t i;

  if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
    rx_packet_handle = RAIL_GetRxPacketInfo(mux_rail_handle,
                                            RAIL_RX_PACKET_HANDLE_NEWEST,
                                            &rx_info);
    assert(RAIL_GetRxPacketDetailsAlt(mux_rail_handle,
                                      rx_packet_handle,
                                      &packet_details) == RAIL_STATUS_NO_ERROR);
  }

  if (events & RAIL_EVENT_IEEE802154_DATA_REQUEST_COMMAND) {
    RAIL_GetRxIncomingPacketInfo(railHandle, &data_req_Info);
  }

  // Bubble up only events that should be raised to each protocol.
  for (i = 0; i < SUPPORTED_PROTOCOL_COUNT; i++) {
    RAIL_Events_t enabled_events = (protocol_context[i].events & events);

    // This protocol context is not in use
    if (protocol_context[i].rail_config == NULL) {
      continue;
    }

    if (enabled_events & RAIL_EVENTS_TX_COMPLETION) {
      if (i != active_tx_protocol_index) {
        enabled_events &= ~RAIL_EVENTS_TX_COMPLETION;
      } else {
        uint16_t unused_tail = protocol_context[i].fifo_tx_info.tx_size - protocol_context[i].fifo_tx_info.tx_init_length;
        uint16_t unused = RAIL_GetTxFifoSpaceAvailable(mux_rail_handle);
        if (unused > unused_tail) {
          uint16_t unused_head = unused - unused_tail;
          memmove(protocol_context[i].fifo_tx_info.data_ptr,
                  protocol_context[i].fifo_tx_info.data_ptr + unused_head,
                  protocol_context[i].fifo_tx_info.tx_size - unused);
        }

        protocol_context[i].fifo_tx_info.tx_init_length =
          protocol_context[i].fifo_tx_info.tx_size - unused;

        if (!RAIL_GetTxPacketsRemaining(mux_rail_handle)) {
          fn_set_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_TX_IN_PROGRESS, false);
          fn_set_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_TX_SCHEDULED, false);
        }
        if (!(enabled_events & RAIL_EVENT_TX_PACKET_SENT)) {
          // Any event other than a successful packet sent would unset the wait
          // for ack flag.
          fn_set_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_WAIT_FOR_ACK, false);
        }

        if (!tx_in_progress()) {
          start_pending_tx = true;
        }
      }
    }

    // We got a SCHEDULED_TX_STARTED event: we transition the protocol that
    // originally invoked the StartScheduledTX() API to "tx in progress".
    if (events & RAIL_EVENT_SCHEDULED_TX_STARTED) {
      if (fn_get_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_TX_SCHEDULED)) {
        fn_set_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_TX_IN_PROGRESS, true);
        fn_set_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_TX_SCHEDULED, false);
      } else {
        enabled_events &= ~RAIL_EVENT_SCHEDULED_TX_STARTED;
      }
    }

    if (events & RAIL_EVENT_RX_SCHEDULED_RX_END) {
      if (fn_get_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_RX_SCHEDULED)) {
        fn_set_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_RX_SCHEDULED, false);
      } else {
        enabled_events &= ~RAIL_EVENT_RX_SCHEDULED_RX_END;
      }
    }

    if (enabled_events & RAIL_EVENT_RX_PACKET_RECEIVED) {
      // The protocol is currently on a different channel or idling or the
      // packet did not satisfy any of the protocol filtering: we mask out the
      // RAIL_EVENT_RX_PACKET_RECEIVED event.
      if (rx_channel != protocol_context[i].channel
          // MAC Acks do not contain any addressing information
          // Do not check for filterMask on Rx packets that are acks
          || (!packet_details.isAck && (rx_info.filterMask & protocol_context[i].addr_filter_mask_802154) == 0)
          || (packet_details.isAck && (i != active_tx_protocol_index)) ) {
        enabled_events &= ~RAIL_EVENT_RX_PACKET_RECEIVED;
      } else {
        if (packet_details.isAck) {
          fn_set_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_WAIT_FOR_ACK, false);
          start_pending_tx = true;
        }
      }
    }

    // Deal with ACK timeout after possible RX completion in case RAIL
    // notifies us of the ACK and the timeout simultaneously -- we want
    // the ACK to win over the timeout.
    if (enabled_events & (RAIL_EVENT_RX_ACK_TIMEOUT | RAIL_EVENT_CONFIG_UNSCHEDULED)) {
      if ( i != active_tx_protocol_index) {
        enabled_events &= ~(RAIL_EVENT_RX_ACK_TIMEOUT | RAIL_EVENT_CONFIG_UNSCHEDULED);
      } else if (fn_get_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_WAIT_FOR_ACK)) {
        fn_set_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_WAIT_FOR_ACK, false);
        start_pending_tx = true;
      }
    }

    if (enabled_events & RAIL_EVENT_IEEE802154_DATA_REQUEST_COMMAND) { // ToDo: do we also need to check the packet type?
      if (rx_channel != protocol_context[i].channel
          || (data_req_Info.filterMask & protocol_context[i].addr_filter_mask_802154) == 0) {
        enabled_events &= ~RAIL_EVENT_IEEE802154_DATA_REQUEST_COMMAND;
      } else {
      }
    }

    if ((enabled_events) != 0) {
      protocol_context[i].rail_config->eventsCallback(&protocol_context[i],
                                                      enabled_events);
    }
  }

  if (start_pending_tx) {
    fn_start_pending_tx();
  }
}

// TODO: We do not prioritize the next protocol within this function if we need
// any kind of priority taking place we need to add that in.
static RAIL_Status_t fn_start_pending_tx(void)
{
  uint8_t i;

  // We are currently TXing, nothing to do.
  if (tx_in_progress()) {
    return RAIL_STATUS_NO_ERROR;
  }
  // Set the TX power before starting a new transmission if there is a pending
  // setTXpower to be done.
  fn_update_current_tx_power();

  for (i = 0; i < SUPPORTED_PROTOCOL_COUNT; i++) {
    // Pending scheduled TX
    if (fn_get_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_SCHEDULED_TX_PENDING)) {
      RAIL_Status_t status = RAIL_STATUS_NO_ERROR;
      if ( !check_lock_permissions(i)) {
        // Post a tx blocked event to notify mac state machines
        fn_mux_rail_events_callback(mux_rail_handle, RAIL_EVENT_TX_BLOCKED);
        continue;
      }
      //TODO: we might need to check if there is already scheduled TX, and it is by somebody with higher priority?
      // a new scheduled TX can overwrite the existing one with no priority considerataions
      if (fn_get_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_SETUP_TX_FIFO)) {
        protocol_context[i].fifo_tx_info.tx_size = RAIL_SetTxFifo(mux_rail_handle, protocol_context[i].fifo_tx_info.data_ptr,
                                                                  protocol_context[i].fifo_tx_info.tx_init_length,
                                                                  protocol_context[i].fifo_tx_info.tx_size);
      }

      fn_set_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_TX_SCHEDULED, true);

      if (protocol_context[i].csma_tx_info.tx_type == SCHEDULED_TX_CCA_CSMA_REQUEST) {
        status = RAIL_StartScheduledCcaCsmaTx(mux_rail_handle,
                                              protocol_context[i].csma_tx_info.channel,
                                              protocol_context[i].csma_tx_info.options,
                                              (fn_get_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_SCHEDULE_TX_CONFIG_VALID)
                                               ? &protocol_context[i].csma_tx_info.scheduler_config : NULL),
                                              (fn_get_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_CSMA_CONFIG_VALID)
                                               ? &protocol_context[i].csma_tx_info.csmaConfig : NULL),
                                              (fn_get_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_SCHEDULER_INFO_VALID)
                                               ? &protocol_context[i].csma_tx_info.schedulerInfo : NULL));
      } else if (protocol_context[i].csma_tx_info.tx_type == SCHEDULED_TX_REQUEST) {
        status = RAIL_StartScheduledTx(mux_rail_handle,
                                       protocol_context[i].csma_tx_info.channel,
                                       protocol_context[i].csma_tx_info.options,
                                       (fn_get_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_SCHEDULE_TX_CONFIG_VALID)
                                        ? &protocol_context[i].csma_tx_info.scheduler_config : NULL),
                                       (fn_get_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_SCHEDULER_INFO_VALID)
                                        ? &protocol_context[i].csma_tx_info.schedulerInfo : NULL));
      } else {
        // TX type corrupted?
        assert(0);
      }

      fn_set_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_SCHEDULED_TX_PENDING, false);

      // This would result to assert in lower-mac code
      assert(status == RAIL_STATUS_NO_ERROR);

      return RAIL_STATUS_NO_ERROR;
    }

    // Pending normal TX
    if (fn_get_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_START_TX_PENDING)) {
      RAIL_Status_t status = RAIL_STATUS_NO_ERROR;

      if ( !check_lock_permissions(i)) {
        // Post a tx blocked event to notify mac state machines
        fn_mux_rail_events_callback(mux_rail_handle, RAIL_EVENT_TX_BLOCKED);
        continue;
      }
      if (fn_get_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_CONFIG_REPEATED_TX)) {
        RAIL_SetNextTxRepeat(mux_rail_handle, &protocol_context[i].tx_repeat_config);
        fn_set_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_CONFIG_REPEATED_TX, false);
      }

      if (fn_get_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_SETUP_TX_FIFO)) {
        // We have to keep calling the setTxFifo() before each call to
        // RAIL_writeTxFifo() since  protocol B might have reset/recreated its
        // own txFifo buffer in between each call to writeTxFifo by protocol A
        protocol_context[i].fifo_tx_info.tx_size =
          RAIL_SetTxFifo(mux_rail_handle, protocol_context[i].fifo_tx_info.data_ptr,
                         protocol_context[i].fifo_tx_info.tx_init_length,
                         protocol_context[i].fifo_tx_info.tx_size);
      }

      fn_set_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_TX_IN_PROGRESS, true);

      if (protocol_context[i].csma_tx_info.options & RAIL_TX_OPTION_WAIT_FOR_ACK) {
        fn_set_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_WAIT_FOR_ACK, true);
      }

      if (protocol_context[i].csma_tx_info.tx_type == SINGLE_TX_CCA_CSMA_REQUEST) {
        status = RAIL_StartCcaCsmaTx(mux_rail_handle,
                                     protocol_context[i].csma_tx_info.channel,
                                     protocol_context[i].csma_tx_info.options,
                                     (fn_get_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_CSMA_CONFIG_VALID)
                                      ? &protocol_context[i].csma_tx_info.csmaConfig : NULL),
                                     (fn_get_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_SCHEDULER_INFO_VALID)
                                      ? &protocol_context[i].csma_tx_info.schedulerInfo : NULL));
      } else if (protocol_context[i].csma_tx_info.tx_type == SINGLE_TX_REQUEST) {
        status = RAIL_StartTx(mux_rail_handle,
                              protocol_context[i].csma_tx_info.channel,
                              protocol_context[i].csma_tx_info.options,
                              (fn_get_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_SCHEDULER_INFO_VALID)
                               ? &protocol_context[i].csma_tx_info.schedulerInfo  : NULL));
      } else {
        // TX type corrupted?
        assert(0);
      }

      fn_set_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_START_TX_PENDING, false);

      // This would result to assert in lower-mac code
      assert(status == RAIL_STATUS_NO_ERROR);

      return RAIL_STATUS_NO_ERROR;
    }
  }

  return RAIL_STATUS_NO_ERROR;
}

static void fn_update_current_tx_power(void)
{
  RAIL_TxPower_t max_power = RAIL_TX_POWER_MIN;
  uint8_t i;

  for (i = 0; i < SUPPORTED_PROTOCOL_COUNT; i++) {
    if (protocol_context[i].tx_power != RAIL_TX_POWER_LEVEL_INVALID
        && protocol_context[i].tx_power > max_power) {
      max_power = protocol_context[i].tx_power;
    }
  }

  if (max_power != current_tx_power && !tx_in_progress()) {
    current_tx_power = max_power;
    RAIL_SetTxPowerDbm(mux_rail_handle, current_tx_power);
  }
}

#ifdef EMBER_TEST
sl_rail_util_ieee802154_stack_status_t sl_rail_mux_ieee802154_on_event(
  RAIL_Handle_t railHandle,
  sl_rail_util_ieee802154_stack_event_t stack_event,
  uint32_t supplement)
{
  return SL_RAIL_UTIL_IEEE802154_STACK_STATUS_SUCCESS;
}
#else //!EMBER_TEST
static bool check_event_filter(uint8_t context_index,
                               uint16_t flag,
                               bool enable)
{
  fn_set_context_flag_by_index(context_index, flag, enable);

  if (!enable) {
    for (uint8_t i = 0; i < SUPPORTED_PROTOCOL_COUNT; i++) {
      if (fn_get_context_flag_by_index(context_index, flag)) {
        // filter stop event since the radio operation
        // is in progress on another stack
        return false;
      }
    }
  }
  return true;
}

static bool filter_stack_event(uint8_t context_index,
                               sl_rail_util_ieee802154_stack_event_t stack_event)
{
  bool filter_event = false;
  switch (stack_event) {
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_STARTED:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACCEPTED:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACKING:
      filter_event = check_event_filter(context_index,
                                        RAIL_MUX_PROTOCOL_FLAGS_STACK_EVENT_RX_ACTIVE,
                                        true);
      break;
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_CORRUPTED:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACK_BLOCKED:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACK_ABORTED:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_FILTERED:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ENDED:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACK_SENT:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_SIGNAL_DETECTED:
      filter_event = check_event_filter(context_index,
                                        RAIL_MUX_PROTOCOL_FLAGS_STACK_EVENT_RX_ACTIVE,
                                        false);
      break;
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_STARTED:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_PENDED_PHY:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_CCA_SOON:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_PENDED_MAC:
      filter_event = check_event_filter(context_index,
                                        RAIL_MUX_PROTOCOL_FLAGS_STACK_EVENT_TX_ACTIVE,
                                        true);
      break;
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_ACK_WAITING:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_ACK_RECEIVED:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_ACK_TIMEDOUT:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_BLOCKED:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_ABORTED:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_ENDED:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_IDLED:
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_CCA_BUSY:
      filter_event = check_event_filter(context_index,
                                        RAIL_MUX_PROTOCOL_FLAGS_STACK_EVENT_TX_ACTIVE,
                                        false);
      break;
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_LISTEN:
      filter_event = check_event_filter(context_index,
                                        RAIL_MUX_PROTOCOL_FLAGS_STACK_EVENT_RADIO_ACTIVE,
                                        true);
      break;
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_IDLED:
      filter_event = check_event_filter(context_index,
                                        RAIL_MUX_PROTOCOL_FLAGS_STACK_EVENT_RADIO_ACTIVE,
                                        false);
      break;
    case SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TICK:
    default:
      break;
  }
  return filter_event;
}

sl_rail_util_ieee802154_stack_status_t sl_rail_mux_ieee802154_on_event(
  RAIL_Handle_t railHandle,
  sl_rail_util_ieee802154_stack_event_t stack_event,
  uint32_t supplement)
{
  RAIL_MUX_DECLARE_IRQ_STATE;

  RAIL_Status_t status = SL_RAIL_UTIL_IEEE802154_STACK_STATUS_SUCCESS;
  uint8_t context_index = fn_get_context_index(railHandle);

  RAIL_MUX_ENTER_CRITICAL();
  if (filter_stack_event(context_index, stack_event)) {
    status = sl_rail_util_ieee802154_on_event(stack_event, supplement);
  }
  RAIL_MUX_EXIT_CRITICAL();

  return status;
}
#endif

#ifdef PRINT_DEBUG_ON
uint16_t fn_get_tx_init(uint8_t index)
{
  return protocol_context[index].fifo_tx_info.tx_init_length;
}
void fn_print_flags(void)
{
  uint8_t i;

  fprintf(stderr, "\nGlobal Flags: %x %x %x \n", fn_get_global_flag(RAIL_MUX_FLAGS_RAIL_INIT_STARTED),
          fn_get_global_flag(RAIL_MUX_FLAGS_RAIL_INIT_COMPLETED),
          fn_get_global_flag(RAIL_MUX_FLAGS_IEEE802154_INIT_COMPLETED));

  for (i = 0; i < SUPPORTED_PROTOCOL_COUNT; i++) {
    fprintf(stderr, "\nContext %d Flags: %x %x %x %x %x\n", i, fn_get_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_INIT_CB_PENDING),
            fn_get_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_SETUP_TX_FIFO),
            fn_get_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_START_TX_PENDING),
            fn_get_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_TX_IN_PROGRESS),
            fn_get_context_flag_by_index(i, RAIL_MUX_PROTOCOL_FLAGS_WAIT_FOR_ACK) );
  }
}
#endif

void sl_rail_mux_update_active_radio_config(void)
{
  RAIL_Idle(mux_rail_handle, RAIL_IDLE_ABORT, true);
  sl_rail_util_ieee802154_config_radio(mux_rail_handle);
  if (rx_channel != INVALID_CHANNEL) {
    RAIL_StartRx(mux_rail_handle, rx_channel, NULL);
  }
}
