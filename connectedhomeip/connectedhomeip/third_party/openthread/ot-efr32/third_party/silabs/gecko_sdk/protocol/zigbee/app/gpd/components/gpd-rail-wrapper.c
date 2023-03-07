/***************************************************************************//**
 * @file
 * @brief Routines built on RAIL functions used by the GPD.
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
#include "gpd-components-common.h"

static RAIL_Handle_t railHandle = NULL;
static bool     rfReady = false;

// This buffer is required to be RAIL_FIFO_ALIGNMENT aligned,
// which is 4 bytes for Series 2 and 1 byte for Series 1
static union {
  // Used to align this buffer as needed
  RAIL_FIFO_ALIGNMENT_TYPE align[GP_FIFO_SIZE / RAIL_FIFO_ALIGNMENT];
  uint8_t fifo[GP_FIFO_SIZE];
} txFifoAligned;
#define railTxFifo (txFifoAligned.fifo)

void sl_rail_util_on_event(RAIL_Handle_t railHandle,
                           RAIL_Events_t events)
{
  (void)railHandle;

  if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
    CORE_irqState_t c = CORE_EnterCritical();
    uint16_t bytesAvailable = RAIL_GetRxFifoBytesAvailable(railHandle);
    uint16_t rxReceived = RAIL_ReadRxFifo(railHandle, emberGpdGetRxMpdu(), bytesAvailable);
    emberGpdIncomingMessageHandler(emberGpdGetRxMpdu(), rxReceived);
    CORE_ExitCritical(c);
  }
}

void sl_rail_util_on_rf_ready(RAIL_Handle_t railHandle)
{
  (void)railHandle;
  rfReady = true;
}

void emberGpdRailProvideRailHandle(RAIL_Handle_t handle)
{
  railHandle = handle;
}

void emberGpdRailStartRxWrapper(uint8_t channel)
{
  RAIL_StartRx(railHandle, channel, NULL);
}

void emberGpdRailIdleWrapper(void)
{
  RAIL_Idle(railHandle, RAIL_IDLE, true);
}

void emberGpdRailStartTxWrapper(bool skipCca,
                                uint16_t channel)
{
  RAIL_Status_t status;
  if (skipCca) {
    status = RAIL_StartTx(railHandle,
                          channel,
                          RAIL_TX_OPTIONS_DEFAULT,
                          NULL);
  } else {
    RAIL_CsmaConfig_t txCsmaOptions = RAIL_CSMA_CONFIG_802_15_4_2003_2p4_GHz_OQPSK_CSMA;
    status = RAIL_StartCcaCsmaTx(railHandle,
                                 channel,
                                 RAIL_TX_OPTIONS_DEFAULT,
                                 &txCsmaOptions,
                                 NULL);
  }
  if (status != 0) {
    while (true) ; // Error - Trap it
  }
}

void emberGpdRailWriteTxFifoWrapper(const uint8_t *dataPtr,
                                    uint16_t writeLength)
{
  uint16_t writtenLength = RAIL_WriteTxFifo(railHandle,
                                            dataPtr,
                                            writeLength,
                                            true);
  if (writtenLength != writeLength) {
    while (true) ;
  }
}

uint16_t emberGpdRailGetRadioEntropyWrapper(uint8_t *dataPtr, uint16_t dataLength)
{
  return RAIL_GetRadioEntropy(railHandle, dataPtr, dataLength);
}

void emberGpdRadioInit(void)
{
  // Set TX FIFO, and verify that the size is correct
  uint16_t fifoSize = RAIL_SetTxFifo(railHandle, railTxFifo, 0, GP_FIFO_SIZE);
  if (fifoSize != GP_FIFO_SIZE) {
    while (true) ;
  }

  // Write the PAN ID as 0xFFFF
  if (RAIL_IEEE802154_SetPanId(railHandle, 0xFFFF, 0x00) != RAIL_STATUS_NO_ERROR) {
    while (true) ;
  }
#if defined(EMBER_AF_PLUGIN_APPS_APPLICATION_ID) && (EMBER_AF_PLUGIN_APPS_APPLICATION_ID == EMBER_GPD_APP_ID_IEEE_ID)
  uint8_t gpdIeee[] = EMBER_AF_PLUGIN_APPS_IEEE;
  uint8_t gpdEndpoint = EMBER_AF_PLUGIN_APPS_GPD_ENDPOINT;
  emberGpdAfPluginGetGpdIeeeCallback(gpdIeee,
                                     &gpdEndpoint);
  // Set Long address.
  if (RAIL_IEEE802154_SetLongAddress(railHandle, gpdIeee, 0x00) != RAIL_STATUS_NO_ERROR) {
    while (true) ;
  }
#endif

  while (!rfReady) ;
}
