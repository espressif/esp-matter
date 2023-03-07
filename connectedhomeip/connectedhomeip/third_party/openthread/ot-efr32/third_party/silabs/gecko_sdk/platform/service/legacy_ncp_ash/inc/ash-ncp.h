/***************************************************************************//**
 * @file
 * @brief Header for ASH Network Co-Processor
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
#ifndef __ASH_NCP_H__
#define __ASH_NCP_H__

/** @description  Allocates buffers held until ashReleaseStaticBuffers()
 *  is called.  Returns the head of the allocated buffer chain for use
 *  by the higher layer.
 */
EmberMessageBuffer ashAllocateStaticBuffers(void);

/** @description  Releases the buffers allocated by ashAllocateStaticBuffers()
 *  if none are in use.
 */
bool ashReleaseStaticBuffers(void);

/** @description  Initializes the ASH protocol and serial port.
 */
void ashStart(void);

/** @description  Sends a frame to the host.
 *
 * @param buffer: index of a linked buffer containing the DATA frame to send
 * @param highPriority: set true if the frame should be sent even if the host
 * is not ready.
 */
void ashSend(EmberMessageBuffer buffer, bool highPriority);

/** @description  Calls ASH protocol to manage transmission of DATA and
 * other frames to the host.
 */
void ashSendExec(void);

/** @description  Returns a DATA frame, if one has been received.
 *
 * @param outBuf: pointer to a variable where the index of the linked buffers
 * containing a complete data frame will be written. The caller is responsible
 * for freeing these buffers when appropriate. If no frame is returned, the
 * index is set to EMBER_NULL_MESSAGE_BUFFER.
 *
 * @return  EZSP_SUCCESS
 *          EZSP_ASH_IN_PROGRESS
 *          EZSP_ASH_NO_RX_DATA
 *          EZSP_ASH_NO_RX_SPACE
 *          EZSP_ASH_NCP_FATAL_ERROR
 */
EzspStatus ashReceive(EmberMessageBuffer *outBuf);

/** @description  Checks whether there is room in the serial output buffer.
 *
 * @return  EZSP_SUCCESS
 *          EZSP_ASH_NO_TX_SPACE
 */
EzspStatus ashSerialWriteAvailable(void);

/** @description  Reads a byte from the serial port, if one is available.
 *
 * @param byte:   pointer to a variable where the byte read will be output
 *
 * @return  EZSP_SUCCESS
 *          EZSP_ASH_NO_RX_DATA
 */
EzspStatus ashSerialReadByte(uint8_t *byte);

/** @description  Writes a byte to the serial output buffer.
 *
 * @param byte:   byte to write
 */
void ashSerialWriteByte(uint8_t byte);

#ifdef SLEEPY_EZSP_UART
bool serialHostIsActive(void);
bool serialWaitingForHostToWake(void);
void serialMonitorRxd(uint8_t sleepMode);
void serialPowerDown(void);
void serialPowerUp(void);
void serialSendAwakeSignal(void);
void serialInhibitCallbackSignal(void);

#endif

/** @description  Informs ASH whether or not cany callbacks are pending.
 *
 */
void ashPendingCallbacks(bool pending);

//                                CAUTION
// These macro provide direct read-only access to the manufacturing tokens used
// by the ASH (EZSP-UART) protocol.
// These macros *should not be used* elsewhere due to the possibility of
// non-portability to future revisions of the manufacturing tokens.

#if defined(CORTEXM3)
  #define ashConfigAddr (USERDATA_BASE | (MFG_ASH_CONFIG_LOCATION & 0x0FFF))
  #define ashReadConfig(member) (((const AshNcpConfig *)ashConfigAddr)->member)
  #define ashReadConfigOrDefault(member, defVal) \
  ((ashReadConfig(member) != 0xFFFF) ? ashReadConfig(member) : defVal)
#else // for simulation, always return default value
  #define ashReadConfigOrDefault(member, defaultValue) \
  (defaultValue)
#endif

// Define the ASH configuration manufacturing indexed token as a struct.
// Each member must be a word and the struct size may not exceed 16 words.
// Token values of 0xFFFF are ignored, and a default value is used instead.
// All times are in milliseconds.
typedef struct
{ uint16_t
    baudRate,           // SerialBaudRate enum value
    traceFlags,         // trace output control bit flags (see defs below)
    unused0,            // (not used)
    txK,                // max frames that can be sent w/o being ACKed (1-7)
    randomize,          // enables randomizing DATA frame payloads
    ackTimeInit,        // adaptive rec'd ACK timeout initial value
    ackTimeMin,         //  "     "     "     "     "  minimum
    ackTimeMax,         //  "     "     "     "     "  maximum
    maxTimeouts,        // ACK timeouts needed to enter the ERROR state
    unused1,            // (not used)
    rebootDelay,        // reboot delay before sending RSTACK
    unused2,            // (not used)
    unused3,            // (not used)
    unused4,            // (not used)
    nrTime;             // time after which a rec'd nFlag expires
} AshNcpConfig;

//ASH configuration manufacturing token storage locations
//              word
//             offset
//baudRate      0x80   6=9600, 8=19200, A=38400, C=57600, F=115200
//traceFlags    0x81
//unused0       0x82
//txK           0x83
//randomize     0x84
//ackTimeInit   0x85
//ackTimeMin    0x86
//ackTimeMax    0x87
//maxTimeouts   0x88
//unused1       0x89
//rebootDelay   0x8A
//unused2       0x8B
//unused3       0x8C
//unused4       0x8D
//nrTime        0x8E

#ifdef HAL_UARTNCP_BAUD_RATE
#define ASH_BAUD_RATE HAL_UARTNCP_BAUD_RATE
#else
// EMHAL-2285 if using hardware flow control we are using FIFO mode with DMA
// which can support baud 115200
#if (EMBER_SERIAL1_MODE == EMBER_SERIAL_FIFO) && defined(EMBER_SERIAL1_RTSCTS)
#define ASH_BAUD_RATE    BAUD_115200
#else
#define ASH_BAUD_RATE    BAUD_57600
#endif
#endif
// Enable VUART trace: B0: basic frame data, B1: verbose frame data, B2: events
#define ASH_TRACE_FLAGS           7
// Define sliding window size: the maximum number of unacknowledged frames
// that can be outstanding before blocking further DATA frame transmission.
// Note that it must be the case that 1 <= K <= 7.
#define ASH_TX_K                  5
// Enable randomizing DATA frame payloads
#define ASH_RANDOMIZE          true
// Adaptive timeout values for newly-transmitted DATA frames to be ACK'ed
// Units are milliseconds. Values must be < 8192 to avoid overflow.
#define ASH_TIME_DATA_INIT     1600
#define ASH_TIME_DATA_MIN       400
#define ASH_TIME_DATA_MAX      3200
// Consecutive ACK timeouts (minus 1) needed to enter the ERROR state.
// The value 0xFF means the NCP will not disconnect due to timeouts.
#define ASH_MAX_TIMEOUTS          3
// Delay before sending RSTACK, needed by some RS-232 to TTL converters.
#ifdef SLEEPY_EZSP_ASH
  #define ASH_REBOOT_DELAY        0
#else
  #define ASH_REBOOT_DELAY       1000
#endif
// Preload value for the NR timer used in flow control. Since this timer
// is only one byte, care must be taken to avoid overflow. Each tick is
// 16 milliseconds, and the largest acceptable value is 127*16 = 2032 ms.
#define ASH_NR_TIME            1000

// Bits in traceFlags
#define TRACE_VUART_FRAMES_BASIC    1
#define TRACE_VUART_FRAMES_VERBOSE  2
#define TRACE_VUART_EVENTS          4

#endif //__ASH_NCP_H__
