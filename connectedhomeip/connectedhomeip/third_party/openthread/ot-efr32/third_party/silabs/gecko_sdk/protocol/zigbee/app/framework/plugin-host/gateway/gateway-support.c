/***************************************************************************//**
 * @file
 * @brief Gateway specific behavior for a host application.
 *   In this case we assume our application is running on
 *   a PC with Unix library support, connected to an NCP via serial uart.
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
#include "app/ezsp-host/ezsp-host-common.h"
#include "app/ezsp-host/ezsp-host-io.h"
#include "app/ezsp-host/ezsp-host-ui.h"

#include "app/framework/util/af-event.h"
#include "app/util/ezsp/serial-interface.h"
#include "app/util/serial/sl_zigbee_command_interpreter.h"
#include "app/util/serial/linux-serial.h"
#include "app/framework/plugin-host/gateway/gateway-support.h"
#ifdef UC_BUILD
#include "gateway-config.h"
#endif // UC_BUILD

#include "app/framework/plugin-host/file-descriptor-dispatch/file-descriptor-dispatch.h"

#include <sys/time.h>   // for select()
#include <sys/types.h>  // ""
#include <unistd.h>     // ""
#include <errno.h>      // ""
#include <stdio.h>

//------------------------------------------------------------------------------
// Globals

// If the application wishes to limit how long the select() call will yield
// for, they can do it by specifying a max timeout.  This may be necessary
// if the main() loop expects to be serviced at some regular interval.
// Ideally the application code can use an event, but it is easier to
// tune it this way.  0xFFFFFFFFUL = no read timeout, thus allowing the
// select() call to yield forever if there are no events scheduled.
#define MAX_READ_TIMEOUT_MS  EMBER_AF_PLUGIN_GATEWAY_MAX_WAIT_FOR_EVENT_TIMEOUT_MS
#define MAX_FDS              EMBER_AF_PLUGIN_GATEWAY_MAX_FDS
#define INVALID_FD           -1

static const char* debugLabel = "gateway-debug";
static const bool debugOn = false;

static const char cliPrompt[] = "foo";//was ZA_PROMPT;

//------------------------------------------------------------------------------
// External Declarations

//------------------------------------------------------------------------------
// Forward Declarations
int emberAfPluginGatewaySelectFileDescriptorsCallback(int*list, int maxSize);

static void debugPrint(const char* formatString, ...);
static void ezspFileDescriptorReadyCallback(void* data, EmberAfFileDescriptorOperation operation);
static void ezspSerialPortCallback(EzspSerialPortEvent event, int fileDescriptor);

//------------------------------------------------------------------------------
// Functions

static EmberStatus gatewayBackchannelStart(void)
{
  if (backchannelEnable) {
    if (EMBER_SUCCESS != backchannelStartServer(SERIAL_PORT_CLI)) {
      fprintf(stderr,
              "Fatal: Failed to start backchannel services for CLI.\n");
      return EMBER_ERR_FATAL;
    }

    if (EMBER_SUCCESS != backchannelStartServer(SERIAL_PORT_RAW)) {
      fprintf(stderr,
              "Fatal: Failed to start backchannel services for RAW data.\n");
      return EMBER_ERR_FATAL;
    }
  }
  return EMBER_SUCCESS;
}

void gatewayBackchannelStop(void)
{
  if (backchannelEnable) {
    backchannelStopServer(SERIAL_PORT_CLI);
    backchannelStopServer(SERIAL_PORT_RAW);
  }
}

bool emberAfMainStartCallback(int* returnCode,
                              int argc,
                              char** argv)
{
  debugPrint("gatewaitInit()");

  // This will process EZSP command-line options as well as determine
  // whether the backchannel should be turned on.
  if (!ezspProcessCommandOptions(argc, argv)) {
    *returnCode = EMBER_ERR_FATAL;
    return true;
  }

  *returnCode = gatewayBackchannelStart();
  if (*returnCode != EMBER_SUCCESS) {
    return true;
  }

  emberSerialSetPrompt(cliPrompt);

#ifndef UC_BUILD
  emberSerialCommandCompletionInit(emberCommandTable);
#endif
  return false;
}

#ifndef UC_BUILD
static void debugPrintYieldDuration(uint32_t msToNextEvent, uint8_t eventIndex)
{
// Temporarily stub to enable build
  if (msToNextEvent == 0xFFFFFFFFUL) {
    debugPrint("Yield forever (or until FD is ready)");
  } else if (msToNextEvent > MAX_READ_TIMEOUT_MS) {
    debugPrint("Yield %d ms until MAX_READ_TIMEOUT_MS", msToNextEvent);
  } else {
    debugPrint("Yield %d ms until %s event ",
               msToNextEvent,
               emberAfGetEventString(eventIndex));
  }
}
#endif // UC_BUILD

void emberAfPluginGatewayTickCallback(void)
{
  // If the CLI process is waiting for the 'go-ahead' to prompt the user
  // and read input, we need to tell it to do that before going to sleep
  // (potentially indefinitely) via select().
#ifndef EZSP_HOST
  emberSerialSendReadyToRead(APP_SERIAL);
#endif

  uint8_t index;
  uint32_t msToNextEvent = emberAfMsToNextEventExtended(0xFFFFFFFFUL, &index);
#ifndef UC_BUILD
  debugPrintYieldDuration(msToNextEvent, index);
#endif
  msToNextEvent = (msToNextEvent > MAX_READ_TIMEOUT_MS
                   ? MAX_READ_TIMEOUT_MS
                   : msToNextEvent);

  emberAfPluginFileDescriptorDispatchWaitForEvents(msToNextEvent);
}

void emberAfPluginGatewayInitCallback(SLXU_INIT_ARG)
{
  int fdList[MAX_FDS];
  int count = 0;
  int i;

  EmberAfFileDescriptorDispatchStruct dispatchStruct = {
    NULL, // callback
    NULL, // data passed to callback
    EMBER_AF_FILE_DESCRIPTOR_OPERATION_READ,
    -1,
  };
  dispatchStruct.fileDescriptor = emberSerialGetInputFd(0);
  if (dispatchStruct.fileDescriptor != -1
      && EMBER_SUCCESS != emberAfPluginFileDescriptorDispatchAdd(&dispatchStruct)) {
    emberAfCorePrintln("Error: Gateway Plugin failed to register serial Port 0 FD");
  }
  dispatchStruct.fileDescriptor = emberSerialGetInputFd(1);
#if defined(ZA_CLI_FULL)
  if (dispatchStruct.fileDescriptor != -1
      && EMBER_SUCCESS != emberAfPluginFileDescriptorDispatchAdd(&dispatchStruct)) {
    emberAfCorePrintln("Error: Gateway Plugin failed to register serial Port 1 FD");
  }
#endif

#ifdef EZSP_SPI
  // For SPI, we need the nHOST_INT line as well
  EmberAfFileDescriptorDispatchStruct spiDispatchStruct = {
    NULL, // callback
    NULL, // data passed to callback
    EMBER_AF_FILE_DESCRIPTOR_OPERATION_EXCEPT,
    -1,
  };
  spiDispatchStruct.fileDescriptor = serialGetSpiFd();
  if (spiDispatchStruct.fileDescriptor != -1
      && EMBER_SUCCESS != emberAfPluginFileDescriptorDispatchAdd(&spiDispatchStruct)) {
    emberAfCorePrintln("Error: Gateway Plugin failed to register SPI FD");
  }
#endif // EZSP_SPI

  ezspSerialPortRegisterCallback(ezspSerialPortCallback);
  if (ezspSerialGetFd() != NULL_FILE_DESCRIPTOR) {
    ezspSerialPortCallback(EZSP_SERIAL_PORT_OPENED, ezspSerialGetFd());
  }

  MEMSET(fdList, 0xFF, sizeof(int) * MAX_FDS);
  count = emberAfPluginGatewaySelectFileDescriptorsCallback(fdList, MAX_FDS);
  for (i = 0; i < count; i++) {
    dispatchStruct.fileDescriptor = fdList[i];
    if (EMBER_SUCCESS != emberAfPluginFileDescriptorDispatchAdd(&dispatchStruct)) {
      emberAfCorePrintln("Error: Gateway plugin failed to add FD %d for watching.", fdList[i]);
    }
  }
}

static void debugPrint(const char* formatString, ...)
{
  if (debugOn) {
    va_list ap;
    fprintf(stderr, "[%s] ", debugLabel);
    va_start(ap, formatString);
    vfprintf(stderr, formatString, ap);
    va_end(ap);
    fprintf(stderr, "\n");
    fflush(stderr);
  }
}

// EMZIGBEE-2126 case where host/NCP USB disconnect causes host CPU spike.
// Withdraw EZSP file descriptor from select polling after 10 consecutive
// "ready" polls with no data actually available.
#define MAX_CONSECUTIVE_TIMES_NO_EZSP_DATA (10)
static uint8_t consecutiveTimesEzspNoData = 0;
static void ezspFileDescriptorReadyCallback(void* data, EmberAfFileDescriptorOperation operation)
{
  if (operation == EMBER_AF_FILE_DESCRIPTOR_OPERATION_READ) {
    uint16_t count = 0;
    if (ezspSerialReadAvailable(&count) == EZSP_SUCCESS && count > 0) {
      consecutiveTimesEzspNoData = 0; // reset
    } else {
      ++consecutiveTimesEzspNoData;
    }
    if (consecutiveTimesEzspNoData >= MAX_CONSECUTIVE_TIMES_NO_EZSP_DATA) {
      // Assume EZSP connection is broken, withdraw file descriptor (data)
      emberAfPluginFileDescriptorDispatchRemove((int) data);
    }
  }
}

static void ezspSerialPortCallback(EzspSerialPortEvent event, int fileDescriptor)
{
  assert(fileDescriptor != NULL_FILE_DESCRIPTOR);

  if (event == EZSP_SERIAL_PORT_CLOSED) {
    debugPrint("EZSP serial port closed.  FD=%d", fileDescriptor);
    emberAfPluginFileDescriptorDispatchRemove(fileDescriptor);
  } else if (event == EZSP_SERIAL_PORT_OPENED) {
    EmberAfFileDescriptorDispatchStruct dispatchStruct = {
      ezspFileDescriptorReadyCallback,   // callback
      (void *) fileDescriptor,   // data passed to callback
      EMBER_AF_FILE_DESCRIPTOR_OPERATION_READ,
      fileDescriptor,
    };

    debugPrint("Registered EZSP FD %d", fileDescriptor);
    if (EMBER_SUCCESS != emberAfPluginFileDescriptorDispatchAdd(&dispatchStruct)) {
      emberAfCorePrintln("Error: Gateway Plugin failed to register EZSP FD %d", fileDescriptor);
    }
  }
}
