/***************************************************************************//**
 * @file
 * @brief EZSP stack callbacks header
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

#ifndef EM260_CALLBACKS_H
#define EM260_CALLBACKS_H
bool addToCallbackQueue(uint8_t *callback,
                        uint8_t callbackLength);
bool addToCallbackQueueExtended(uint8_t *callback,
                                uint8_t callbackLength,
                                EmberMessageBuffer buffer,
                                uint8_t *additionalData,
                                uint8_t additionalDataLength);
void writeLqiAndRssi(uint8_t *loc);
extern Buffer callbackQueue;
extern bool callbackOverflow;
extern bool uartSynchCallbacks;
bool callbackPending(void);

#endif //EM260_CALLBACKS_H
