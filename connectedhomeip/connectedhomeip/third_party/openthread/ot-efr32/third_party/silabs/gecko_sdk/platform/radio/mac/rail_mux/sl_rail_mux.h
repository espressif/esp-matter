/***************************************************************************//**
 * @file sl_rail_mux.h
 * @brief RAIL Multiplexer APIs, types, macros and globals.
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

#ifndef RAIL_MUX_H
#define RAIL_MUX_H

#include PLATFORM_HEADER
#include "sl_status.h"

// TODO: possibly turn this into a CMSIS option
#define SUPPORTED_PROTOCOL_COUNT 2

// Global flags
#define RAIL_MUX_FLAGS_RAIL_INIT_STARTED                                  0x0001
#define RAIL_MUX_FLAGS_RAIL_INIT_COMPLETED                                0x0002
#define RAIL_MUX_FLAGS_IEEE802154_INIT_COMPLETED                          0x0004
#define RAIL_MUX_FLAGS_RAIL_SET_PTI_DONE                                  0x0008
#define RAIL_MUX_FLAGS_RAIL_CONFIG_TX_POWER_DONE                          0x0010

// Protocol-specific flags
#define RAIL_MUX_PROTOCOL_FLAGS_INIT_CB_PENDING                           0x0001
#define RAIL_MUX_PROTOCOL_FLAGS_SETUP_TX_FIFO                             0x0002
#define RAIL_MUX_PROTOCOL_FLAGS_START_TX_PENDING                          0x0004
#define RAIL_MUX_PROTOCOL_FLAGS_TX_IN_PROGRESS                            0x0008
#define RAIL_MUX_PROTOCOL_FLAGS_WAIT_FOR_ACK                              0x0010
#define RAIL_MUX_PROTOCOL_FLAGS_SCHEDULED_TX_PENDING                      0x0020
#define RAIL_MUX_PROTOCOL_FLAGS_TX_SCHEDULED                              0x0040
#define RAIL_MUX_PROTOCOL_FLAGS_CONFIG_REPEATED_TX                        0x0080
#define RAIL_MUX_PROTOCOL_FLAGS_CSMA_CONFIG_VALID                         0x0100
#define RAIL_MUX_PROTOCOL_FLAGS_SCHEDULER_INFO_VALID                      0x0200
#define RAIL_MUX_PROTOCOL_FLAGS_SCHEDULE_TX_CONFIG_VALID                  0x0400
#define RAIL_MUX_PROTOCOL_FLAGS_RX_SCHEDULED                              0x0800
#define RAIL_MUX_PROTOCOL_FLAGS_STACK_EVENT_RX_ACTIVE                     0x1000
#define RAIL_MUX_PROTOCOL_FLAGS_STACK_EVENT_TX_ACTIVE                     0x2000
#define RAIL_MUX_PROTOCOL_FLAGS_STACK_EVENT_RADIO_ACTIVE                  0x4000
#define RAIL_MUX_PROTOCOL_FLAGS_LOCK_ACTIVE                               0x8000

#define RAIL_MUX_PROTOCOL_ACTIVE_TX_FLAGS \
  (RAIL_MUX_PROTOCOL_FLAGS_TX_IN_PROGRESS | RAIL_MUX_PROTOCOL_FLAGS_WAIT_FOR_ACK)

// Other macros
#ifdef INVALID_CHANNEL
#undef INVALID_CHANNEL
#endif
#define INVALID_CHANNEL 0xFFFF

#define INVALID_CONTEXT_INDEX 0xFF

// normally 15.4 protocols have use max packet size of 128, but e.g.Zigbee CSL needs 512 bytes
#define RAIL_MUX_MAX_BUFFER_SIZE 512 // 4 * 128 in ZB CSL

// For 802.15.4 the filtering bits in the RAIL_AddrFilterMask_t are:
//  7     6     5     4       3    2    1    0
//  Addr3 Addr2 Addr1 AddrB | Pan3 Pan2 Pan1 PanB
#define RAIL_FILTERING_MASK_BROADCAST_ENABLED  0x11

#include "rail_types.h"
#include "rail_ieee802154.h"
#include "rail_util_ieee802154/sl_rail_util_ieee802154_stack_event.h"

typedef enum {
  SCHEDULED_TX_REQUEST,
  SINGLE_TX_REQUEST,
  SINGLE_TX_CCA_CSMA_REQUEST,
  SCHEDULED_TX_CCA_CSMA_REQUEST,
} sli_tx_type_t;

typedef struct {
  sli_tx_type_t tx_type;
  uint16_t channel;
  RAIL_TxOptions_t options;
  RAIL_CsmaConfig_t csmaConfig;
  RAIL_ScheduleTxConfig_t scheduler_config;
  RAIL_SchedulerInfo_t schedulerInfo;
} sli_csma_tx_info_t;

typedef struct {
  uint16_t tx_init_length;
  uint16_t tx_size;
  uint8_t *data_ptr;
} sli_fifo_tx_info_t;

typedef struct {
  RAIL_Config_t *rail_config;
  RAIL_InitCompleteCallbackPtr_t init_callback;
  RAIL_RadioConfigChangedCallback_t config_channels_callback;
  RAIL_Events_t events;
  volatile uint16_t flags;
  uint16_t channel;
  RAIL_TxPower_t tx_power;
  RAIL_MultiTimer_t timer;
  RAIL_TimerCallback_t timer_callback;

  // 802.15.4 specific fields
  RAIL_IEEE802154_AddrConfig_t addr_802154;
  bool is_pan_coordinator_802154;
  uint8_t addr_filter_mask_802154;

  // TODO: all protocol-specific information should be added here
  sli_fifo_tx_info_t fifo_tx_info;
  sli_csma_tx_info_t csma_tx_info;
  RAIL_TxRepeatConfig_t tx_repeat_config;
} sl_rail_mux_context_t;

// System-wide initialization callback
void sli_rail_mux_local_init(void);

void sl_rail_mux_YieldRadio(RAIL_Handle_t railHandle);

RAIL_Status_t sl_rail_mux_ConfigRxOptions(RAIL_Handle_t railHandle,
                                          RAIL_RxOptions_t mask,
                                          RAIL_RxOptions_t options);

int8_t sl_rail_mux_GetRssiOffset(RAIL_Handle_t railHandle);

void sl_rail_mux_CancelTimer(RAIL_Handle_t railHandle);

RAIL_Status_t sl_rail_mux_SetTimer(RAIL_Handle_t railHandle,
                                   RAIL_Time_t time,
                                   RAIL_TimeMode_t mode,
                                   RAIL_TimerCallback_t cb);

bool sl_rail_mux_IsRxAutoAckPaused(RAIL_Handle_t railHandle);

void sl_rail_mux_GetRxIncomingPacketInfo(RAIL_Handle_t railHandle,
                                         RAIL_RxPacketInfo_t *pPacketInfo);

RAIL_Status_t sl_rail_mux_SetFreqOffset(RAIL_Handle_t railHandle,
                                        RAIL_FrequencyOffset_t freqOffset);

RAIL_Status_t sl_rail_mux_SetTaskPriority(RAIL_Handle_t railHandle,
                                          uint8_t priority,
                                          RAIL_TaskType_t taskType);

RAIL_Status_t sl_rail_mux_IEEE802154_Config2p4GHzRadio(RAIL_Handle_t railHandle);

RAIL_Status_t sl_rail_mux_ConfigEvents(RAIL_Handle_t railHandle,
                                       RAIL_Events_t mask,
                                       RAIL_Events_t events);

RAIL_Handle_t sl_rail_mux_Init(RAIL_Config_t *railCfg,
                               RAIL_InitCompleteCallbackPtr_t cb);

RAIL_Status_t sl_rail_mux_SetPtiProtocol(RAIL_Handle_t railHandle,
                                         RAIL_PtiProtocol_t protocol);

void sl_rail_mux_PauseRxAutoAck(RAIL_Handle_t railHandle,
                                bool pause);

void sl_rail_mux_Idle(RAIL_Handle_t railHandle,
                      RAIL_IdleMode_t mode,
                      bool wait);

void sl_rail_mux_CopyRxPacket(uint8_t *pDest,
                              const RAIL_RxPacketInfo_t *pPacketInfo);

uint16_t sl_rail_mux_ConfigChannels(RAIL_Handle_t railHandle,
                                    const RAIL_ChannelConfig_t *config,
                                    RAIL_RadioConfigChangedCallback_t cb);

RAIL_Status_t sl_rail_mux_IEEE802154_Init(RAIL_Handle_t railHandle,
                                          const RAIL_IEEE802154_Config_t *config);

RAIL_Status_t sl_rail_mux_ConvertLqi(RAIL_Handle_t railHandle,
                                     RAIL_ConvertLqiCallback_t cb);

RAIL_Status_t sl_rail_mux_Calibrate(RAIL_Handle_t railHandle,
                                    RAIL_CalValues_t *calValues,
                                    RAIL_CalMask_t calForce);

RAIL_Status_t sl_rail_mux_IEEE802154_CalibrateIr2p4Ghz(RAIL_Handle_t railHandle,
                                                       uint32_t *imageRejection);

uint16_t sl_rail_mux_GetRadioEntropy(RAIL_Handle_t railHandle,
                                     uint8_t *buffer,
                                     uint16_t bytes);

RAIL_Status_t sl_rail_mux_IEEE802154_SetShortAddress(RAIL_Handle_t railHandle,
                                                     uint16_t shortAddr,
                                                     uint8_t index);

RAIL_Status_t sl_rail_mux_IEEE802154_SetLongAddress(RAIL_Handle_t railHandle,
                                                    const uint8_t *longAddr,
                                                    uint8_t index);

RAIL_Status_t sl_rail_mux_IEEE802154_SetPanId(RAIL_Handle_t railHandle,
                                              uint16_t panId,
                                              uint8_t index);

RAIL_Status_t sl_rail_mux_IEEE802154_SetPanCoordinator(RAIL_Handle_t railHandle,
                                                       bool isPanCoordinator);

RAIL_Status_t sl_rail_mux_IsValidChannel(RAIL_Handle_t railHandle, uint16_t channel);

int16_t sl_rail_mux_GetRssi(RAIL_Handle_t railHandle, bool wait);

uint32_t sl_rail_mux_GetSymbolRate(RAIL_Handle_t railHandle);

uint32_t sl_rail_mux_GetBitRate(RAIL_Handle_t railHandle);

RAIL_Time_t sl_rail_mux_GetTime(void);

RAIL_CalMask_t sl_rail_mux_GetPendingCal(RAIL_Handle_t railHandle);

RAIL_Status_t sl_rail_mux_CalibrateTemp(RAIL_Handle_t railHandle);

RAIL_RxPacketHandle_t sl_rail_mux_GetRxPacketInfo(RAIL_Handle_t railHandle,
                                                  RAIL_RxPacketHandle_t packetHandle,
                                                  RAIL_RxPacketInfo_t *pPacketInfo);

RAIL_Status_t sl_rail_mux_GetRxPacketDetailsAlt(RAIL_Handle_t railHandle,
                                                RAIL_RxPacketHandle_t packetHandle,
                                                RAIL_RxPacketDetails_t *pPacketDetails);

RAIL_Status_t sl_rail_mux_GetRxTimeSyncWordEnd(RAIL_Handle_t railHandle,
                                               uint16_t totalPacketBytes,
                                               RAIL_Time_t *pPacketTime);

uint16_t sl_rail_mux_PeekRxPacket(RAIL_Handle_t railHandle,
                                  RAIL_RxPacketHandle_t packetHandle,
                                  uint8_t *pDst,
                                  uint16_t len,
                                  uint16_t offset);

RAIL_Status_t sl_rail_mux_IEEE802154_GetAddress(RAIL_Handle_t railHandle,
                                                RAIL_IEEE802154_Address_t *pAddress);

RAIL_Status_t sl_rail_mux_IEEE802154_SetFramePending(RAIL_Handle_t railHandle);

RAIL_SchedulerStatus_t sl_rail_mux_GetSchedulerStatus(RAIL_Handle_t railHandle);

RAIL_Status_t sl_rail_mux_GetTxPowerConfig(RAIL_Handle_t railHandle,
                                           RAIL_TxPowerConfig_t *config);

RAIL_TxPower_t sl_rail_mux_GetTxPowerDbm(RAIL_Handle_t railHandle);

RAIL_Status_t sl_rail_mux_ConfigTxPower(RAIL_Handle_t railHandle,
                                        const RAIL_TxPowerConfig_t *config);

RAIL_Status_t sl_rail_mux_SetTxPowerDbm(RAIL_Handle_t railHandle,
                                        RAIL_TxPower_t power);

RAIL_RadioState_t sl_rail_mux_GetRadioState(RAIL_Handle_t railHandle);

RAIL_Status_t sl_rail_mux_StartRx(RAIL_Handle_t railHandle,
                                  uint16_t channel,
                                  const RAIL_SchedulerInfo_t *schedulerInfo);

uint16_t sl_rail_mux_SetTxFifo(RAIL_Handle_t railHandle,
                               uint8_t *addr,
                               uint16_t initLength,
                               uint16_t size);

RAIL_Status_t sl_rail_mux_StartCcaCsmaTx(RAIL_Handle_t railHandle,
                                         uint16_t channel,
                                         RAIL_TxOptions_t options,
                                         const RAIL_CsmaConfig_t *csmaConfig,
                                         const RAIL_SchedulerInfo_t *schedulerInfo);

RAIL_Status_t sl_rail_mux_StartTx(RAIL_Handle_t railHandle,
                                  uint16_t channel,
                                  RAIL_TxOptions_t options,
                                  const RAIL_SchedulerInfo_t *schedulerInfo);

RAIL_Status_t sl_rail_mux_StartTxStream(RAIL_Handle_t railHandle,
                                        uint16_t channel,
                                        RAIL_StreamMode_t mode);

RAIL_Status_t sl_rail_mux_StopTxStream(RAIL_Handle_t railHandle);

uint8_t sl_rail_mux_IEEE802154_ConvertRssiToEd(int8_t rssiDbm);

RAIL_Status_t sl_rail_mux_IEEE802154_SetPromiscuousMode(RAIL_Handle_t railHandle,
                                                        bool enable);

RAIL_Status_t sl_rail_mux_SetCcaThreshold(RAIL_Handle_t railHandle,
                                          int8_t ccaThresholdDbm);

RAIL_Status_t sl_rail_mux_SetRxTransitions(RAIL_Handle_t railHandle,
                                           const RAIL_StateTransitions_t *transitions);
RAIL_Status_t sl_rail_mux_ConfigCal(RAIL_Handle_t railHandle,
                                    RAIL_CalMask_t calEnable);
RAIL_Status_t sl_rail_mux_EnablePaAutoMode(RAIL_Handle_t railHandle, bool enable);

uint16_t sl_rail_mux_ReadRxFifo(RAIL_Handle_t railHandle,
                                uint8_t *dataPtr,
                                uint16_t readLength);

uint16_t sl_rail_mux_GetRxFifoBytesAvailable(RAIL_Handle_t railHandle);

RAIL_Status_t sl_rail_mux_StartScheduledTx(RAIL_Handle_t railHandle,
                                           uint16_t channel,
                                           RAIL_TxOptions_t options,
                                           const RAIL_ScheduleTxConfig_t *config,
                                           const RAIL_SchedulerInfo_t *schedulerInfo);

RAIL_Status_t sl_rail_mux_ConfigSleep(RAIL_Handle_t railHandle,
                                      RAIL_SleepConfig_t sleepConfig);

RAIL_Status_t sl_rail_mux_StartAverageRssi(RAIL_Handle_t railHandle,
                                           uint16_t channel,
                                           RAIL_Time_t averagingTimeUs,
                                           const RAIL_SchedulerInfo_t *schedulerInfo);

uint16_t sl_rail_mux_WriteTxFifo(RAIL_Handle_t railHandle,
                                 const uint8_t *dataPtr,
                                 uint16_t writeLength,
                                 bool reset);

RAIL_Status_t sl_rail_mux_IEEE802154_WriteEnhAck(RAIL_Handle_t railHandle,
                                                 const uint8_t *ackData,
                                                 uint8_t ackDataLen);

RAIL_Status_t sl_rail_mux_ReleaseRxPacket(RAIL_Handle_t railHandle,
                                          RAIL_RxPacketHandle_t packetHandle);

RAIL_RxPacketHandle_t sl_rail_mux_HoldRxPacket(RAIL_Handle_t railHandle);

int16_t sl_rail_mux_GetAverageRssi(RAIL_Handle_t railHandle);

RAIL_Status_t sl_rail_mux_GetRxTimeSyncWordEndAlt(RAIL_Handle_t railHandle,
                                                  RAIL_RxPacketDetails_t *pPacketDetails);

RAIL_IEEE802154_PtiRadioConfig_t sl_rail_mux_IEEE802154_GetPtiRadioConfig(RAIL_Handle_t railHandle);

RAIL_Status_t sl_rail_mux_IEEE802154_EnableEarlyFramePending(RAIL_Handle_t railHandle,
                                                             bool enable);

uint16_t sl_rail_mux_SetTxFifoThreshold(RAIL_Handle_t railHandle, uint16_t txThreshold);

RAIL_Status_t sl_rail_mux_RAIL_ScheduleRx(RAIL_Handle_t railHandle,
                                          uint16_t channel,
                                          const RAIL_ScheduleRxConfig_t *cfg,
                                          const RAIL_SchedulerInfo_t *schedulerInfo);

RAIL_Status_t sl_rail_mux_SetNextTxRepeat(RAIL_Handle_t railHandle,
                                          const RAIL_TxRepeatConfig_t *repeatConfig);

void sl_rail_mux_update_active_radio_config(void);

sl_rail_util_ieee802154_stack_status_t sl_rail_mux_ieee802154_on_event(RAIL_Handle_t railHandle,
                                                                       sl_rail_util_ieee802154_stack_event_t stack_event,
                                                                       uint32_t supplement);

/**
 * In a MUX configuration, where a single radio is used concurrently
 * by two or more protocol stacks, this function allows the caller to
 * lock the radio for exclusive use. This will prevent the other multiplexed
 * protocol stacks from performing certain RAIL operations including but
 * not limited to future transmits, scheduled tx, etc. Note that this does not
 * prevent transmits and receives that were scheduled prior to the lock being acquired
 *
 * Due to its disruptive nature, the caller
 * must attempt to return things to normal by unlocking the radio in a timely
 * fashion. Lock may be used to perform composite radio operations without
 * interruptions from other protocol stacks.
 *
 * NOTE: Part of this function runs with interrupts disabled
 *
 * @param[in] railHandle  Rail handle of protocol stack requesting the lock
 *
 * @retval SL_STATUS_FAIL  Radio lock unsuccessful.
 * @retval SL_STATUS_OK   Radio lock successful.
 *
 */
sl_status_t sli_rail_mux_lock_radio (RAIL_Handle_t railHandle);

/**
 * In a MUX configuration, where a single radio is used concurrently
 * by two or more protocol stacks, this function allows the caller to
 * resume multiplexed radio operation from a locked state. The caller of
 * the lock function must be the one to unlock the radio from a locked state.
 * Unlock operations that are performed when there is no existing lock on the
 * radio shall always be allowed.
 *
 * NOTE: Part of this function runs with interrupts disabled
 *
 * @param[in] railHandle  Rail handle of protocol stack requesting the lock
 *
 * @retval SL_STATUS_FAIL  Radio unlock unsuccessful.
 * @retval SL_STATUS_OK   Radio unlock successful.
 *
 */
sl_status_t sli_rail_mux_unlock_radio (RAIL_Handle_t railHandle);

RAIL_Status_t sl_rail_mux_GetChannel(RAIL_Handle_t railHandle, uint16_t *channel);

//------------------------------------------------------------------------------
// Internals

#if !defined(HIDDEN)
#ifdef EMBER_SCRIPTED_TEST
#define HIDDEN
#else
#define HIDDEN static
#endif
#endif

#ifdef EMBER_TEST
#define RAIL_MUX_DECLARE_IRQ_STATE
#define RAIL_MUX_ENTER_CRITICAL()
#define RAIL_MUX_EXIT_CRITICAL()
#else // !EMBER_TEST
#include "em_core.h"
#define RAIL_MUX_DECLARE_IRQ_STATE CORE_DECLARE_IRQ_STATE
#define RAIL_MUX_ENTER_CRITICAL() CORE_ENTER_CRITICAL()
#define RAIL_MUX_EXIT_CRITICAL() CORE_EXIT_CRITICAL()
#endif // EMBER_TEST

#endif //RAIL_MUX_H
