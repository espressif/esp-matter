/***************************************************************************//**
 * @file
 * @brief Implements the communication core functionality
 *        for the EZSP SPI protocol
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

#include "api/btl_reset_info.h"
#include "btl_ezsp_spi.h"
#include "debug/btl_debug.h"

#if defined(BOOTLOADER_NONSECURE)
// NS headers
  #include "core/btl_bootload_ns.h"
  #include "core/btl_reset_ns.h"
#else
  #include "core/btl_bootload.h"
  #include "core/btl_reset.h"
#endif

#include "driver/btl_driver_delay.h"
#include "driver/btl_driver_spi_peripheral.h"

// ‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
// Local variables

static EzspSpiState_t state;
static EzspSpiTxMessage_t reply;
static EzspSpiTxMessage_t interrupt;
static EzspSpiRxMessage_t rxBuffer;

static bool done = false;
static bool imageError = false;
static bool interruptPending = false;
static bool errorPending = true;
static bool querySent = false;

static bool transferActive = false;
static unsigned int transferPolls = 0;

// -----------------------------------------------------------------------------
// Static consts

static const EzspSpiQueryResponse_t queryResponse = {
  .btlCommand = EZSP_SPI_FRAME_BTL_QUERYRESP,
  .btlActive = 0x01,
  .manufacturerId = { 0xFF, 0xFF },
  .hardwareTag = { 0 },
  .btlCapabilities = 0x00,
  // CortexM3 platform
  .platform = 4,
  // EFR32 micro
  .micro = 24,
  // EFR32 PHY
  .phy = 9,
  .btlMajorVersion = BOOTLOADER_VERSION_MAIN_MAJOR,
  .btlMinorVersion = BOOTLOADER_VERSION_MAIN_MINOR
};

// ‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
// Local function prototypes

static void ezspSpi_parse(ParserContext_t *parserContext,
                          const BootloaderParserCallbacks_t* parseCb,
                          ImageProperties_t *imageProps,
                          EzspSpiRxMessage_t* buffer);
static uint8_t ezspSpi_parseFrame(ParserContext_t *parserContext,
                                  const BootloaderParserCallbacks_t* parseCb,
                                  ImageProperties_t *imageProps,
                                  EzspSpiRxMessage_t* buffer);
static void ezspSpi_error(uint8_t error);

// -----------------------------------------------------------------------------
// Static local functions

static void ezspSpi_parse(ParserContext_t *parserContext,
                          const BootloaderParserCallbacks_t* parseCb,
                          ImageProperties_t *imageProps,
                          EzspSpiRxMessage_t* buffer)
{
  switch (buffer->data.btlMessage.command) {
    case EZSP_SPI_COMMAND_SPI_STATUS:
      reply.messageBuffer[0] = EZSP_SPI_RESPONSE_ALIVE;
      reply.messageBuffer[1] = EZSP_SPI_FRAME_TERMINATOR;
      reply.messageLength = 2;
      return;

    case EZSP_SPI_COMMAND_SPI_VERSION:
      reply.messageBuffer[0] = EZSP_SPI_RESPONSE_VERSION;
      reply.messageBuffer[1] = EZSP_SPI_FRAME_TERMINATOR;
      reply.messageLength = 2;
      return;

    case EZSP_SPI_COMMAND_BTL_FRAME:
    case EZSP_SPI_COMMAND_EZSP_FRAME:
      // Parsing logic executes command, and overwrites reply if necessary
      reply.messageBuffer[0] = EZSP_SPI_COMMAND_BTL_FRAME;
      reply.messageBuffer[3] = EZSP_SPI_FRAME_TERMINATOR;
      reply.messageLength = 1;
      reply.messageBuffer[2] = ezspSpi_parseFrame(parserContext,
                                                  parseCb,
                                                  imageProps,
                                                  buffer);
      reply.messageBuffer[1] = reply.messageLength;
      reply.messageLength += 3;

      if (reply.messageBuffer[2] == EZSP_SPI_FRAME_BTL_FILEABORT) {
        interrupt.messageBuffer[0] = XMODEM_CMD_CAN;
        interrupt.messageLength = 1;
        imageError = true;
      }
      return;
    default:
      ezspSpi_error(EZSP_SPI_ERROR_UNSUPPORTED);
      return;
  }
}

// Parses the EZSP frame content and populates a reply
// Returns synchronous answer byte
static uint8_t ezspSpi_parseFrame(ParserContext_t *parserContext,
                                  const BootloaderParserCallbacks_t* parseCb,
                                  ImageProperties_t *imageProps,
                                  EzspSpiRxMessage_t* buffer)
{
  int32_t retVal;
  uint8_t response;

  if ((buffer->data.btlMessage.length == 1)
      && (buffer->data.btlMessage.payload.buffer[0]
          == EZSP_SPI_FRAME_BTL_QUERY)) {
    // Bootloader query
    if (interruptPending) {
      // Reply with the interrupt data we wanted to get out
      memcpy(&reply.messageBuffer[2],
             interrupt.messageBuffer,
             interrupt.messageLength);
      reply.messageBuffer[2 + interrupt.messageLength]
        = EZSP_SPI_FRAME_TERMINATOR;
      reply.messageLength = interrupt.messageLength;
      interruptPending = false;
      return reply.messageBuffer[2];
    } else {
      // Reply with bootloader data
      memcpy(interrupt.messageBuffer,
             &queryResponse,
             sizeof(EzspSpiQueryResponse_t));
      interrupt.messageLength = sizeof(EzspSpiQueryResponse_t);
      interruptPending = true;
      querySent = true;
      return EZSP_SPI_FRAME_BTL_QUERYFOUND;
    }
  } else {
    // Parse as XMODEM
    if (!querySent) {
      return EZSP_SPI_FRAME_BTL_NO_QUERY;
    }

    if ((buffer->data.btlMessage.payload.xmodemPacket.header == XMODEM_CMD_SOH)
        && (buffer->data.btlMessage.length != (XMODEM_DATA_SIZE + 5))) {
      return EZSP_SPI_FRAME_BTL_BLOCKERR_PARTIAL;
    }

    retVal = xmodem_parsePacket(&(buffer->data.btlMessage.payload.xmodemPacket),
                                &response);

    // Store XMODEM response as an interrupt for later
    interrupt.messageBuffer[0] = response;
    if (response == XMODEM_CMD_ACK) {
      // Add the packet number
      interrupt.messageBuffer[1] = xmodem_getLastPacketNumber();
      interrupt.messageBuffer[2] = 0;
      interrupt.messageLength = 3;
    } else {
      interrupt.messageLength = 1;
    }
    interruptPending = true;

    switch (retVal) {
      case BOOTLOADER_OK:
        transferActive = true;
        transferPolls = 0;
        // Push data through GBL parser and flash
#if defined(BOOTLOADER_NONSECURE)
        (void)parserContext;
        (void)parseCb;
        retVal = parser_parse(buffer->data.btlMessage.payload.xmodemPacket.data,
                              XMODEM_DATA_SIZE,
                              imageProps);
#else
        retVal = parser_parse(parserContext,
                              imageProps,
                              buffer->data.btlMessage.payload.xmodemPacket.data,
                              XMODEM_DATA_SIZE,
                              parseCb);
#endif
        if ((retVal != BOOTLOADER_OK) && !imageProps->imageCompleted) {
          // Error condition! Break off transfer
          return EZSP_SPI_FRAME_BTL_FILEABORT;
        } else if (imageProps->imageCompleted && !imageProps->imageVerified) {
          // Error condition! Break off transfer
          return EZSP_SPI_FRAME_BTL_FILEABORT;
        } else {
          return EZSP_SPI_FRAME_BTL_BLOCKOK;
        }

      case BOOTLOADER_ERROR_XMODEM_CRCL:
        return EZSP_SPI_FRAME_BTL_BLOCKERR_CRCL;

      case BOOTLOADER_ERROR_XMODEM_CRCH:
        return EZSP_SPI_FRAME_BTL_BLOCKERR_CRCH;

      case BOOTLOADER_ERROR_XMODEM_PKTNUM:
        return EZSP_SPI_FRAME_BTL_BLOCKERR_CHK;

      case BOOTLOADER_ERROR_XMODEM_PKTDUP:
        return EZSP_SPI_FRAME_BTL_BLOCKERR_DUP;

      case BOOTLOADER_ERROR_XMODEM_PKTSEQ:
        return EZSP_SPI_FRAME_BTL_BLOCKERR_SEQUENCE;

      case BOOTLOADER_ERROR_XMODEM_DONE:
        // XMODEM transfer complete, but we didn't receive a complete/valid GBL file
        if (!imageProps->imageCompleted || !imageProps->imageVerified) {
          return EZSP_SPI_FRAME_BTL_FILEABORT;
        }

#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
        // SE upgrade with upgrade version < running version. Abort the transfer.
        if (imageProps->contents & BTL_IMAGE_CONTENT_SE) {
          if (!bootload_checkSeUpgradeVersion(imageProps->seUpgradeVersion)) {
            return EZSP_SPI_FRAME_BTL_FILEABORT;
          }
        }
#endif

        done = true;
        // EZSP-SPI expects packet number + 1 on ACK'ing the EOT command
        interrupt.messageBuffer[1] = xmodem_getLastPacketNumber() + 1;
        return EZSP_SPI_FRAME_BTL_FILEDONE;

      case BOOTLOADER_ERROR_XMODEM_CANCEL:
        return EZSP_SPI_FRAME_BTL_BLOCKOK;

      case BOOTLOADER_ERROR_XMODEM_NO_SOH:
        return EZSP_SPI_FRAME_BTL_BLOCKERR_SOH;

      default:
        return EZSP_SPI_FRAME_BTL_FILEABORT;
    }
  }
}

// Updates the reply buffer with the given error message
static void ezspSpi_error(uint8_t error)
{
  if (!errorPending) {
    reply.messageBuffer[0] = error;
    reply.messageBuffer[1] = 0;
    reply.messageBuffer[2] = EZSP_SPI_FRAME_TERMINATOR;
    reply.messageLength = 3;

    errorPending = true;
  }
}

// -----------------------------------------------------------------------------
// Global Functions

void bootloader_ezsp_communication_init(void)
{
  bootloader_ezsp_init();
  // Init SPI peripheral driver
  spi_peripheral_init();
}

int32_t bootloader_ezsp_communication_start(void)
{
  // Assert nHOST_INT to signal to the host that we have data to send.
  // But first check for a handshake
  bootloader_ezsp_wakeHandshake();
  nHOST_ASSERT();
  state = EZSP_SPI_STATE_RESETMESSAGE;
  reply.messageBuffer[0] = EZSP_SPI_ERROR_RESET;
  reply.messageBuffer[1] = EZSP_SPI_DEFAULT_RESET_REASON;
  reply.messageBuffer[2] = EZSP_SPI_FRAME_TERMINATOR;
  reply.messageLength = 3;
  xmodem_reset();

  return BOOTLOADER_OK;
}

int32_t bootloader_ezsp_communication_main(ImageProperties_t *imageProps,
                                           ParserContext_t *parserContext,
                                           const BootloaderParserCallbacks_t* parseCb)
{
  int32_t retVal = -1;
  size_t rxSize;
  bool exit = false;
  bool timeout = false;

  while (!exit) {
    switch (state) {
      case EZSP_SPI_STATE_PROCESSING:
        spi_peripheral_enableReceiver(false);
        spi_peripheral_flush(false, true);

        if (errorPending) {
          delay_microseconds(500);
        } else {
          ezspSpi_parse(parserContext,
                        parseCb,
                        imageProps,
                        &rxBuffer);
        }
        state = EZSP_SPI_STATE_ANSWERING;
        break;

      case EZSP_SPI_STATE_ANSWERING:
        retVal = spi_peripheral_sendBuffer(reply.messageBuffer,
                                           reply.messageLength);

        if (retVal == BOOTLOADER_OK) {
          // Assert nHOST_INT to signal TX availability
          nHOST_ASSERT();
          while (spi_peripheral_getTxBytesLeft() >= reply.messageLength) ;
          nHOST_DEASSERT();
          errorPending = false;
        }
        while (nCS_ACTIVE()) ;

        spi_peripheral_enableTransmitter(false);

        if (interruptPending) {
          state = EZSP_SPI_STATE_INTERRUPTING;
        } else {
          exit = done | imageError;
          state = EZSP_SPI_STATE_IDLE;
        }
        break;

      case EZSP_SPI_STATE_RESETMESSAGE:
      case EZSP_SPI_STATE_INTERRUPTING:
        // Wait for SS to deassert
        while (nCS_ACTIVE()) ;

        delay_microseconds(20);
        nHOST_ASSERT();

        state = EZSP_SPI_STATE_IDLE;
        break;

      case EZSP_SPI_STATE_IDLE:
        // Wait for SS to deassert before re-enabling the receiver to avoid
        // garbage in the RX buffer
        while (nCS_ACTIVE()) ;

        // Re-enable the receiver to be ready for incoming commands
        spi_peripheral_enableReceiver(true);

        if (transferActive) {
          delay_milliseconds(5000, false);
        } else {
          delay_milliseconds(1000, false);
        }

        while (!nCS_ACTIVE()) {
          // During idle time, watch for WAKE handshake
          bootloader_ezsp_wakeHandshake();
          // Also send XMODEM-CRC polling characters. Every second (up to 60)
          // when transfer hasn't started yet, every 5 seconds (up to 60) when
          // a transfer is active.
          if (delay_expired() && !interruptPending) {
            transferPolls++;
            if (transferActive) {
              if (transferPolls > 12) {
                // Timed out, cancel transfer
                timeout = true;
                break;
              } else {
                delay_milliseconds(5000, false);
              }
            } else {
              if (transferPolls > 60) {
                // Timed out, cancel transfer
                timeout = true;
                break;
              } else {
                delay_milliseconds(1000, false);
              }
            }
          }
        }

        if (timeout) {
          return BOOTLOADER_ERROR_COMMUNICATION_ERROR;
        }

        // Wait for the receiver to send a command
        rxBuffer.data.btlMessage.command = 0xFFU;
        while (rxBuffer.data.btlMessage.command == 0xFFU) {
          spi_peripheral_receiveByte(&(rxBuffer.data.btlMessage.command));
        }

        nHOST_DEASSERT();

        // Get length or terminator
        spi_peripheral_receiveByte(&(rxBuffer.data.btlMessage.length));

        // Parse command byte
        if ((rxBuffer.data.btlMessage.command == EZSP_SPI_COMMAND_BTL_FRAME)
            || (rxBuffer.data.btlMessage.command
                == EZSP_SPI_COMMAND_EZSP_FRAME)) {
          // Check for oversized packets
          if (rxBuffer.data.btlMessage.length > 133) {
            ezspSpi_error(EZSP_SPI_ERROR_OVERSIZED);
          }

          // Get packet data and terminator
          retVal = spi_peripheral_receiveBuffer(
            rxBuffer.data.btlMessage.payload.buffer,
            rxBuffer.data.btlMessage.length + 1,
            &rxSize,
            true,
            500);

          // Check status and terminator
          if (retVal != BOOTLOADER_OK) {
            ezspSpi_error(EZSP_SPI_ERROR_ABORTED);
          } else if (rxBuffer.data.btlMessage.payload.buffer[rxSize - 1]
                     != EZSP_SPI_FRAME_TERMINATOR) {
            ezspSpi_error(EZSP_SPI_ERROR_NO_TERMINATOR);
          }
        } else {
          // Check terminator
          if (rxBuffer.data.btlMessage.length != EZSP_SPI_FRAME_TERMINATOR) {
            ezspSpi_error(EZSP_SPI_ERROR_NO_TERMINATOR);
          }
        }

        state = EZSP_SPI_STATE_PROCESSING;
        break;
      default:
        break;
    }
  }

  if (imageError) {
    return BOOTLOADER_ERROR_COMMUNICATION_IMAGE_ERROR;
  }

  if (done) {
#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
    if (imageProps->contents & BTL_IMAGE_CONTENT_SE) {
      // Install SE upgrade
#if defined(BOOTLOADER_NONSECURE)
      bootload_commitSeUpgrade();
#else
      bootload_commitSeUpgrade(BTL_UPGRADE_LOCATION);
#endif
      // If we get here, the SE upgrade failed, reset and enter back to the bootloader
      reset_resetWithReason(BOOTLOADER_RESET_REASON_BOOTLOAD);
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

      // If we get here, the bootloader upgrade failed, reset and enter back to the bootloader
      reset_resetWithReason(BOOTLOADER_RESET_REASON_BOOTLOAD);
    }

    return BOOTLOADER_OK;
  }

  return BOOTLOADER_ERROR_COMMUNICATION_DONE;
}

void bootloader_ezsp_communication_shutdown(void)
{
  // Reset USART
  spi_peripheral_deinit();
  bootloader_ezsp_shutdown();
}
