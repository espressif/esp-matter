/***************************************************************************//**
 * @file
 * @brief Set of APIs for the sl_connect_sdk_ota_bootloader_test_common component.
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
// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"
#include "app_log.h"
#include "sl_connect_sdk_ota_bootloader_test_common.h"
#include "sl_connect_sdk_btl-interface.h"
#include "sl_cli.h"

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// The image tag the client shall accept.
uint8_t ota_bootloader_test_image_tag = DEFAULT_IMAGE_TAG;
/// Default behavior to OTA resume counter reset.
bool ota_resume_start_counter_reset = DEFAULT_COUNTER_RESET;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * Brief description of my_public_function().
 *****************************************************************************/
void cli_bootloader_get_version(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  uint16_t bl_version;

  emberAfPluginBootloaderInterfaceGetVersion(&bl_version);

  app_log_info("bootloader version: %d\n", bl_version);
}

/**************************************************************************//**
 * Brief description of my_public_function().
 *****************************************************************************/
void cli_bootloader_init(sl_cli_command_arg_t *arguments)
{
  (void) arguments;

  if (emberAfPluginBootloaderInterfaceInit()) {
    app_log_info("bootloader init succeeded!\n");
  } else {
    app_log_error("bootloader init failed! wrong chip?\n");
  }
}

/**************************************************************************//**
 * Brief description of my_public_function().
 *****************************************************************************/
void cli_bootloader_sleep(sl_cli_command_arg_t *arguments)
{
  (void) arguments;

  emberAfPluginBootloaderInterfaceSleep();
  app_log_info("sleep bootloader and flash part\n");
}

void cli_bootloader_flash_erase(sl_cli_command_arg_t *arguments)
{
  (void) arguments;

  app_log_info("flash erase started\n");
  emberAfPluginBootloaderInterfaceChipErase();
  ota_resume_start_counter_reset = true;
}

/**************************************************************************//**
 * Brief description of my_public_function().
 *****************************************************************************/
void cli_bootloader_validate_image(sl_cli_command_arg_t *arguments)
{
  (void) arguments;

  if (!emberAfPluginBootloaderInterfaceValidateImage()) {
    app_log_error("Image is invalid!\n");
  } else {
    app_log_info("Image is valid!\n");
  }
}

/**************************************************************************//**
 * Brief description of my_public_function().
 *****************************************************************************/
void cli_bootloader_flash_erase_slot(sl_cli_command_arg_t *arguments)
{
  if ((arguments == NULL)
      || (arguments->argv == NULL)
      || (arguments->argv[arguments->arg_ofs + 0] == NULL)) {
    app_log_error("argument error\n");
    return;
  }

  uint32_t slot = sl_cli_get_argument_uint32(arguments, 0);

  app_log_info("flash erasing slot %lu started\n", (long unsigned int) slot);

  if ( emberAfPluginBootloaderInterfaceChipEraseSlot(slot) ) {
    app_log_info("flash erase successful!\n");
  } else {
    app_log_error("flash erase failed!\n");
  }
}

/**************************************************************************//**
 * Brief description of my_public_function().
 *****************************************************************************/
void cli_bootloader_flash_image(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  bootloader_flash_image();
}

/**************************************************************************//**
 * Brief description of my_public_function().
 *****************************************************************************/
void cli_bootloader_flash_read(sl_cli_command_arg_t *arguments)
{
  if ((arguments == NULL)
      || (arguments->argv == NULL)
      || (arguments->argv[arguments->arg_ofs + 0] == NULL)
      || (arguments->argv[arguments->arg_ofs + 1] == NULL)) {
    app_log_error("argument error\n");
    return;
  }

  uint32_t address = sl_cli_get_argument_uint32(arguments, 0);
  uint8_t length = sl_cli_get_argument_uint8(arguments, 1);
  uint8_t buff[255];

  if (emberAfPluginBootloaderInterfaceRead(address, length, buff)) {
    app_log_info("flash read succeeded!\n");
    app_log_info("address: %lu, length: %d, data:\n", (long unsigned int) address, length);
    for (uint8_t i = 0; i < length; i++) {
      app_log_info("0x%x ", buff[i]);
    }
    app_log_info("\n");
  } else {
    app_log_error("flash read failed!\n");
  }
}

/**************************************************************************//**
 * Brief description of my_public_function().
 *****************************************************************************/
void cli_bootloader_flash_write(sl_cli_command_arg_t *arguments)
{
  if ((arguments == NULL)
      || (arguments->argv == NULL)
      || (arguments->argv[arguments->arg_ofs + 0] == NULL)
      || (arguments->argv[arguments->arg_ofs + 1] == NULL)) {
    app_log_error("argument error\n");
    return;
  }

  uint32_t address = sl_cli_get_argument_uint32(arguments, 0);
  size_t length;

  uint8_t *data_buff = sl_cli_get_argument_hex(arguments, 1, &length);

  if (emberAfPluginBootloaderInterfaceWrite(address, length, data_buff)) {
    app_log_info("flash write succeeded!\n");
  } else {
    app_log_error("flash write failed!\n");
  }
}

/**************************************************************************//**
 * Brief description of my_public_function().
 *****************************************************************************/
void cli_bootloader_set_tag(sl_cli_command_arg_t *arguments)
{
  if ((arguments == NULL)
      || (arguments->argv == NULL)
      || (arguments->argv[arguments->arg_ofs + 0] == NULL)) {
    app_log_error("argument error\n");
    return;
  }

  uint8_t new_image_tag = 0;
  new_image_tag = sl_cli_get_argument_uint8(arguments, 0);
  if (new_image_tag != ota_bootloader_test_image_tag) {
    ota_bootloader_test_image_tag = new_image_tag;
    ota_resume_start_counter_reset = true;
  }
  app_log_info("image tag set\n");
}

/**************************************************************************//**
 * This function initiates a bootload.
 *****************************************************************************/
void bootloader_flash_image(void)
{
  if (!emberAfPluginBootloaderInterfaceIsBootloaderInitialized()) {
    if (!emberAfPluginBootloaderInterfaceInit()) {
      app_log_error("bootloader init failed\n");
      return;
    }
  }

  emberAfPluginBootloaderInterfaceBootload();

  // If we get here bootload process failed.
  app_log_info("bootload failed!\n");
}
