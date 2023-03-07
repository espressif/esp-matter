/***************************************************************************//**
 * @file
 * @brief The interface down to the serial protocol being used to
 * transport EZSP frames (either SPI or UART).
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

#ifndef SILABS_APP_EM260_SERIAL_INTERFACE_H
#define SILABS_APP_EM260_SERIAL_INTERFACE_H

void serialInit(EmberEvent* event);
bool serialReleaseBuffers(void);
void serialAllocateBuffers(void);
bool serialOkToSleep(void);
bool serialOkToBootload(void);
bool serialCommandReceived(void);
void serialTransmit(void);
void serialResponseReady(bool highPriority);
void serialPendingCallbacks(bool pending);
void serialDebugTrace(void);
EmberMessageBuffer serialFillBufferFromCommand(uint8_t startIndex,
                                               uint8_t length);
uint8_t serialGetCommandLength(void);
void serialSetResponseLength(uint8_t data);
#define serialGetCommandByte(index)        (ezspFrameContents[(index)])
#define serialGetResponseByte(index)       (ezspFrameContents[(index)])
#define serialSetResponseByte(index, data) (ezspFrameContents[(index)] = (data))
bool serialCallbackResponse(void);

void serialMfglibFillPacket(uint8_t *packet);
void serialDelayTest(uint16_t delay);

#ifdef SLEEPY_EZSP_ASH
  #include "hal/micro/generic/ash-ncp.h"
#else
  #define serialHostIsActive() !serialOkToSleep()
  #define serialWaitingForHostToWake() (false)
  #define serialSendAwakeSignal()
  #define serialInhibitCallbackSignal()
  #if defined(EZSP_SPI) && !defined (EMBER_TEST)
    #define serialPowerDown() halHostSerialPowerdown()
    #define serialPowerUp() halHostSerialPowerup()
  #else
    #define serialPowerDown()
    #define serialPowerUp()
  #endif
#endif

#endif // SILABS_APP_EM260_SERIAL_INTERFACE_H
