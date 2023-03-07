/***************************************************************************//**
 * @file
 * @brief Communication component implementing XMODEM
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

#ifndef BTL_COMM_XMODEM_H
#define BTL_COMM_XMODEM_H

#include "core/btl_util.h"
MISRAC_DISABLE
#include "em_common.h"
MISRAC_ENABLE

#include "api/btl_interface.h"
#if defined(BOOTLOADER_NONSECURE)
  #include "parser/gbl/btl_gbl_parser_ns.h"
#else
  #include "parser/gbl/btl_gbl_parser.h"
#endif

#include "communication/xmodem-parser/btl_xmodem.h"

/***************************************************************************//**
 * @addtogroup Components
 * @{
 * @addtogroup Communication
 * @{
 * @page communication_xmodem UART XMODEM
 *   By enabling the UART XMODEM communication component, the bootloader
 *   communication interface implements the XMODEM-CRC protocol over UART. This
 *   component makes the bootloader compatible with the legacy
 *   `serial-uart-bootloader` that was previously released with the EmberZnet
 *   and SL-Thread wireless stacks.
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

SL_PACK_START(1)
typedef struct {
  uint8_t padding;
  XmodemPacket_t packet;
} SL_ATTRIBUTE_PACKED XmodemReceiveBuffer_t;
SL_PACK_END()

typedef enum {
  MENU,
  IDLE,
  INIT_TRANSFER,
  WAIT_FOR_DATA,
  RECEIVE_DATA,
  BOOT,
  COMPLETE,
} XmodemState_t;

/** @endcond */

/***************************************************************************//**
 * Initialize hardware for the UART XMODEM Bootloader communication.
 ******************************************************************************/
void bootloader_xmodem_communication_init(void);

/***************************************************************************//**
 * Initialize communication between the UART XMODEM bootloader
 * and external host.
 *
 * @return Error code indicating success or failure.
 ******************************************************************************/
int32_t bootloader_xmodem_communication_start(void);

/***************************************************************************//**
 * Communication main for the UART XMODEM bootloader.
 *
 * @param imageProps    The image file processed
 * @param parseCb       Bootloader parser callbacks
 *
 * @return Error code indicating success or failure.
 ******************************************************************************/
int32_t bootloader_xmodem_communication_main(ImageProperties_t *imageProps,
                                             const BootloaderParserCallbacks_t* parseCb);

/** @} addtogroup Communication
 *  @} addtogroup Components
*/
#endif // BTL_COMM_XMODEM_H
