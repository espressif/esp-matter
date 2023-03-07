/***************************************************************************//**
 * @file
 * @brief Callbacks from BLE Apploader OTA DFU library
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

#ifndef BTL_APPLOADER_CALLBACK_H
#define BTL_APPLOADER_CALLBACK_H

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

// --------------------------------
// Callbacks from AppLoader library

void bootloader_apploader_connection_complete();
void bootloader_apploader_disconnection_complete();
uint32_t bootloader_apploader_get_bootloader_version();
uint32_t bootloader_apploader_get_application_version();
int32_t bootloader_apploader_parse_gbl(uint8_t *data, size_t len);
int32_t bootloader_apploader_parser_init();
int32_t bootloader_apploader_parser_finish();

/** @endcond */

#endif // BTL_APPLOADER_CALLBACK_H
