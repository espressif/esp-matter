/***************************************************************************//**
 * @file
 * @brief Communication component implementing the BGAPI UART DFU protocol
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

#ifndef BTL_COMM_BGAPI_H
#define BTL_COMM_BGAPI_H

#include "core/btl_util.h"
#include "sl_status.h"
#include "api/btl_interface.h"

#if defined(BOOTLOADER_NONSECURE)
  #include "parser/gbl/btl_gbl_parser_ns.h"
#else
  #include "parser/gbl/btl_gbl_parser.h"
#endif

MISRAC_DISABLE
#include "em_common.h"
MISRAC_ENABLE

/***************************************************************************//**
 * @addtogroup Components
 * @{
 * @addtogroup Communication
 * @{
 * @page communication_bgapi BGAPI UART DFU
 *   By enabling the BGAPI communication component, the bootloader communication
 *   interface implements the UART DFU protocol using BGAPI commands. This
 *   component makes the bootloader compatible with the legacy UART bootloader that
 *   was previously released with the Silicon Labs Bluetooth stack.
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

SL_PACK_START(1)
typedef struct {
  uint8_t type;
  uint8_t len;
  uint8_t class;
  uint8_t command;
} SL_ATTRIBUTE_PACKED BgapiPacketHeader_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  uint8_t mode;
} SL_ATTRIBUTE_PACKED BgapiDataCmdSysReset_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  uint32_t address;
} SL_ATTRIBUTE_PACKED BgapiDataCmdDfuFlashSetAddress_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  uint8_t len;
  uint8_t data[256];
} SL_ATTRIBUTE_PACKED BgapiDataCmdDfuFlashUpload_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  uint16_t error;
} SL_ATTRIBUTE_PACKED BgapiDataRsp_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  uint32_t bootloaderVersion;
} SL_ATTRIBUTE_PACKED BgapiDataEvtBoot_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  uint16_t error;
} SL_ATTRIBUTE_PACKED BgapiDataEvtBootFailure_t;
SL_PACK_END()

SL_PACK_START(1)
typedef struct {
  uint8_t padding[3]; // 3 bytes padding to ensure DFU Flash Upload data[] array
                      // is word aligned
  BgapiPacketHeader_t header;
  union body {
    BgapiDataCmdSysReset_t            reset;
    BgapiDataCmdDfuFlashSetAddress_t  flashSetAddress;
    BgapiDataCmdDfuFlashUpload_t      flashUpload;
    BgapiDataRsp_t                    response;
    BgapiDataEvtBoot_t                boot;
    BgapiDataEvtBootFailure_t         bootFailure;
  } body;
} SL_ATTRIBUTE_PACKED BgapiPacket_t;
SL_PACK_END()

// --------------------------------
// Commands

typedef enum {
// DFU commands
  DFU_RESET = 0x00,
  DFU_FLASH_SET_ADDRESS = 0x01,
  DFU_FLASH_UPLOAD = 0x02,
  DFU_FLASH_UPLOAD_FINISH = 0x03,
// System commands
  SYSTEM_RESET = 0x01
} BgapiCommands_t;

#define BGAPI_PACKET_TYPE_COMMAND 0x20
#define BGAPI_PACKET_TYPE_EVENT   0xA0

#define BGAPI_PACKET_CLASS_DFU    0x00
#define BGAPI_PACKET_CLASS_SYSTEM 0x01

// --------------------------------
// Events

#define BGAPI_EVENT_DFU_BOOT()            \
  {                                       \
    .header = { 0xA0, 0x04, 0x00, 0x00 }, \
    .body.boot.bootloaderVersion = 0x0    \
  }

#define BGAPI_EVENT_DFU_BOOT_FAILURE(error) \
  {                                         \
    .header = { 0xA0, 0x02, 0x00, 0x01 },   \
    .body.response = { (error) }            \
  }

// --------------------------------
// Responses

#define BGAPI_RESPONSE_DEFAULT            \
  {                                       \
    .header = { 0x20, 0x02, 0x00, 0x00 }, \
    .body.response = { 0x0000 }           \
  }

/** @endcond */

/***************************************************************************//**
 * Initialize hardware for the BGAPI UART DFU Bootloader communication.
 ******************************************************************************/
void bootloader_bgapi_communication_init(void);

/***************************************************************************//**
 * Initialize communication between the BGAPI UART DFU bootloader
 * and external host.
 *
 * @return Error code indicating success or failure.
 ******************************************************************************/
int32_t bootloader_bgapi_communication_start(void);

/***************************************************************************//**
 * Communication main for the BGAPI UART DFU bootloader.
 *
 * @param imageProps    The image file processed
 * @param parserContext Image parser context
 * @param parseCb       Bootloader parser callbacks
 *
 * @return Error code indicating success or failure.
 ******************************************************************************/
int32_t bootloader_bgapi_communication_main(ImageProperties_t *imageProps,
                                            ParserContext_t *parserContext,
                                            const BootloaderParserCallbacks_t* parseCb);

/** @} addtogroup Communication */
/** @} addtogroup Components */
#endif // BTL_COMM_BGAPI_H
