/***************************************************************************//**
 * @file
 * @brief Header for ASH Host functions
 *
 * See @ref ash_util for documentation.
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

/** @addtogroup ash_util
 *
 * See ash-host.h.
 *
 *@{
 */

#ifndef SILABS_ASH_HOST_H
#define SILABS_ASH_HOST_H
#include "../ezsp-host-common.h"

#define ASH_MAX_TIMEOUTS          6   /*!< timeouts before link is judged down */
#define ASH_MAX_WAKE_TIME         150 /*!< max time in msecs for ncp to wake */

#define ASH_PORT_LEN              40  /*!< length of serial port name string */

// Bits in traceFlags to enable various host trace outputs
#define TRACE_FRAMES_BASIC        1   /*!< frames sent and received */
#define TRACE_FRAMES_VERBOSE      2   /*!< basic frames + internal variables */
#define TRACE_EVENTS              4   /*!< events */
#define TRACE_EZSP                8   /*!< EZSP commands, responses and callbacks */
#define TRACE_EZSP_VERBOSE        16  /*!< additional EZSP information */

// resetMethod values
#define ASH_RESET_METHOD_RST      0   /*!< send RST frame */
#define ASH_RESET_METHOD_DTR      1   /*!< reset using DTR */
#define ASH_RESET_METHOD_CUSTOM   2   /*!< hook for user-defined reset */
#define ASH_RESET_METHOD_NONE     3   /*!< no reset - for testing */

// ashSelectHostConfig() values
#define ASH_HOST_CONFIG_115200_RTSCTS   0
#define ASH_HOST_CONFIG_57600_XONXOFF   1

/** @brief Configuration parameters: values must be defined before calling ashResetNcp()
 * or ashStart(). Note that all times are in milliseconds.
 */
typedef struct {
  char serialPort[ASH_PORT_LEN];  /*!< serial port name */
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
} AshHostConfig;

#define ashReadConfig(member) \
  (ashHostConfig.member)

#define ashReadConfigOrDefault(member, defval) \
  (ashHostConfig.member)

#define ashWriteConfig(member, value) \
  do { ashHostConfig.member = value; } while (0)

#define BUMP_HOST_COUNTER(mbr) do { ashCount.mbr++; } while (0)
#define ADD_HOST_COUNTER(op, mbr) do { ashCount.mbr += op; }  while (0)

typedef struct {
  uint32_t txBytes;             /*!< total bytes transmitted */
  uint32_t txBlocks;            /*!< blocks transmitted */
  uint32_t txData;              /*!< DATA frame data fields bytes transmitted */
  uint32_t txAllFrames;         /*!< frames of all types transmitted */
  uint32_t txDataFrames;        /*!< DATA frames transmitted */
  uint32_t txAckFrames;         /*!< ACK frames transmitted */
  uint32_t txNakFrames;         /*!< NAK frames transmitted */
  uint32_t txReDataFrames;      /*!< DATA frames retransmitted */
  uint32_t txN0Frames;          /*!< ACK and NAK frames with nFlag 0 transmitted */
  uint32_t txN1Frames;          /*!< ACK and NAK frames with nFlag 1 transmitted */
  uint32_t txCancelled;         /*!< frames cancelled (with ASH_CAN byte) */

  uint32_t rxBytes;             /*!< total bytes received */
  uint32_t rxBlocks;            /*!< blocks received         */
  uint32_t rxData;              /*!< DATA frame data fields bytes received */
  uint32_t rxAllFrames;         /*!< frames of all types received         */
  uint32_t rxDataFrames;        /*!< DATA frames received                    */
  uint32_t rxAckFrames;         /*!< ACK frames received                       */
  uint32_t rxNakFrames;         /*!< NAK frames received                       */
  uint32_t rxReDataFrames;      /*!< retransmitted DATA frames received        */
  uint32_t rxN0Frames;          /*!< ACK and NAK frames with nFlag 0 received  */
  uint32_t rxN1Frames;          /*!< ACK and NAK frames with nFlag 1 received  */
  uint32_t rxCancelled;         /*!< frames cancelled (with ASH_CAN byte) */

  uint32_t rxCrcErrors;         /*!< frames with CRC errors */
  uint32_t rxCommErrors;        /*!< frames with comm errors (with ASH_SUB byte) */
  uint32_t rxTooShort;          /*!< frames shorter than minimum */
  uint32_t rxTooLong;           /*!< frames longer than maximum */
  uint32_t rxBadControl;        /*!< frames with illegal control byte */
  uint32_t rxBadLength;         /*!< frames with illegal length for type of frame */
  uint32_t rxBadAckNumber;      /*!< frames with bad ACK numbers */
  uint32_t rxNoBuffer;          /*!< DATA frames discarded due to lack of buffers */
  uint32_t rxDuplicates;        /*!< duplicate retransmitted DATA frames */
  uint32_t rxOutOfSequence;     /*!< DATA frames received out of sequence */
  uint32_t rxAckTimeouts;       /*!< received ACK timeouts */
} AshCount;

extern AshHostConfig ashHostConfig;
extern AshCount ashCount;
extern bool ncpSleepEnabled;

/** @brief Selects a set of host configuration parameters. To select
 * a configuration other than the default, must be called before ashStart().
 *
 * @param config  one of the following:
 *  -            ::ASH_HOST_CONFIG_115200_RTSCTS (default)
 *  -            ::ASH_HOST_CONFIG_57600_XONXOFF
 *
 *
 * @return
 * - ::EZSP_SUCCESS
 * _ ::EZSP_ASH_HOST_FATAL_ERROR
 */
EzspStatus ashSelectHostConfig(uint8_t config);

/** @brief Initializes the ASH protocol, and waits until the NCP
 * finishes rebooting, or a non-recoverable error occurs.
 *
 * @return
 * - ::EZSP_SUCCESS
 * - ::EZSP_ASH_HOST_FATAL_ERROR
 * - ::EZSP_ASH_NCP_FATAL_ERROR
 */
EzspStatus ashStart(void);

/** @brief Stops the ASH protocol - flushes and closes the serial port,
 *  clears all queues, stops timers, etc. Does not affect any host configuration
 *  parameters.
 */
void ashStop(void);

/** @brief Adds a DATA frame to the transmit queue to send to the NCP.
 *  Frames that are too long or too short will not be sent, and frames
 *  will not be added to the queue if the host is not in the Connected
 *  state, or the NCP is not ready to receive a DATA frame or if there
 *  is no room in the queue;
 *
 * @param len    length of data field
 *
 * @param inptr  pointer to array containing the data to be sent
 *
 * @return
 * - ::EZSP_SUCCESS
 * - ::EZSP_ASH_NO_TX_SPACE
 * - ::EZSP_DATA_FRAME_TOO_SHORT
 * - ::EZSP_DATA_FRAME_TOO_LONG
 * - ::EZSP_ASH_NOT_CONNECTED
 */
EzspStatus ashSend(uint8_t len, const uint8_t *inptr);

/** @brief Initializes the ASH serial port and (if enabled)
 *  resets the NCP. The method used to do the reset is specified by the
 *  the host configuration parameter resetMethod.
 *
 *  When the reset method is sending a RST frame, the caller should
 *  retry NCP resets a few times if it fails.
 *
 * @return
 * - ::EZSP_SUCCESS
 * - ::EZSP_ASH_HOST_FATAL_ERROR
 * - ::EZSP_ASH_NCP_FATAL_ERROR
 */
EzspStatus ashResetNcp(void);

/** @brief Wakes up the NCP by sending two 0xFF bytes. When the NCP wakes,
 *  it sends back an 0xFF byte.
 *
 * @param init set true on the first call to this function, starts timer
 *
 * @return
 * - ::EZSP_ASH_IN_PROGRESS       NCP is not yet awake, but has not timed out
 * - ::EZSP_SUCCESS               NCP is swake
 * - ::EZSP_ASH_HOST_FATAL_ERROR  NCP did not wake within ASH_MAX_WAKE_TIME
 */
EzspStatus ashWakeUpNcp(bool init);

/** @brief Indicates if the host is in the Connected state.
 *  If not, the host and NCP cannot exchange DATA frames. Note that
 *  this function does not actively confirm that communication with
 *  NCP is healthy, but simply returns its last known status.
 *
 * @return
 * - true      host and NCP can exchange DATA frames
 * - false     host and NCP cannot now exchange DATA frames
 */
bool ashIsConnected(void);

/** @brief Manages outgoing communication to the NCP, including
 *  DATA frames as well as the frames used for initialization and
 *  error detection and recovery.
 */
void ashSendExec(void);

/** @brief Processes all received frames.
 *  Received DATA frames are appended to the receive queue if there is room.
 *
 * @return
 * - ::EZSP_SUCCESS
 * - ::EZSP_ASH_IN_PROGRESS
 * - ::EZSP_ASH_NO_RX_DATA
 * - ::EZSP_ASH_NO_RX_SPACE
 * - ::EZSP_ASH_HOST_FATAL_ERROR
 * - ::EZSP_ASH_NCP_FATAL_ERROR
 */
EzspStatus ashReceiveExec(void);

/** @brief Returns the next DATA frame received, if there is one.
 *  To be more precise, the head of the receive queue is copied into the
 *  specified buffer and then freed.
 *
 * @param len     length of the DATA frame if one was returnnd
 *
 * @param buffer  array into which the DATA frame should be copied
 *
 * @return
 * - ::EZSP_SUCCESS
 * - ::EZSP_ASH_NO_RX_DATA
 * - ::EZSP_ASH_NOT_CONNECTED
 */
EzspStatus ashReceive(uint8_t *len, uint8_t *buffer);

/** @brief Returns true if the host can sleep without causing errors in the
 *  ASH protocol.
 *
 */
bool ashOkToSleep(void);

#endif //__ASH_HOST_H___

/** @} END addtogroup
 */
