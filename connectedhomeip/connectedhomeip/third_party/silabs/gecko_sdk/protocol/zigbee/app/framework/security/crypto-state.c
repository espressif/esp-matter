/***************************************************************************//**
 * @file
 * @brief This file records the state of crypto operations so that the application
 * can defer processing until after crypto operations have completed.
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

#include "app/framework/util/common.h"
#include "crypto-state.h"

//------------------------------------------------------------------------------

static EmAfCryptoStatus cryptoStatus = EM_AF_NO_CRYPTO_OPERATION;
#if defined(EMBER_AF_PRINT_ENABLE) && defined(EMBER_AF_PRINT_SECURITY)
static const char * cryptoStatusText[] = EM_AF_CRYPTO_STATUS_TEXT;
#endif //defined(EMBER_AF_PRINT_ENABLE) && defined(EMBER_AF_PRINT_SECURITY)

// If we are on a host micro, there is the possibility that a crypto
// operation ends on the NCP and we fail to hear about it, in this case
// we need a timeout to safeguard against this flag getting locked
#if defined(EZSP_HOST) && !defined(UC_BUILD)
EmberEventControl emAfCryptoOperationTimeoutEventControl;
void emAfCryptoOperationTimeoutEvent(void)
{
  if (cryptoStatus == EM_AF_CRYPTO_OPERATION_IN_PROGRESS) {
    emAfSetCryptoStatus(EM_AF_NO_CRYPTO_OPERATION);
  }
}
EmberEventData emAfCryptoEvents[] = {
  { &emAfCryptoOperationTimeoutEventControl, emAfCryptoOperationTimeoutEvent },
  { NULL, NULL }
};
#endif //EZSP_HOST

//------------------------------------------------------------------------------

EmAfCryptoStatus emAfGetCryptoStatus(void)
{
#if defined(EZSP_HOST) && !defined(UC_BUILD)
  // Everytime someone asks for the crypto state, we check our
  // crypto timeout
  emberRunEvents(emAfCryptoEvents);
#endif //EZSP_HOST

  return cryptoStatus;
}

void emAfSetCryptoStatus(EmAfCryptoStatus newStatus)
{
  cryptoStatus = newStatus;
#if defined(EMBER_AF_PRINT_ENABLE) && defined(EMBER_AF_PRINT_SECURITY)
  emberAfSecurityPrintln("Crypto state: %p", cryptoStatusText[cryptoStatus]);
#endif // defined(EMBER_AF_PRINT_ENABLE) && defined(EMBER_AF_PRINT_SECURITY)

#if defined(EZSP_HOST) && !defined(UC_BUILD)
  // If crypto is starting, we set the timeout, otherwise we cancel it.
  if (cryptoStatus == EM_AF_CRYPTO_OPERATION_IN_PROGRESS) {
    emberEventControlSetDelayMS(emAfCryptoOperationTimeoutEventControl,
                                CRYPTO_OPERATION_TIMEOUT_MS);
  } else {
    emberEventControlSetInactive(emAfCryptoOperationTimeoutEventControl);
  }
#endif //EZSP_HOST
}
