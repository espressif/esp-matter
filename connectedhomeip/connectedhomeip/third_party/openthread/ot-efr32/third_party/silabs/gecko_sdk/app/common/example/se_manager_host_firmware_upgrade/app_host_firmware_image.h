/***************************************************************************//**
 * @file app_host_firmware_image.h
 * @brief The host firmware image data.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef APP_HOST_FIRMWARE_IMAGE_H
#define APP_HOST_FIRMWARE_IMAGE_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_host_firmware_upgrade.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Get start address of the host firmware image.
 *
 * @returns Returns start address of the host firmware image.
 ******************************************************************************/
const uint8_t * get_host_firmware_addr(void);

/***************************************************************************//**
 * Get size of the host firmware image.
 *
 * @returns Returns size of the host firmware image.
 ******************************************************************************/
uint32_t get_host_firmware_size(void);

#endif  // APP_HOST_FIRMWARE_IMAGE_H
