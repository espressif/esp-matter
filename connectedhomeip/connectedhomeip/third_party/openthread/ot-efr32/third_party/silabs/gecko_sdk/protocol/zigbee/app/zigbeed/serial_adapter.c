/***************************************************************************//**
 * @file
 * @brief Adaptation of "emberSerial" API to posix target.
 *
 * See @ref
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

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "ember-types.h"
#include "serial_adapter.h"
#include "openthread/openthread-system.h"
#include "stack/include/ember.h"
#include "app/em260/serial-interface.h"
#include "app/framework/common/zigbee_app_framework_event.h"

#define NULL_FILE_DESCRIPTOR  (-1)
#define MAX_OUT_BLOCK_LEN     512   // maximum bytes to output at one time
#define MAX_IN_BLOCK_LEN      512   // maximum bytes to input at one time

#define DEFAULT_SERIAL_PORT "/tmp/ttyZigbeed"
#define DEFAULT_OUT_BLOCK_LEN 1
#define DEFAULT_IN_BLOCK_LEN 256
#define SERIAL_PORT_NAME_MAX_LEN 40

char serialPort[SERIAL_PORT_NAME_MAX_LEN] = DEFAULT_SERIAL_PORT;

static int serialFd = NULL_FILE_DESCRIPTOR;   // file descriptor for serial port
static uint8_t outBuffer[MAX_OUT_BLOCK_LEN];  // array to buffer output
static uint8_t *outBufRd;                     // outBuffer read pointer
static uint8_t *outBufWr;                     // outBuffer write pointer
static uint16_t outBlockLen;                  // bytes to buffer before writing
static uint8_t inBuffer[MAX_IN_BLOCK_LEN];    // array to buffer input
static uint8_t *inBufRd;                      // inBuffer read pointer
static uint8_t *inBufWr;                      // inBuffer write pointer
static uint16_t inBlockLen;                   // bytes to read ahead

static void serialClose(void)
{
  if (serialFd != NULL_FILE_DESCRIPTOR) {
    tcflush(serialFd, TCIOFLUSH);
    close(serialFd);
    serialFd = NULL_FILE_DESCRIPTOR;
  }
}

static void writeFlush(void)
{
  int16_t count;

  if (outBufWr - outBufRd) {
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

static EmberStatus readAvailable(uint16_t *count)
{
  int16_t bytesRead;
  if (inBufRd == inBufWr) {
    inBufRd = inBufWr = inBuffer;
    bytesRead = read(serialFd, inBuffer, inBlockLen);
    if (bytesRead > 0) {
      inBufWr += bytesRead;
    }
  }
  *count = inBufWr - inBufRd;
  if (inBufRd == inBufWr) {
    return EMBER_SERIAL_RX_EMPTY;
  } else {
    return EMBER_SUCCESS;
  }
}

//------------------------------------------------------------------------------
// emberSerial Functions

EmberStatus emberSerialInit(uint8_t port,
                            SerialBaudRate rate,
                            SerialParity parity,
                            uint8_t stopBits)
{
#ifdef IO_LOG
  logFile = fopen(IO_LOG, "w");
#endif
  struct termios tios;
  outBufRd = outBuffer;
  outBufWr = outBuffer;
  outBlockLen = DEFAULT_OUT_BLOCK_LEN;
  if (outBlockLen > MAX_OUT_BLOCK_LEN) {
    outBlockLen = MAX_OUT_BLOCK_LEN;
  }
  inBufRd = inBuffer;
  inBufWr = inBuffer;
  inBlockLen = DEFAULT_IN_BLOCK_LEN;
  if (inBlockLen > MAX_IN_BLOCK_LEN) {
    inBlockLen = MAX_IN_BLOCK_LEN;
  }

  // Make sure any previous file descriptor is nicely closed.
  // This should only be necessary during a failure recovery when the
  // host encountered an error.
  serialClose();

  serialFd = open(serialPort,
                  O_RDWR | O_NOCTTY | O_NONBLOCK);

  if (serialFd == NULL_FILE_DESCRIPTOR) {
    fprintf(stderr, "Failed to open %s: %s\r\n", serialPort, strerror(errno));
    fprintf(stderr, "Use socat to create PTYs for zigbeed and the host app. Eg:\r\n");
    fprintf(stderr, "socat -x -v pty,link=/dev/ttyZigbeeNCP pty,link=/tmp/ttyZigbeeNCP\r\n");
    serialClose();
    return EMBER_ERR_FATAL;
  }

  tcflush(serialFd, TCIOFLUSH);       // flush all input and output data
  fcntl(serialFd, F_SETFL, O_NDELAY);
  tcgetattr(serialFd, &tios);         // get current serial port options

  tios.c_iflag &= ~(BRKINT | INLCR | IGNCR | ICRNL | INPCK
                    | ISTRIP | IMAXBEL | IXON | IXOFF | IXANY);

  tios.c_lflag &= ~(ICANON | ECHO | IEXTEN | ISIG);  // raw input

  tios.c_oflag &= ~OPOST;               // raw output

  (void) memset(tios.c_cc, _POSIX_VDISABLE, NCCS);  // disable all control chars

  tcsetattr(serialFd, TCSAFLUSH, &tios);  // set EZSP serial port options

  return EMBER_SUCCESS;
}

EmberStatus emberSerialWriteByte(uint8_t port, uint8_t dataByte)
{
  *outBufWr++ = dataByte;
  if (outBufWr >= &outBuffer[outBlockLen]) {
    writeFlush();
  }
  return EMBER_SUCCESS;
}

uint16_t emberSerialWriteAvailable(uint8_t port)
{
  if ( (outBufWr < &outBuffer[outBlockLen]) && (outBufRd == outBuffer) ) {
    return 1;
  } else {
    writeFlush();
    return 0;
  }
}

EmberStatus emberSerialReadByte(uint8_t port, uint8_t *dataByte)
{
  EmberStatus status;
  uint16_t count;

  status = readAvailable(&count);
  if (status == EMBER_SUCCESS) {
    *dataByte = *inBufRd++;
  }
  return status;
}

// from serial/serial.c
// needed by sl_legacy_printf.c
EmberStatus emberSerialWriteString(uint8_t port, PGM_P string)
{
  return EMBER_SUCCESS;
}

void sli_serial_adapter_tick_callback(void)
{
  otSysMainloopContext mainloop;
  uint32_t timeoutMs = emberMsToNextStackEvent();
  uint32_t appMs = sli_zigbee_ms_to_next_app_framework_event();
  timeoutMs = (timeoutMs < appMs ? timeoutMs : appMs);

  // Clear mainloop FDs
  FD_ZERO(&mainloop.mReadFdSet);
  FD_ZERO(&mainloop.mWriteFdSet);
  FD_ZERO(&mainloop.mErrorFdSet);

  // Update mainloop initial FD and its timeout value
  mainloop.mMaxFd           = serialFd;
  mainloop.mTimeout.tv_sec  = timeoutMs / 1000;
  mainloop.mTimeout.tv_usec = (timeoutMs - mainloop.mTimeout.tv_sec * 1000) * 1000;

  // Update mainloop FDs to monitor
  FD_SET(serialFd, &mainloop.mReadFdSet);
  FD_SET(serialFd, &mainloop.mErrorFdSet);
  otSysMainloopUpdate(NULL, &mainloop);

  if (otSysMainloopPoll(&mainloop) >= 0) {
    otSysMainloopProcess(NULL, &mainloop);
  } else if (errno != EINTR) {
    //printf("%d\n", errno);
    assert(false);
  }
}
