/***************************************************************************//**
 * @file
 * @brief Routines for managing messages for sleepy end devices.
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
#include "sleepy-message-queue.h"

#ifdef UC_BUILD
#include "sleepy-message-queue-config.h"
#endif

//------------------------------------------------------------------------------
// Forward Declaration
#ifdef UC_BUILD
sl_zigbee_event_t emberAfPluginSleepyMessageQueueTimeoutEvent;
#define msgTimeoutEvent (&emberAfPluginSleepyMessageQueueTimeoutEvent)
void emberAfPluginSleepyMessageQueueTimeoutEventHandler(SLXU_UC_EVENT);
#else
EmberEventControl emberAfPluginSleepyMessageQueueTimeoutEventControl;

// Use a shorter name to make the code more readable
#define msgTimeoutEvent emberAfPluginSleepyMessageQueueTimeoutEventControl
#endif
//------------------------------------------------------------------------------
// Internal Prototypes & Structure Definitions

// Internal supporting functions
static void  emSleepyMessageQueueInitEntry(uint8_t index);
static void  emRestartMessageTimer(void);
static uint8_t emGetFirstUnusedQueueIndex(void);

//static void emRefreshMessageTimeouts( void );

typedef struct {
  EmberAfSleepyMessage sleepyMsg;
  uint8_t  status;
  uint32_t timeoutMSec;
} emSleepyMessage;

#define SLEEPY_MSG_QUEUE_NUM_ENTRIES  EMBER_AF_PLUGIN_SLEEPY_MESSAGE_QUEUE_SLEEPY_QUEUE_SIZE
#define MAX_MESSAGE_TIMEOUT_SEC  (0x7fffffff >> 10)

emSleepyMessage SleepyMessageQueue[SLEEPY_MSG_QUEUE_NUM_ENTRIES];

enum {
  SLEEPY_MSG_QUEUE_STATUS_UNUSED = 0x00,
  SLEEPY_MSG_QUEUE_STATUS_USED   = 0x01,
};

//------------------------------------------------------------------------------

#ifdef UC_BUILD
void emberAfPluginSleepyMessageQueueInitCallback(uint8_t init_level)
{
  switch (init_level) {
    case SL_ZIGBEE_INIT_LEVEL_EVENT:
    {
      slxu_zigbee_event_init(&emberAfPluginSleepyMessageQueueTimeoutEvent,
                             emberAfPluginSleepyMessageQueueTimeoutEventHandler);
      break;
    }
    case SL_ZIGBEE_INIT_LEVEL_LOCAL_DATA:
    {
      // Initialize sleepy buffer plugin.
      uint8_t x;
      for ( x = 0; x < SLEEPY_MSG_QUEUE_NUM_ENTRIES; x++ ) {
        emSleepyMessageQueueInitEntry(x);
      }
      emberAfAppPrintln("Initialized Sleepy Message Queue");
      break;
    }
  }
}
#else // !UC_BUILD
void emberAfPluginSleepyMessageQueueInitCallback(void)
{
  slxu_zigbee_event_init(msgTimeoutEvent,
                         emberAfPluginSleepyMessageQueueTimeoutEventHandler);
  // Initialize sleepy buffer plugin.
  uint8_t x;
  for ( x = 0; x < SLEEPY_MSG_QUEUE_NUM_ENTRIES; x++ ) {
    emSleepyMessageQueueInitEntry(x);
  }
  emberAfAppPrintln("Initialized Sleepy Message Queue");
}
#endif  // UC_BUILD

static void emSleepyMessageQueueInitEntry(uint8_t x)
{
  if ( x < SLEEPY_MSG_QUEUE_NUM_ENTRIES ) {
    SleepyMessageQueue[x].sleepyMsg.payload = NULL;
    SleepyMessageQueue[x].sleepyMsg.length = 0;
    SleepyMessageQueue[x].sleepyMsg.payloadId = 0;
    MEMSET(SleepyMessageQueue[x].sleepyMsg.dstEui64, 0, EUI64_SIZE);
    SleepyMessageQueue[x].status = SLEEPY_MSG_QUEUE_STATUS_UNUSED;
    SleepyMessageQueue[x].timeoutMSec = 0;
  }
}

uint8_t emberAfPluginSleepyMessageQueueGetNumUnusedEntries()
{
  uint8_t x;
  uint8_t cnt = 0;
  for ( x = 0; x < SLEEPY_MSG_QUEUE_NUM_ENTRIES; x++ ) {
    if ( SleepyMessageQueue[x].status == SLEEPY_MSG_QUEUE_STATUS_UNUSED ) {
      cnt++;
    }
  }
  return cnt;
}

static uint8_t emGetFirstUnusedQueueIndex()
{
  uint8_t x;
  for ( x = 0; x < SLEEPY_MSG_QUEUE_NUM_ENTRIES; x++ ) {
    if ( SleepyMessageQueue[x].status == SLEEPY_MSG_QUEUE_STATUS_UNUSED ) {
      break;
    }
  }
  if ( x >= SLEEPY_MSG_QUEUE_NUM_ENTRIES ) {
    x = EMBER_AF_PLUGIN_SLEEPY_MESSAGE_INVALID_ID;
  }
  return x;
}

#define MAX_DELAY_QS  0x7FFF
#define MAX_DELAY_MS  0x7FFF
static void emRestartMessageTimer()
{
  uint32_t smallestTimeoutIndex = SLEEPY_MSG_QUEUE_NUM_ENTRIES;
  uint32_t smallestTimeoutMSec = 0xFFFFFFFF;
  uint32_t timeNowMs;
  uint32_t remainingMs;
  uint32_t delayQs;
  uint8_t x;

  timeNowMs = halCommonGetInt32uMillisecondTick();

  for ( x = 0; x < SLEEPY_MSG_QUEUE_NUM_ENTRIES; x++ ) {
    if ( SleepyMessageQueue[x].status == SLEEPY_MSG_QUEUE_STATUS_USED ) {
      if ( timeGTorEqualInt32u(timeNowMs, SleepyMessageQueue[x].timeoutMSec) ) {
        // Timeout already expired - break out of loop - process immediately.
        smallestTimeoutIndex = x;
        smallestTimeoutMSec = 0;
        break;
      } else {
        remainingMs = elapsedTimeInt32u(timeNowMs, SleepyMessageQueue[x].timeoutMSec);
        if ( remainingMs < smallestTimeoutMSec ) {
          smallestTimeoutMSec = remainingMs;
          smallestTimeoutIndex = x;
        }
      }
    }
  }
  // Now know the smallest timeout index, and the smallest timeout value.
  if ( smallestTimeoutIndex < SLEEPY_MSG_QUEUE_NUM_ENTRIES ) {
    // Run the actual timer as a QS timer since that allows us to delay longer
    // than a u16 MS timer would.
    delayQs = smallestTimeoutMSec >> 8;
    delayQs++;    // Round up to the next quarter second tick.
    if ( delayQs > MAX_DELAY_QS ) {
      delayQs = MAX_DELAY_QS;
    }

    slxu_zigbee_event_set_delay_qs(msgTimeoutEvent, delayQs);
    emberAfAppPrintln("Restarting sleepy message timer for %d Qsec.", delayQs);
  }
}

EmberAfSleepyMessageId emberAfPluginSleepyMessageQueueStoreMessage(EmberAfSleepyMessage *pmsg, uint32_t timeoutSec)
{
  uint8_t x;

  if ( timeoutSec <= MAX_MESSAGE_TIMEOUT_SEC ) {
    x = emGetFirstUnusedQueueIndex();
    if ( x < SLEEPY_MSG_QUEUE_NUM_ENTRIES ) {
      MEMCOPY( (uint8_t *)&SleepyMessageQueue[x].sleepyMsg, (uint8_t *)pmsg, sizeof(EmberAfSleepyMessage) );
      SleepyMessageQueue[x].status = SLEEPY_MSG_QUEUE_STATUS_USED;
      SleepyMessageQueue[x].timeoutMSec = (timeoutSec << 10) + halCommonGetInt32uMillisecondTick();

      // Reschedule timer after adding the new message since new message could have the shortest timeout.
      emRestartMessageTimer();
    }
    return x;
  }
  return EMBER_AF_PLUGIN_SLEEPY_MESSAGE_INVALID_ID;
}

EmberAfSleepyMessageId emberAfPluginSleepyMessageQueueGetPendingMessageId(EmberEUI64 dstEui64)
{
  uint8_t  smallestTimeoutIndex = SLEEPY_MSG_QUEUE_NUM_ENTRIES;
  uint32_t smallestTimeoutMSec = 0xFFFFFFFF;
  uint32_t timeNowMs;
  uint32_t remainingMs;
  uint8_t  x;
  uint8_t  stat;

  timeNowMs = halCommonGetInt32uMillisecondTick();
  for ( x = 0; x < SLEEPY_MSG_QUEUE_NUM_ENTRIES; x++ ) {
    if ( SleepyMessageQueue[x].status == SLEEPY_MSG_QUEUE_STATUS_USED ) {
      stat = MEMCOMPARE(SleepyMessageQueue[x].sleepyMsg.dstEui64, dstEui64, EUI64_SIZE);
      if ( !stat ) {
        // Matching entry found - look for match with smallest timeout.
        if ( timeGTorEqualInt32u(timeNowMs, SleepyMessageQueue[x].timeoutMSec) ) {
          // Timeout already expired - break out of loop - process immediately.
          smallestTimeoutIndex = x;
          smallestTimeoutMSec = 0;
          break;
        } else {
          remainingMs = elapsedTimeInt32u(timeNowMs, SleepyMessageQueue[x].timeoutMSec);
          if ( remainingMs < smallestTimeoutMSec ) {
            smallestTimeoutMSec = remainingMs;
            smallestTimeoutIndex = x;
          }
        }
      }
    }
  }
  if ( smallestTimeoutIndex >= SLEEPY_MSG_QUEUE_NUM_ENTRIES ) {
    smallestTimeoutIndex = EMBER_AF_PLUGIN_SLEEPY_MESSAGE_INVALID_ID;
  }
  return smallestTimeoutIndex;
}

uint32_t emMessageMSecRemaining(EmberAfSleepyMessageId sleepyMsgId)
{
  uint32_t remainingMs = 0xFFFFFFFF;
  uint32_t timeNowMs = halCommonGetInt32uMillisecondTick();

  if ( (sleepyMsgId < SLEEPY_MSG_QUEUE_NUM_ENTRIES) && (SleepyMessageQueue[sleepyMsgId].status == SLEEPY_MSG_QUEUE_STATUS_USED) ) {
    if ( timeGTorEqualInt32u(timeNowMs, SleepyMessageQueue[sleepyMsgId].timeoutMSec) ) {
      remainingMs = 0;
    } else {
      remainingMs = elapsedTimeInt32u(timeNowMs, SleepyMessageQueue[sleepyMsgId].timeoutMSec);
    }
  }
  return remainingMs;
}

#define PRINT_TIME(x)  PrintMessageTimeInfo(x)
void PrintMessageTimeInfo(uint8_t x)
{
  if ( (x < SLEEPY_MSG_QUEUE_NUM_ENTRIES) && (SleepyMessageQueue[x].status == SLEEPY_MSG_QUEUE_STATUS_USED) ) {
    emberAfAppPrintln("==== USED MESSAGE, x=%d", x);
    emberAfAppPrintln("  currTime=%d, timeout=%d", halCommonGetInt32uMillisecondTick(), SleepyMessageQueue[x].timeoutMSec);
  }
}

bool emberAfPluginSleepyMessageQueueGetPendingMessage(EmberAfSleepyMessageId sleepyMsgId, EmberAfSleepyMessage *pmsg)
{
  if ( (sleepyMsgId < SLEEPY_MSG_QUEUE_NUM_ENTRIES) && (SleepyMessageQueue[sleepyMsgId].status == SLEEPY_MSG_QUEUE_STATUS_USED) ) {
    MEMCOPY( (uint8_t *)pmsg, (uint8_t *)&SleepyMessageQueue[sleepyMsgId].sleepyMsg, sizeof(EmberAfSleepyMessage) );
    return true;
  }
  return false;
}

uint8_t emberAfPluginSleepyMessageQueueGetNumMessages(EmberEUI64 dstEui64)
{
  uint8_t x;
  uint8_t stat;
  uint8_t matchCnt = 0;

  for ( x = 0; x < SLEEPY_MSG_QUEUE_NUM_ENTRIES; x++ ) {
    if ( SleepyMessageQueue[x].status == SLEEPY_MSG_QUEUE_STATUS_USED ) {
      stat = MEMCOMPARE(SleepyMessageQueue[x].sleepyMsg.dstEui64, dstEui64, EUI64_SIZE);
      if ( !stat ) {
        matchCnt++;
      }
    }
  }
  return matchCnt;
}

bool emberAfPluginSleepyMessageQueueRemoveMessage(EmberAfSleepyMessageId  sleepyMsgId)
{
  if ( sleepyMsgId < SLEEPY_MSG_QUEUE_NUM_ENTRIES ) {
    emSleepyMessageQueueInitEntry(sleepyMsgId);
    emRestartMessageTimer();    // Restart the message timer
    return true;
  }
  return false;
}

void emberAfPluginSleepyMessageQueueRemoveAllMessages(EmberEUI64 dstEui64)
{
  uint8_t x;
  uint8_t stat;
  uint8_t matchCnt = 0;

  for ( x = 0; x < SLEEPY_MSG_QUEUE_NUM_ENTRIES; x++ ) {
    if ( SleepyMessageQueue[x].status == SLEEPY_MSG_QUEUE_STATUS_USED ) {
      stat = MEMCOMPARE(SleepyMessageQueue[x].sleepyMsg.dstEui64, dstEui64, EUI64_SIZE);
      if ( !stat ) {
        emSleepyMessageQueueInitEntry(x);
        matchCnt++;
      }
    }
  }
  if ( matchCnt ) {
    // At least one entry was removed.  Restart the message timer.
    emRestartMessageTimer();
  }
}

void emberAfPluginSleepyMessageQueueTimeoutEventHandler(SLXU_UC_EVENT)
{
  uint32_t timeNowMs;
  uint8_t x;

  slxu_zigbee_event_set_inactive(msgTimeoutEvent);
  timeNowMs = halCommonGetInt32uMillisecondTick();

  for ( x = 0; x < SLEEPY_MSG_QUEUE_NUM_ENTRIES; x++ ) {
    if ( SleepyMessageQueue[x].status == SLEEPY_MSG_QUEUE_STATUS_USED ) {
      if ( timeGTorEqualInt32u(timeNowMs, SleepyMessageQueue[x].timeoutMSec) ) {
        // Found entry that expired.
        // Invoke callback to notify about message timeout.
        // Allow the callback the opportunity to access elements in this entry.
        emberAfAppPrintln("Expiring Message %d", x);
        emberAfPluginSleepyMessageQueueMessageTimedOutCallback(x);

        // After timeout callback completes, mark event as unused.
        emSleepyMessageQueueInitEntry(x);
      }
    }
  }

  // Restart timer if another message is buffered.
  emRestartMessageTimer();
}
