/***************************************************************************//**
 * @file
 * @brief Host implementation of the interface described in serial-interface.h
 * using the SPI protocol.
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
#include "app/util/ezsp/ezsp-protocol.h"
#include "app/util/ezsp/ezsp.h"
#include "app/util/ezsp/serial-interface.h"
#include "app/util/ezsp/ezsp-frame-utilities.h"

#if defined(EMBER_TEST)
  #include "hal/micro/unix/simulation/spi-protocol.h"
#elif defined(UNIX_HOST)
  #include "hal/micro/unix/host/spi-protocol-common.h"
#else
  #include "spi-protocol-common.h"
#endif // EMBER_TEST

//------------------------------------------------------------------------------
// Global Variables

static bool waitingForResponse = false;
uint8_t *ezspFrameContents;
uint8_t *ezspFrameLengthLocation;

//------------------------------------------------------------------------------
// Serial Interface Downwards

EzspStatus ezspInit(void)
{
  ezspFrameLengthLocation = halNcpFrame;
  ezspFrameContents = halNcpFrame + 1;
  return halNcpHardReset();
}

bool ezspCallbackPending(void)
{
  if (!waitingForResponse) {
    return (halNcpHasData() || ncpHasCallbacks);
  } else {
    return false;
  }
}

void ezspWakeUp(void)
{
  halNcpWakeUp();
}

void ezspClose(void)
{
  // Nothing to do.
}

uint8_t serialPendingResponseCount(void)
{
  return 0;
}

EzspStatus serialResponseReceived(void)
{
  if (waitingForResponse) {
    EzspStatus status;
    status = halNcpPollForResponse();
    if (status != EZSP_SPI_WAITING_FOR_RESPONSE) {
      waitingForResponse = false;
    }
    return status;
  } else {
    return EZSP_SPI_WAITING_FOR_RESPONSE;
  }
}

EzspStatus serialSendCommand()
{
  halNcpSendCommand();
  waitingForResponse = true;
  return EZSP_SUCCESS;
}

uint8_t serialGetCommandLength(void)
{
  return *ezspFrameLengthLocation;
}

int serialGetSpiInterruptFd(void)
{
  return halNcpGetIntFd();
}
