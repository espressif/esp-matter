/***************************************************************************//**
 * @file
 * @brief Code to display or retrieve the presence or absence of
 * Ember stack libraries on the device.
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

#if defined EZSP_HOST
  #include "stack/include/ember-types.h"
  #include "stack/include/error.h"
  #include "hal/hal.h"
  #include "app/util/ezsp/ezsp-protocol.h"
  #include "app/util/ezsp/ezsp.h"
  #include "stack/include/library.h"
#else
  #include "stack/include/ember.h"
  #include "hal/hal.h"
#endif

#ifdef UC_BUILD
#include "serial/serial.h"
#else
#include "plugin/serial/serial.h"
#endif
#include "app/util/common/common.h"

static const char * libraryNames[] = {
  EMBER_LIBRARY_NAMES
};

#if defined(UC_BUILD) && !defined(PRO_COMPLIANCE)
uint8_t serialPort = APP_SERIAL;
#endif

void printAllLibraryStatus(SL_CLI_COMMAND_ARG)
{
  uint8_t i = EMBER_FIRST_LIBRARY_ID;
  while (i < EMBER_NUMBER_OF_LIBRARIES) {
    EmberLibraryStatus status = emberGetLibraryStatus(i);
    if (status == EMBER_LIBRARY_ERROR) {
      (void) emberSerialPrintfLine(serialPort, "Error retrieving info for library ID %d",
                                   i);
    } else {
      (void) emberSerialPrintfLine(serialPort,
        #ifdef UC_BUILD
                                   "%s library%s present",
        #else
                                   "%p library%p present",
        #endif
                                   libraryNames[i],
                                   ((status & EMBER_LIBRARY_PRESENT_MASK)
                                    ? ""
                                    : " NOT"));
      if ((status & EMBER_LIBRARY_PRESENT_MASK) != 0U) {
        if (i == EMBER_ZIGBEE_PRO_LIBRARY_ID
            || i == EMBER_SECURITY_CORE_LIBRARY_ID) {
          (void) emberSerialPrintfLine(serialPort,
                                       ((status
                                         & EMBER_ZIGBEE_PRO_LIBRARY_HAVE_ROUTER_CAPABILITY)
                                        ? "  Have Router Support"
                                        : "  End Device Only"));
        }
        if (i == EMBER_PACKET_VALIDATE_LIBRARY_ID
            && (status
                & EMBER_LIBRARY_PRESENT_MASK)) {
          (void) emberSerialPrintfLine(serialPort,
                                       ((status & EMBER_PACKET_VALIDATE_LIBRARY_ENABLED)
                                        ? "  Enabled"
                                        : "  Disabled"));
        }
      }
    }
    (void) emberSerialWaitSend(serialPort);
    i++;
  }
}

bool isLibraryPresent(uint8_t libraryId)
{
  EmberLibraryStatus status = emberGetLibraryStatus(libraryId);
  return (status != EMBER_LIBRARY_ERROR
          && (status & EMBER_LIBRARY_PRESENT_MASK));
}
