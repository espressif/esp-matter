/***************************************************************************//**
 * @file
 * @brief The interface down to the serial protocol being used to
 * transport EZSP frames (SPI/UART/USB).
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

#ifndef SILABS_SERIAL_INTERFACE_H
#define SILABS_SERIAL_INTERFACE_H

// Macros for reading and writing frame bytes.
#if defined(EMBER_EZSP_UNIT_TEST)
uint8_t serialGetResponseByte(uint32_t index);
#else
  #define serialGetResponseByte(index)      (ezspFrameContents[(index)])
#endif

#define serialSetCommandByte(index, data) (ezspFrameContents[(index)] = (data))

// The length of the current EZSP frame.  The higher layer writes this when
// sending a command and reads it when processing a response.
extern uint8_t *ezspFrameLengthLocation;

// Macros for reading and writing the frame length.
#define serialSetCommandLength(length) (*ezspFrameLengthLocation = (length))
#define serialGetResponseLength()      (*ezspFrameLengthLocation)

// Returns the number of EZSP responses that have been received by the serial
// protocol and are ready to be collected by the EZSP layer via
// serialResponseReceived().
uint8_t serialPendingResponseCount(void);

// Checks whether a new EZSP response frame has been received. Returns
// EZSP_SUCCESS if a new response has been received. Returns
// EZSP_SPI_WAITING_FOR_RESPONSE or EZSP_NO_RX_DATA if no new response has
// been received. Any other return value means that an error has been detected
// by the serial protocol layer.
EzspStatus serialResponseReceived(void);

// Sends the current EZSP command frame. Returns EZSP_SUCCESS if the command was
// sent successfully. Any other return value means that an error has been
// detected by the serial protocol layer.
EzspStatus serialSendCommand(void);

// Set when the ncp has indicated it has a pending callback by seting the
// callback flag in the frame control byte or (uart version only) by sending
// an an ASH_WAKE byte between frames.
extern bool ncpHasCallbacks;

// Tests that the host is able to properly hold off transmitting in response
// to the ncp's flow control request.
EzspStatus serialTestFlowControl(void);

//Returns the total Length of the incoming frame
uint8_t serialGetCommandLength(void);

#ifdef EZSP_SPI
// Returns the file descriptor for the SPI interrupt line (nHOST_INT)
int serialGetSpiInterruptFd(void);
  #define serialGetSpiFd()  serialGetSpiInterruptFd()
#else
  #define serialGetSpiFd()  - 1
#endif

#endif // SILABS_SERIAL_INTERFACE_H
