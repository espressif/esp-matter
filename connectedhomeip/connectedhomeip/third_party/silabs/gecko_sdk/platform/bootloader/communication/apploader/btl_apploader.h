/***************************************************************************//**
 * @file
 * @brief Communication component implementing BLE Apploader OTA DFU protocol
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

#ifndef BTL_APPLOADER_H
#define BTL_APPLOADER_H

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
 * @page communication_apploader Bluetooth Apploader OTA DFU
 *   By enabling the Apploader communication component, the bootloader communication
 *   interface implements Bluetooth Apploader over-the-air (OTA) device firmware
 *   upgrade (DFU) protocol.
 ******************************************************************************/

/***************************************************************************//**
 * Initialize hardware for the BLE Apploader OTA DFU Bootloader communication.
 ******************************************************************************/
void bootloader_apploader_communication_init(void);

/***************************************************************************//**
 * Initialize communication between the BLE Apploader OTA DFU bootloader
 * and external host.
 *
 * @return Error code indicating success or failure.
 ******************************************************************************/
int32_t bootloader_apploader_communication_start(void);

/***************************************************************************//**
 * Communication main for the BLE Apploader OTA DFU bootloader.
 *
 * @param imageProps     The image file processed
 * @param parserContext  Image parser context
 * @param decryptContext Image decryption context
 * @param authContext    Image authentication context
 * @param parseCb        Bootloader parser callbacks
 *
 * @return Error code indicating success or failure.
 ******************************************************************************/
int32_t bootloader_apploader_communication_main(ImageProperties_t *imageProps,
                                                void *parserContext,
                                                void *decryptContext,
                                                void *authContext,
                                                const BootloaderParserCallbacks_t *parseCb);

/** @} addtogroup Communication */
/** @} addtogroup Components */
#endif // BTL_APPLOADER_H
