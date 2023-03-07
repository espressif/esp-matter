/***************************************************************************//**
 * @file
 * @brief SoC specific code related to the event table.
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

#include "../../include/af.h"
#include "../../util/common.h"
#include "load-control-event-table.h"

// necessary for supporting ECDSA
#include "stack/include/cbke-crypto-engine.h"

#include "app/framework/security/crypto-state.h"

#ifdef UC_BUILD
void emAfDemandResponseLoadControlClusterDsaSignCallback(EmberStatus status, EmberMessageBuffer message)
#else // !UC_BUILD
void emberDsaSignHandler(EmberStatus status, EmberMessageBuffer message)
#endif // UC_BUILD
{
  // Message has been queued by the stack for sending.  Nothing more to do.
  emAfCryptoOperationComplete();

  if (status != EMBER_SUCCESS) {
    emAfNoteSignatureFailure();
  }

  emberAfDemandResponseLoadControlClusterPrintln("emberDsaSignHandler() returned 0x%x",
                                                 status);
}
