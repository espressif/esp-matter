/***************************************************************************//**
 * @file
 * @brief Implementation of the interface described in serial-interface.h
 * using the ASH UART protocol.
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

#include PLATFORM_HEADER

#include "stack/include/ember-types.h"

#include "hal/hal.h"
#include "app/util/ezsp/ezsp-protocol.h"
#include "app/util/ezsp/ezsp.h"
#include "app/util/ezsp/serial-interface.h"
#include "ash-protocol.h"
#include "ash-common.h"
#include "app/ezsp-host/ash/ash-host.h"
#include "app/ezsp-host/ezsp-host-io.h"
#include "app/ezsp-host/ezsp-host-priv.h"
#include "app/ezsp-host/ezsp-host-queues.h"
#include "app/util/ezsp/ezsp-frame-utilities.h"

#define elapsedTimeInt16u(oldTime, newTime) \
  ((uint16_t) ((uint16_t)(newTime) - (uint16_t)(oldTime)))

//------------------------------------------------------------------------------
// Global Variables

static bool waitingForResponse = false;
static uint16_t waitStartTime;
#define WAIT_FOR_RESPONSE_TIMEOUT (ASH_MAX_TIMEOUTS * ashReadConfig(ackTimeMax))

static uint8_t ezspFrameLength;
uint8_t *ezspFrameLengthLocation = &ezspFrameLength;
static uint8_t ezspFrameContentsStorage[EZSP_MAX_FRAME_LENGTH];
uint8_t *ezspFrameContents = ezspFrameContentsStorage;

//------------------------------------------------------------------------------
// Serial Interface Downwards

EzspStatus ezspInit(void)
{
  EzspStatus status;
  uint8_t i;
  for (i = 0; i < 5; i++) {
    status = ashResetNcp();
    if (status != EZSP_SUCCESS) {
      return status;
    }
    status = ashStart();
    if (status == EZSP_SUCCESS) {
      return status;
    }
  }
  return status;
}

bool ezspCallbackPending(void)
{
  return (ncpSleepEnabled && ncpHasCallbacks);
}

void ezspClose(void)
{
  ezspSerialClose();
}

static bool checkConnection(void)
{
  bool connected = ashIsConnected();
  if (!connected) {
    // Attempt to restore the connection. This will reset the EM260.
    ezspClose();
    ezspInit();
  }

  return connected;
}

uint8_t serialPendingResponseCount(void)
{
  return ezspQueueLength(&rxQueue);
}

WEAK_TEST EzspStatus serialResponseReceived(void)
{
  EzspStatus status;
  EzspBuffer *buffer;
  EzspBuffer *dropBuffer = NULL;
  if (!checkConnection()) {
    ezspTraceEzspVerbose("serialResponseReceived(): EZSP_NOT_CONNECTED");
    return EZSP_NOT_CONNECTED;
  }
  ashSendExec();
  status = ashReceiveExec();
  if (status != EZSP_SUCCESS
      && status != EZSP_ASH_IN_PROGRESS
      && status != EZSP_NO_RX_DATA) {
    ezspTraceEzspVerbose("serialResponseReceived(): ashReceiveExec(): 0x%x",
                         status);
    return status;
  }
  if (waitingForResponse
      && elapsedTimeInt16u(waitStartTime, halCommonGetInt16uMillisecondTick())
      > WAIT_FOR_RESPONSE_TIMEOUT) {
    waitingForResponse = false;
    ezspTraceEzspFrameId("no response", ezspFrameContents);
    ezspTraceEzspVerbose("serialResponseReceived(): EZSP_ERROR_NO_RESPONSE");
    return EZSP_ERROR_NO_RESPONSE;
  }
  status = EZSP_NO_RX_DATA;
  buffer = ezspQueuePrecedingEntry(&rxQueue, NULL);
  while (buffer != NULL) {
    // While we are waiting for a response to a command, we use the asynch
    // callback flag to ignore asynchronous callbacks. This allows our caller
    // to assume that no callbacks will appear between sending a command and
    // receiving its response.
    if (waitingForResponse
        && (buffer->data[EZSP_FRAME_CONTROL_INDEX]
            & EZSP_FRAME_CONTROL_ASYNCH_CB)
        ) {
      if (ezspFreeListLength(&rxFree) == 0) {
        dropBuffer = buffer;
      }
      buffer = ezspQueuePrecedingEntry(&rxQueue, buffer);
    } else {
      ezspTraceEzspVerbose("serialResponseReceived(): ID=0x%x Seq=0x%x Buffer=%u",
                           buffer->data[EZSP_FRAME_ID_INDEX],
                           buffer->data[EZSP_SEQUENCE_INDEX],
                           buffer);
      ezspRemoveQueueEntry(&rxQueue, buffer);
      (void) memcpy(ezspFrameContents, buffer->data, buffer->len);
      ezspTraceEzspFrameId("got response", buffer->data);
      ezspFrameLength = buffer->len;
      ezspFreeBuffer(&rxFree, buffer);
      ezspTraceEzspVerbose("serialResponseReceived(): ezspFreeBuffer(): %u", buffer);
      buffer = NULL;
      status = EZSP_SUCCESS;
      waitingForResponse = false;
    }
  }
  if (dropBuffer != NULL) {
    ezspRemoveQueueEntry(&rxQueue, dropBuffer);
    ezspFreeBuffer(&rxFree, dropBuffer);
    ezspTraceEzspFrameId("dropping", dropBuffer->data);
    ezspTraceEzspVerbose("serialResponseReceived(): ezspFreeBuffer(): drop %u", dropBuffer);
    ezspTraceEzspVerbose("serialResponseReceived(): ezspErrorHandler(): EZSP_ERROR_QUEUE_FULL");
    ezspErrorHandler(EZSP_ERROR_QUEUE_FULL);
  }
  return status;
}

WEAK_TEST EzspStatus serialSendCommand(void)
{
  EzspStatus status;
  if (!checkConnection()) {
    ezspTraceEzspVerbose("serialSendCommand(): EZSP_NOT_CONNECTED");
    return EZSP_NOT_CONNECTED;
  }

  ezspTraceEzspFrameId("send command", ezspFrameContents);
  status = ashSend(ezspFrameLength, ezspFrameContents);
  if (status != EZSP_SUCCESS) {
    ezspTraceEzspVerbose("serialSendCommand(): ashSend(): 0x%x", status);
    return status;
  }
  waitingForResponse = true;
  ezspTraceEzspVerbose("serialSendCommand(): ID=0x%x Seq=0x%x",
                       ezspFrameContents[EZSP_FRAME_ID_INDEX],
                       ezspFrameContents[EZSP_SEQUENCE_INDEX]);
  waitStartTime = halCommonGetInt16uMillisecondTick();
  return status;
}

uint8_t serialGetCommandLength(void)
{
  return *ezspFrameLengthLocation;
}

bool ezspOkToSleep(void)
{
  return
    (ncpSleepEnabled
     && (ezspSleepMode != EZSP_FRAME_CONTROL_IDLE)
     && ashOkToSleep() );
}

void ezspEnableNcpSleep(bool enable)
{
  uint8_t enableval = (uint8_t) enable;
  ezspSetValue(EZSP_VALUE_UART_SYNCH_CALLBACKS, 1, &enableval);
  ncpSleepEnabled = enable;
}

void ezspWakeUp(void)
{
  EzspStatus status = ashWakeUpNcp(true);
  while (status == EZSP_ASH_IN_PROGRESS) {
    simulatedTimePasses();
    status = ashWakeUpNcp(false);
  }
}

// Offset from start of ezspEcho frame to data payload - skip over length byte
#define ECHO_DATA_OFFSET      (EZSP_EXTENDED_PARAMETERS_INDEX + 1)
// Maximum length of ezspEcho data
#define ECHO_MAX_DATA_LENGTH  (EZSP_MAX_FRAME_LENGTH - ECHO_DATA_OFFSET)

EzspStatus serialTestFlowControl(void)
{
  uint8_t buffer[ECHO_MAX_DATA_LENGTH];
  uint8_t rnum;

  // Construct the largest possible echo command frame, in which every data byte
  // is preceded by an escape byte, by setting all data to the ASH_FLAG value.
  // If data randomization is enabled, the data array must be XOR'ed with the
  // ASH pseudo-random sequence. Note that the random sequence must be cycled
  // enough times to skip over the preceding bytes in the EZSP frame.
  rnum = ashRandomizeArray(0, buffer, ECHO_DATA_OFFSET);
  (void) memset(buffer, ASH_FLAG, ECHO_MAX_DATA_LENGTH);
  if (ashReadConfig(randomize)) {
    (void)ashRandomizeArray(rnum, buffer, ECHO_MAX_DATA_LENGTH);
  }

  // Use delay command to have the NCP pause before reading the next command,
  // then see if the frame had to be retransmitted due to buffer overflow.
  ezspDelayTest(100);   // wait 100 milliseconds before reading next command
  ashCount.txReDataFrames = 0;
  (void)ezspEcho(ECHO_MAX_DATA_LENGTH, buffer, buffer);
  ezspTick();
  return (ashCount.txReDataFrames == 0) ? EZSP_SUCCESS
         : EZSP_HOST_FATAL_ERROR;
}
