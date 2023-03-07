/***************************************************************************//**
 * @file
 * @brief Set of APIs for the ota-broadcast-bootloader-test plugin.
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
#include "sl_connect_sdk_ota_bootloader_test_common.h"
#include "app_log.h"
#include "sl_cli.h"
#include "sl_connect_sdk_btl-interface.h"

#if defined(SL_CATALOG_CONNECT_OTA_BROADCAST_BOOTLOADER_SERVER_PRESENT)
#include "ota-broadcast-bootloader-server.h"
#endif // SL_CATALOG_CONNECT_OTA_BROADCAST_BOOTLOADER_SERVER_PRESENT

#if defined(SL_CATALOG_CONNECT_OTA_BROADCAST_BOOTLOADER_CLIENT_PRESENT)
#include "ota-broadcast-bootloader-client.h"
#endif // SL_CATALOG_CONNECT_OTA_BROADCAST_BOOTLOADER_CLIENT_PRESENT

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Maximum number of nodes for image transmission
#define MAX_TARGET_LIST_SIZE                     50

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// Timing event for scheduling the client bootload event.
EmberEventControl emAfPluginOtaBootloaderTestEventControl;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
#if defined(SL_CATALOG_CONNECT_OTA_BROADCAST_BOOTLOADER_SERVER_PRESENT)
/// Number of registered client nodes subject to image transmission.
static uint8_t target_list_length;
#endif
/// Client nodes list for image transmission.
static EmberNodeId target_list[MAX_TARGET_LIST_SIZE];

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * Timing event to schedule a bootload.
 *****************************************************************************/
void emAfPluginOtaBootloaderTestEventHandler(void)
{
  emberEventControlSetInactive(emAfPluginOtaBootloaderTestEventControl);
  bootloader_flash_image();
}

#if defined(SL_CATALOG_CONNECT_OTA_BROADCAST_BOOTLOADER_SERVER_PRESENT)
/**************************************************************************//**
 * A callback invoked on the OTA Bootloader Server during an
 * image distribution process to retrieve a contiguous segment of the image
 * being distributed.
 *****************************************************************************/
bool emberAfPluginOtaBootloaderServerGetImageSegmentCallback(uint32_t start_index,
                                                             uint32_t end_index,
                                                             uint8_t image_tag,
                                                             uint8_t *image_segment)
{
  app_log_info("(server): get segment, start: %lu, end: %lu, tag: 0x%x\n",
               (long unsigned int) start_index, (long unsigned int) end_index, image_tag);

  //Initialize bootloader (and flash part) if not yet initialized or in shutdown.
  if ( !emberAfPluginBootloaderInterfaceIsBootloaderInitialized() ) {
    if ( !emberAfPluginBootloaderInterfaceInit() ) {
      return false;
    }
  }

  if ( !emberAfPluginBootloaderInterfaceRead(start_index, end_index - start_index + 1, image_segment) ) {
    return false;
  }

  app_log_info(".");

  return true;
}

/**************************************************************************//**
 * A callback invoked on the OTA Bootloader Server when the
 * image distribution process is terminated. The application can use the
 * ::emberAfPluginBootloaderServerGetTargetStatus() API to retrieve the status
 * reported by each target device.
 *****************************************************************************/
void emberAfPluginOtaBootloaderServerImageDistributionCompleteCallback(EmberAfOtaBootloaderStatus status)
{
  uint8_t i;

  app_log_info("image distribution completed, 0x%x\n", status);

  for (i = 0; i < target_list_length; i++) {
    uint8_t application_status;

    EmberAfOtaBootloaderTargetStatus status =
      emberAfPluginBootloaderServerGetTargetStatus(target_list[i],
                                                   &application_status);

    app_log_info("Target index %d ID 0x%2X: status 0x%x appStatus 0x%x\n",
                 i, target_list[i], status, application_status);
  }

  emberAfPluginBootloaderInterfaceSleep();
}

/**************************************************************************//**
 *  A callback invoked on the OTA Bootloader Server when a
 * bootload request process has completed. Within this callback, the application
 * should use the ::emberAfPluginBootloaderServerGetTargetStatus() API to
 * retrieve the status and the application status reported by each target.
 *****************************************************************************/
void emberAfPluginBootloaderServerRequestTargetsBootloadCompleteCallback(EmberAfOtaBootloaderStatus status)
{
  uint8_t i;

  app_log_info("bootload request completed, 0x%x\n", status);

  for (i = 0; i < target_list_length; i++) {
    uint8_t application_status;

    EmberAfOtaBootloaderTargetStatus status =
      emberAfPluginBootloaderServerGetTargetStatus(target_list[i],
                                                   &application_status);

    app_log_info("Target index %d ID 0x%2X: status 0x%x appStatus 0x%x\n",
                 i, target_list[i], status, application_status);
  }
}

#endif // SL_CATALOG_CONNECT_OTA_BROADCAST_BOOTLOADER_SERVER_PRESENT

//------------------------------------------------------------------------------
// OTA Bootloader Client implemented callbacks.

#if defined(SL_CATALOG_CONNECT_OTA_BROADCAST_BOOTLOADER_CLIENT_PRESENT)

/**************************************************************************//**
 * A callback invoked when the OTA Bootloader Client starts
 * receiving a new image. The application can choose to start receiving the
 * image or it can ignore it. If the application chooses to receive the image,
 * other images sent out by other servers are ignored until the client completes
 * this download.
 *****************************************************************************/
bool emberAfPluginOtaBootloaderClientNewIncomingImageCallback(EmberNodeId serverId,
                                                              EmberNodeId *alternateServerId,
                                                              uint8_t imageTag)
{
  (void)alternateServerId;
  (void)serverId;
  // The client shall accept images with matching tag
  bool accept = (imageTag == ota_bootloader_test_image_tag);

  app_log_info("new incoming image %s (tag=0x%x)\n",
               ((accept) ? "ACCEPTED" : "REFUSED"),
               imageTag);

  return accept;
}

/**************************************************************************//**
 * A callback invoked when an image
 * segment, that is part of an image that the application elected to download,
 * was received on the OTA Bootloader Client.
 *****************************************************************************/
void emberAfPluginOtaBootloaderClientIncomingImageSegmentCallback(EmberNodeId serverId,
                                                                  uint32_t startIndex,
                                                                  uint32_t endIndex,
                                                                  uint8_t imageTag,
                                                                  uint8_t *imageSegment)
{
  (void)serverId;
  app_log_info("(client): incoming segment, start: %d, end: %d, tag: 0x%x\n",
               startIndex, endIndex, imageTag);

  //Initialize bootloader (and flash part) if not yet initialized or in shutdown.
  if ( !emberAfPluginBootloaderInterfaceIsBootloaderInitialized() ) {
    if ( !emberAfPluginBootloaderInterfaceInit() ) {
      app_log_error("init failed\n");

      emberAfPluginOtaBootloaderClientAbortImageDownload(imageTag,
                                                         APPLICATION_STATUS_FLASH_INIT_FAILED);
      return;
    }
  }

  if ( !emberAfPluginBootloaderInterfaceWrite(startIndex,
                                              endIndex - startIndex + 1,
                                              imageSegment) ) {
    app_log_error("write failed\n");

    emberAfPluginOtaBootloaderClientAbortImageDownload(imageTag,
                                                       APPLICATION_STATUS_FLASH_WRITE_FAILED);
    return;
  }

  app_log_info(".");
}

/**************************************************************************//**
 * A callback invoked on an OTA Bootloader Client to indicate
 * that an image downlaod is completed.
 *****************************************************************************/
void emberAfPluginOtaBootloaderClientImageDownloadCompleteCallback(EmberAfOtaBootloaderStatus status,
                                                                   uint8_t imageTag,
                                                                   uint32_t imageSize)
{
  if (status == EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_SUCCESS) {
    app_log_info("Image download COMPLETED tag=0x%x size=%d\n",
                 imageTag, imageSize);
  } else {
    app_log_error("Image download FAILED status=0x%x\n", status);
  }
}

/**************************************************************************//**
 *  A callback invoked by the OTA Bootloader Client plugin to indicate
 * that an OTA Bootloader Server has requested to perform a bootload operation
 * at a certain point in time in the future.
 *****************************************************************************/
bool emberAfPluginOtaBootloaderClientIncomingRequestBootloadCallback(EmberNodeId serverId,
                                                                     uint8_t imageTag,
                                                                     uint32_t bootloadDelayMs,
                                                                     uint8_t *applicationStatus)
{
  (void)serverId;
  // The client shall bootload an image with matching tag.

  if (applicationStatus == NULL) {
    app_log_error("argument error\n");
    return false;
  }

  bool accept = (imageTag == ota_bootloader_test_image_tag);

  if (accept) {
    app_log_info("bootload request for image with tag 0x%x accepted, will bootload in %d ms\n",
                 imageTag, bootloadDelayMs);
    // Schedule a bootload action.
    emberEventControlSetDelayMS(emAfPluginOtaBootloaderTestEventControl,
                                bootloadDelayMs);
  } else {
    *applicationStatus = APPLICATION_STATUS_WRONG_IMAGE_TAG;
    app_log_info("bootload request refused (tag 0x%x doesn't match)\n",
                 imageTag);
  }

  return accept;
}

#endif // SL_CATALOG_CONNECT_OTA_BROADCAST_BOOTLOADER_CLIENT_PRESENT

//------------------------------------------------------------------------------
// CLI commands.

/**************************************************************************//**
 * Sets up a target node at the given index in the list of target nodes.
 *****************************************************************************/
void cli_bootloader_broadcast_set_target(sl_cli_command_arg_t *arguments)
{
  if ((arguments == NULL)
      || (arguments->argv == NULL)
      || (arguments->argv[arguments->arg_ofs + 0] == NULL)
      || (arguments->argv[arguments->arg_ofs + 1] == NULL)) {
    app_log_error("argument error\n");
    return;
  }

  uint8_t target_index = sl_cli_get_argument_uint8(arguments, 0);
  EmberNodeId target_id = sl_cli_get_argument_uint16(arguments, 1);

  if (target_index >= MAX_TARGET_LIST_SIZE) {
    app_log_info("invalid index!\n");
    return;
  }

  target_list[target_index] = target_id;
  app_log_info("target set\n");
}

/**************************************************************************//**
 * Initiates a broadcast image distribution over the air, describing the image
 * with the given image tag, sent to the given number of nodes set up in the
 * target list.
 *****************************************************************************/
void cli_bootloader_broadcast_broadcast_distribute(sl_cli_command_arg_t *arguments)
{
#if defined(SL_CATALOG_CONNECT_OTA_BROADCAST_BOOTLOADER_SERVER_PRESENT)
  if ((arguments == NULL)
      || (arguments->argv == NULL)
      || (arguments->argv[arguments->arg_ofs + 0] == NULL)
      || (arguments->argv[arguments->arg_ofs + 1] == NULL)
      || (arguments->argv[arguments->arg_ofs + 2] == NULL)) {
    app_log_error("argument error\n");
    return;
  }

  uint32_t image_size = sl_cli_get_argument_uint32(arguments, 0);
  uint8_t image_tag = sl_cli_get_argument_uint8(arguments, 1);
  target_list_length = sl_cli_get_argument_uint8(arguments, 2);

  EmberAfOtaBootloaderStatus status =
    emberAfPluginOtaBootloaderServerInitiateImageDistribution(image_size,
                                                              image_tag,
                                                              target_list,
                                                              target_list_length);
  if (status == EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_SUCCESS) {
    app_log_info("image distribution initiated\n");
  } else {
    app_log_error("image distribution failed 0x%x\n", status);
  }
#else
  (void)arguments;
  app_log_info("OTA bootloader server plugin not included\n");
#endif // SL_CATALOG_CONNECT_OTA_BROADCAST_BOOTLOADER_SERVER_PRESENT
}

/**************************************************************************//**
 * Server-side request is sent to all target nodes to flash the received image
 * and restart.
 *****************************************************************************/
void cli_bootloader_broadcast_request_bootload(sl_cli_command_arg_t *arguments)
{
#if defined(SL_CATALOG_CONNECT_OTA_BROADCAST_BOOTLOADER_SERVER_PRESENT)
  if ((arguments == NULL)
      || (arguments->argv == NULL)
      || (arguments->argv[arguments->arg_ofs + 0] == NULL)
      || (arguments->argv[arguments->arg_ofs + 1] == NULL)
      || (arguments->argv[arguments->arg_ofs + 2] == NULL)) {
    app_log_error("argument error\n");
    return;
  }

  uint32_t delay_ms = sl_cli_get_argument_uint32(arguments, 0);
  uint8_t image_tag = sl_cli_get_argument_uint8(arguments, 1);
  target_list_length = sl_cli_get_argument_uint8(arguments, 2);

  EmberAfOtaBootloaderStatus status =
    emberAfPluginBootloaderServerInitiateRequestTargetsBootload(delay_ms,
                                                                image_tag,
                                                                target_list,
                                                                target_list_length);
  if (status == EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_SUCCESS) {
    app_log_info("bootload request initiated\n");
  } else {
    app_log_error("bootload request failed 0x%x\n", status);
  }
#else
  (void)arguments;
  app_log_warning("OTA bootloader server plugin not included\n");
#endif // SL_CATALOG_CONNECT_OTA_BROADCAST_BOOTLOADER_SERVER_PRESENT
}
