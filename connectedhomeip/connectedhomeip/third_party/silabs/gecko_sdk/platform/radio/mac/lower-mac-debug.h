/***************************************************************************//**
 * @brief Lower MAC debug definitions.
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

#ifndef LOWER_MAC_DEBUG_H
#define LOWER_MAC_DEBUG_H

#if defined(LOWER_MAC_DEBUG)

#if defined(MAC_DEBUG_TOKEN)
#define LOWER_MAC_DEBUG_MAX_ACTIONS      LOWER_MAC_DEBUG_ACTION_TOKEN_TABLE_SIZE
#else
#define LOWER_MAC_DEBUG_MAX_ACTIONS      50
#endif

#define ACTIONS_DUMP_SERIAL_PORT         1
#define INTERNAL_FLAG_COUNT              16

#define RAIL_RADIO_STATE_COUNT           6
#define LOWER_MAC_STATE_COUNT            6

// #defines below are used to ensure fix-length printing
// separates flag when printing multiple flags
#define PRINT_FLAG_SEPERATOR             "+"
// character limit for all columns except flags
#define PRINT_GENERIC_FIELD_LENGTH       20
// character limit when printing flags
#define PRINT_FLAGS_FIELD_LENGTH         75
// indicates fixed field length printing
#define FIX_FIELD_LENGTH                 1
// indicate string is a flag
#define IS_FLAG                          1

enum Lower_Mac_Actions {
  LOWER_MAC_DEBUG_ACTION_SET_RADIO_STATE_IDLE   =    0,
  LOWER_MAC_DEBUG_ACTION_SET_RADIO_STATE_RX     =    1,
  LOWER_MAC_DEBUG_ACTION_SET_RADIO_STATE_TX     =    2,
  LOWER_MAC_DEBUG_ACTION_SET_TX_FIFO            =    3,
  LOWER_MAC_DEBUG_ACTION_TX_DATA_SENT           =    4,
  LOWER_MAC_DEBUG_ACTION_TX_ACK_SENT            =    5,
  LOWER_MAC_DEBUG_ACTION_TX_FAILED_DATA         =    6,
  LOWER_MAC_DEBUG_ACTION_TX_FAILED_ACK          =    7,
  LOWER_MAC_DEBUG_ACTION_RX_PACKET              =    8,
  LOWER_MAC_DEBUG_ACTION_RX_ACK_TIMEOUT         =    9,
  LOWER_MAC_DEBUG_ACTION_SET_CHANNEL            =    10,
  LOWER_MAC_DEBUG_ACTION_MAC_EVENT              =    11,
  LOWER_MAC_DEBUG_ACTION_MAC_TICK               =    12,
  LOWER_MAC_DEBUG_ACTION_CALIBRATE              =    13,
  LOWER_MAC_DEBUG_ACTION_GET_RSSI               =    14,
  LOWER_MAC_DEBUG_ACTION_SYMBOL_TIMER           =    15,
  LOWER_MAC_DEBUG_ACTION_ACK_SENT_WORKAROUND    =    16,
  LOWER_MAC_DEBUG_ACTION_RAIL_EVENTS            =    17,
  LOWER_MAC_DEBUG_ACTION_SCHEDULER_EVENT        =    18,
  LOWER_MAC_DEBUG_ACTION_ASSERT                 =    19,
  LOWER_MAC_DEBUG_ACTION_CSL_LAST_WAKEUP_FRAME_ADDED =  20,
  LOWER_MAC_DEBUG_ACTION_CSL_PAYLOAD_ADDED      =    21,
  LOWER_MAC_DEBUG_ACTION_CSL_TX_COMPLETE        =    22,
  LOWER_MAC_DEBUG_ACTION_CSL_TX_FIFO_THRESHOLD  =    23,
  LOWER_MAC_DEBUG_ACTION_COUNT
};

// arrays for decoding int to string for human friendly printing
const char*  debugActionPrintTags[LOWER_MAC_DEBUG_ACTION_COUNT] = {
  "SET_RADIO_STATE_IDLE",
  "SET_RADIO_STATE_RX",
  "SET_RADIO_STATE_TX",
  "SET_TX_FIFO",
  "TX_DATA_SENT",
  "TX_ACK_SENT",
  "TX_FAILED_DATA",
  "TX_FAILED_ACK",
  "RX_PACKET",
  "RX_ACK_TIMEOUT",
  "SET_CHANNEL",
  "MAC_EVENT",
  "MAC_TICK",
  "CALIBRATE",
  "GET_RSSI",
  "SYMBOL_TIMER",
  "ACK_SENT_WORKAROUND",
  "RAIL_EVENTS",
  "SCHEDULER_EVENT",
  "ASSERT",
  "CSL_LAST_WAKEUP_FRAME_ADDED",
  "CSL_PAYLOAD_ADDED",
  "CSL_TX_COMPLETE",
  "CSL_TX_FIFO_THRESHOLD"
};

// These states are defined in super/platform/radio/rail_lib/common/rail_types.h
const char* radioStatePrintTags[RAIL_RADIO_STATE_COUNT] = {
  "INACTIVE",
  "IDLE",
  "RX",
  "RX_ACTIVE",
  "TX",
  "TX_ACTIVE"
};

// Flags below must be same as those defined in ./lower-mac-rail-802.15.4.c
// miscInternalFlag is 16 bit bitmask
const char* internalFlagsPrintTags[16] = {
  "RADIO_INIT_DONE",
  "ONGOING_TX_DATA",
  "ONGOING_TX_ACK",
  "WAITING_FOR_ACK",
  "SYMBOL_TIMER_RUNNING",
  "CURRENT_TX_USE_CSMA",
  "DATA_POLL_FRAME_PENDING_SET",
  "CALIBRATION_NEEDED",
  "IDLE_PENDING",
  "CURRENT_TX_USE_WAKEUP_FRAMES",
  "UNUSED", "UNSUED", "UNUSED", "UNUSED", "UNUSED", "UNUSED"
};

const char* lowerMacStatePrintTags[LOWER_MAC_STATE_COUNT] = {
  "IDLE",
  "EXPECTING_DATA",
  "BUSY",
  "TX_NO_ACK",
  "TX_WAITING_FOR_ACK",
  "RADIO_INIT"
};

static uint16_t debugInterruptDisableCounter = 0;
static struct {
  bool pending;
  bool isAck;
} debugTx;

static uint8_t debugActionsTailIndex = 0;

typedef struct {
  uint8_t action;
  uint8_t lowerMacState;
  uint16_t flags;
  uint8_t radioState;
  uint32_t infoLow;
  uint32_t infoHigh;
  uint32_t timestamp;
} DebugAction;

// this variable keeps track of number of flag chars printed
// to align all cloumns and ensure fix-length printing
static uint16_t flagCharsPrinted;
static DebugAction debugActions[LOWER_MAC_DEBUG_MAX_ACTIONS];
static DebugAction debugActionsCopy[LOWER_MAC_DEBUG_MAX_ACTIONS];

extern EmberStatus emberSerialGuaranteedPrintf(uint8_t port,
                                               const char * formatString,
                                               ...);

void LOWER_MAC_DEBUG_INIT(void)
{
  MEMSET(debugActions, 0xFF, sizeof(DebugAction) * LOWER_MAC_DEBUG_MAX_ACTIONS);
}

void LOWER_MAC_DEBUG_ADD_ACTION(uint8_t action, uint64_t info)
{
  uint8_t index;

  // Move the tail within a critical section
  ATOMIC(
    index = debugActionsTailIndex;
    debugActionsTailIndex = (debugActionsTailIndex + 1)
                            % LOWER_MAC_DEBUG_MAX_ACTIONS;
    );

  // Write the old tail
  debugActions[index].action = action;
  debugActions[index].lowerMacState = emLowerMacState;
  debugActions[index].flags = miscInternalFlags;
  debugActions[index].radioState = RAIL_GetRadioState(connectRailHandle);
  debugActions[index].infoLow = (info & 0xFFFFFFFF);
  debugActions[index].infoHigh = (info >> 32);
  debugActions[index].timestamp = RAIL_GetTime();
}

// helper function to return debug string to be printed
// for human friendly report

static char* GET_DEBUG_PRINT_STRING(const char* array[],
                                    uint8_t length,
                                    uint8_t idx)
{
  if (idx < length) {
    return (char*)array[idx];
  } else {
    return "Default";
  }
}

static void LOWER_MAC_DEBUG_DUMP_STRING(char* string,
                                        uint8_t fixWidth,
                                        uint8_t isFlag)
{
  uint8_t j = 0;
#if defined(EMBER_SCRIPTED_TEST)
  while (string[j] != '\0') {
    simPrint("%c", string[j]);
    j++;
  }
  if (isFlag) {
    flagCharsPrinted += j;
  }
  if (fixWidth) {
    simPrint(" ");
    while (j < PRINT_GENERIC_FIELD_LENGTH) {
      simPrint(" ");
      j++;
    }
    simPrint(", ");
  }
#else
  halResetWatchdog();
  while (string[j] != '\0') {
    emberSerialGuaranteedPrintf(ACTIONS_DUMP_SERIAL_PORT, "%c", string[j]);
    j++;
  }
  if (isFlag) {
    flagCharsPrinted += j;
  }
  if (fixWidth) {
    emberSerialGuaranteedPrintf(ACTIONS_DUMP_SERIAL_PORT, " ");
    while (j < PRINT_GENERIC_FIELD_LENGTH) {
      emberSerialGuaranteedPrintf(ACTIONS_DUMP_SERIAL_PORT, " ");
      j++;
    }
    // for CSV file
    emberSerialGuaranteedPrintf(ACTIONS_DUMP_SERIAL_PORT, ", ");
  }
#endif
}

static void FIX_FLAG_PRINT_LENGTH()
{
  while (flagCharsPrinted < PRINT_FLAGS_FIELD_LENGTH) {
#if defined(EMBER_SCRIPTED_TEST)
    simPrint(" ");
#else
    emberSerialGuaranteedPrintf(ACTIONS_DUMP_SERIAL_PORT, " ");
#endif
    flagCharsPrinted++;
  }
  // for CSV file
  emberSerialGuaranteedPrintf(ACTIONS_DUMP_SERIAL_PORT, ", ");
}

void LOWER_MAC_DEBUG_PRINT_COLUMN_HEADERS(void)
{
  // print column headers
  LOWER_MAC_DEBUG_DUMP_STRING("\r\n", !FIX_FIELD_LENGTH, !IS_FLAG);
  LOWER_MAC_DEBUG_DUMP_STRING("ACTION", FIX_FIELD_LENGTH, !IS_FLAG);
  LOWER_MAC_DEBUG_DUMP_STRING("Lower Mac State", FIX_FIELD_LENGTH, !IS_FLAG);
  LOWER_MAC_DEBUG_DUMP_STRING("Radio State", FIX_FIELD_LENGTH, !IS_FLAG);
  // print flag with set field
  flagCharsPrinted = 0;
  LOWER_MAC_DEBUG_DUMP_STRING("Flags", !FIX_FIELD_LENGTH, IS_FLAG);
  FIX_FLAG_PRINT_LENGTH();
  LOWER_MAC_DEBUG_DUMP_STRING("Info High, ", !FIX_FIELD_LENGTH, !IS_FLAG);
  LOWER_MAC_DEBUG_DUMP_STRING("Info Low, ", !FIX_FIELD_LENGTH, !IS_FLAG);
  LOWER_MAC_DEBUG_DUMP_STRING("Time Stamp\r\n", !FIX_FIELD_LENGTH, !IS_FLAG);
}

// "flags" is a bitwise or of multiple flas
// This funtion prints the flag corresponding to each bit
// in a fixed width field
void LOWER_MAC_DEBUG_PRINT_FLAGS(uint16_t localFlagsCopy)
{
  uint8_t bitNum = 0, flagsPrinted = 0;
  char* internalFlagsString;

  flagCharsPrinted = 0;

  if ( localFlagsCopy == 0xFFFF) {
    LOWER_MAC_DEBUG_DUMP_STRING("Default", !FIX_FIELD_LENGTH, IS_FLAG);
    FIX_FLAG_PRINT_LENGTH();
  } else {
    while (localFlagsCopy && bitNum < INTERNAL_FLAG_COUNT) {
      if (localFlagsCopy & 1) {
        if (flagsPrinted) {
          LOWER_MAC_DEBUG_DUMP_STRING(PRINT_FLAG_SEPERATOR,
                                      !FIX_FIELD_LENGTH,
                                      IS_FLAG);
        }
        internalFlagsString = GET_DEBUG_PRINT_STRING(internalFlagsPrintTags,
                                                     INTERNAL_FLAG_COUNT,
                                                     bitNum);
        LOWER_MAC_DEBUG_DUMP_STRING(internalFlagsString,
                                    !FIX_FIELD_LENGTH,
                                    IS_FLAG);
        flagsPrinted = 1;
      }
      bitNum++;
      localFlagsCopy >>= 1;
    }
    FIX_FLAG_PRINT_LENGTH();
  }
}
void LOWER_MAC_DEBUG_DUMP_ACTIONS(void)
{
  uint8_t i, tailIndexCopy;
  uint16_t localFlagsCopy;
  char* debugActionString;
  char* radioStateString;
  char* lowerMacStateString;

  ATOMIC(
    tailIndexCopy = debugActionsTailIndex;
    MEMCOPY(debugActionsCopy,
            debugActions,
            sizeof(DebugAction) * LOWER_MAC_DEBUG_MAX_ACTIONS);
    );

  LOWER_MAC_DEBUG_PRINT_COLUMN_HEADERS();

  for (i = 0; i < LOWER_MAC_DEBUG_MAX_ACTIONS; i++) {
    uint8_t index = (tailIndexCopy + i) % LOWER_MAC_DEBUG_MAX_ACTIONS;

    debugActionString =
      GET_DEBUG_PRINT_STRING(debugActionPrintTags,
                             LOWER_MAC_DEBUG_ACTION_COUNT,
                             debugActionsCopy[index].action);

    lowerMacStateString =
      GET_DEBUG_PRINT_STRING(lowerMacStatePrintTags,
                             LOWER_MAC_STATE_COUNT,
                             debugActionsCopy[index].lowerMacState);

    radioStateString =
      GET_DEBUG_PRINT_STRING(radioStatePrintTags,
                             RAIL_RADIO_STATE_COUNT,
                             debugActionsCopy[index].radioState);

    LOWER_MAC_DEBUG_DUMP_STRING(debugActionString, FIX_FIELD_LENGTH, !IS_FLAG);
    LOWER_MAC_DEBUG_DUMP_STRING(lowerMacStateString,
                                FIX_FIELD_LENGTH,
                                !IS_FLAG);
    LOWER_MAC_DEBUG_DUMP_STRING(radioStateString, FIX_FIELD_LENGTH, !IS_FLAG);

    localFlagsCopy = debugActionsCopy[index].flags;
    LOWER_MAC_DEBUG_PRINT_FLAGS(localFlagsCopy);

#if defined(EMBER_SCRIPTED_TEST)
    simPrint("x%4x,  x%4x,  %u\r\n",
             debugActionsCopy[index].infoHigh,
             debugActionsCopy[index].infoLow,
             debugActionsCopy[index].timestamp);
#else
    emberSerialGuaranteedPrintf(ACTIONS_DUMP_SERIAL_PORT,
                                "x%4x, x%4x, %u\r\n",
                                debugActionsCopy[index].infoHigh,
                                debugActionsCopy[index].infoLow,
                                debugActionsCopy[index].timestamp);
#endif
  }
}

#if defined(MAC_DEBUG_TOKEN)

void LOWER_MAC_DEBUG_DUMP_ACTIONS_TO_TOKEN(void)
{
  tokTypeStackMacDebugAction tok;
  uint8_t i, tailIndexCopy;

  ATOMIC(
    tailIndexCopy = debugActionsTailIndex;
    MEMCOPY(debugActionsCopy,
            debugActions,
            sizeof(DebugAction) * LOWER_MAC_DEBUG_MAX_ACTIONS);
    );

  for (i = 0; i < LOWER_MAC_DEBUG_MAX_ACTIONS; i++) {
    uint8_t index = (tailIndexCopy + i) % LOWER_MAC_DEBUG_MAX_ACTIONS;
    tok.action = debugActionsCopy[index].action;
    tok.lowerMacState = debugActionsCopy[index].lowerMacState;
    tok.flags = debugActionsCopy[index].flags;
    tok.radioState = debugActionsCopy[index].radioState;
    // TODO: update token-related functions to support 64-bit info field.
    tok.infoLow = debugActionsCopy[index].infoLow;
    tok.infoHigh = debugActionsCopy[index].infoHigh;
    tok.timestamp = debugActionsCopy[index].timestamp;

    halCommonSetIndexedToken(TOKEN_STACK_MAC_DEBUG_ACTION_TABLE, i, &tok);
  }
}

void LOWER_MAC_DEBUG_PRINT_ACTIONS_FROM_TOKEN(void)
{
  tokTypeStackMacDebugAction tok;
  uint8_t i;
  uint16_t localFlagsCopy;
  char* debugActionString;
  char* radioStateString;
  char* lowerMacStateString;

  LOWER_MAC_DEBUG_PRINT_COLUMN_HEADERS();

  for (i = 0; i < LOWER_MAC_DEBUG_ACTION_TOKEN_TABLE_SIZE; i++) {
    halCommonGetIndexedToken(&tok, TOKEN_STACK_MAC_DEBUG_ACTION_TABLE, i);

    debugActionString = GET_DEBUG_PRINT_STRING(debugActionPrintTags,
                                               LOWER_MAC_DEBUG_ACTION_COUNT,
                                               tok.action);

    lowerMacStateString = GET_DEBUG_PRINT_STRING(lowerMacStatePrintTags,
                                                 LOWER_MAC_STATE_COUNT,
                                                 tok.lowerMacState);

    radioStateString = GET_DEBUG_PRINT_STRING(radioStatePrintTags,
                                              RAIL_RADIO_STATE_COUNT,
                                              tok.radioState);

    LOWER_MAC_DEBUG_DUMP_STRING(debugActionString, FIX_FIELD_LENGTH, !IS_FLAG);
    LOWER_MAC_DEBUG_DUMP_STRING(lowerMacStateString,
                                FIX_FIELD_LENGTH,
                                !IS_FLAG);
    LOWER_MAC_DEBUG_DUMP_STRING(radioStateString, FIX_FIELD_LENGTH, !IS_FLAG);

    localFlagsCopy  = tok.flags;
    LOWER_MAC_DEBUG_PRINT_FLAGS(localFlagsCopy);

#if defined(EMBER_SCRIPTED_TEST)
    simPrint("x%4x, x%4x, %u\r\n",
             tok.infoHigh,
             tok.infoLow,
             tok.timestamp);
#else
    halResetWatchdog();
    emberSerialGuaranteedPrintf(ACTIONS_DUMP_SERIAL_PORT,
                                "x%4x, x%4x, %u\r\n",
                                tok.infoHigh,
                                tok.infoLow,
                                tok.timestamp);
#endif
  }
}

void LOWER_MAC_DEBUG_CLEAR_ACTIONS_FROM_TOKEN(void)
{
  tokTypeStackMacDebugAction tok;
  uint8_t i;

  MEMSET(&tok, 0, sizeof(tokTypeStackMacDebugAction));

  for (i = 0; i < LOWER_MAC_DEBUG_ACTION_TOKEN_TABLE_SIZE; i++) {
    halCommonSetIndexedToken(TOKEN_STACK_MAC_DEBUG_ACTION_TABLE, i, &tok);
  }
}

#else // !MAC_DEBUG_TOKEN

#define LOWER_MAC_DEBUG_DUMP_ACTIONS_TO_TOKEN()
#define LOWER_MAC_DEBUG_PRINT_ACTIONS_FROM_TOKEN()
#define LOWER_MAC_DEBUG_CLEAR_ACTIONS_FROM_TOKEN()

#endif // MAC_DEBUG_TOKEN

#define LOWER_MAC_DECLARE_INTERRUPT_STATE()   DECLARE_INTERRUPT_STATE
#define LOWER_MAC_RESTORE_INT()     \
  do {                              \
    debugInterruptDisableCounter--; \
    RESTORE_INTERRUPTS();           \
  } while (0)

#define LOWER_MAC_DISABLE_INT()     \
  do {                              \
    debugInterruptDisableCounter++; \
    DISABLE_INTERRUPTS();           \
  } while (0)

#define LOWER_MAC_ASSERT(exp)                                              \
  do {                                                                     \
    if (!(exp)) {                                                          \
      LOWER_MAC_DEBUG_ADD_ACTION(LOWER_MAC_DEBUG_ACTION_ASSERT, __LINE__); \
      assert(0);                                                           \
    }                                                                      \
  } while (0)

#define LOWER_MAC_ASSERT_INTERRUPT_COUNTER()             \
  do {                                                   \
    LOWER_MAC_ASSERT(debugInterruptDisableCounter == 0); \
  } while (0)

#define LOWER_MAC_DEBUG_ASSERT_OUTGOING_PENDING(pend, ack) \
  do {                                                     \
    LOWER_MAC_ASSERT(debugTx.pending == pend);             \
    if (pend) {                                            \
      LOWER_MAC_ASSERT(debugTx.isAck == ack);              \
    }                                                      \
  } while (0)

void LOWER_MAC_DEBUG_SET_OUTGOING_PENDING(bool pending, bool isAck)
{
  debugTx.pending = pending;
  debugTx.isAck = isAck;
}

// Sleep timer is not supported in simulatiom.
#ifndef EMBER_TEST
#include "sl_sleeptimer.h"

#define LOWER_MAC_DEBUG_TX_SENT_TIMEOUT_MS  1000

static sl_sleeptimer_timer_handle_t debugTxTimerId;

static void debugTxTimerCallback(sl_sleeptimer_timer_handle_t* timerId,
                                 void *user)
{
  LOWER_MAC_ASSERT(0);
}

void LOWER_MAC_DEBUG_START_TX_SENT_TIMER(bool checkIsRunning)
{
  bool isRunning;

  LOWER_MAC_ASSERT(sl_sleeptimer_is_timer_running(&debugTxTimerId, &isRunning)
                   == SL_STATUS_OK);

  if (checkIsRunning) {
    LOWER_MAC_ASSERT(!isRunning);
  }

  LOWER_MAC_ASSERT(sl_sleeptimer_restart_timer_ms(&debugTxTimerId,
                                                  LOWER_MAC_DEBUG_TX_SENT_TIMEOUT_MS,
                                                  debugTxTimerCallback,
                                                  NULL,
                                                  0u,
                                                  0u) == SL_STATUS_OK);
}

void LOWER_MAC_DEBUG_CANCEL_TX_SENT_TIMER(bool checkIsRunning)
{
  bool isRunning;

  LOWER_MAC_ASSERT(sl_sleeptimer_is_timer_running(&debugTxTimerId, &isRunning)
                   == SL_STATUS_OK);

  if (checkIsRunning) {
    LOWER_MAC_ASSERT(isRunning);
  }

  sl_sleeptimer_stop_timer(&debugTxTimerId);
}

#else // EMBER_TEST
#define LOWER_MAC_DEBUG_START_TX_SENT_TIMER(checkIsRunning)
#define LOWER_MAC_DEBUG_CANCEL_TX_SENT_TIMER(checkIsRunning)
#endif // EMBER_TEST

#else // LOWER_MAC_DEBUG
#define LOWER_MAC_DECLARE_INTERRUPT_STATE()   DECLARE_INTERRUPT_STATE
#define LOWER_MAC_RESTORE_INT()               RESTORE_INTERRUPTS()
#define LOWER_MAC_DISABLE_INT()               DISABLE_INTERRUPTS()
#define LOWER_MAC_ASSERT_INTERRUPT_COUNTER()
#define LOWER_MAC_DEBUG_ASSERT_OUTGOING_PENDING(pending, isAck)
#define LOWER_MAC_DEBUG_SET_OUTGOING_PENDING(pending, isAck)
#define LOWER_MAC_DEBUG_TX_SENT_TIMER_INIT()
#define LOWER_MAC_DEBUG_START_TX_SENT_TIMER(checkIsRunning)
#define LOWER_MAC_DEBUG_CANCEL_TX_SENT_TIMER(checkIsRunning)
#define LOWER_MAC_DEBUG_INIT()
#define LOWER_MAC_DEBUG_ADD_ACTION(action, info)
#define LOWER_MAC_ASSERT(exp) assert(exp)
#define LOWER_MAC_DEBUG_DUMP_ACTIONS_TO_TOKEN()
void LOWER_MAC_DEBUG_DUMP_ACTIONS(void)
{
}
void LOWER_MAC_DEBUG_PRINT_ACTIONS_FROM_TOKEN(void)
{
}
#endif // LOWER_MAC_DEBUG

#endif // LOWER_MAC_DEBUG_H
