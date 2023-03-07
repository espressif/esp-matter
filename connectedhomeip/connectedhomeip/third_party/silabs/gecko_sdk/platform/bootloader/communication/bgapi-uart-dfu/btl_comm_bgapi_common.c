/***************************************************************************//**
 * @file
 * @brief Implements the communication core functionality
 *        for the BGAPI UART DFU protocol
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

// -----------------------------------------------------------------------------
// Includes

#include "btl_comm_bgapi.h"
#include "config/btl_config.h"
#include "driver/btl_serial_driver.h"

#if defined(BOOTLOADER_NONSECURE)
// NS headers
  #include "core/btl_bootload_ns.h"
  #include "core/btl_reset_ns.h"
#else
  #include "core/btl_bootload.h"
  #include "core/btl_reset.h"
#endif

// Debug
#include "debug/btl_debug.h"

#include <string.h>

// -----------------------------------------------------------------------------
// Static local functions

static int32_t sendPacket(BgapiPacket_t *packet)
{
  size_t len = sizeof(BgapiPacketHeader_t) + packet->header.len;

  return uart_sendBuffer((uint8_t *)&(packet->header), len, true);
}

static int32_t receivePacket(BgapiPacket_t *packet)
{
  int32_t ret = BOOTLOADER_OK;
  uint8_t requestedBytes;
  size_t receivedBytes;
  uint8_t *buf = (uint8_t *)&(packet->header);

  // Wait for a valid packet header
  do {
    // Wait for bytes to be available in RX buffer
    while (uart_getRxAvailableBytes() == 0) {
      // Do nothing
    }

    // Read the first byte
    requestedBytes = 1;
    ret = uart_receiveBuffer(buf,
                             requestedBytes,
                             &receivedBytes,
                             true,
                             1000);
  } while (packet->header.type != BGAPI_PACKET_TYPE_COMMAND);

  // Read the rest of the header
  requestedBytes = sizeof(BgapiPacketHeader_t) - 1;
  ret = uart_receiveBuffer(buf + 1,
                           requestedBytes,
                           &receivedBytes,
                           true,
                           1000);
  if (ret != BOOTLOADER_OK || receivedBytes != requestedBytes) {
    // Didn't receive entire header within 1000 ms; bail
    return BOOTLOADER_ERROR_COMMUNICATION_ERROR;
  }
  BTL_DEBUG_PRINT("BGAPI packet ");
  BTL_DEBUG_PRINT_CHAR_HEX(packet->header.type);
  BTL_DEBUG_PRINT_CHAR_HEX(packet->header.len);
  BTL_DEBUG_PRINT_CHAR_HEX(packet->header.class);
  BTL_DEBUG_PRINT_CHAR_HEX(packet->header.command);
  BTL_DEBUG_PRINT_LF();

  requestedBytes = packet->header.len;
  if (requestedBytes == 0) {
    // Packet has no payload
    return BOOTLOADER_OK;
  }

  ret = uart_receiveBuffer(buf + sizeof(BgapiPacketHeader_t),
                           requestedBytes,
                           &receivedBytes,
                           true,
                           3000);
  if (ret != BOOTLOADER_OK || receivedBytes != requestedBytes) {
    BTL_DEBUG_PRINT("Recvd ");
    BTL_DEBUG_PRINT_WORD_HEX(receivedBytes);
    BTL_DEBUG_PRINT_LF();
    // Didn't receive entire packet within 3000 ms; bail
    return BOOTLOADER_ERROR_COMMUNICATION_ERROR;
  }

  return ret;
}

// -----------------------------------------------------------------------------
// Global Functions

void bootloader_bgapi_communication_init(void)
{
  // Init UART driver
  uart_init();
}

int32_t bootloader_bgapi_communication_start(void)
{
  int32_t ret = BOOTLOADER_OK;

  if (reset_getResetReason() == BOOTLOADER_RESET_REASON_BADAPP) {
    // Sent evt_dfu_boot_failure
    BgapiPacket_t failEvent
      = BGAPI_EVENT_DFU_BOOT_FAILURE(SL_STATUS_SECURITY_IMAGE_CHECKSUM_ERROR);
    ret = sendPacket(&failEvent);
  }
  // Send evt_dfu_boot
  BgapiPacket_t event = BGAPI_EVENT_DFU_BOOT();
  event.body.boot.bootloaderVersion = bootload_getBootloaderVersion();

  ret = sendPacket(&event);
  return ret;
}

int32_t bootloader_bgapi_communication_main(ImageProperties_t *imageProps,
                                            ParserContext_t *parserContext,
                                            const BootloaderParserCallbacks_t* parseCb)
{
  int32_t ret = BOOTLOADER_OK;

  BgapiPacket_t command;
  BgapiPacket_t response = BGAPI_RESPONSE_DEFAULT;

  while (1) {
    // Wait for data
    memset(&command, 0, sizeof(BgapiPacket_t));
    ret = receivePacket(&command);
    if (ret != BOOTLOADER_OK) {
      BTL_DEBUG_PRINT("RX packet fail: ");
      BTL_DEBUG_PRINT_WORD_HEX(ret);
      BTL_DEBUG_PRINT_LF();
      continue;
    }

    if (command.header.class == BGAPI_PACKET_CLASS_DFU) {
      // BGAPI DFU command
      switch (command.header.command) {
        case DFU_FLASH_SET_ADDRESS:
          BTL_DEBUG_PRINTLN("Flash set addr");
          // Do nothing -- command is deprecated
          response.header.command = DFU_FLASH_SET_ADDRESS;
          response.body.response.error = SL_STATUS_OK;
          ret = sendPacket(&response);
          break;

        case DFU_FLASH_UPLOAD:
          BTL_DEBUG_PRINTLN("Flash upload");
          // Process file contents
#if defined(BOOTLOADER_NONSECURE)
          (void)parserContext;
          (void)parseCb;
          ret = parser_parse(command.body.flashUpload.data,
                             command.body.flashUpload.len,
                             imageProps);
#else
          ret = parser_parse(parserContext,
                             imageProps,
                             command.body.flashUpload.data,
                             command.body.flashUpload.len,
                             parseCb);
#endif
          response.header.command = DFU_FLASH_UPLOAD;

          if ((ret == BOOTLOADER_OK) || (ret == BOOTLOADER_ERROR_PARSER_EOF)) {
            response.body.response.error = SL_STATUS_OK;
          } else {
            response.body.response.error =
              SL_STATUS_SECURITY_IMAGE_CHECKSUM_ERROR;
          }
          ret = sendPacket(&response);
          break;

        case DFU_FLASH_UPLOAD_FINISH:
          BTL_DEBUG_PRINTLN("Flash finish");
          response.header.command = DFU_FLASH_UPLOAD_FINISH;
          if (imageProps->imageCompleted && imageProps->imageVerified) {
            response.body.response.error = SL_STATUS_OK;
          } else {
            response.body.response.error =
              SL_STATUS_SECURITY_IMAGE_CHECKSUM_ERROR;
          }
          ret = sendPacket(&response);

          if (imageProps->imageCompleted && imageProps->imageVerified) {
#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
            if (imageProps->contents & BTL_IMAGE_CONTENT_SE) {
              if (bootload_checkSeUpgradeVersion(imageProps->seUpgradeVersion)) {
                // Install SE upgrade
#if defined(BOOTLOADER_NONSECURE)
                bootload_commitSeUpgrade();
#else
                bootload_commitSeUpgrade(BTL_UPGRADE_LOCATION);
#endif
              }
            }
#endif
            if (imageProps->contents & BTL_IMAGE_CONTENT_BOOTLOADER) {
              if (imageProps->bootloaderVersion > bootload_getBootloaderVersion()) {
                // Install bootloader upgrade
#if defined(BOOTLOADER_NONSECURE)
                bootload_commitBootloaderUpgrade(imageProps->bootloaderUpgradeSize);
#else
                bootload_commitBootloaderUpgrade(BTL_UPGRADE_LOCATION, imageProps->bootloaderUpgradeSize);
#endif
              }
            }
          }
          break;

        default:
          // Do nothing if packet is of an unknown type
          BTL_DEBUG_PRINT("Unknown DFU command: ");
          BTL_DEBUG_PRINT_CHAR_HEX(command.header.command);
          BTL_DEBUG_PRINT_LF();
          break;
      }
    } else if (command.header.class == BGAPI_PACKET_CLASS_SYSTEM) {
      // BGAPI System command
      switch (command.header.command) {
        case SYSTEM_RESET:
          BTL_DEBUG_PRINTLN("Reset request");
          if (imageProps->imageCompleted && !imageProps->imageVerified) {
            reset_resetWithReason(BOOTLOADER_RESET_REASON_BADIMAGE);
          } else if (command.body.reset.mode == 1) {
            reset_resetWithReason(BOOTLOADER_RESET_REASON_BOOTLOAD);
          } else {
            reset_resetWithReason(BOOTLOADER_RESET_REASON_GO);
          }
          break;

        default:
          // Do nothing if packet is of an unknown type
          BTL_DEBUG_PRINT("Unknown System command: ");
          BTL_DEBUG_PRINT_CHAR_HEX(command.header.command);
          BTL_DEBUG_PRINT_LF();
          break;
      }
    } else {
      // Do nothing if the packet isn't a DFU or SYSTEM packet
      BTL_DEBUG_PRINTLN("Unknown command");
    }
  }
}
