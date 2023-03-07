/***************************************************************************//**
 * @file
 * @brief Generic code related to the receipt and processing of interpan
 * messages.
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

#include "af.h"
#include "interpan.h"

/** @brief Pre Message Received
 *
 * This function is called by the Interpan plugin when an interpan message is
 * received but has not yet been handled by the plugin or the framework. The
 * application should return true if the message was handled.
 *
 * @param header The inter-PAN header Ver.: always
 * @param msgLen The message payload length Ver.: always
 * @param message The message payload Ver.: always
 */
WEAK(bool emberAfPluginInterpanPreMessageReceivedCallback(const EmberAfInterpanHeader *header,
                                                          uint8_t msgLen,
                                                          uint8_t *message))
{
  return false;
}

/** @brief Message Received Over Fragments
 *
 * This function is called by the Interpan plugin when a fully reconstructed
 * message has been received over inter-PAN fragments, or IPMFs.
 *
 * @param header The inter-PAN header Ver.: always
 * @param msgLen The message payload length Ver.: always
 * @param message The message payload Ver.: always
 */
WEAK(void emberAfPluginInterpanMessageReceivedOverFragmentsCallback(const EmberAfInterpanHeader *header,
                                                                    uint8_t msgLen,
                                                                    uint8_t *message))
{
}

/** @brief Fragment Transmission Failed
 *
 * This function is called by the Interpan plugin when a fragmented
 * transmission has failed.
 *
 * @param interpanFragmentationStatus The status describing why transmission
 * failed Ver.: always
 * @param fragmentNum The fragment number that encountered the failure
 * Ver.: always
 */
WEAK(void emberAfPluginInterpanFragmentTransmissionFailedCallback(uint8_t interpanFragmentationStatus,
                                                                  uint8_t fragmentNum))
{
}
