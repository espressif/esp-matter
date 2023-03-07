/***************************************************************************//**
 * @file
 * @brief This is the base test application. It handles basic RAIL configuration
 *   as well as transmit, receive, and various debug modes.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(SL_COMPONENT_CATALOG_PRESENT)
  #include "sl_component_catalog.h"
#endif

#include "rail.h"
#include "rail_ieee802154.h"
#include "rail_mfm.h"
#include "rail_zwave.h"
#include "sl_rail_util_init.h"
#include "sl_rail_util_protocol.h"
#if defined(SL_CATALOG_RAIL_UTIL_ANT_DIV_PRESENT)
  #include "sl_rail_util_ant_div.h"
#endif
#include "buffer_pool_allocator_config.h"

#include "em_chip.h"
#include "em_core.h"
#include "em_rmu.h"
#include "em_emu.h"

#include "gpiointerrupt.h"
#include "response_print.h"
#include "buffer_pool_allocator.h"
#include "circular_queue.h"

#include "app_common.h"
#include "app_trx.h"

#ifdef RAILAPP_RMR
#include "railapp_rmr.h"
#endif

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  #include "sl_power_manager.h"
#endif

#include "sl_rail_test_config.h"

#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
#include "coexistence-802154.h"
#endif

#ifdef SL_CATALOG_TIMING_TEST_PRESENT
#include "sl_rail_util_timing_test.h"
#endif

_Static_assert(sizeof(RailAppEvent_t) <= 40,
               "Adjust BUFFER_POOL_ALLOCATOR_BUFFER_SIZE_MAX per sizeof(RailAppEvent_t) growth");

// Includes for Silicon Labs-only, internal testing
#ifdef RPC_TESTING
#include "vs_rpc.h"
#else
#define RPC_Server_Init()
#define RPC_Server_Tick()
#endif

// Add a way to override the default setting for printingEnabled
#if defined(SL_RAIL_UTIL_PRINTING_DEFAULT) && !(SL_RAIL_UTIL_PRINTING_DEFAULT)
#define RAIL_PRINTING_DEFAULT_BOOL  false
#else
#define RAIL_PRINTING_DEFAULT_BOOL  true
#endif

// Add a way to override the default setting for skipCalibrations
#if defined(SL_RAIL_UTIL_SKIP_CALIBRATIONS_DEFAULT) && (SL_RAIL_UTIL_SKIP_CALIBRATIONS_DEFAULT)
#define RAIL_SKIP_CALIBRATIONS_BOOL true
#else
#define RAIL_SKIP_CALIBRATIONS_BOOL false
#endif

#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT
extern void sl_rail_util_ieee801254_on_rail_event(RAIL_Handle_t railHandle, RAIL_Events_t events);
#endif // SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT
#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
extern void sl_bt_ll_coex_handle_events(RAIL_Events_t events);
#endif //SL_CATALOG_RAIL_UTIL_COEX_PRESENT

// External control and status variables
Counters_t counters = { 0 };
bool receiveModeEnabled = false;
#if SL_RAIL_UTIL_INIT_INST0_ENABLE && SL_RAIL_UTIL_INIT_TRANSITIONS_INST0_ENABLE
RAIL_RadioState_t rxSuccessTransition = SL_RAIL_UTIL_INIT_TRANSITION_INST0_RX_SUCCESS;
#else
RAIL_RadioState_t rxSuccessTransition = RAIL_RF_STATE_IDLE;
#endif
uint8_t logLevel = PERIPHERAL_ENABLE | ASYNC_RESPONSE;
int32_t txCount = 0;
int32_t txRepeatCount = 0;
int32_t txRemainingCount = 0;
uint32_t continuousTransferPeriod = SL_RAIL_TEST_CONTINUOUS_TRANSFER_PERIOD;
bool enableRandomTxDelay = false;
uint32_t txAfterRxDelay = 0;
int32_t txCancelDelay = -1;
RAIL_StopMode_t txCancelMode = RAIL_STOP_MODES_NONE; // Use RAIL_Idle()
bool skipCalibrations = RAIL_SKIP_CALIBRATIONS_BOOL;
bool afterRxCancelAck = false;
bool afterRxUseTxBufferForAck = false;
bool schRxStopOnRxEvent = false;
uint32_t rssiDoneCount = 0; // HW rssi averaging
float averageRssi = -128;
bool printTxAck = false;
RAIL_VerifyConfig_t configVerify = { 0 };
int16_t lqiOffset = 0;
const char buildDateTime[] = __DATE__ " " __TIME__;
bool rxHeld = false;
volatile bool rxProcessHeld = false;
volatile uint32_t packetsHeld = 0U;

// Internal app state variables
static uint32_t startTransmitCounter = 0;
uint32_t internalTransmitCounter = 0;
volatile uint32_t ccaSuccesses = 0;
volatile uint32_t sentAckPackets = 0;

// Globals accessed in an interrupt context declared volatile.
static volatile bool receivingPacket = false;
volatile RAIL_Events_t lastTxStatus = 0;
volatile RAIL_Events_t lastTxAckStatus = 0;
volatile uint32_t failPackets = 0;
volatile uint32_t failAckPackets = 0;
static volatile bool packetTx = false;
static volatile bool finishTxSequence = false;
static volatile bool finishTxAckSequence = false;

RAIL_ScheduleTxConfig_t nextPacketTxTime = {
  0,
  RAIL_TIME_ABSOLUTE,
  RAIL_SCHEDULED_TX_DURING_RX_POSTPONE_TX
};
Queue_t railAppEventQueue;
volatile uint32_t eventsMissed = 0U;
static RAIL_Time_t txStartTime;

// Variable which holds the receive frequency offset for the period of time
// between when the frequency offset is measured (in either the
// RAIL_EVENT_RX_SYNC1_DETECT event or the RAIL_EVENT_RX_SYNC2_DETECT event)
// until reception of the packet is completed or aborted.
RAIL_FrequencyOffset_t rxFreqOffset = RAIL_FREQUENCY_OFFSET_INVALID;

// Variable which contains data of the most recently
// received beam packet, and a bool to specify whether
// or not one was received.
ZWaveBeamData_t beamData = {
  .channelIndex = RAIL_CHANNEL_HOPPING_INVALID_INDEX
};
volatile bool beamReceived = false;
static volatile uint32_t railTimerExpireTime = 0;
static volatile uint32_t railTimerConfigExpireTime = 0;
static volatile bool     railTimerExpired = false;
static volatile bool     calibrateRadio = false;
bool volatile newTxError = false;
static volatile bool     rxAckTimeout = false;
static volatile uint32_t ackTimeoutDuration = 0;
RAIL_Events_t enablePrintEvents = RAIL_EVENTS_NONE;
bool printRxErrorPackets = false;
bool printRxFreqOffsetData = false;
bool printingEnabled = RAIL_PRINTING_DEFAULT_BOOL;

// Names of RAIL_EVENT defines. This should align with rail_types.h
const char * const eventNames[] = RAIL_EVENT_STRINGS;
const uint8_t numRailEvents = COUNTOF(eventNames);

// Channel Hopping configuration structures
#if RAIL_FEAT_CHANNEL_HOPPING
uint32_t channelHoppingBufferSpace[CHANNEL_HOPPING_BUFFER_SIZE];
uint32_t *channelHoppingBuffer = channelHoppingBufferSpace;
#endif

#ifdef SL_CATALOG_TIMING_TEST_PRESENT
extern volatile bool enableRxPacketEventTimeCapture;
#endif

// Channel Variable
uint16_t channel = 0;

// Generic
RAIL_Handle_t railHandle;

// Make default packet work with the length-decoding schemes of 802.15.4, BLE,
// and Z-Wave
uint8_t txData[SL_RAIL_TEST_MAX_PACKET_LENGTH] = {
  0x0F, 0x0E, 0x11, 0x22, 0x33, 0x44, 0x55, 0x0F,
  0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE,
};
uint16_t txDataLen = 16;

static union {
  // Used to align this buffer as needed
  RAIL_FIFO_ALIGNMENT_TYPE align[SL_RAIL_TEST_TX_BUFFER_SIZE / RAIL_FIFO_ALIGNMENT];
  uint8_t fifo[SL_RAIL_TEST_TX_BUFFER_SIZE];
} txFifo;

uint8_t ackData[RAIL_AUTOACK_MAX_LENGTH] = {
  0x0F, 0x0E, 0xF1, 0xE2, 0xD3, 0xC4, 0xB5, 0x0F,
  0x97, 0x88, 0x79, 0x6A, 0x5B, 0x4C, 0x3D, 0x2E,
};
uint8_t ackDataLen = 16;

// Static RAIL callbacks
static void RAILCb_RssiAverageDone(RAIL_Handle_t railHandle);
uint8_t RAILCb_ConvertLqi(uint8_t lqi, int8_t rssi);
static bool RAILCb_QueueOverflow(const Queue_t *queue, void *data);

// Structures that hold default TX & RX Options
RAIL_TxOptions_t txOptions = RAIL_TX_OPTIONS_DEFAULT;
RAIL_RxOptions_t rxOptions = RAIL_RX_OPTIONS_DEFAULT;

// Data Management
RAIL_DataConfig_t railDataConfig = {
  .txSource = SL_RAIL_UTIL_INIT_DATA_FORMAT_INST0_TX_SOURCE,
  .rxSource = SL_RAIL_UTIL_INIT_DATA_FORMAT_INST0_RX_SOURCE,
  .txMethod = SL_RAIL_UTIL_INIT_DATA_FORMAT_INST0_TX_MODE,
  .rxMethod = SL_RAIL_UTIL_INIT_DATA_FORMAT_INST0_RX_MODE,
};

// Called during main loop
void sendPacketIfPending(void);
void finishTxSequenceIfPending(void);
void changeAppModeIfPending(void);
void printNewTxError(void);
void checkTimerExpiration(void);
void processPendingCalibrations(void);
void printAckTimeout(void);

static void heldRxProcess(void)
{
  if (!rxProcessHeld) {
    return;
  }

  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();
  uint32_t packetsHeldLocal = packetsHeld;
  if (packetsHeldLocal == 0) {
    rxProcessHeld = false;
  }
  RAIL_RxPacketHandle_t packetHandle
    = processRxPacket(railHandle, RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE);
  CORE_EXIT_CRITICAL();

  if (packetHandle == RAIL_RX_PACKET_HANDLE_INVALID) {
    if (packetsHeldLocal > 0) {
      responsePrintError("heldRxProcess", 0xEF, "Unexpected invalid packetHandle");
    }
  } else {
    RAIL_ReleaseRxPacket(railHandle, packetHandle);
    CORE_ENTER_CRITICAL();
    packetsHeld--;
    CORE_EXIT_CRITICAL();
    if (packetsHeldLocal == 0) {
      responsePrintError("heldRxProcess", 0xEF, "Unexpected valid packetHandle");
    }
  }
}

// Copy src bytes into txData array starting at offset, and update txDataLength.
static void changeTxPayload(uint32_t offset,
                            uint8_t *src,
                            uint16_t srcLen,
                            uint16_t pktLen)
{
  if (offset + srcLen <= sizeof(txData)) {   // be sure it fits
    memcpy(&txData[offset], &src[0], srcLen);
    txDataLen = pktLen;
  } else {
    responsePrintError("changeTxPayload",
                       0xF0,
                       "New packet data overflows the txData buffer size.");
    while (1) ;
  }
}

// Function called from sl_system_init before the main super loop.
void sl_rail_test_internal_app_init(void)
{
  // Get RAIL handle.
  railHandle = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);

  // Get proprietary channel config index, if support enabled.
  if (SL_RAIL_UTIL_INIT_RADIO_CONFIG_SUPPORT_INST0_ENABLE) {
    configIndex = SL_RAIL_UTIL_INIT_PROTOCOL_PROPRIETARY_INST0_INDEX;
  }

  // Initialize the RPC Server
  RPC_Server_Init();

  // Grab the reset cause
  uint32_t resetCause = RMU_ResetCauseGet();
  RMU_ResetCauseClear(); // So resetCause is rational and not an accumulated mess
  // Release GPIOs that were held by EM4h to ensure proper startup
  EMU_UnlatchPinRetention();

  // Initialize hardware for application
  appHalInit();

  // Initialize txOptions & rxOptions
#if defined(SL_CATALOG_RAIL_UTIL_ANT_DIV_PRESENT)
  sl_rail_util_ant_div_init_tx_options(&txOptions);
  sl_rail_util_ant_div_init_rx_options(&rxOptions);
#endif

  // Make sure the response printer mirrors the default printingEnabled state
  responsePrintEnable(printingEnabled);
  // Print app initialization information.
  RAILTEST_PRINTF("\n");
  responsePrint("reset", "App:%s,Built:%s,Cause:0x%x",
                SL_RAIL_TEST_APP_NAME, buildDateTime, resetCause);
  printChipInfo();
  getPti(NULL);

  // Set TX FIFO, and verify that the size is correct
  if (configureTxFifo() != RAIL_STATUS_NO_ERROR) {
    while (1) ;
  }

  (void) RAIL_GetChannel(railHandle, &channel);

  // Register an LQI conversion callback.
  RAIL_ConvertLqi(railHandle, &RAILCb_ConvertLqi);

  RAIL_ConfigRxOptions(railHandle, RAIL_RX_OPTIONS_ALL, rxOptions);

  // Initialize the queue we use for tracking packets
  if (!queueInit(&railAppEventQueue, BUFFER_POOL_ALLOCATOR_POOL_SIZE)) {
    while (1) ;
  }
  queueOverflow(&railAppEventQueue, &RAILCb_QueueOverflow);

#ifdef _SILICON_LABS_32B_SERIES_1
  if (resetCause & RMU_RSTCAUSE_EM4RST) {
    responsePrint("sleepWoke", "EM:4%c,SerialWakeup:No,RfSensed:%s",
                  (((EMU->EM4CTRL & EMU_EM4CTRL_EM4STATE)
                    == EMU_EM4CTRL_EM4STATE_EM4S) ? 's' : 'h'),
                  RAIL_IsRfSensed(railHandle) ? "Yes" : "No");
    // Always turn off RfSense when waking back up from EM4
    (void) RAIL_StartRfSense(railHandle, RAIL_RFSENSE_OFF, 0, NULL);
  }
#elif ((_SILICON_LABS_32B_SERIES_2_CONFIG == 2) || (_SILICON_LABS_32B_SERIES_2_CONFIG == 7))
  if (resetCause & EMU_RSTCAUSE_EM4) {
    responsePrint("sleepWoke", "EM:4s,SerialWakeup:No,RfSensed:%s",
                  RAIL_IsRfSensed(railHandle) ? "Yes" : "No");
    // Always turn off RfSense when waking back up from EM4
    (void) RAIL_StartRfSense(railHandle, RAIL_RFSENSE_OFF, 0, NULL);
  }
#else
  (void) resetCause;
#endif

  // Fill out the default TX packet with a useful pattern so it's not
  // all zeros if user extends the TX length.
  for (unsigned int i = txDataLen; i < sizeof(txData); i++) {
    txData[i] = i;
  }

  // Change the default TX packet payload to be protocol-specific.
  if (SL_RAIL_UTIL_PROTOCOL_IS_IEEE802154_2G4(SL_RAIL_UTIL_INIT_PROTOCOL_INST0_DEFAULT)) {
    // Default 802.15.4 2.4 GHz packet.
    uint8_t changes[] = { 0x0c, 0x63, 0x88, 0xbe, 0xff, 0xff, 0xff, 0xff,
                          0x11, 0x22, 0x04 };
    // Change 11 bytes at offset 0 of txData, and new packet length is 11.
    changeTxPayload(0, &changes[0], sizeof(changes), 11);
  } else if (SL_RAIL_UTIL_PROTOCOL_IS_IEEE802154_GB868(SL_RAIL_UTIL_INIT_PROTOCOL_INST0_DEFAULT)) {
    // Default 802.15.4 2.4 GB868 packet.
    uint8_t changes[] = { 0x18, 0x28 };
    // Change 2 bytes at offset 0 of txData, and new packet length is 20.
    changeTxPayload(0, &changes[0], sizeof(changes), 20);
  } else if (SL_RAIL_UTIL_PROTOCOL_IS_ZWAVE(SL_RAIL_UTIL_INIT_PROTOCOL_INST0_DEFAULT)) {
    // Default Z-Wave packet.
    uint8_t changes[] = { 0x20 };
    // Change 1 byte at offset 7 of txData, and new packet length is 200.
    changeTxPayload(7, &changes[0], sizeof(changes), 200);
  } else {
    // Use the default packet that already has length-decoding schemes for
    // 802.15.4, BLE, and Z-Wave.
  }

  // Initialize autoack data
  RAIL_WriteAutoAckFifo(railHandle, ackData, ackDataLen);

  // RX isn't validated yet so lets not go into receive just yet
  RAIL_StartRx(railHandle, channel, NULL); // Start in receive mode
  receiveModeEnabled = true;
}

volatile uint16_t rxDataSourceEventState = RX_DATA_SOURCE_EVENT_STATE_CHECKED;

static void checkRxDataSource(void)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  if (rxDataSourceEventState >= RX_DATA_SOURCE_EVENT_STATE_SUSPENDED) {
    RAIL_ConfigEvents(railHandle, RAIL_EVENT_RX_FIFO_ALMOST_FULL,
                      RAIL_EVENT_RX_FIFO_ALMOST_FULL);
    // Separate calls each of which uses fast-path code in library
    RAIL_ConfigEvents(railHandle, (RAIL_EVENT_RX_FIFO_OVERFLOW
                                   | RAIL_EVENT_RX_FIFO_FULL),
                      (RAIL_EVENT_RX_FIFO_OVERFLOW | RAIL_EVENT_RX_FIFO_FULL));
  }
  rxDataSourceEventState = RX_DATA_SOURCE_EVENT_STATE_CHECKED;
  CORE_EXIT_CRITICAL();
}

// Function called from sl_system_process_action within the main super loop.
void sl_rail_test_internal_app_process_action(void)
{
  RPC_Server_Tick();

  // Change app mode first so that any new actions can take effect this loop
  changeAppModeIfPending();

  rfSensedCheck();

  sendPacketIfPending();

  printNewTxError();

  finishTxSequenceIfPending();

  printRailAppEvents();

  checkTimerExpiration();

  processPendingCalibrations();

  printAckTimeout();

  heldRxProcess();

  checkRxDataSource();
}

/******************************************************************************
 * RAIL Callback Implementation
 *****************************************************************************/
// Override weak function called by callback sli_rail_util_on_rf_ready.
void sl_rail_util_on_rf_ready(RAIL_Handle_t railHandle)
{
  (void)railHandle;
  LedSet(0);
  LedSet(1);
}

static bool RAILCb_QueueOverflow(const Queue_t *queue, void *data)
{
  (void)queue;
  // The event queue is overflowing, and I want to overwrite the oldest event
  // pointer (in favor of the newer event information), so I need to free
  // the memory associated with that old, event pointer here.
  memoryFree(data);
  return true; // allow the overwrite
}

void RAILCb_SwTimerExpired(RAIL_Handle_t railHandle)
{
  railTimerExpireTime = RAIL_GetTime();
  railTimerConfigExpireTime = RAIL_GetTimer(railHandle);
  railTimerExpired = true;
}

#if RAIL_IEEE802154_SUPPORTS_G_MODESWITCH && defined(WISUN_MODESWITCHPHRS_ARRAY_SIZE)
void RAILCb_ModeSwitchMultiTimerExpired(RAIL_MultiTimer_t *tmr,
                                        RAIL_Time_t expectedTimeOfEvent,
                                        void *cbArg)
{
  (void)tmr;
  (void)expectedTimeOfEvent;
  (void)cbArg;

  if (modeSwitchState == TX_ON_NEW_PHY) {
    restartModeSwitchSequence();
  }
  if (modeSwitchState == RX_ON_NEW_PHY) {
    if (RAIL_IsValidChannel(railHandle, modeSwitchBaseChannel)
        == RAIL_STATUS_NO_ERROR) {
      changeChannel(modeSwitchBaseChannel);
      modeSwitchBaseChannel = 0xFFFFU;
      modeSwitchNewChannel = 0xFFFFU;
    }
  }
}

void restartModeSwitchSequence(void)
{
  // Re-start sequence: switch back on base channel
  if (RAIL_IsValidChannel(railHandle, modeSwitchBaseChannel)
      == RAIL_STATUS_NO_ERROR) {
    changeChannel(modeSwitchBaseChannel);
    // Write MS PHR in txData
    txData[0] = MSphr[0];
    txData[1] = MSphr[1];
    modeSwitchState = TX_MS_PACKET;
    // Send MS packet
    txCount = 1;
    pendPacketTx();
    sendPacketIfPending(); // txCount is decremented in this function
  }
}

void endModeSwitchSequence(void)
{
  if (modeSwitchLifeReturn) {
    changeChannel(modeSwitchBaseChannel);
  }
  modeSwitchDelayUs = 0;
  modeSwitchBaseChannel = 0xFFFFU;
  modeSwitchNewChannel = 0xFFFFU;
  setNextAppMode(NONE, NULL);
}
#endif

void RAILCb_TimerExpired(RAIL_Handle_t railHandle)
{
  if (inAppMode(NONE, NULL)) {
    if (abortRxDelay != 0) {
      RAIL_Idle(railHandle, RAIL_IDLE_ABORT, true);
    } else {
      railTimerExpireTime = RAIL_GetTime();
      railTimerConfigExpireTime = RAIL_GetTimer(railHandle);
      railTimerExpired = true;
    }
  } else if (currentAppMode() == PER) {
#if defined (SL_RAIL_TEST_PER_PORT) && defined(SL_RAIL_TEST_PER_PIN)
    GPIO_PinOutToggle(SL_RAIL_TEST_PER_PORT, SL_RAIL_TEST_PER_PIN);
    counters.perTriggers += GPIO_PinOutGet(SL_RAIL_TEST_PER_PORT, SL_RAIL_TEST_PER_PIN);
    perCount -= GPIO_PinOutGet(SL_RAIL_TEST_PER_PORT, SL_RAIL_TEST_PER_PIN);
#endif // SL_RAIL_TEST_PER_PORT && SL_RAIL_TEST_PER_PIN
    if (perCount < 1) {
#if defined (SL_RAIL_TEST_PER_PORT) && defined(SL_RAIL_TEST_PER_PIN)
      GPIO_PinOutClear(SL_RAIL_TEST_PER_PORT, SL_RAIL_TEST_PER_PIN);
#endif // SL_RAIL_TEST_PER_PORT && SL_RAIL_TEST_PER_PIN
      enableAppMode(PER, false, NULL);
    } else {
      RAIL_SetTimer(railHandle, perDelay, RAIL_TIME_DELAY, &RAILCb_TimerExpired);
    }
  } else {
    pendPacketTx();
  }
}

uint8_t RAILCb_ConvertLqi(uint8_t lqi, int8_t rssi)
{
  (void)rssi;
  // Put any custom LQI conversion code here.
  // In this application, lqiOffset is between -255 and 255 but LQI is uint8_t:
  int16_t newLqi = lqiOffset + lqi;
  if (newLqi < 0) {
    newLqi = 0;     // uint8_t min
  } else if (newLqi > 0xFF) {
    newLqi = 0xFF;  // uint8_t max
  }
  return (uint8_t)newLqi;
}

static void RAILCb_RssiAverageDone(RAIL_Handle_t railHandle)
{
  void *rssiHandle = memoryAllocate(sizeof(RailAppEvent_t));
  RailAppEvent_t *rssi = (RailAppEvent_t *)memoryPtrFromHandle(rssiHandle);
  if (rssi == NULL) {
    eventsMissed++;
    return;
  }
  rssi->type = AVERAGE_RSSI;
  rssi->rssi.rssi = RAIL_GetAverageRssi(railHandle);
  queueAdd(&railAppEventQueue, rssiHandle);
  rssiDoneCount++;
}

// Override weak function called by callback RAILCb_AssertFailed.
void sl_rail_util_on_assert_failed(RAIL_Handle_t railHandle, uint32_t errorCode)
{
  (void)railHandle;
  static const char* railErrorMessages[] = RAIL_ASSERT_ERROR_MESSAGES;
  const char *errorMessage = "Unknown";

  // If this error code is within the range of known error messages then use
  // the appropriate error message.
  if (errorCode < (sizeof(railErrorMessages) / sizeof(char*))) {
    errorMessage = railErrorMessages[errorCode];
  }
  // Print a message about the assert that triggered
  extern volatile int RAIL_AssertLineNumber;
  responsePrint("assert",
                "code:%d,line:%d,message:%s",
                errorCode,
                RAIL_AssertLineNumber,
                errorMessage);
  serialWaitForTxIdle();
  // Reset the chip since an assert is a fatal error
  NVIC_SystemReset();
}

// Override weak function called by callback sli_rail_util_on_event.
void sl_rail_util_on_event(RAIL_Handle_t railHandle, RAIL_Events_t events)
{
#ifdef SL_CATALOG_TIMING_TEST_PRESENT
  if (enableRxPacketEventTimeCapture
      && (events & RAIL_EVENT_RX_PACKET_RECEIVED)) {
    sl_rac_info_start.radioStateTimerTick = *RAIL_TimerTick;
    enableRxPacketEventTimeCapture = false;
  }
#endif //SL_CATALOG_TIMING_TEST_PRESENT
  enqueueEvents(events);
  if (events & RAIL_EVENT_CAL_NEEDED) {
    calibrateRadio = true;
  }
  if (events & RAIL_EVENT_RSSI_AVERAGE_DONE) {
    RAILCb_RssiAverageDone(railHandle);
  }

  // RX Events
  if (events & RAIL_EVENT_RX_TIMING_DETECT) {
    counters.timingDetect++;
  }
  if (events & RAIL_EVENT_RX_TIMING_LOST) {
    counters.timingLost++;
  }
  if (events & RAIL_EVENT_RX_PREAMBLE_LOST) {
    counters.preambleLost++;
  }
  if (events & RAIL_EVENT_RX_PREAMBLE_DETECT) {
    counters.preambleDetect++;
  }
  if (events & (RAIL_EVENT_RX_SYNC1_DETECT | RAIL_EVENT_RX_SYNC2_DETECT)) {
    receivingPacket = true;
    counters.syncDetect++;
    rxFifoPrep();
    if (printRxFreqOffsetData) {
      rxFreqOffset = RAIL_GetRxFreqOffset(railHandle);
    }
    if (abortRxDelay != 0) {
      RAIL_SetTimer(railHandle, abortRxDelay, RAIL_TIME_DELAY, &RAILCb_TimerExpired);
    }
  }
  if (events & RAIL_EVENT_IEEE802154_DATA_REQUEST_COMMAND) {
    if (RAIL_IEEE802154_IsEnabled(railHandle)) {
      counters.dataRequests++;
      RAILCb_IEEE802154_DataRequestCommand(railHandle);
    }
#if RAIL_FEAT_ZWAVE_SUPPORTED
    else if (RAIL_ZWAVE_IsEnabled(railHandle)) {
      RAILCb_ZWAVE_LrAckData(railHandle);
    }
#endif //RAIL_FEAT_ZWAVE_SUPPORTED
    else {
      // Other protocols ignore this event
    }
  }
#if RAIL_FEAT_ZWAVE_SUPPORTED
  if (events & RAIL_EVENT_ZWAVE_BEAM) {
    if (RAIL_ZWAVE_IsEnabled(railHandle)) {
      counters.rxBeams++;
      RAILCb_ZWAVE_BeamFrame(railHandle);
    }
  }
#endif //RAIL_FEAT_ZWAVE_SUPPORTED
  if (events & RAIL_EVENT_RX_FIFO_ALMOST_FULL) {
    counters.rxFifoAlmostFull++;
    RAILCb_RxFifoAlmostFull(railHandle);
  }
  if (events & RAIL_EVENT_RX_FIFO_FULL) {
    if (rxHeld) {
      rxProcessHeld = true; // Try to avoid overflow by processing held packets
    }
    counters.rxFifoFull++;
  }
  if (events & (RAIL_EVENT_RX_FIFO_OVERFLOW
                | RAIL_EVENT_RX_ADDRESS_FILTERED
                | RAIL_EVENT_RX_PACKET_ABORTED
                | RAIL_EVENT_RX_FRAME_ERROR
                | RAIL_EVENT_RX_PACKET_RECEIVED)) {
    // All of the above events cause a packet to not be received
    receivingPacket = false;
    if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
      RAILCb_RxPacketReceived(railHandle);
#if RAIL_IEEE802154_SUPPORTS_G_MODESWITCH && defined(WISUN_MODESWITCHPHRS_ARRAY_SIZE)
      if (modeSwitchState == RX_ON_NEW_PHY && modeSwitchLifeReturn) {
        RAIL_SetMultiTimer(&modeSwitchMultiTimer,
                           RX_MODE_SWITCH_DELAY_US,
                           RAIL_TIME_DELAY,
                           &RAILCb_ModeSwitchMultiTimerExpired,
                           NULL);
      }
 #endif
    }
    if (rxFifoManual && (railDataConfig.rxMethod != PACKET_MODE)) {
      (void)RAIL_HoldRxPacket(railHandle);
    }
    if (events & RAIL_EVENT_RX_FIFO_OVERFLOW) {
      counters.rxOfEvent++;
      if (railDataConfig.rxSource == RX_PACKET_DATA) {
        RAILCb_RxPacketAborted(railHandle);
      } else {
        // Treat similar to RX_FIFO_ALMOST_FULL: consume RX data
        RAILCb_RxFifoAlmostFull(railHandle);
        // Since we disable RX after a overflow, go ahead and
        // turn RX back on to continue collecting data.
        if (receiveModeEnabled) {
          RAIL_StartRx(railHandle, channel, NULL);
        }
      }
    }
    if (events & RAIL_EVENT_RX_ADDRESS_FILTERED) {
      counters.addrFilterEvent++;
      RAILCb_RxPacketAborted(railHandle);
    }
    if (events & RAIL_EVENT_RX_PACKET_ABORTED) {
      counters.rxFail++;
      RAILCb_RxPacketAborted(railHandle);
    }
    if (events & RAIL_EVENT_RX_FRAME_ERROR) {
      counters.frameError++;
      RAILCb_RxPacketAborted(railHandle);
    }
  }
  if (events & RAIL_EVENT_RX_ACK_TIMEOUT) {
    counters.ackTimeout++;
    rxAckTimeout = true;
    //TODO: packetTime depends on txTimePosition;
    //      this code assumes default position (PACKET_END).
    ackTimeoutDuration = RAIL_GetTime()
                         - previousTxAppendedInfo.timeSent.packetTime;
  }
  // End scheduled receive mode if an appropriate end or error event is received
  if ((events & (RAIL_EVENT_RX_SCHEDULED_RX_END
                 | RAIL_EVENT_RX_SCHEDULED_RX_MISSED))
      || ((schRxStopOnRxEvent && inAppMode(RX_SCHEDULED, NULL))
          && (events & (RAIL_EVENT_RX_ADDRESS_FILTERED
                        | RAIL_EVENT_RX_PACKET_ABORTED
                        | RAIL_EVENT_RX_FIFO_OVERFLOW
                        | RAIL_EVENT_RX_FRAME_ERROR)))) {
    // N.B. RAIL_EVENT_RX_PACKET_RECEIVED was handled in its callback already
    enableAppMode(RX_SCHEDULED, false, NULL);
  }

  // TX Events
  if (events & RAIL_EVENT_TX_START_CCA) {
    counters.lbtStartCca++;
  }
  if (events & RAIL_EVENT_TX_CCA_RETRY) {
    counters.lbtRetry++;
  }
  if (events & RAIL_EVENT_TX_CHANNEL_CLEAR) {
    counters.lbtSuccess++;
    ccaSuccesses++;
    if ((txOptions & RAIL_TX_OPTION_CCA_ONLY) != 0U) {
      lastTxStatus = events;
      newTxError = true; // This is a 'pretend error'; see printNewTxError()
      // This doesn't counters.userTx++;
      //@TODO: Should we instead initiate an immediate transmit here?
      scheduleNextTx();
    }
  }
#if RAIL_SUPPORTS_MFM
  if (events & RAIL_EVENT_MFM_TX_BUFFER_DONE) {
    if (railDataConfig.txSource == TX_MFM_DATA) {
      counters.userTx++;
    }
  }
#endif
  if (events & RAIL_EVENT_TX_STARTED) {
    counters.userTxStarted++;
    (void) RAIL_GetTxTimePreambleStart(railHandle, RAIL_TX_STARTED_BYTES,
                                       &txStartTime);
  }
  if (events & RAIL_EVENT_TX_FIFO_ALMOST_EMPTY) {
    RAILCb_TxFifoAlmostEmpty(railHandle);
  }
  // Process TX success before any failures in case an auto-repeat fails
  if (events & RAIL_EVENT_TX_PACKET_SENT) {
    counters.userTx++;
    txRemainingCount = RAIL_GetTxPacketsRemaining(railHandle);
    if (txRemainingCount != txRepeatCount) {
      counters.userTxRemainingErrors++;
    }
    if (txRemainingCount > 0) {
      // Defer calling RAILCb_TxPacketSent() to last of auto-repeat transmits
      internalTransmitCounter++;
      if (txRepeatCount != RAIL_TX_REPEAT_INFINITE_ITERATIONS) {
        txRepeatCount--;
      }
    } else {
      txRepeatCount = 0;
      RAILCb_TxPacketSent(railHandle, false);
    }
  }
  if (events & (RAIL_EVENT_TX_ABORTED
                | RAIL_EVENT_TX_BLOCKED
                | RAIL_EVENT_TX_UNDERFLOW
                | RAIL_EVENT_TX_CHANNEL_BUSY
                | RAIL_EVENT_TX_SCHEDULED_TX_MISSED)) {
    lastTxStatus = events;
    txRemainingCount = RAIL_GetTxPacketsRemaining(railHandle);
    if ((txRepeatCount != RAIL_TX_REPEAT_INFINITE_ITERATIONS)
        && (txRepeatCount > 0)
        && ((events & (RAIL_EVENT_TX_ABORTED | RAIL_EVENT_TX_UNDERFLOW)) == 0U)) {
      txRepeatCount++; // A transmit never happened
    }
    if (txRemainingCount != txRepeatCount) {
      counters.userTxRemainingErrors++;
    }
    txRepeatCount = 0;
    newTxError = true;
    failPackets++;
    scheduleNextTx();

    // Increment counters for TX events
    if (events & RAIL_EVENT_TX_ABORTED) {
      counters.userTxAborted++;
    }
    if (events & RAIL_EVENT_TX_BLOCKED) {
      counters.userTxBlocked++;
    }
    if (events & RAIL_EVENT_TX_UNDERFLOW) {
      counters.userTxUnderflow++;
    }
#if RAIL_IEEE802154_SUPPORTS_G_MODESWITCH && defined(WISUN_MODESWITCHPHRS_ARRAY_SIZE)
    if (modeSwitchState == TX_MS_PACKET) {
      // Restore first 2 bytes overwritten by Mode Switch PHR
      txData[0] = txData_2B[0];
      txData[1] = txData_2B[1];

      modeSwitchState = IDLE;
      modeSwitchDelayUs = 0;
      modeSwitchBaseChannel = 0xFFFFU;
      modeSwitchNewChannel = 0xFFFFU;
      setNextAppMode(NONE, NULL);
    }
 #endif
  }
  // Put this here too so that we do these things twice
  // in the case that an ack and a non ack have completed
  if (events & RAIL_EVENT_TXACK_PACKET_SENT) {
    counters.ackTx++;
    RAILCb_TxPacketSent(railHandle, true);
  }
  if (events & (RAIL_EVENT_TXACK_ABORTED
                | RAIL_EVENT_TXACK_BLOCKED
                | RAIL_EVENT_TXACK_UNDERFLOW)) {
    lastTxAckStatus = events;
    failAckPackets++;
    pendFinishTxAckSequence();

    // Increment counters for TXACK events
    if (events & RAIL_EVENT_TXACK_ABORTED) {
      counters.ackTxAborted++;
    }
    if (events & RAIL_EVENT_TXACK_BLOCKED) {
      counters.ackTxBlocked++;
    }
    if (events & RAIL_EVENT_TXACK_UNDERFLOW) {
      counters.ackTxUnderflow++;
    }
  }
  if (events & RAIL_EVENT_RX_CHANNEL_HOPPING_COMPLETE) {
    RAILCb_RxChannelHoppingComplete(railHandle);
  }
  if (events & RAIL_EVENT_PA_PROTECTION) {
    counters.paProtect++;
  }
#if RAIL_IEEE802154_SUPPORTS_G_MODESWITCH && defined(WISUN_MODESWITCHPHRS_ARRAY_SIZE)
  if (events & RAIL_EVENT_IEEE802154_MODESWITCH_START) {
    modeSwitchState = RX_ON_NEW_PHY;
    modeSwitchBaseChannel = channel;
    channel = getLikelyChannel();
    modeSwitchNewChannel = channel;
  }
  if (events & RAIL_EVENT_IEEE802154_MODESWITCH_END) {
    modeSwitchState = IDLE;
    channel = getLikelyChannel();
    modeSwitchBaseChannel = 0xFFFFU;
    modeSwitchNewChannel = 0xFFFFU;
  }
#endif
#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT
  if (RAIL_IEEE802154_IsEnabled(railHandle)) {
    sl_rail_util_ieee801254_on_rail_event(railHandle, events);
  }
#endif //SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT
#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
  if (RAIL_BLE_IsEnabled(railHandle)) {
    sl_bt_ll_coex_handle_events(events);
  }
#endif //SL_CATALOG_RAIL_UTIL_COEX_PRESENT
}

volatile bool allowPowerManagerSleep = false;
// This app-level function overrides a weak power manager implementation and
// gets called in sl_power_manager_handler.c.
// By default prevent the RAILtest App from sleeping and
// keep CLI responsive.
bool app_is_ok_to_sleep(void)
{
  return allowPowerManagerSleep;
}

/******************************************************************************
 * Application Helper Functions
 *****************************************************************************/
void processPendingCalibrations(void)
{
  // Only calibrate the radio when not currently transmitting or in a
  // transmit mode. Also don't try to calibrate while receiving a packet
  bool calsInMode = inAppMode(NONE, NULL);
  if (calibrateRadio && calsInMode && !skipCalibrations && !receivingPacket) {
    RAIL_CalMask_t pendingCals = RAIL_GetPendingCal(railHandle);

    counters.calibrations++;
    calibrateRadio = false;

    // Perform the necessary calibrations and don't save the results
    if (pendingCals & RAIL_CAL_TEMP_VCO) {
      RAIL_CalibrateTemp(railHandle);
    }

    if (pendingCals & RAIL_CAL_ONETIME_IRCAL) {
      RAIL_AntennaSel_t rfPath = RAIL_ANTENNA_AUTO;
      RAIL_Status_t retVal = RAIL_GetRfPath(railHandle, &rfPath);

      if (retVal == RAIL_STATUS_NO_ERROR) {
        // Disable the radio if we have to do IRCAL
        RAIL_Idle(railHandle, RAIL_IDLE_ABORT, false);
        RAIL_CalibrateIrAlt(railHandle, NULL, rfPath);
        if (receiveModeEnabled) {
          RAIL_StartRx(railHandle, channel, NULL);
        }
      }
    }
  }
}

RAIL_FrequencyOffset_t getRxFreqOffset(void)
{
  RAIL_FrequencyOffset_t retVal = rxFreqOffset;
  rxFreqOffset = RAIL_FREQUENCY_OFFSET_INVALID;
  return retVal;
}

void checkTimerExpiration(void)
{
  if (railTimerExpired) {
    railTimerExpired = false;
    uint32_t paramRailTimerExpireTime = railTimerExpireTime;

    responsePrint("timerCb",
                  "TimerExpiredCallback:%u,ConfiguredExpireTime:%u",
                  paramRailTimerExpireTime,
                  railTimerConfigExpireTime);
  }
}

void printNewTxError(void)
{
  RAIL_Events_t paramLastTxStatus;

  if (newTxError) {
    newTxError = false;
    if (lastTxStatus & (RAIL_EVENT_TX_UNDERFLOW | RAIL_EVENT_TX_ABORTED)) {
      if (logLevel & ASYNC_RESPONSE) {
        paramLastTxStatus = lastTxStatus;
        responsePrint("txPacket",
                      "txStatus:Error,"
                      "errorReason:Tx underflow or abort,"
                      "errorCode:0x%x%08x",
                      (uint32_t)(paramLastTxStatus >> 32),
                      (uint32_t)(paramLastTxStatus));
      }
    }
    if (lastTxStatus & RAIL_EVENT_TX_BLOCKED) {
      if (logLevel & ASYNC_RESPONSE) {
        paramLastTxStatus = lastTxStatus;
        responsePrint("txPacket",
                      "txStatus:Error,"
                      "errorReason:Tx blocked,"
                      "errorCode:0x%x%08x",
                      (uint32_t)(paramLastTxStatus >> 32),
                      (uint32_t)(paramLastTxStatus));
      }
    }
    if (lastTxStatus & RAIL_EVENT_TX_SCHEDULED_TX_MISSED) {
      if (logLevel & ASYNC_RESPONSE) {
        paramLastTxStatus = lastTxStatus;
        responsePrint("txPacket",
                      "txStatus:Error,"
                      "errorReason:SchedTx missed,"
                      "errorCode:0x%x%08x",
                      (uint32_t)(paramLastTxStatus >> 32),
                      (uint32_t)(paramLastTxStatus));
      }
    }
    if (lastTxStatus & RAIL_EVENT_TX_CHANNEL_BUSY) {
      if (logLevel & ASYNC_RESPONSE) {
        paramLastTxStatus = lastTxStatus;
        responsePrint("txPacket",
                      "txStatus:Error,"
                      "errorReason:Tx channel busy,"
                      "errorCode:0x%x%08x",
                      (uint32_t)(paramLastTxStatus >> 32),
                      (uint32_t)(paramLastTxStatus));
      }
      counters.txChannelBusy++;
    }
    if (lastTxStatus & RAIL_EVENT_TX_CHANNEL_CLEAR) {
      if (logLevel & ASYNC_RESPONSE) {
        responsePrint("txPacket",
                      "txStatus:ChannelClear");
      }
    }
  }
}

void printAckTimeout(void)
{
  if (rxAckTimeout) {
    rxAckTimeout = false;
    responsePrint("rxAckTimeout",
                  "ackTimeoutDuration:%d",
                  ackTimeoutDuration);
  }
}

void changeChannel(uint32_t i)
{
  channel = i;
  updateGraphics();
  // Apply the new channel immediately if you are in receive already
  if (receiveModeEnabled
      || ((RAIL_GetRadioState(railHandle) & RAIL_RF_STATE_RX) != 0U)) {
    RAIL_Status_t status = RAIL_StartRx(railHandle, channel, NULL);

    // Lock up if changing the channel failed since calls to this are supposed
    // to be checked for errors
    if (status != RAIL_STATUS_NO_ERROR) {
      responsePrintError("changeChannel",
                         0xF0,
                         "FATAL, call to RAIL_StartRx() failed (%u)", status);
      while (1) ;
    }
  }
#if RAIL_IEEE802154_SUPPORTS_G_MODESWITCH && defined(WISUN_MODESWITCHPHRS_ARRAY_SIZE)
  if (modeSwitchState != IDLE) {
    void *modeSwitchChangeChannelHandle = memoryAllocate(sizeof(RailAppEvent_t));
    RailAppEvent_t *modeSwitchChangeChannel = (RailAppEvent_t *)memoryPtrFromHandle(modeSwitchChangeChannelHandle);
    if (modeSwitchChangeChannel == NULL) {
      eventsMissed++;
      return;
    }
    modeSwitchChangeChannel->type = MODE_SWITCH_CHANGE_CHANNEL;
    modeSwitchChangeChannel->modeSwitchChangeChannel.channel = channel;
    queueAdd(&railAppEventQueue, modeSwitchChangeChannelHandle);
  }
  if (i != modeSwitchNewChannel) {
    modeSwitchState = IDLE;
  }
#endif
}

void pendPacketTx(void)
{
  packetTx = true;
}

RAIL_Status_t chooseTxType(void)
{
  // Invalidate the previous TX's start time
  txStartTime = 0U;
  if (currentAppMode() == TX_SCHEDULED || currentAppMode() == SCHTX_AFTER_RX) {
    if (txType == TX_TYPE_CSMA) {
      return RAIL_StartScheduledCcaCsmaTx(railHandle, channel, txOptions, &nextPacketTxTime,
                                          csmaConfig, NULL);
    } else if (txType == TX_TYPE_LBT) {
      return RAIL_StartScheduledCcaLbtTx(railHandle, channel, txOptions, &nextPacketTxTime,
                                         lbtConfig, NULL);
    } else {
      return RAIL_StartScheduledTx(railHandle, channel, txOptions, &nextPacketTxTime, NULL);
    }
  } else if (txType == TX_TYPE_LBT) {
    return RAIL_StartCcaLbtTx(railHandle, channel, txOptions, lbtConfig, NULL);
  } else if (txType == TX_TYPE_CSMA) {
    return RAIL_StartCcaCsmaTx(railHandle, channel, txOptions, csmaConfig, NULL);
  } else {
    return RAIL_StartTx(railHandle, channel, txOptions, NULL);
  }
}

void sendPacketIfPending(void)
{
  if (packetTx) {
    packetTx = false;
    uint8_t txStatus;

    // Don't decrement in continuous mode
    if (currentAppMode() != TX_CONTINUOUS) {
      txCount--;
    }

    // Generate the payload and start transmitting
    if (currentAppMode() != TX_UNDERFLOW) { // Force underflows in this mode
      // Load packet data before transmitting if manual loading is not enabled
      if (!txFifoManual) {
        loadTxData(txData, txDataLen);
      }
    }

    txStatus = chooseTxType();

    if (txStatus != 0) {
      lastTxStatus = txStatus;
      failPackets++;
      scheduleNextTx(); // No callback will fire, so fake it
    } else if (currentAppMode() == TX_CANCEL) {
      usDelay(txCancelDelay);
      txRepeatCount = 0;
      if (txCancelMode == RAIL_STOP_MODES_NONE) {
        RAIL_Idle(railHandle, RAIL_IDLE_ABORT, false);
      } else {
        RAIL_StopTx(railHandle, txCancelMode);
      }
    }
  }
}

void pendFinishTxSequence(void)
{
  finishTxSequence = true;
}

void pendFinishTxAckSequence(void)
{
  finishTxAckSequence = true;
}

void finishTxSequenceIfPending(void)
{
  if (finishTxSequence) {
    // Defer finishing to next main-loop iteration if Tx completion
    // event snuck in after printNewTxError() but before this call.
    if (newTxError) {
      return;
    }
    finishTxSequence = false;

    // Compute the number of packets sent
    uint32_t sentPackets = internalTransmitCounter - startTransmitCounter;
    // Don't log if we didn't send any packets
    if ((logLevel & ASYNC_RESPONSE)
        && ((failPackets != 0) || (sentPackets != 0) || (ccaSuccesses != 0))) {
      RAIL_Events_t paramLastTxStatus = lastTxStatus;
      uint32_t paramFailPackets = failPackets;
      // Print the number of sent and failed packets
      responsePrint("txEnd",
                    "txStatus:%s,"
                    "transmitted:%u,"
                    "lastTxTime:%u,"
                    "timePos:%u,"
                    "lastTxStart:%u,"
                    "ccaSuccess:%u,"
                    "failed:%u,"
                    "lastTxStatus:0x%x%08x,"
                    "txRemain:%d,"
                    "isAck:False",
                    (paramFailPackets == 0
                     ? "Complete"
                     : (sentPackets == 0 ? "Error" : "Partial")),
                    sentPackets,
                    previousTxAppendedInfo.timeSent.packetTime,
                    previousTxAppendedInfo.timeSent.timePosition,
                    txStartTime,
                    ccaSuccesses,
                    paramFailPackets,
                    (uint32_t)(paramLastTxStatus >> 32),
                    (uint32_t)(paramLastTxStatus),
                    txRemainingCount);
    }
    startTransmitCounter = internalTransmitCounter;
    failPackets = 0;
    ccaSuccesses = 0;
    lastTxStatus = 0;
  }
  if (finishTxAckSequence) {
    finishTxAckSequence = false;

    if ((logLevel & ASYNC_RESPONSE) && printTxAck) {
      RAIL_Events_t paramLastTxAckStatus = lastTxAckStatus;
      uint32_t paramFailAckPackets = failAckPackets;
      uint32_t paramSentAckPackets = sentAckPackets;
      // Print the number of sent and failed packets
      responsePrint("txEnd",
                    "txStatus:%s,"
                    "transmitted:%u,"
                    "lastTxTime:%u,"
                    "timePos:%u,"
                    "failed:%u,"
                    "lastTxStatus:0x%x%08x,"
                    "isAck:True",
                    paramFailAckPackets == 0
                    ? "Complete"
                    : (paramSentAckPackets == 0 ? "Error" : "Partial"),
                    paramSentAckPackets,
                    previousTxAckAppendedInfo.timeSent.packetTime,
                    previousTxAckAppendedInfo.timeSent.timePosition,
                    paramFailAckPackets,
                    (uint32_t)(paramLastTxAckStatus >> 32),
                    (uint32_t)(paramLastTxAckStatus));
    }
    sentAckPackets = 0;
    failAckPackets = 0;
    lastTxAckStatus = 0;
  }
}

void printPacket(char *cmdName,
                 uint8_t *data,
                 uint16_t dataLength,
                 RxPacketData_t *packetData)
{
  // Print out a length 0 packet message if no packet was given
  if ((data == NULL) && (packetData == NULL)) {
    responsePrint(cmdName, "len:0");
    return;
  }

  // If this is an Rx packet print the appended info
  responsePrintStart(cmdName);
  if (packetData != NULL) {
    responsePrintContinue(
      "len:%d,timeUs:%u,timePos:%u,crc:%s,filterMask:0x%x,rssi:%d,lqi:%d,phy:%d",
      packetData->dataLength,
      packetData->appendedInfo.timeReceived.packetTime,
      packetData->appendedInfo.timeReceived.timePosition,
      (packetData->appendedInfo.crcPassed) ? "Pass" : "Fail",
      packetData->filterMask,
      packetData->appendedInfo.rssi,
      packetData->appendedInfo.lqi,
      packetData->appendedInfo.subPhyId);
    responsePrintContinue(
      "isAck:%s,syncWordId:%d,antenna:%d,channelHopIdx:%d,channel:%u",
      packetData->appendedInfo.isAck ? "True" : "False",
      packetData->appendedInfo.syncWordId,
      packetData->appendedInfo.antennaId,
      packetData->appendedInfo.channelHoppingChannelIndex,
      packetData->appendedInfo.channel);
    if (RAIL_IEEE802154_IsEnabled(railHandle)) {
      responsePrintContinue(
        "ed154:%u,lqi154:%u",
        RAIL_IEEE802154_ConvertRssiToEd(packetData->appendedInfo.rssi),
        RAIL_IEEE802154_ConvertRssiToLqi(packetData->appendedInfo.lqi,
                                         packetData->appendedInfo.rssi));
    }
    if (printRxFreqOffsetData) {
      if (packetData->freqOffset == RAIL_FREQUENCY_OFFSET_INVALID) {
        responsePrintContinue("freqOffset:Invalid");
      } else {
        responsePrintContinue("freqOffset:%d", packetData->freqOffset);
      }
    }
  } else {
    responsePrintContinue("len:%d", dataLength);
  }
  if (data != NULL) {
    // Manually print out payload bytes iteratively, so that we don't need to
    // reserve a RAM buffer. Finish the response here.
    RAILTEST_PRINTF("{payload:");
    for (int i = 0; i < dataLength; i++) {
      RAILTEST_PRINTF(" 0x%.2x", data[i]);
    }
    RAILTEST_PRINTF("}");
  }
  RAILTEST_PRINTF("}\n");
}

void enqueueEvents(RAIL_Events_t events)
{
  events &= enablePrintEvents;
  if (events != RAIL_EVENTS_NONE) {
    void *railEventHandle = memoryAllocate(sizeof(RailAppEvent_t));
    RailAppEvent_t *railEvent = (RailAppEvent_t *)memoryPtrFromHandle(railEventHandle);
    if (railEvent == NULL) {
      eventsMissed++;
      return;
    }

    railEvent->type = RAIL_EVENT;

    // No need to disable interrupts; this is only called from interrupt
    // context and RAIL doesn't support nested interrupts/events.
    railEvent->railEvent.timestamp = RAIL_GetTime();
    railEvent->railEvent.events[1] = (uint32_t)(events >> 32);
    railEvent->railEvent.events[0] = (uint32_t)(events);

    queueAdd(&railAppEventQueue, railEventHandle);
  }
}

void printRailEvents(RailEvent_t *railEvent)
{
  RAIL_Events_t events = (((RAIL_Events_t) railEvent->events[1] << 32)
                          | railEvent->events[0]);
  if ((events >> numRailEvents) != RAIL_EVENTS_NONE) {
    responsePrintError("printRailEvents", 0x2,
                       "Unknown RAIL Events:0x%x%08x",
                       (uint32_t)(events >> 32),
                       (uint32_t)(events));
    return;
  }
  for (unsigned int i = 0U; events != RAIL_EVENTS_NONE; i++, events >>= 1) {
    if ((events & 1U) != RAIL_EVENTS_NONE) {
      responsePrint("event",
                    "timestamp:%u,eventName:RAIL_EVENT_%s",
                    railEvent->timestamp,
                    eventNames[i]);
    }
  }
}

char *handleToString(RAIL_Handle_t railHandle)
{
  (void)railHandle;
  return "r";//for RAILtest vs MP configuration
}

RAIL_Status_t configureTxFifo(void)
{
  uint16_t fifoSize = RAIL_SetTxFifo(railHandle, txFifo.fifo, 0, SL_RAIL_TEST_TX_BUFFER_SIZE);
  if (fifoSize != SL_RAIL_TEST_TX_BUFFER_SIZE) {
    return RAIL_STATUS_INVALID_PARAMETER;
  }
  return RAIL_STATUS_NO_ERROR;
}
