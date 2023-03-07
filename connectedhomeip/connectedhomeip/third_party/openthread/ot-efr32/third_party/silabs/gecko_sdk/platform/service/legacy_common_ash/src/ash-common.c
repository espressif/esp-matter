/***************************************************************************//**
 * @file
 * @brief  ASH functions common to Host and Network Co-Processor
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#include <stddef.h>
#include "stack/include/ember-types.h"
#include "hal/hal.h"
#include "ash-protocol.h"
#ifdef EZSP_HOST
#ifdef EMBER_STACK_CONNECT
#include "ezsp-host/ash/ash-host.h"
#else
#include "app/ezsp-host/ash/ash-host.h"
#endif
#else
#include "ash-ncp.h"
#endif
#include "ash-common.h"

//------------------------------------------------------------------------------
// Preprocessor definitions

// Define constants for the LFSR in ashRandomizeBuffer()
#define LFSR_POLY   0xB8      // polynomial
#define LFSR_SEED   0x42      // initial value (seed)

//------------------------------------------------------------------------------
// Global Variables

uint16_t ashAckTimer;           // time ashAckTimer started: 0 means not ready
uint16_t ashAckPeriod;          // time used to check ashAckTimer expiry (msecs)
uint8_t ashNrTimer;             // not ready timer (16 msec units)
bool ashDecodeInProgress;  // frame decode in progress

//------------------------------------------------------------------------------
// Local Variables

// Variables used in encoding frames
static bool encodeEscFlag; // true when preceding byte was escaped
static uint8_t encodeFlip;      // byte to send after ASH_ESC
static uint16_t encodeCrc;
static uint8_t encodeState;     // encoder state: 0 = control/data bytes
                                // 1 = crc low byte, 2 = crc high byte, 3 = flag
static uint8_t encodeCount;     // bytes remaining to encode

// Variables used in decoding frames
static uint8_t decodeLen;       // bytes in frame, plus CRC, clamped to limit +1:
                                // high values also used to record certain errors
static uint8_t decodeFlip;      // ASH_FLIP if previous byte was ASH_ESC
static uint8_t decodeByte1;     // a 2 byte queue to avoid outputting crc bytes -
static uint8_t decodeByte2;     // at frame end, they contain the received crc
static uint16_t decodeCrc;

//------------------------------------------------------------------------------
// Forward Declarations

static uint8_t ashEncodeStuffByte(uint8_t byte);

//------------------------------------------------------------------------------
// Functions

uint8_t ashEncodeByte(uint8_t len, uint8_t byte, uint8_t *offset)
{
  if (len) {                  // start a new frame if len is non-zero
    encodeCount = len;
    *offset = 0;
    encodeState = 0;
    encodeEscFlag = false;
    encodeCrc = 0xFFFF;
  }

  if (encodeEscFlag) {        // was an escape sent last time?
    encodeEscFlag = false;
    return encodeFlip;        // then send data byte with bit flipped
  }

  if (encodeState == 0) {         // control and data field bytes
    encodeCrc = halCommonCrc16(byte, encodeCrc);
    if (--encodeCount == 0) {
      encodeState = 1;
    } else {
      ++(*offset);
    }
    return ashEncodeStuffByte(byte);
  } else if (encodeState == 1) {  // crc high byte
    encodeState = 2;
    return ashEncodeStuffByte((uint8_t)(encodeCrc >> 8));
  } else if (encodeState == 2) {  // crc low byte
    encodeState = 3;
    return ashEncodeStuffByte((uint8_t)(encodeCrc & 0xFF));
  }                               // flag byte - end of frame
  *offset = 0xFF;
  return ASH_FLAG;
}

// A helper for ashEncodeByte(), this determines whether a byte
// about to be sent is a reserved value that must be escaped.
static uint8_t ashEncodeStuffByte(uint8_t byte)
{
  if ((byte == ASH_XON)
      || (byte == ASH_XOFF)
      || (byte == ASH_SUB)
      || (byte == ASH_CAN)
      || (byte == ASH_ESC)
      || (byte == ASH_FLAG)) {
    encodeEscFlag = true;
    encodeFlip = byte ^ ASH_FLIP;
    return ASH_ESC;
  } else {
    return byte;
  }
}

EzspStatus ashDecodeByte(uint8_t byte, uint8_t *out, uint8_t *outLen)
{
  EzspStatus status = EZSP_ASH_IN_PROGRESS;

  if (!ashDecodeInProgress) {
    decodeLen = 0;
    decodeByte1 = 0;
    decodeByte2 = 0;
    decodeFlip = 0;
    decodeCrc = 0xFFFF;
  }

  switch (byte) {
    case ASH_FLAG:            // flag byte (frame delimiter)
      if (decodeLen == 0) {   // if no frame data, not end flag, so ignore it
        decodeFlip = 0;       // ignore isolated data escape between flags
        break;
      } else if (decodeLen == 0xFF) {
        status = EZSP_ASH_COMM_ERROR;
      } else if (decodeCrc != ((uint16_t)decodeByte2 << 8) + decodeByte1) {
        status = EZSP_ASH_BAD_CRC;
      } else if (decodeLen < ASH_MIN_FRAME_WITH_CRC_LEN) {
        status = EZSP_ASH_TOO_SHORT;
      } else if (decodeLen > ASH_MAX_FRAME_WITH_CRC_LEN) {
        status = EZSP_ASH_TOO_LONG;
      } else {
        status = EZSP_SUCCESS;
      }
      break;

    case ASH_ESC:             // byte stuffing escape byte
      decodeFlip = ASH_FLIP;
      break;

    case ASH_CAN:             // cancel frame without an error
      status = EZSP_ASH_CANCELLED;
      break;

    case ASH_SUB:             // discard remainder of frame
      decodeLen = 0xFF;       // special value flags low level comm error
      break;

#ifdef EZSP_HOST
    // If host is using RTS/CTS, ignore any XON/XOFFs received from the NCP.
    // If using XON/XOFF, the host driver must remove them from the input stream.
    // If it doesn't, it probably means the driver isn't setup for XON/XOFF,
    // so issue an error to flag the serial port driver problem.
    case ASH_XON:
    case ASH_XOFF:
      if (!ashReadConfig(rtsCts)) {
        status = EZSP_ASH_ERROR_XON_XOFF;
      }
      break;
#endif

    default:                  // a normal byte
      byte ^= decodeFlip;
      decodeFlip = 0;
      if (decodeLen <= ASH_MAX_FRAME_WITH_CRC_LEN) {// limit length to max + 1
        ++decodeLen;
      }
      if (decodeLen > ASH_CRC_LEN) { // compute frame CRC even if too long
        decodeCrc = halCommonCrc16(decodeByte2, decodeCrc);
        if (decodeLen <= ASH_MAX_FRAME_WITH_CRC_LEN) { // store to only max len
          *out = decodeByte2;
          *outLen = decodeLen - ASH_CRC_LEN; // CRC is not output, reduce length
        }
      }
      decodeByte2 = decodeByte1;
      decodeByte1 = byte;
      break;
  }  // end switch (byte)

  ashDecodeInProgress = (status == EZSP_ASH_IN_PROGRESS);
  return status;
}

uint8_t ashRandomizeArray(uint8_t seed, uint8_t *buf, uint8_t len)
{
  if (seed == 0) {
    seed = LFSR_SEED;
  }
  while (len--) {
    *buf++ ^= seed;
    seed = (seed & 1) ? ((seed >> 1) ^ LFSR_POLY) : (seed >> 1);
  }
  return seed;
}

void ashStartAckTimer(void)
{
  ashAckTimer = halCommonGetInt16uMillisecondTick();
  if (ashAckTimer == 0) {     // 0 means the timer is not running, so fudge
    ashAckTimer = 0xFFFF;     // result if it happens to be 0
  }
}

bool ashAckTimerHasExpired(void)
{
  if (ashAckTimer == 0) {     // if timer is not running, return false
    return false;
  }
  return ((int16_t)(halCommonGetInt16uMillisecondTick() - ashAckTimer)
          >= ashAckPeriod);
}

void ashAdjustAckPeriod(bool expired)
{
  uint16_t maxTime = ashReadConfigOrDefault(ackTimeMax, ASH_TIME_DATA_MAX);
  uint16_t minTime = ashReadConfigOrDefault(ackTimeMin, ASH_TIME_DATA_MIN);

  if (expired) {                        // if expired, double the period
    ashAckPeriod += ashAckPeriod;
  } else if (ashAckTimer) {             // adjust period only if running
    uint16_t lastAckTime;                 // time elapsed since timer was started
    uint32_t temp = ashAckPeriod;
    // compute time to receive acknowledgement, then stop timer
    lastAckTime = halCommonGetInt16uMillisecondTick() - ashAckTimer;
    temp = (temp << 3) - temp;
    temp += lastAckTime << 2;
    temp >>= 3;
    ashAckPeriod = (uint16_t)temp;
  }

  if (ashAckPeriod > maxTime) {         // keep ashAckPeriod within limits
    ashAckPeriod = maxTime;
  } else if (ashAckPeriod < minTime) {
    ashAckPeriod = minTime;
  }
  ashAckTimer = 0;                      // always stop the timer
}

void ashStartNrTimer(void)
{
  ashNrTimer =
    (halCommonGetInt16uMillisecondTick()
     + ashReadConfigOrDefault(nrTime, ASH_NR_TIME)) >> ASH_NR_TIMER_BIT;
  if (ashNrTimer == 0) {
    ashNrTimer = 0xFF;
  }
}

bool ashNrTimerHasExpired(void)
{
  uint8_t now;

  if (ashNrTimer) {
    now = halCommonGetInt16uMillisecondTick() >> ASH_NR_TIMER_BIT;
    if ((int8_t)(now - ashNrTimer) >= 0) {
      ashNrTimer = 0;
    }
  }
  return (!ashNrTimer);
}
