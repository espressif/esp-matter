/***************************************************************************//**
 * @file
 * @brief  ASH protocol Host functions
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
#include "ash-protocol.h"
#include "ash-common.h"
#include "app/ezsp-host/ash/ash-host.h"
#include "app/ezsp-host/ezsp-host-io.h"
#include "app/ezsp-host/ezsp-host-priv.h"
#include "app/ezsp-host/ezsp-host-queues.h"
#include "app/ezsp-host/ezsp-host-ui.h"
#include "app/ezsp-host/ash/ash-host-ui.h"
#include "app/util/ezsp/serial-interface.h"

//------------------------------------------------------------------------------
// Preprocessor definitions

// Values for sendState
#define SEND_STATE_IDLE       0
#define SEND_STATE_SHFRAME    1
#define SEND_STATE_TX_DATA    2
#define SEND_STATE_RETX_DATA  3

// Bits in ashFlags
#define FLG_REJ               0x01          // Reject Condition
#define FLG_RETX              0x02          // Retransmit Condition
#define FLG_NAK               0x04          // send NAK
#define FLG_ACK               0x08          // send ACK
#define FLG_RST               0x10          // send RST
#define FLG_CAN               0x20          // send immediate CAN
#define FLG_CONNECTED         0x40          // in CONNECTED state, else ERROR
#define FLG_NR               0x100          // not ready to receive DATA frames
#define FLG_NRTX             0x200          // last transmitted NR status

// Values returned by ashFrameType()
#define TYPE_INVALID          0
#define TYPE_DATA             1
#define TYPE_ACK              2
#define TYPE_NAK              3
#define TYPE_RSTACK           4
#define TYPE_ERROR            5

#define txControl (txBuffer[0])             // more descriptive aliases
#define rxControl (rxBuffer[0])
#define TX_BUFFER_LEN ASH_HOST_SHFRAME_TX_LEN
#define RX_BUFFER_LEN ASH_HOST_SHFRAME_RX_LEN

//------------------------------------------------------------------------------
// Global Variables

EzspStatus ncpError;                        // ncp error or reset code
EzspStatus hostError;                        // host error code

AshCount ashCount;                          // struct of ASH counters

bool ncpSleepEnabled;                    // ncp is enabled to sleep

// Config 0 (default) : 115200 bps with RTS/CTS flow control
#define ASH_HOST_CONFIG_DEFAULT                                                   \
  {                                                                               \
    "/dev/ttyS0",       /* serial port name                                  */   \
    115200,             /* baud rate (bits/second)                           */   \
    1,                  /* stop bits                                         */   \
    true,               /* true enables RTS/CTS flow control, false XON/XOFF */   \
    256,                /* max bytes to buffer before writing to serial port */   \
    256,                /* max bytes to read ahead from serial port          */   \
    0,                  /* trace output control bit flags                    */   \
    3,                  /* max frames sent without being ACKed (1-7)         */   \
    true,               /* enables randomizing DATA frame payloads           */   \
    800,                /* adaptive rec'd ACK timeout initial value          */   \
    400,                /*  "     "     "     "     "  minimum value         */   \
    2400,               /*  "     "     "     "     "  maximum value         */   \
    2500,               /* time allowed to receive RSTACK after ncp is reset */   \
    RX_FREE_LWM,        /* if free buffers < limit, host receiver isn't ready */  \
    RX_FREE_HWM,        /* if free buffers > limit, host receiver is ready   */   \
    480,                /* time until a set nFlag must be resent (max 2032)  */   \
    ASH_RESET_METHOD_RST, /* method used to reset ncp                          */ \
  }

// Host configuration structure
AshHostConfig ashHostConfig = ASH_HOST_CONFIG_DEFAULT;

//------------------------------------------------------------------------------
// Local Variables

static AshHostConfig ashHostConfigArray[] =
{
  // Config 0: defined above
  ASH_HOST_CONFIG_DEFAULT,

  { // Config 1: 57600 bps with XON/XOFF flow control
    "/dev/ttyS0",         // serial port name
    57600,                // baud rate (bits/second)
    1,                    // stop bits
    false,                // true enables RTS/CTS flow control, false XON/XOFF
    256,                  // max bytes to buffer before writing to serial port
    256,                  // max bytes to read ahead from serial port
    0,                    // trace output control bit flags
    3,                    // max frames sent without being ACKed (1-7)
    true,                 // enables randomizing DATA frame payloads
    800,                  // adaptive rec'd ACK timeout initial value
    400,                  //  "     "     "     "     "  minimum value
    2400,                 //  "     "     "     "     "  maximum value
    2500,                 // time allowed to receive RSTACK after ncp is reset
    RX_FREE_LWM,          // if free buffers < limit, host receiver isn't ready
    RX_FREE_HWM,          // if free buffers > limit, host receiver is ready
    480,                  // time until a set nFlag must be resent (max 2032)
    ASH_RESET_METHOD_RST  // method used to reset ncp
  },
};

static uint8_t txBuffer[TX_BUFFER_LEN];       // outgoing short frames
static uint8_t rxBuffer[RX_BUFFER_LEN];       // incoming short frames
static uint8_t sendState;                     // ashSendExec() state variable
static uint8_t ackRx;                         // frame ack'ed from remote peer
static uint8_t ackTx;                         // frame ack'ed to remote peer
static uint8_t frmTx;                         // next frame to be transmitted
static uint8_t frmReTx;                       // next frame to be retransmitted
static uint8_t frmRx;                         // next frame expected to be rec'd
static uint8_t frmReTxHead;                   // frame at retx queue's head
static uint8_t ashTimeouts;                   // consecutive timeout counter
static uint16_t ashFlags;                     // bit flags for top-level logic
static EzspBuffer *rxDataBuffer;             // rec'd DATA frame buffer
static uint8_t rxLen;                         // rec'd frame length

//------------------------------------------------------------------------------
// Forward Declarations

static EzspStatus ashReceiveFrame(void);
static uint16_t ashFrameType(uint16_t control, uint16_t len);
static void ashRejectFrame(void);
static void ashFreeNonNullRxBuffer(void);
static void ashScrubReTxQueue(void);
static void ashStartRetransmission(void);
static void ashInitVariables(void);
static void ashDataFrameFlowControl(void);
static EzspStatus ashHostDisconnect(uint8_t error);
static EzspStatus ashNcpDisconnect(uint8_t error);
static EzspStatus ashReadFrame(void);
static void ashRandomizeBuffer(uint8_t *buffer, uint8_t len);

//------------------------------------------------------------------------------
// Functions implementing the interface upward to EZSP

EzspStatus ashSelectHostConfig(uint8_t cfg)
{
  uint8_t status;

  if (cfg < (sizeof(ashHostConfigArray) / sizeof(ashHostConfigArray[0])) ) {
    ashHostConfig = ashHostConfigArray[cfg];
    status = EZSP_SUCCESS;
  } else {
    hostError = EZSP_ASH_ERROR_NCP_TYPE;
    status = EZSP_HOST_FATAL_ERROR;
  }
  return status;
}

EzspStatus ashResetNcp(void)
{
  EzspStatus status;

  ashInitVariables();
  ezspTraceEvent("\r\n=== ASH started ===\r\n");
  switch (ashReadConfig(resetMethod)) {
    case ASH_RESET_METHOD_RST:  // ask ncp to reset itself using RST frame
      status = ezspSerialInit();
      ashFlags = FLG_RST | FLG_CAN;
      break;
    case ASH_RESET_METHOD_DTR:  // DTR is connected to nRESET
      ezspResetDtr();
      status = ezspSerialInit();
      break;
    case ASH_RESET_METHOD_CUSTOM: // a hook for a custom reset method
      ezspResetCustom();
      status = ezspSerialInit();
      break;
    case ASH_RESET_METHOD_NONE: // no reset - for testing
      status = ezspSerialInit();
      break;
    default:
      hostError = EZSP_ASH_ERROR_RESET_METHOD;
      status = EZSP_HOST_FATAL_ERROR;
      break;
  } // end of switch(ashReadConfig(resetMethod)
  return status;
}

EzspStatus ashStart(void)
{
  EzspStatus status;
  if (ashReadConfig(resetMethod) != ASH_RESET_METHOD_NONE) {
    ezspSerialReadFlush();
  }
  ashSetAndStartAckTimer(ashReadConfig(timeRst));
  while (!(ashFlags & FLG_CONNECTED)) {
    ashSendExec();
    status = ashReceiveExec();
    if (hostError == EZSP_ASH_ERROR_RESET_FAIL) {
      status = EZSP_HOST_FATAL_ERROR;
    }
    if (  (status == EZSP_HOST_FATAL_ERROR)
          || (status == EZSP_ASH_NCP_FATAL_ERROR) ) {
      return status;
    }
    simulatedTimePasses();
  }
  ashStopAckTimer();
  return EZSP_SUCCESS;
}

void ashStop(void)
{
  ezspTraceEvent("======== ASH stopped ========\r\n");
  ezspSerialClose();
  ashInitVariables();
}

EzspStatus ashReceiveExec(void)
{
  EzspStatus status;

  do {
    status = ashReceiveFrame();
    simulatedTimePasses();
  } while (status == EZSP_ASH_IN_PROGRESS);
  return status;
}

static EzspStatus ashReceiveFrame(void)
{
  uint8_t ackNum;
  uint8_t frmNum;
  uint16_t frameType;
  EzspStatus status;

  // Check for errors that might have been detected in ashSendExec()
  if (hostError != EZSP_NO_ERROR) {
    return EZSP_HOST_FATAL_ERROR;
  }
  if (ncpError != EZSP_NO_ERROR) {
    return EZSP_ASH_NCP_FATAL_ERROR;
  }

  // Read data from serial port and assemble a frame until complete, aborted
  // due to an error, cancelled, or there is no more serial data available.
  do {
    status = ashReadFrame();
    switch (status) {
      case EZSP_SUCCESS:
        break;
      case EZSP_ASH_IN_PROGRESS:
        break;
      case EZSP_NO_RX_DATA:
        return status;
      case EZSP_ASH_CANCELLED:
        if (ashFlags & FLG_CONNECTED) {     // ignore the cancel before RSTACK
          BUMP_HOST_COUNTER(rxCancelled);
          ashTraceEventRecdFrame("cancelled");
        }
        break;
      case EZSP_ASH_BAD_CRC:
        BUMP_HOST_COUNTER(rxCrcErrors);
        ashRejectFrame();
        ashTraceEventRecdFrame("CRC error");
        break;
      case EZSP_ASH_COMM_ERROR:
        BUMP_HOST_COUNTER(rxCommErrors);
        ashRejectFrame();
        ashTraceEventRecdFrame("comm error");
        break;
      case EZSP_ASH_TOO_SHORT:
        BUMP_HOST_COUNTER(rxTooShort);
        ashRejectFrame();
        ashTraceEventRecdFrame("too short");
        break;
      case EZSP_ASH_TOO_LONG:
        BUMP_HOST_COUNTER(rxTooLong);
        ashRejectFrame();
        ashTraceEventRecdFrame("too long");
        break;
      case EZSP_ASH_ERROR_XON_XOFF:
        return ashHostDisconnect(status);
      default:
        assert(false);
    } // end of switch (status)
  } while (status != EZSP_SUCCESS);

  // Got a complete frame - validate its control and length.
  // On an error the type returned will be TYPE_INVALID.
  frameType = ashFrameType(rxControl, rxLen);

  // Free buffer allocated for a received frame if:
  //    DATA frame, and out of order
  //    DATA frame, and not in the CONNECTED state
  //    not a DATA frame
  if (frameType == TYPE_DATA) {
    if ( !(ashFlags & FLG_CONNECTED) || (ASH_GET_FRMNUM(rxControl) != frmRx) ) {
      ashFreeNonNullRxBuffer();
    }
  } else {
    ashFreeNonNullRxBuffer();
  }
  ashTraceFrame(false);                       // trace output (if enabled)

  // Process frames received while not in the connected state -
  // ignore everything except RSTACK and ERROR frames
  if (!(ashFlags & FLG_CONNECTED)) {
    // RSTACK frames have the ncp ASH version in the first data field byte,
    // and the reset reason in the second byte
    if (frameType == TYPE_RSTACK) {
      if (rxBuffer[1] != ASH_VERSION) {
        return ashHostDisconnect(EZSP_ASH_ERROR_VERSION);
      }
      // Ignore a RSTACK if the reset reason doesn't match our reset method
      switch (ashReadConfig(resetMethod)) {
        case ASH_RESET_METHOD_RST:
          if (rxBuffer[2] != EM2XX_RESET_SOFTWARE) {
            return EZSP_ASH_IN_PROGRESS;
          }
          break;
        // Note that an EM2xx reports resets from nRESET as power on resets
        case ASH_RESET_METHOD_DTR:
        case ASH_RESET_METHOD_CUSTOM:
        case ASH_RESET_METHOD_NONE:
          if ( (rxBuffer[2] != EM2XX_RESET_EXTERNAL)
               && (rxBuffer[2] != EM2XX_RESET_POWERON) ) {
            return EZSP_ASH_IN_PROGRESS;
          }
          break;
      }
      ncpError = EZSP_NO_ERROR;
      ashStopAckTimer();
      ashTimeouts = 0;
      ashSetAckPeriod(ashReadConfig(ackTimeInit));
      ashFlags = FLG_CONNECTED | FLG_ACK;
      ashTraceEventTime("ASH connected");
    } else if (frameType == TYPE_ERROR) {
      return ashNcpDisconnect(rxBuffer[2]);
    }
    return EZSP_ASH_IN_PROGRESS;
  }

  // Connected - process the ackNum in ACK, NAK and DATA frames
  if (  (frameType == TYPE_DATA)
        || (frameType == TYPE_ACK)
        || (frameType == TYPE_NAK) ) {
    ackNum = ASH_GET_ACKNUM(rxControl);
    if ( !WITHIN_RANGE(ackRx, ackNum, frmTx) ) {
      BUMP_HOST_COUNTER(rxBadAckNumber);
      ezspTraceEvent("bad ackNum");
      frameType = TYPE_INVALID;
    } else if (ackNum != ackRx) {               // new frame(s) ACK'ed?
      ackRx = ackNum;
      ashTimeouts = 0;
      if (ashFlags & FLG_RETX) {                // start timer if unACK'ed frames
        ashStopAckTimer();
        if (ackNum != frmReTx) {
          ashStartAckTimer();
        }
      } else {
        ashAdjustAckPeriod(false);              // factor ACK time into period
        if (ackNum != frmTx) {                  // if more unACK'ed frames,
          ashStartAckTimer();                   // then restart ACK timer
        }
        ashScrubReTxQueue();                    // free buffer(s) in ReTx queue
      }
    }
  }

  // Process frames received while connected
  switch (frameType) {
    case TYPE_DATA:
      frmNum = ASH_GET_FRMNUM(rxControl);
      if (frmNum == frmRx) {                  // is frame in sequence?
        if (rxDataBuffer == NULL) {           // valid frame but no memory?
          BUMP_HOST_COUNTER(rxNoBuffer);
          ashTraceEventRecdFrame("no buffer available");
          ashRejectFrame();
          return EZSP_NO_RX_SPACE;
        }
        if (rxControl & ASH_RFLAG_MASK) {     // if retransmitted, force ACK
          ashFlags |= FLG_ACK;
        }
        ashFlags &= ~(FLG_REJ | FLG_NAK);     // clear the REJ condition
        INC8(frmRx);
        ashRandomizeBuffer(rxDataBuffer->data, rxDataBuffer->len);
        ezspAddQueueTail(&rxQueue, rxDataBuffer);// add frame to receive queue
        ezspTraceEzspFrameId("add to queue", rxDataBuffer->data);
        ezspTraceEzspVerbose("ashReceiveFrame(): ID=0x%x Seq=0x%x Buffer=%u",
                             rxDataBuffer->data[EZSP_FRAME_ID_INDEX],
                             rxDataBuffer->data[EZSP_SEQUENCE_INDEX],
                             rxDataBuffer);
        ADD_HOST_COUNTER(rxDataBuffer->len, rxData);
        return EZSP_SUCCESS;
      } else {                                // frame is out of sequence
        if (rxControl & ASH_RFLAG_MASK) {     // if retransmitted, force ACK
          BUMP_HOST_COUNTER(rxDuplicates);
          ashFlags |= FLG_ACK;
        } else {                              // 1st OOS? then set REJ, send NAK
          if ((ashFlags & FLG_REJ) == 0) {
            BUMP_HOST_COUNTER(rxOutOfSequence);
            ashTraceEventRecdFrame("out of sequence");
          }
          ashRejectFrame();
        }
      }
      break;
    case TYPE_ACK:                            // already fully processed
      break;
    case TYPE_NAK:                            // start retransmission if needed
      ashStartRetransmission();
      break;
    case TYPE_RSTACK:                         // unexpected ncp reset
      ncpError = rxBuffer[2];
      return ashHostDisconnect(EZSP_ASH_ERROR_NCP_RESET);
    case TYPE_ERROR:                          // ncp error
      return ashNcpDisconnect(rxBuffer[2]);
    case TYPE_INVALID:                        // reject invalid frames
      ashTraceArray((uint8_t *)"Rec'd frame:", rxLen, rxBuffer);
      ashRejectFrame();
      break;
  } // end switch(frameType)
  return EZSP_ASH_IN_PROGRESS;
} // end of ashReceiveExec()

void ashSendExec(void)
{
  static uint8_t offset;
  uint8_t out, in, len;
  static EzspBuffer *buffer;

  // Check for received acknowledgement timer expiry
  if (ashAckTimerHasExpired()) {
    if (ashFlags & FLG_CONNECTED) {
      if (ackRx != ((ashFlags & FLG_RETX) ? frmReTx : frmTx) ) {
        BUMP_HOST_COUNTER(rxAckTimeouts);
        ashAdjustAckPeriod(true);
        ashTraceEventTime("Timer expired waiting for ACK");
        if (++ashTimeouts >= ASH_MAX_TIMEOUTS) {
          (void)ashHostDisconnect(EZSP_ASH_ERROR_TIMEOUTS);
          return;
        }
        ashStartRetransmission();
      } else {
        ashStopAckTimer();
      }
    } else {
      (void)ashHostDisconnect(EZSP_ASH_ERROR_RESET_FAIL);
    }
  }

  while (ezspSerialWriteAvailable() == EZSP_SUCCESS) {
    // Send ASH_CAN character immediately, ahead of any other transmit data
    if (ashFlags & FLG_CAN) {
      if (sendState == SEND_STATE_IDLE) {   // sending RST or just woke NCP
        ezspSerialWriteByte(ASH_CAN);
      } else if (sendState == SEND_STATE_TX_DATA) { // cancel frame in progress
        BUMP_HOST_COUNTER(txCancelled);
        ezspSerialWriteByte(ASH_CAN);
        ashStopAckTimer();
        sendState = SEND_STATE_IDLE;
      }
      ashFlags &= ~FLG_CAN;
      continue;
    }
    switch (sendState) {
      // In between frames - do some housekeeping and decide what to send next
      case SEND_STATE_IDLE:
        // If retransmitting, set the next frame to send to the last ackNum
        // received, then check to see if retransmission is now complete.
        if (ashFlags & FLG_RETX) {
          if (WITHIN_RANGE(frmReTx, ackRx, frmTx)) {
            frmReTx = ackRx;
          }
          if (frmReTx == frmTx) {
            ashFlags &= ~FLG_RETX;
            ashScrubReTxQueue();
          }
        }

        ashDataFrameFlowControl();            // restrain ncp if needed

        // See if a short frame is flagged to be sent
        // The order of the tests below - RST, NAK and ACK -
        // sets the relative priority of sending these frame types.
        if (ashFlags & FLG_RST) {
          txControl = ASH_CONTROL_RST;
          ashSetAndStartAckTimer(ashReadConfig(timeRst));
          len = 1;
          ashFlags &= ~(FLG_RST | FLG_NAK | FLG_ACK);
          sendState = SEND_STATE_SHFRAME;
        } else if (ashFlags & (FLG_NAK | FLG_ACK) ) {
          if (ashFlags & FLG_NAK) {
            txControl = ASH_CONTROL_NAK + (frmRx << ASH_ACKNUM_BIT);
            ashFlags &= ~(FLG_NRTX | FLG_NAK | FLG_ACK);
          } else {
            txControl = ASH_CONTROL_ACK + (frmRx << ASH_ACKNUM_BIT);
            ashFlags &= ~(FLG_NRTX | FLG_ACK);
          }
          if (ashFlags & FLG_NR) {
            txControl |= ASH_NFLAG_MASK;
            ashFlags |= FLG_NRTX;
            ashStartNrTimer();
          }
          ackTx = frmRx;
          len = 1;
          sendState = SEND_STATE_SHFRAME;
          // See if retransmitting DATA frames for error recovery
        } else if (ashFlags & FLG_RETX) {
          buffer = ezspQueueNthEntry(&reTxQueue, MOD8(frmTx - frmReTx) );
          len = buffer->len + 1;
          txControl = ASH_CONTROL_DATA
                      | (frmReTx << ASH_FRMNUM_BIT)
                      | (frmRx << ASH_ACKNUM_BIT)
                      | ASH_RFLAG_MASK;
          sendState = SEND_STATE_RETX_DATA;
          // See if an ACK should be generated
        } else if (ackTx != frmRx) {
          ashFlags |= FLG_ACK;
          break;
          // Send a DATA frame if ready
        } else if ( !ezspQueueIsEmpty(&txQueue)
                    && WITHIN_RANGE(ackRx, frmTx, ackRx + ashReadConfig(txK) - 1) ) {
          buffer = ezspQueueHead(&txQueue);
          len = buffer->len + 1;
          ADD_HOST_COUNTER(len - 1, txData);
          txControl = ASH_CONTROL_DATA
                      | (frmTx << ASH_FRMNUM_BIT)
                      | (frmRx << ASH_ACKNUM_BIT);
          sendState = SEND_STATE_TX_DATA;
          // Otherwise there's nothing to send
        } else {
          ezspSerialWriteFlush();
          return;
        }

        // Start frame - ashEncodeByte() is inited by a non-zero length argument
        ashTraceFrame(true);                  // trace output (if enabled)
        out = ashEncodeByte(len, txControl, &offset);
        ezspSerialWriteByte(out);
        break;

      case SEND_STATE_SHFRAME:                // sending short frame
        if (offset != 0xFF) {
          in = txBuffer[offset];
          out = ashEncodeByte(0, in, &offset);
          ezspSerialWriteByte(out);
        } else {
          sendState = SEND_STATE_IDLE;
        }
        break;

      case SEND_STATE_TX_DATA:                // sending data frame
      case SEND_STATE_RETX_DATA:              // resending data frame
        if (offset != 0xFF) {
          in = offset ? buffer->data[offset - 1] : txControl;
          out = ashEncodeByte(0, in, &offset);
          ezspSerialWriteByte(out);
        } else {
          if (sendState == SEND_STATE_TX_DATA) {
            INC8(frmTx);
            buffer = ezspRemoveQueueHead(&txQueue);
            ezspAddQueueTail(&reTxQueue, buffer);
          } else {
            INC8(frmReTx);
          }
          if (ashAckTimerIsNotRunning()) {
            ashStartAckTimer();
          }
          ackTx = frmRx;
          sendState = SEND_STATE_IDLE;
        }
        break;
    }   // end of switch(sendState)
  }     // end of while (ezspSerialWriteAvailable() == EZSP_SUCCESS)
  ezspSerialWriteFlush();
} // end of ashSendExec()

bool ashIsConnected(void)
{
  return ((ashFlags & FLG_CONNECTED) != 0);
}

EzspStatus ashReceive(uint8_t *len, uint8_t *inbuf)
{
  EzspBuffer *buffer;

  *len = 0;
  if (!(ashFlags & FLG_CONNECTED)) {
    return EZSP_NOT_CONNECTED;
  }
  if (ezspQueueIsEmpty(&rxQueue)) {
    return EZSP_NO_RX_DATA;
  }
  buffer = ezspRemoveQueueHead(&rxQueue);
  (void) memcpy(inbuf, buffer->data, buffer->len);
  *len = buffer->len;
  ezspFreeBuffer(&rxFree, buffer);
  ezspTraceEzspVerbose("ashReceive(): ezspFreeBuffer(): %u", buffer);
  return EZSP_SUCCESS;
}

// After verifying that the data field length is within bounds,
// copies data frame to a buffer and appends it to the transmit queue.
EzspStatus ashSend(uint8_t len, const uint8_t *inptr)
{
  EzspBuffer *buffer;

  if (len < ASH_MIN_DATA_FIELD_LEN ) {
    return EZSP_DATA_FRAME_TOO_SHORT;
  } else if (len > ASH_MAX_DATA_FIELD_LEN) {
    return EZSP_DATA_FRAME_TOO_LONG;
  }
  if (!(ashFlags & FLG_CONNECTED)) {
    return EZSP_NOT_CONNECTED;
  }
  buffer = ezspAllocBuffer(&txFree);
  ezspTraceEzspVerbose("ashSend(): ezspAllocBuffer(): %u", buffer);
  if (buffer == NULL) {
    return EZSP_NO_TX_SPACE;
  }
  (void) memcpy(buffer->data, inptr, len);
  buffer->len = len;
  ashRandomizeBuffer(buffer->data, buffer->len);
  ezspAddQueueTail(&txQueue, buffer);
  ashSendExec();
  return EZSP_SUCCESS;
}

EzspStatus ashWakeUpNcp(bool init)
{
  static uint16_t start;
  uint16_t now;
  uint16_t bytes;

  if (ezspSerialReadAvailable(&bytes) == EZSP_SUCCESS) {
    return EZSP_SUCCESS;
  }
  now = halCommonGetInt16uMillisecondTick();
  if (init) {
    start = now;
    ezspSerialWriteByte(ASH_WAKE);
    ezspSerialWriteFlush();
    ezspSerialWriteByte(ASH_WAKE);
    ezspSerialWriteFlush();
  }
  if ((now - start) > ASH_MAX_WAKE_TIME) {
    return EZSP_HOST_FATAL_ERROR;
  }
  return EZSP_ASH_IN_PROGRESS;
}

bool ashOkToSleep(void)
{
  uint16_t count;

  return
    (
    !ashDecodeInProgress                  // don't have a partial frame
    && (ezspSerialReadAvailable(&count) == EZSP_NO_RX_DATA)   // no rx data
    && ezspQueueIsEmpty(&rxQueue)         // no rx frames to process
    && !ncpHasCallbacks                   // no pending callbacks
    && (ashFlags == FLG_CONNECTED)        // no pending ACKs, NAKs, etc.
    && (ackTx == frmRx)                   // do not need to send an ACK
    && (ackRx == frmTx)                   // not waiting to receive an ACK
    && (sendState == SEND_STATE_IDLE)     // nothing being transmitted now
    && ezspQueueIsEmpty(&txQueue)         // nothing waiting to transmit
    && ezspSerialOutputIsIdle()            // nothing in OS buffers or UART FIFO
    );
}

//------------------------------------------------------------------------------
// Utility functions

// Initialize ASH variables, timers and queues, but not the serial port
static void ashInitVariables()
{
  ashFlags = 0;
  ashDecodeInProgress = false;
  ackRx = 0;
  ackTx = 0;
  frmTx = 0;
  frmReTx = 0;
  frmRx = 0;
  frmReTxHead = 0;
  ashTimeouts = 0;
  ncpError = EZSP_NO_ERROR;
  hostError = EZSP_NO_ERROR;
  sendState = SEND_STATE_IDLE;
  ashStopAckTimer();
  ashStopNrTimer();
  ezspInitQueues();
  ashClearCounters(&ashCount);
}

// Check free rx buffers to see whether able to receive DATA frames: set or
// clear NR flag appropriately. Inform ncp of our status using the nFlag in
// ACKs and NAKs. Note that not ready status must be refreshed if it persists
// beyond a maximum time limit.
static void ashDataFrameFlowControl(void)
{
  uint8_t freeRxBuffers;

  if (ashFlags & FLG_CONNECTED) {
    freeRxBuffers = ezspFreeListLength(&rxFree);
    // Set/clear NR flag based on the number of buffers free
    if (freeRxBuffers < ashReadConfig(nrLowLimit)) {
      ashFlags |= FLG_NR;
    } else if (freeRxBuffers > ashReadConfig(nrHighLimit)) {
      ashFlags &= ~FLG_NR;
      ashStopNrTimer();    //** needed??
    }
    // Force an ACK (or possibly NAK) if we need to send an updated nFlag
    // due to either a changed NR status or to refresh a set nFlag
    if (ashFlags & FLG_NR) {
      if ( !(ashFlags & FLG_NRTX) || ashNrTimerHasExpired()) {
        ashFlags |= FLG_ACK;
        ashStartNrTimer();
      }
    } else {
      (void)ashNrTimerHasExpired();  // esnure timer checked often
      if (ashFlags & FLG_NRTX) {
        ashFlags |= FLG_ACK;
        ashStopNrTimer();    //** needed???
      }
    }
  } else {
    ashStopNrTimer();
    ashFlags &= ~(FLG_NRTX | FLG_NR);
  }
}

// If not already retransmitting, and there are unacked frames, start
// retransmitting after the last frame that was acked.
static void ashStartRetransmission(void)
{
  if ( !(ashFlags & FLG_RETX) && (ackRx != frmTx) ) {
    ashStopAckTimer();
    frmReTx = ackRx;
    ashFlags |= (FLG_RETX | FLG_CAN);
  }
}

// If the last control byte received was a DATA control,
// and we are connected and not already in the reject condition,
// then send a NAK and set the reject condition.
static void ashRejectFrame(void)
{
  if ( ((rxControl & ASH_DFRAME_MASK) == ASH_CONTROL_DATA)
       && ((ashFlags & (FLG_REJ | FLG_CONNECTED)) == FLG_CONNECTED) ) {
    ashFlags |= (FLG_REJ | FLG_NAK);
  }
}

static void ashFreeNonNullRxBuffer(void)
{
  if (rxDataBuffer != NULL) {
    ezspFreeBuffer(&rxFree, rxDataBuffer);
    ezspTraceEzspVerbose("ashFreeNonNullRxBuffer(): ezspFreeBuffer(): %u", rxDataBuffer);
    rxDataBuffer = NULL;
  }
}

static void ashScrubReTxQueue(void)
{
  EzspBuffer *buffer;

  while (ackRx != frmReTxHead) {
    buffer = ezspRemoveQueueHead(&reTxQueue);
    ezspFreeBuffer(&txFree, buffer);
    ezspTraceEzspVerbose("ashScrubReTxQueue(): ezspFreeBuffer(): %u", buffer);
    INC8(frmReTxHead);
  }
}

static EzspStatus ashHostDisconnect(uint8_t error)
{
  ashFlags = 0;
  hostError = error;
  ashTraceDisconnected(error);
  return EZSP_HOST_FATAL_ERROR;
}

static EzspStatus ashNcpDisconnect(uint8_t error)
{
  ashFlags = 0;
  ncpError = error;
  ashTraceDisconnected(error);
  return EZSP_ASH_NCP_FATAL_ERROR;
}

static EzspStatus ashReadFrame(void)
{
  uint8_t index;
  uint8_t in;
  uint8_t out;
  EzspStatus status;

  if (!ashDecodeInProgress) {
    rxLen = 0;
    rxDataBuffer = NULL;
  }

  do {
    // Get next byte from serial port, return if no data
    status = ezspSerialReadByte(&in);
    if (status == EZSP_NO_RX_DATA) {
      break;
    }

    // 0xFF byte signals a callback is pending when between frames
    // in synchronous (polled) callback mode.
    if (!ashDecodeInProgress && (in == ASH_WAKE)) {
      if (ncpSleepEnabled) {
        ncpHasCallbacks = true;
      }
      status = EZSP_ASH_IN_PROGRESS;
      continue;
    }

    // Decode next input byte - note that many input bytes do not produce
    // an output byte. Return on any error in decoding.
    index = rxLen;
    status = ashDecodeByte(in, &out, &rxLen);
    if ( (status != EZSP_ASH_IN_PROGRESS) && (status != EZSP_SUCCESS) ) {
      ashFreeNonNullRxBuffer();
      break;                  // discard an invalid frame
    }

    if (rxLen != index) {           // if input byte produced an output byte
      if (rxLen <= RX_BUFFER_LEN) { // if a short frame, return in rxBuffer
        rxBuffer[index] = out;
        // If a longer DATA frame, allocate an EzspBuffer for it.
        // (Note the control byte is always returned in rxControl. Even if
        // no buffer can be allocated, the control's ackNum must be processed.)
      } else {
        if (rxLen == RX_BUFFER_LEN + 1) {   // alloc buffer, copy prior data
          rxDataBuffer = ezspAllocBuffer(&rxFree);
          ezspTraceEzspVerbose("ashReadFrame(): ezspAllocBuffer(): %u", rxDataBuffer);
          if (rxDataBuffer != NULL) {
            (void) memcpy(rxDataBuffer->data, rxBuffer + 1, RX_BUFFER_LEN - 1);
            rxDataBuffer->len = RX_BUFFER_LEN - 1;
          }
        }
        if (rxDataBuffer != NULL) {     // copy next byte to buffer
          rxDataBuffer->data[index - 1] = out;  // -1 since control is omitted
          rxDataBuffer->len = index;
        }
      }
    }
  } while (status == EZSP_ASH_IN_PROGRESS);
  return status;
} // end of ashReadFrame()

// If enabled, exclusive-OR buffer data with a pseudo-random sequence
static void ashRandomizeBuffer(uint8_t *buffer, uint8_t len)
{
  if (ashReadConfig(randomize)) {
    (void)ashRandomizeArray(0, buffer, len);// zero inits the random sequence
  }
}

// Determines frame type from the control byte then validates its length.
// If invalid type or length, returns TYPE_INVALID.
static uint16_t ashFrameType(uint16_t control, uint16_t len)
{
  if (control == ASH_CONTROL_RSTACK) {
    if (len == ASH_FRAME_LEN_RSTACK) {
      return TYPE_RSTACK;
    }
  } else if (control == ASH_CONTROL_ERROR) {
    if (len == ASH_FRAME_LEN_ERROR) {
      return TYPE_ERROR;
    }
  } else if ( (control & ASH_DFRAME_MASK) == ASH_CONTROL_DATA) {
    if (len >= ASH_FRAME_LEN_DATA_MIN) {
      return TYPE_DATA;
    }
  } else if ( (control & ASH_SHFRAME_MASK) == ASH_CONTROL_ACK) {
    if (len == ASH_FRAME_LEN_ACK) {
      return TYPE_ACK;
    }
  } else if ( (control & ASH_SHFRAME_MASK) == ASH_CONTROL_NAK) {
    if (len == ASH_FRAME_LEN_NAK) {
      return TYPE_NAK;
    }
  } else {
    BUMP_HOST_COUNTER(rxBadControl);
    ashTraceEventRecdFrame("illegal control");
    return TYPE_INVALID;
  }
  BUMP_HOST_COUNTER(rxBadLength);
  ashTraceEventRecdFrame("illegal length");
  return TYPE_INVALID;
} // end of ashFrameType()

// Functions that read local variables for tracing
uint8_t readTxControl(void)
{
  return txControl;
};
uint8_t readRxControl(void)
{
  return rxControl;
};
uint8_t readAckRx(void)
{
  return ackRx;
};
uint8_t readAckTx(void)
{
  return ackTx;
};
uint8_t readFrmTx(void)
{
  return frmTx;
};
uint8_t readFrmReTx(void)
{
  return frmReTx;
};
uint8_t readFrmRx(void)
{
  return frmRx;
};
uint8_t readAshTimeouts(void)
{
  return ashTimeouts;
};
