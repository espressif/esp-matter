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
#include "api/btl_errorcode.h"
#include "communication/xmodem-parser/btl_xmodem.h"
#include "security/btl_crc16.h"

#include "debug/btl_debug.h"

static uint8_t packetNumber;
static bool started;

void xmodem_reset(void)
{
  packetNumber = 0;
  started = false;
}

int32_t xmodem_parsePacket(XmodemPacket_t *packet, uint8_t *response)
{
  uint16_t crc16 = 0;

  switch (packet->header) {
    case XMODEM_CMD_SOH:
      // Packet number must start at 1, and must monotonically increase
      if (!started) {
        if (packet->packetNumber != 0x01) {
          *response = XMODEM_CMD_NAK;
          return BOOTLOADER_ERROR_XMODEM_PKTNUM;
        }
        started = true;
      } else {
        if (packet->packetNumber == packetNumber) {
          BTL_DEBUG_PRINTLN("replay");
          *response = XMODEM_CMD_ACK;
          return BOOTLOADER_ERROR_XMODEM_PKTDUP;
        } else if (packet->packetNumber != (uint8_t)(packetNumber + 1)) {
          BTL_DEBUG_PRINTLN("ooseq");
          *response = XMODEM_CMD_NAK;
          return BOOTLOADER_ERROR_XMODEM_PKTSEQ;
        }
      }

      // Byte 3 is the two's complement of the packet number in the second byte
      if (packet->packetNumber + packet->packetNumberC != 0xFF) {
        BTL_DEBUG_PRINTLN("compl");
        *response = XMODEM_CMD_NAK;
        return BOOTLOADER_ERROR_XMODEM_PKTNUM;
      }

      // Bytes 132-133 contain a 16-bit CRC over the data bytes
      crc16 = btl_crc16Stream(packet->data, XMODEM_DATA_SIZE, crc16);

      if (((crc16 >> 8) & 0xFF) != packet->crcH) {
        BTL_DEBUG_PRINTLN("crch");
        *response = XMODEM_CMD_NAK;
        return BOOTLOADER_ERROR_XMODEM_CRCH;
      }

      if ((crc16 & 0xFF) != packet->crcL) {
        BTL_DEBUG_PRINTLN("crcl");
        *response = XMODEM_CMD_NAK;
        return BOOTLOADER_ERROR_XMODEM_CRCL;
      }

      packetNumber = packet->packetNumber;
      *response = XMODEM_CMD_ACK;
      return BOOTLOADER_OK;

    case XMODEM_CMD_EOT:
      BTL_DEBUG_PRINTLN("EOT");
      *response = XMODEM_CMD_ACK;
      return BOOTLOADER_ERROR_XMODEM_DONE;
      break;
    case XMODEM_CMD_CAN:
    case XMODEM_CMD_C:
    case XMODEM_CMD_CTRL_C:
      BTL_DEBUG_PRINTLN("CAN");
      *response = XMODEM_CMD_CAN;
      return BOOTLOADER_ERROR_XMODEM_CANCEL;
    default:
      BTL_DEBUG_PRINTLN("UNK");
      *response = XMODEM_CMD_CAN;
      return BOOTLOADER_ERROR_XMODEM_NO_SOH;
  }
}

uint8_t xmodem_getLastPacketNumber(void)
{
  return packetNumber;
}
