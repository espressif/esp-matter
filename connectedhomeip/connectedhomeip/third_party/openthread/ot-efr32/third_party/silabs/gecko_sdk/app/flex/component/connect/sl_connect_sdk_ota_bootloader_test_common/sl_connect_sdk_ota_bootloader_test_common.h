/***************************************************************************//**
 * @file
 * @brief sl_connect_sdk_ota_bootloader_test_common.h
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#ifndef _SL_CONNECT_SDK_OTA_BOOTLOADER_TEST_COMMON_H_
#define _SL_CONNECT_SDK_OTA_BOOTLOADER_TEST_COMMON_H_

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
// Application error status codes.
/// Client failed to initialize the image storage.
#define APPLICATION_STATUS_FLASH_INIT_FAILED     0xA0
/// Client failed to write the image segment received.
#define APPLICATION_STATUS_FLASH_WRITE_FAILED    0xA1
/// Client refused the request due to mismatching image tag.
#define APPLICATION_STATUS_WRONG_IMAGE_TAG       0xA2
/// Arbitrary image tag default value.
#define DEFAULT_IMAGE_TAG                        0x89
/// Default behavior to reset the OTA resume counter flag
#define DEFAULT_COUNTER_RESET                    false

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// Common OTA bootloader Image Tag.
extern uint8_t ota_bootloader_test_image_tag;
/// Flag to resume counter reset.
extern bool ota_resume_start_counter_reset;

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/**************************************************************************//**
 * This function initiates a bootload, meaning it will make the bootloader
 * reset and replace the appication firmware with the one received from the
 * server in the designated image storage space.
 *****************************************************************************/
extern void bootloader_flash_image(void);

#endif // _SL_CONNECT_SDK_OTA_BOOTLOADER_TEST_COMMON_H_
