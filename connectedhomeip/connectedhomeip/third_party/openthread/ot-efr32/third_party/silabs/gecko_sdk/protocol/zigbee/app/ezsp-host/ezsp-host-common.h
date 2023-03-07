/***************************************************************************//**
 * @file
 * @brief Common header for EZSP Host functions
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

#ifndef SILABS_EZSP_HOST_COMMON_H
#define SILABS_EZSP_HOST_COMMON_H

/** @brief Configuration parameters: values must be defined before calling ashResetNcp()
 * or ashStart(). Note that all times are in milliseconds.
 */
typedef struct {
  char serialPort[40];  /*!< serial port name */
  uint32_t baudRate;      /*!< baud rate (bits/second) */
  uint8_t  stopBits;      /*!< stop bits */
  uint8_t  rtsCts;        /*!< true enables RTS/CTS flow control, false XON/XOFF */
  uint16_t outBlockLen;   /*!< max bytes to buffer before writing to serial port */
  uint16_t inBlockLen;    /*!< max bytes to read ahead from serial port */
  uint8_t  traceFlags;    /*!< trace output control bit flags */
  uint8_t  txK;           /*!< max frames sent without being ACKed (1-7) */
  uint8_t  randomize;     /*!< enables randomizing DATA frame payloads */
  uint16_t ackTimeInit;   /*!< adaptive rec'd ACK timeout initial value */
  uint16_t ackTimeMin;    /*!< adaptive rec'd ACK timeout minimum value */
  uint16_t ackTimeMax;    /*!< adaptive rec'd ACK timeout maximum value */
  uint16_t timeRst;       /*!< time allowed to receive RSTACK after ncp is reset */
  uint8_t  nrLowLimit;    /*!< if free buffers < limit, host receiver isn't ready */
  uint8_t  nrHighLimit;   /*!< if free buffers > limit, host receiver is ready */
  uint16_t nrTime;        /*!< time until a set nFlag must be resent (max 2032) */
  uint8_t  resetMethod;   /*!< method used to reset ncp */
} EzspHostConfig;

extern EzspStatus hostError;
extern EzspStatus ncpError;

#if defined(EZSP_UART)
  #ifndef EZSP_ASH
    #define EZSP_ASH 1
  #endif
#endif // EZSP_UART

#if defined(EZSP_ASH) && defined (EZSP_USB)
  #error Conflicting EZSP modes
#endif

#endif //__EZSP_HOST_COMMON_H__
