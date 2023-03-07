/***************************************************************************//**
 * @file
 * @brief XMODEM parser component for Silicon Labs Bootloader.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef BTL_XMODEM_H
#define BTL_XMODEM_H

#include <stdbool.h>
#include <stddef.h>

#include "core/btl_util.h"

MISRAC_DISABLE
#include "em_common.h"
MISRAC_ENABLE

/***************************************************************************//**
 * @addtogroup Components
 * @{
 * @addtogroup Communication
 * @{
 * @addtogroup CommunicationUtils Utils
 * @{
 * @addtogroup Xmodem XMODEM Parser
 * @{
 * @brief Parser for XMODEM packets
 * @details
 *   XMODEM packet parser supporting XMODEM-CRC.
 ******************************************************************************/

/// Size of an XMODEM packet
#define XMODEM_DATA_SIZE              128

/***************************************************************************//**
 * @addtogroup Commands
 * @{
 ******************************************************************************/

/// Start of Header
#define XMODEM_CMD_SOH                (0x01)
/// End of Transmission
#define XMODEM_CMD_EOT                (0x04)
/// Acknowledge
#define XMODEM_CMD_ACK                (0x06)
/// Not Acknowledge
#define XMODEM_CMD_NAK                (0x15)
/// Cancel
#define XMODEM_CMD_CAN                (0x18)
/// Ctrl+C
#define XMODEM_CMD_CTRL_C             (0x03)
/// ASCII 'C'
#define XMODEM_CMD_C                  (0x43)

/** @} addtogroup Commands */

SL_PACK_START(1)
/// XMODEM packet
typedef struct {
  uint8_t header;                   ///< Packet header (@ref XMODEM_CMD_SOH)
  uint8_t packetNumber;             ///< Packet sequence number
  uint8_t packetNumberC;            ///< Complement of packet sequence number
  uint8_t data[XMODEM_DATA_SIZE];   ///< Payload
  uint8_t crcH;                     ///< CRC high byte
  uint8_t crcL;                     ///< CRC low byte
} SL_ATTRIBUTE_PACKED XmodemPacket_t;
SL_PACK_END()

/***************************************************************************//**
 * Reset the XMODEM parser to start a new transfer.
 ******************************************************************************/
void xmodem_reset(void);

/***************************************************************************//**
 * Parse an XMODEM packet.
 *
 * @param[in] packet The XMODEM packet to parse.
 * @param[out] response The XMODEM response to the parsed frame
 *
 * @return @ref BOOTLOADER_OK on success, else error code
 ******************************************************************************/
int32_t xmodem_parsePacket(XmodemPacket_t *packet, uint8_t *response);

/***************************************************************************//**
 * Return the packet number of the last packet that was successfully parsed.
 *
 * @return Last packet number received. Defaults to 0 if nothing has been
 *         received yet.
 ******************************************************************************/
uint8_t xmodem_getLastPacketNumber(void);

/** @} addtogroup XmodemParser */
/** @} addtogroup CommunicationUtils */
/** @} addtogroup Communication */
/** @} addtogroup Components */
#endif // BTL_XMODEM_H
