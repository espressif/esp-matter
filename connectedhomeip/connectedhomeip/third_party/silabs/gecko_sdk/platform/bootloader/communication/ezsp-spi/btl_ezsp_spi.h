/***************************************************************************//**
 * @file
 * @brief EZSP-SPI communication header for Gecko Bootloader.
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

#ifndef BTL_EZSP_SPI_H
#define BTL_EZSP_SPI_H

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "config/btl_config.h"
#include "core/btl_util.h"
MISRAC_DISABLE
#include "em_common.h"
MISRAC_ENABLE
#include "em_gpio.h"
#include "api/btl_interface.h"

#if defined(BOOTLOADER_NONSECURE)
  #include "parser/gbl/btl_gbl_parser_ns.h"
#else
  #include "btl_ezsp_gpio_activation_cfg.h"
  #ifdef BTL_SPI_EUSART_ENABLE
    #include "btl_spi_peripheral_eusart_driver_cfg.h"
  #else
    #include "btl_spi_peripheral_usart_driver_cfg.h"
  #endif
  #include "parser/gbl/btl_gbl_parser.h"
#endif

#include "communication/btl_communication.h"
#include "communication/xmodem-parser/btl_xmodem.h"

/***************************************************************************//**
 * @addtogroup Components
 * @{
 * @addtogroup Communication
 * @{
 * @page communication_ezsp EZSP-SPI
 *   By enabling the EZSP-SPI communication component, the bootloader communication
 *   interface implements the EZSP protocol over SPI. This component makes the
 *   bootloader compatible with the legacy `ezsp-spi-bootloader` that was
 *   previously released with the EmberZnet and SL-Thread wireless stacks.
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

// ‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
// Protocol defines
#define EZSP_SPI_MAX_RX_PACKET_SIZE             (136)
#define EZSP_SPI_MAX_TX_PACKET_SIZE             (32)

#define EZSP_SPI_COMMAND_SPI_STATUS             (0x0BU)
#define EZSP_SPI_COMMAND_SPI_VERSION            (0x0AU)
#define EZSP_SPI_COMMAND_BTL_FRAME              (0xFDU)
#define EZSP_SPI_COMMAND_EZSP_FRAME             (0xFEU)

#define EZSP_SPI_FRAME_TERMINATOR               (0xA7U)

#define EZSP_SPI_ERROR_RESET                    (0x00U)
#define EZSP_SPI_ERROR_OVERSIZED                (0x01U)
#define EZSP_SPI_ERROR_ABORTED                  (0x02U)
#define EZSP_SPI_ERROR_NO_TERMINATOR            (0x03U)
#define EZSP_SPI_ERROR_UNSUPPORTED              (0x04U)

#define EZSP_SPI_DEFAULT_RESET_REASON           (0x09U)

#define EZSP_SPI_RESPONSE_VERSION               (0x82U)
#define EZSP_SPI_RESPONSE_ALIVE                 (0xC1U)

#define EZSP_SPI_FRAME_BTL_QUERY                (0x51U)
#define EZSP_SPI_FRAME_BTL_QUERYRESP            (0x52U)
#define EZSP_SPI_FRAME_BTL_QUERYFOUND           (0x1AU)
#define EZSP_SPI_FRAME_BTL_NO_QUERY             (0x4DU)
#define EZSP_SPI_FRAME_BTL_TIMEOUT              (0x16U)
#define EZSP_SPI_FRAME_BTL_FILEDONE             (0x17U)
#define EZSP_SPI_FRAME_BTL_FILEABORT            (0x18U)
#define EZSP_SPI_FRAME_BTL_BLOCKOK              (0x19U)
#define EZSP_SPI_FRAME_BTL_START_TIMEOUT        (0x1BU)
#define EZSP_SPI_FRAME_BTL_BLOCKERR_TIMEOUT     (0x1CU)
#define EZSP_SPI_FRAME_BTL_BLOCKERR_SOH         (0x21U)
#define EZSP_SPI_FRAME_BTL_BLOCKERR_CHK         (0x22U)
#define EZSP_SPI_FRAME_BTL_BLOCKERR_CRCH        (0x23U)
#define EZSP_SPI_FRAME_BTL_BLOCKERR_CRCL        (0x24U)
#define EZSP_SPI_FRAME_BTL_BLOCKERR_SEQUENCE    (0x25U)
#define EZSP_SPI_FRAME_BTL_BLOCKERR_PARTIAL     (0x26U)
#define EZSP_SPI_FRAME_BTL_BLOCKERR_DUP         (0x27U)

// ‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
// Types

typedef enum {
  EZSP_SPI_STATE_IDLE,
  EZSP_SPI_STATE_PROCESSING,
  EZSP_SPI_STATE_ANSWERING,
  EZSP_SPI_STATE_INTERRUPTING,
  EZSP_SPI_STATE_RESETMESSAGE
} EzspSpiState_t;

SL_PACK_START(1)
typedef struct {
  uint8_t messageBuffer[EZSP_SPI_MAX_TX_PACKET_SIZE];
  size_t  messageLength;
} SL_ATTRIBUTE_PACKED EzspSpiTxMessage_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  // Added padding to word-align the incoming XMODEM data
  uint8_t padding[3];
  union {
    uint8_t messageBuffer[EZSP_SPI_MAX_RX_PACKET_SIZE];
    struct {
      uint8_t command;
      uint8_t length;
      // alignment +2
      union {
        uint8_t buffer[EZSP_SPI_MAX_RX_PACKET_SIZE - 2];
        // Inside xmodem packet, data is on +3 alignment
        XmodemPacket_t xmodemPacket;
      } payload;
    } btlMessage;
  } data;
} SL_ATTRIBUTE_PACKED EzspSpiRxMessage_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  uint8_t btlCommand;
  uint8_t btlActive;
  uint8_t manufacturerId[2];
  uint8_t hardwareTag[16];
  uint8_t btlCapabilities;
  uint8_t platform;
  uint8_t micro;
  uint8_t phy;
  uint8_t btlMajorVersion;
  uint8_t btlMinorVersion;
} SL_ATTRIBUTE_PACKED EzspSpiQueryResponse_t;
SL_PACK_END()

/** @endcond */

// -‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
// Helper macros

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#if defined(BOOTLOADER_NONSECURE)
  #define nHOST_ASSERT()    bootloader_ezsp_nhost_assert()
  #define nHOST_DEASSERT()  bootloader_ezsp_nhost_deassert()
  #define nCS_ACTIVE()      bootloader_ezsp_ncs_active()
#else
  #define nHOST_ASSERT()    GPIO_PinOutClear(SL_EZSPSPI_HOST_INT_PORT, \
                                             SL_EZSPSPI_HOST_INT_PIN)
  #define nHOST_DEASSERT()  GPIO_PinOutSet(SL_EZSPSPI_HOST_INT_PORT, \
                                           SL_EZSPSPI_HOST_INT_PIN)
  #ifdef BTL_SPI_EUSART_ENABLE
  #define nCS_ACTIVE()      (GPIO_PinInGet(SL_EUSART_SPINCP_CS_PORT, \
                                           SL_EUSART_SPINCP_CS_PIN) == 0)
  #else
  #define nCS_ACTIVE()      (GPIO_PinInGet(SL_USART_SPINCP_CS_PORT, \
                                           SL_USART_SPINCP_CS_PIN) == 0)
  #endif
#endif

// ‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
// Function declarations

void bootloader_ezsp_init(void);
void bootloader_ezsp_shutdown(void);
void bootloader_ezsp_wakeHandshake(void);
#if defined(BOOTLOADER_NONSECURE)
  void bootloader_ezsp_nhost_assert(void);
  void bootloader_ezsp_nhost_deassert(void);
  bool bootloader_ezsp_ncs_active(void);
#endif

/** @endcond */

/***************************************************************************//**
 * Initialize hardware for the EZSP SPI Bootloader communication.
 ******************************************************************************/
void bootloader_ezsp_communication_init(void);

/***************************************************************************//**
 * Initialize communication between the EZSP SPI bootloader
 * and external host.
 *
 * @return Error code indicating success or failure.
 ******************************************************************************/
int32_t bootloader_ezsp_communication_start(void);

/***************************************************************************//**
 * Communication main for the EZSP SPI bootloader.
 *
 * @param imageProps    The image file processed
 * @param parserContext Image parser context
 * @param parseCb       Bootloader parser callbacks
 *
 * @return Error code indicating success or failure.
 ******************************************************************************/
int32_t bootloader_ezsp_communication_main(ImageProperties_t *imageProps,
                                           ParserContext_t *parserContext,
                                           const BootloaderParserCallbacks_t* parseCb);

/***************************************************************************//**
 * Stop communication between the bootloader and external host.
 ******************************************************************************/
void bootloader_ezsp_communication_shutdown(void);

/** @} addtogroup Communication */
/** @} addtogroup Components */
#endif // BTL_EZSP_SPI_H
