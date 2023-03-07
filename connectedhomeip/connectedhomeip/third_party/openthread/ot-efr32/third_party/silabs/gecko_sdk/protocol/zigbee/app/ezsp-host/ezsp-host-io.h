/***************************************************************************//**
 * @file
 * @brief Header for EZSP host I/O functions
 *
 * See @ref ezsp_util for documentation.
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

#ifndef SILABS_EZSP_HOST_IO_H
#define SILABS_EZSP_HOST_IO_H

/** @addtogroup ezsp_util
 *
 * See ezsp-host-io.h.
 *
 *@{
 */

/** @brief Initializes the serial port for use by EZSP. The port number,
 *  baud rate, stop bits, and flow control method are specifed by the
 *  by the host configuration.
 *
 * @return
 * - ::EZSP_SUCCESS
 * - ::EZSP_HOST_FATAL_ERROR
 */
EzspStatus ezspSerialInit(void);

/** @brief If the serial port is open, discards all I/O data
 *  and closes the port.
 */
void ezspSerialClose(void);

/** @brief Resets the ncp by deasserting and asserting DTR.
 *  This requires a conenction between DTR and nRESET, as there is on the
 *  EM260 breakout board when the on-board USB interface is used.
 */
void ezspResetDtr(void);

/** @brief Custom method for resetting the ncp which must be defined by
 *  the user for their specific hardware and interconect. As shipped, this
 *  function does nothing.
 */
void ezspResetCustom(void);

/** @brief Checks to see if there is space available in the serial
 *  write buffer. If the buffer is full, it is output to the serial port
 *  and it return a "no space indication".
 *
 * @return
 * - ::EZSP_SUCCESS
 * _ ::EZSP_NO_TX_SPACE
 */
EzspStatus ezspSerialWriteAvailable(void);

/** @brief Writes a byte to the serial output buffer.
 *
 * @param byte byte to write
 */
void ezspSerialWriteByte(uint8_t byte);

/** @brief Writes all data the write output buffer to the serial port
 *  and calls fsync(). This is called when a complete frame to be sent to
 *  the ncp has been created.
 */
void ezspSerialWriteFlush(void);

/** @brief Reads a byte from the serial port, if one is available.
 *
 * @param byte pointer to a variable where the byte read will be output
 *
 * @return
 * - ::EZSP_SUCCESS
 * - ::EZSP_NO_RX_DATA
 */
EzspStatus ezspSerialReadByte(uint8_t *byte);

/** @brief Returns number of the bytes available to read from the serial port.
 *
 * @param count pointer to a variable where the byte count will be written
 *
 * @return
 * - ::EZSP_SUCCESS
 * - ::EZSP_NO_RX_DATA
 */
EzspStatus ezspSerialReadAvailable(uint16_t *count);

/** @brief Discards input data from the serial port until there
 *  is none left.
 */
void ezspSerialReadFlush(void);

/** @brief Flushes the ASCII trace output stream.
 */
void ezspDebugFlush(void);

/** @brief Prints ACSII trace information.
 */
#define DEBUG_STREAM  stdout

#ifdef WIN32
  #define ezspDebugPrintf printf
#else
  #define ezspDebugPrintf(...) fprintf(DEBUG_STREAM, __VA_ARGS__)
#endif

#define ezspDebugVfprintf(format, argPointer) \
  vfprintf(DEBUG_STREAM, format, argPointer)

/** @brief Returns the file descriptor associated with the serial port.
 */
int ezspSerialGetFd(void);

/** @brief An enumeration of events that can occur on the serial port.
 */
typedef enum {
  EZSP_SERIAL_PORT_CLOSED = 0,
  EZSP_SERIAL_PORT_OPENED = 1,
} EzspSerialPortEvent;

/** @brief Register a callback that will get called whenever the EZSP serial
 *  file descriptor is opened or closed.
 */
typedef void (*EzspSerialPortCallbackFunction)(EzspSerialPortEvent event, int fileDescriptor);
bool ezspSerialPortRegisterCallback(EzspSerialPortCallbackFunction callback);

/** @brief tests to see if all serial transmit data has actually been shifted
 *  out the host's serial port transmit data pin.
 *  As shipped this is a stub function that must be edited to match the actual
 *  operating system and/or UART hardware.
 * @return  true if all data has been shifted out.
 */
bool ezspSerialOutputIsIdle(void);

#endif //__EZSP_HOST_H__

#if !defined(DOXYGEN_SHOULD_SKIP_THIS)
EzspStatus ezspSetupSerialPort(int* serialPortFdReturn,
                               char* errorStringLocation,
                               int maxErrorLength,
                               bool bootloaderMode);
#endif

/** @brief The classic bad file descriptor.
 */
#define NULL_FILE_DESCRIPTOR  (-1)

/** @} END addtogroup
 */
