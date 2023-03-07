/***************************************************************************//**
 * @file
 * @brief EZSP-USB protocol test
 *
 *  Due to the varied possible combinations of operating systems, serial port
 *  hardware and drivers, it is impossible to anticpate each host system's
 *  software requirements, so host system developers may need to modify the
 *  the functions in this file.
 *
 *  The POSIX API used here is portable and can run under either Linux or
 *  Cygwin/Windows, but an OS-specific API may offer more capabilities and
 *  better performance. Online information about the POSIX API can be found
 * (as of May 2007) at http://www.opengroup.org/
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
#ifndef __USE_MISC
  #define __USE_MISC // allow to define CSTART/CSTOP under std=c99
#endif
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "stack/include/ember-types.h"
#include "app/util/ezsp/ezsp-protocol.h"
#include "app/ezsp-host/ezsp-host-common.h"
#include "app/ezsp-host/ezsp-host-io.h"
#include "app/ezsp-host/ezsp-host-ui.h"

#ifndef STATIC_ASSERT
#ifdef __ICCARM__
  #define STATIC_ASSERT(__condition, __errorstr) \
  static_assert(__condition, __errorstr)
#elif defined(__GNUC__)
  #define STATIC_ASSERT(__condition, __errorstr) \
  _Static_assert(__condition, __errorstr)
#else
  #define STATIC_ASSERT(__condition, __errorstr)
#endif
#endif//STATIC_ASSERT

//------------------------------------------------------------------------------
// Preprocessor definitions

// Define the following symbols to add additional logging or test code to
// ezspSerialWriteByte() or ezspSerialReadByte()

//#define ENABLE_HOSTIO_DEBUG       // must define to enable any debug option
//#define IO_LOG "ezspuart.log"     // log serial data read or written to a file
//#define WR_BAD_RANDOM       2000  // corrupt write data with 1/N probablility
//#define WR_GAP_NTH            15  // insert gap in writing every Nth byte
//#define WR_GAP_TIME       100000  // duration of write gap in usecs
//#define RD_BAD_N_LOW         600  // corrupt bytes between low/high limits
//#define RD_BAD_N_HIGH        800
//#define RD_BAD_RANDOM       2000  // corrupt read data with 1/N probablility

// Define CRTSCTS for both ingoing and outgoing hardware flow control
// Try to resolve the numerous aliases for the bit flags
#if defined(CCTS_OFLOW) && defined(CRTS_IFLOW) && !defined(__NetBSD__)
  #undef CRTSCTS
  #define CRTSCTS (CCTS_OFLOW | CRTS_IFLOW)
#endif
#if defined(CTSFLOW) && defined(RTSFLOW)
  #undef CRTSCTS
  #define CRTSCTS (CTSFLOW | RTSFLOW)
#endif
#ifdef CNEW_RTSCTS
  #undef CRSTCTS
  #define CRTSCTS CNEW_RTSCTS
#endif
#ifndef CRTSCTS
  #define CRTSCTS 0
#endif

#define ASCII_XON   0x11
#define ASCII_XOFF  0x13

// Define the termios bit fields modified by ezspSerialInit
// (CREAD is omitted as it is often not supported by modern hardware)
#define CFLAG_MASK (CLOCAL | CSIZE | PARENB | HUPCL | CRTSCTS)
#define IFLAG_MASK (IXON | IXOFF | IXANY | BRKINT | INLCR | IGNCR | ICRNL \
                    | INPCK | ISTRIP | IMAXBEL)
#define LFLAG_MASK (ICANON | ECHO | IEXTEN | ISIG)
#define OFLAG_MASK (OPOST)

#define ERR_LEN               128   // max length error message

#define MAX_OUT_BLOCK_LEN     512   // maximum bytes to output at one time
#define MAX_IN_BLOCK_LEN      512   // maximum bytes to input at one time

#define BOOTLOADER_BAUD_RATE  115200
#define BOOTLOADER_STOP_BITS  1

//------------------------------------------------------------------------------
// Local Variables

static int serialFd = NULL_FILE_DESCRIPTOR; // file descriptor for serial port
static uint8_t outBuffer[MAX_OUT_BLOCK_LEN];  // array to buffer output
static uint8_t *outBufRd;                     // outBuffer read pointer
static uint8_t *outBufWr;                     // outBuffer write pointer
static uint16_t outBlockLen;                  // bytes to buffer before writing
static uint8_t inBuffer[MAX_IN_BLOCK_LEN];    // array to buffer input
static uint8_t *inBufRd;                      // inBuffer read pointer
static uint8_t *inBufWr;                      // inBuffer write pointer
static uint16_t inBlockLen;                   // bytes to read ahead

static EzspSerialPortCallbackFunction ezspSerialPortCallbackFunction = NULL;

#ifdef ENABLE_HOSTIO_DEBUG
#ifdef IO_LOG
int logCount = 0;
int logWrite = -1;
FILE *logFile;
#endif
#endif

static const struct { int bps; speed_t posixBaud; } baudTable[] =
{ { 600, B600    },
  { 1200, B1200   },
  { 2400, B2400   },
  { 4800, B4800   },
  { 9600, B9600   },
  { 19200, B19200  },
  { 38400, B38400  },
  { 57600, B57600  },
  { 115200, B115200 },
  { 230400, B230400 } };
#define BAUD_TABLE_SIZE (sizeof(baudTable) / sizeof(baudTable[0]) )

//------------------------------------------------------------------------------
// Forward Declarations
static EzspStatus ezspInternalSerialInit(bool ignoreErrors);

//------------------------------------------------------------------------------
// Functions

EzspStatus ezspSerialInit(void)
{
  EzspStatus status;

  status = ezspInternalSerialInit(false);
  if (status != EZSP_SUCCESS) {
    ezspSerialClose();
  }
  return status;
}

////////////////////////////////////////////////////////////////////////////////
// The ASH protocol requires the following serial I/O features:
//  o the ability to read and write "raw" binary data
//  o neither reads or writes may block
//  o flow control using either RTS/CTS or XON/XOFF
//
// Support for the following features is very desirable:
//  o full-duplex operation (simultaneous transmit and receive)
//
// If the EM2xx is setup for 115200 bps, then RTS/CTS must be used.
// At 57600 bps or below, either RTS/CTS or XON/XOFF may be used.
////////////////////////////////////////////////////////////////////////////////

EzspStatus ezspSetupSerialPort(int* serialPortFdReturn,
                               char* errorStringLocation,
                               int maxErrorLength,
                               bool bootloaderMode)
{
  // NOTE:  The bootloader does not support any flow control, so
  // we setup the serial port identical to the way ASH did it but
  // without flow control.  In addition, it only supports 115200 8-n-1

  uint8_t i;
  int bps;
  uint8_t stopBits;
  bool rtsCts;
  struct termios tios, checkTios;
  speed_t baud;
  bool flowControl = !bootloaderMode;

  bps = (bootloaderMode ? BOOTLOADER_BAUD_RATE : readConfig(baudRate));
  stopBits = (bootloaderMode ? BOOTLOADER_STOP_BITS : readConfig(stopBits));
  rtsCts = readConfig(rtsCts);

  // Setting this up front prevents us from closing an unset serial port FD
  // when breaking on error.
  *serialPortFdReturn = NULL_FILE_DESCRIPTOR;

  while (true) { // break out of while on any error
    // ensure BAUD_TABLE_SIZE is less than 'i' so we don't go out of bounds.
    STATIC_ASSERT(BAUD_TABLE_SIZE < 256, "BAUD_TABLE_SIZE must be less than 256");
    for (i = 0; i < BAUD_TABLE_SIZE; i++) {
      if (baudTable[i].bps == bps) {
        break;
      }
    }
    if (i < BAUD_TABLE_SIZE) {
      baud = baudTable[i].posixBaud;
    } else {
      snprintf(errorStringLocation, maxErrorLength, "Invalid baud rate %d bps\r\n", bps);
      break;
    }
    if ((stopBits != 1) && (stopBits != 2)) {
      snprintf(errorStringLocation, maxErrorLength, "Invalid number of stop bits\r\n");
      break;
    }

    ezspSerialClose();

    *serialPortFdReturn = open(readConfig(serialPort),
                               O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (*serialPortFdReturn == NULL_FILE_DESCRIPTOR) {
      snprintf(errorStringLocation,
               maxErrorLength,
               "Serial port %s open failed: %s\r\n",
               readConfig(serialPort),
               strerror(errno));
      fprintf(stdout, "Serial port open failed:%s\r\n", strerror(errno));
      break;
    }
    tcflush(*serialPortFdReturn, TCIOFLUSH);       // flush all input and output data
    fcntl(*serialPortFdReturn, F_SETFL, O_NONBLOCK);
    tcgetattr(*serialPortFdReturn, &tios);         // get current serial port options

    cfsetispeed(&tios, baud);
    cfsetospeed(&tios, baud);

    tios.c_cflag |= CLOCAL;               // ignore modem inputs
    tios.c_cflag |= CREAD;                // receive enable (a legacy flag)
    tios.c_cflag &= ~CSIZE;               // 8 data bits
    tios.c_cflag |= CS8;
    tios.c_cflag &= ~PARENB;              // no parity
    if (stopBits == 1) {
      tios.c_cflag &= ~CSTOPB;
    } else {
      tios.c_cflag |= CSTOPB;
    }
    if (flowControl && rtsCts) {
      tios.c_cflag |= CRTSCTS;
    } else {
      tios.c_cflag &= ~CRTSCTS;
    }

    tios.c_iflag &= ~(BRKINT | INLCR | IGNCR | ICRNL | INPCK
                      | ISTRIP | IMAXBEL | IXON | IXOFF | IXANY);

    if (flowControl && !rtsCts) {
      tios.c_iflag |= (IXON | IXOFF);          // SW flow control
    } else {
      tios.c_iflag &= ~(IXON | IXOFF);
    }

    tios.c_lflag &= ~(ICANON | ECHO | IEXTEN | ISIG);  // raw input

    tios.c_oflag &= ~OPOST;               // raw output

    (void) memset(tios.c_cc, _POSIX_VDISABLE, NCCS);  // disable all control chars
    tios.c_cc[VSTART] = CSTART;           // define XON and XOFF
    tios.c_cc[VSTOP] = CSTOP;

    ////////////////////////////////////////////////////////////////////////////
    // The POSIX standard states that when VMIN > 0 and VTIME == 0, read()
    // is supposed to block if it cannot return VMIN bytes. In fact,
    // if O_NONBLOCK is set, under Cygwin/WinXP read() does not block, but
    // instead sets errno to EAGAIN and returns -1.
    //
    // It is possible that under certain Linux or Embedded Linux variants
    // read() will block, and this code will have to be modified since
    // EZSP cannot function reliably with blocking serial I/O.
    //
    // Some alternative approaches in that case would include:
    //  o using Linux-specific ioctl()s
    //  o using select() on the serial port
    //  o spawning child processes to do serial I/O
    ////////////////////////////////////////////////////////////////////////////

    tios.c_cc[VMIN] = 1;
    tios.c_cc[VTIME] = 0;

    tcsetattr(*serialPortFdReturn, TCSAFLUSH, &tios);  // set EZSP serial port options
    tcgetattr(*serialPortFdReturn, &checkTios);      // and read back the result

    // Verify that the fields written have the right values
    i = (tios.c_cflag ^ checkTios.c_cflag) & CFLAG_MASK;
    if (i) {
      // Try again since macOS mojave seems to not have hardware flow control enabled
      tios.c_cflag &= ~CRTSCTS;
      tcsetattr(*serialPortFdReturn, TCSAFLUSH, &tios);  // set EZSP serial port options
      tcgetattr(*serialPortFdReturn, &checkTios);      // and read back the result
      i = (tios.c_cflag ^ checkTios.c_cflag) & CFLAG_MASK;
      if (i) {
        snprintf(errorStringLocation,
                 maxErrorLength,
                 "Termios cflag(s) in error: 0x%04X\r\n",
                 i);
        break;
      }
    }
    i = (tios.c_iflag ^ checkTios.c_iflag) & IFLAG_MASK;
    if (i) {
      snprintf(errorStringLocation,
               maxErrorLength,
               "Termios iflag(s) in error: 0x%04X\r\n",
               i);
      break;
    }
    i = (tios.c_lflag ^ checkTios.c_lflag) & LFLAG_MASK;
    if (i) {
      snprintf(errorStringLocation,
               maxErrorLength,
               "Termios lflag(s) in error: 0x%04X\r\n",
               i);
      break;
    }
    i = (tios.c_oflag ^ checkTios.c_oflag) & OFLAG_MASK;
    if (i) {
      snprintf(errorStringLocation,
               maxErrorLength,
               "Termios oflag(s) in error: 0x%04X\r\n",
               i);
      break;
    }
    for (i = 0; i < NCCS; i++) {
      if (tios.c_cc[i] != checkTios.c_cc[i]) {
        break;
      }
    }
    if (i != NCCS) {
      snprintf(errorStringLocation,
               maxErrorLength,
               "Termios error at c_cc[%d]\r\n",
               i);
      break;
    }
    if (  (cfgetispeed(&checkTios) != baud)
          || (cfgetospeed(&checkTios) != baud)) {
      snprintf(errorStringLocation,
               maxErrorLength,
               "Could not set baud rate to %d bps\r\n",
               bps);
      break;
    }
    return EZSP_SUCCESS;
  } // while (true)

  // Make sure the string is NULL terminated in case it got truncated.
  errorStringLocation[maxErrorLength - 1] = '\0';

  if (*serialPortFdReturn != NULL_FILE_DESCRIPTOR) {
    close(*serialPortFdReturn);
    *serialPortFdReturn = NULL_FILE_DESCRIPTOR;
  }

  return EZSP_HOST_FATAL_ERROR;
}

static EzspStatus ezspInternalSerialInit(bool ignoreErrors)
{
  char errStr[ERR_LEN];

#ifdef IO_LOG
  logFile = fopen(IO_LOG, "w");
#endif

  errStr[0] = '\0';

  outBufRd = outBuffer;
  outBufWr = outBuffer;
  outBlockLen = readConfig(outBlockLen);
  if (outBlockLen > MAX_OUT_BLOCK_LEN) {
    outBlockLen = MAX_OUT_BLOCK_LEN;
  }
  inBufRd = inBuffer;
  inBufWr = inBuffer;
  inBlockLen = readConfig(inBlockLen);
  if (inBlockLen > MAX_IN_BLOCK_LEN) {
    inBlockLen = MAX_IN_BLOCK_LEN;
  }

  // Make sure any previous file descriptor is nicely closed.
  // This should only be necessary during a failure recovery when the
  // host encountered an error.
  ezspSerialClose();

  if (EZSP_SUCCESS == ezspSetupSerialPort(&serialFd,
                                          errStr,
                                          ERR_LEN,
                                          false)) {  // bootloader mode?
    if (NULL != ezspSerialPortCallbackFunction) {
      ezspSerialPortCallbackFunction(EZSP_SERIAL_PORT_OPENED, serialFd);
    }
    return EZSP_SUCCESS;
  }

  if (ignoreErrors) {
    return EZSP_SUCCESS;
  }
  if (strlen(errStr)) {
    ezspTraceEvent(errStr);
  }
  ezspSerialClose();
  hostError = EZSP_ERROR_SERIAL_INIT;
  return EZSP_HOST_FATAL_ERROR;
}

void ezspSerialClose(void)
{
  if (serialFd != NULL_FILE_DESCRIPTOR) {
    tcflush(serialFd, TCIOFLUSH);
    close(serialFd);

    if (NULL != ezspSerialPortCallbackFunction) {
      ezspSerialPortCallbackFunction(EZSP_SERIAL_PORT_CLOSED, serialFd);
    }

    serialFd = NULL_FILE_DESCRIPTOR;
  }
}

// This function assumes that the ncp can be reset by deasserting DTR.
// Using the POSIX API, this is handled clumsily by closing and reopening
// the serial port. An OS-specific API, such ioctl() under Linux, will do
// this faster and more reliably.
void ezspResetDtr(void)
{
  ezspInternalSerialInit(true);
  ezspSerialClose();
}

// This function is a placeholder to reset the ncp using a GPIO output or
// something similar.
void ezspResetCustom(void)
{
}

//------------------------------------------------------------------------------
// Normal versions of serial I/O functions

#ifndef ENABLE_HOSTIO_DEBUG   // normal version of serial read/write functions

void ezspSerialWriteByte(uint8_t byte)
{
  BUMP_HOST_COUNTER(txBytes);
  *outBufWr++ = byte;
  if (outBufWr >= &outBuffer[outBlockLen]) {
    ezspSerialWriteFlush();
  }
}

EzspStatus ezspSerialReadByte(uint8_t *byte)
{
  EzspStatus status;
  uint16_t count;

  status = ezspSerialReadAvailable(&count);
  if (status == EZSP_SUCCESS) {
    *byte = *inBufRd++;
    BUMP_HOST_COUNTER(rxBytes);
  }
  return status;
}

#endif    // #ifndef ENABLE_HOSTIO_DEBUG

EzspStatus ezspSerialReadAvailable(uint16_t *count)
{
  int16_t bytesRead;
  if (inBufRd == inBufWr) {
    inBufRd = inBufWr = inBuffer;
    bytesRead = read(serialFd, inBuffer, inBlockLen);
    if (bytesRead > 0) {
      BUMP_HOST_COUNTER(rxBlocks);
      inBufWr += bytesRead;
    }
  }
  *count = inBufWr - inBufRd;
  if (inBufRd == inBufWr) {
    return EZSP_NO_RX_DATA;
  } else {
    return EZSP_SUCCESS;
  }
}

EzspStatus ezspSerialWriteAvailable(void)
{
  if ( (outBufWr < &outBuffer[outBlockLen]) && (outBufRd == outBuffer) ) {
    return EZSP_SUCCESS;
  } else {
    ezspSerialWriteFlush();
    return EZSP_NO_TX_SPACE;
  }
}

void ezspSerialWriteFlush(void)
{
  int16_t count;

  if (outBufWr - outBufRd) {
    BUMP_HOST_COUNTER(txBlocks);
    count = write(serialFd, outBufRd, outBufWr - outBufRd);
    if (count > 0) {
      outBufRd += count;
    }
    fsync(serialFd);
    if (outBufRd == outBufWr) {
      outBufRd = outBufWr = outBuffer;
    }
  }
}

void ezspSerialReadFlush(void)
{
  uint8_t byte;
  while (ezspSerialReadByte(&byte) == EZSP_SUCCESS)
    ;
}

void ezspDebugFlush(void)
{
  fflush(DEBUG_STREAM);
}

int ezspSerialGetFd(void)
{
  return serialFd;
}

///////////////////////////////////////////////////////////////////////////////
// NOTE:
// This function must be edited so that it returns true only when all of the
// data sent to the serial driver has been completely transmitted to the NCP.
// This that the serial transmit buffer, UART FIFO and sUART serializer register
// must all be empty.
// This requires driver or UART hardware level software and is dependent on the
// specific operating system/RTOS and UART hardware in use.
///////////////////////////////////////////////////////////////////////////////
bool ezspSerialOutputIsIdle(void)
{
  return true;  //replace with appropriate tests for actual OS and hardware
}

bool ezspSerialPortRegisterCallback(EzspSerialPortCallbackFunction callback)
{
  if (ezspSerialPortCallbackFunction != NULL) {
    return false;
  }
  ezspSerialPortCallbackFunction = callback;
  return true;
}

//------------------------------------------------------------------------------
// Debug versions of serial I/O functions

#ifdef ENABLE_HOSTIO_DEBUG    // debug version of serial read/write functions

void ezspSerialWriteByte(uint8_t byte)
{
  BUMP_HOST_COUNTER(txBytes);
#ifdef IO_LOG
  {
    if (logWrite != 1) {
      logWrite = 1;
      fprintf(logFile, "\r\nSERIAL OUT >>>>>>>>>>\r\n");
      logCount = 0;
    }

    if ( logCount % 16 == 0 ) {
      fprintf(logFile, "%04x: ", logCount / 16);
    }
    fprintf(logFile, "%02x ", byte);
    logCount++;
    if ( logCount % 16 == 0 ) {
      fprintf(logFile, "\r\n");
      fflush(logFile);
    }
  }
#endif

#ifdef WR_BAD_RANDOM
  {
    static bool srandCalled = false;
    if (!srandCalled) {
      srand(0);
      srandCalled = true;
    }
    if (rand() < RAND_MAX / WR_BAD_RANDOM) {
      byte++;
    }
  }
#endif

  // insert gaps in transmission to test ncp receive code
#if defined(WR_GAP_NTH) && defined(WR_GAP_TIME)
  {
    static int waitCounter = 0;
    if (++waitCounter > WR_GAP_NTH) {
      waitCounter = 0;
      usleep(WR_GAP_TIME);
    }
  }
#endif

  *outBufWr++ = byte;
  if (outBufWr >= &outBuffer[outBlockLen]) {
    ezspSerialWriteFlush();
  }
}

EzspStatus ezspSerialReadByte(uint8_t *byte)
{
  int16_t count;

  if (inBufRd == inBufWr) {
    inBufRd = inBufWr = inBuffer;
    count = read(serialFd, inBuffer, inBlockLen);
    if (count > 0) {
      BUMP_HOST_COUNTER(rxBlocks);
      inBufWr += count;
    }
  }
  if (inBufRd == inBufWr) {
    return EZSP_NO_RX_DATA;
  }
  BUMP_HOST_COUNTER(rxBytes);
  *byte = *inBufRd++;

#ifdef IO_LOG
  {
    if ( logWrite != 0 ) {
      logWrite = 0;
      logCount = 0;
      fprintf(logFile, "\r\nSERIAL IN <<<<<<<<<<\r\n");
    }
    if ( logCount % 16 == 0 ) {
      fprintf(logFile, "%04x: ", logCount / 16);
    }
    fprintf(logFile, "%02x ", *byte);
    logCount++;
    if ( logCount % 16 == 0 ) {
      fprintf(logFile, "\r\n");
      fflush(logFile);
    }
  }
#endif

#if defined(RD_BAD_N_LOW) && defined(RD_BAD_N_HIGH)
  {
    static uint32_t count = 0;
    ++count;
    if ( (count >= RD_BAD_N_LOW) && (count <= RD_BAD_N_HIGH) ) {
      (*byte)++;
      if (!readConfig(rtsCts)
          && ((*byte == ASCII_XON) || (*byte == ASCII_XOFF)) ) {
        (*byte)++;
      }
    }
  }
#endif

#ifdef RD_BAD_RANDOM
  {
    static bool srandCalled = false;
    if (!srandCalled) {
      srand(0);
      srandCalled = true;
    }
    if (rand() < RAND_MAX / RD_BAD_RANDOM) {
      (*byte)++;
      if (!readConfig(rtsCts)
          && ((*byte == ASCII_XON) || (*byte == ASCII_XOFF)) ) {
        (*byte)++;
      }
    }
  }
#endif

  return EZSP_SUCCESS;
}

#endif  // #ifdef ENABLE_HOSTIO_DEBUG
