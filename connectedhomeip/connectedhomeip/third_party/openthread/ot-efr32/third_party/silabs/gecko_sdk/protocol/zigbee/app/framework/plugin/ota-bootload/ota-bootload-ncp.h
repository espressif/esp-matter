/***************************************************************************//**
 * @file
 * @brief SPI/UART Interface to bootloading the NCP from a connected host.
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

bool emAfStartNcpBootloaderCommunications(void);
void emAfPostNcpBootload(bool success);
bool emAfRebootNcpAfterBootload(void);

// These primitives are called by the Xmodem code to pass data via
// the specific mechanism (UART or SPI).
bool emAfBootloadSendData(const uint8_t *data, uint16_t length);
bool emAfBootloadSendByte(uint8_t byte);
bool emAfBootloadWaitChar(uint8_t *data, bool expect, uint8_t expected);
