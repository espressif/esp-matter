/***************************************************************************//**
 * @file
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

#ifndef _OTA_UNICAST_BOOTLOADER_TYPES_H_
#define _OTA_UNICAST_BOOTLOADER_TYPES_H_

/**
 * @addtogroup ota_unicast_bootloader_common
 * @brief Macros and types defined for ota-unicast-bootloaders.
 *
 * OTA bootloading plugins are usable to send firmware images Over The Air when
 * the application is running. When the firmware is downloaded to a device, a
 * bootloader can be started to replace the application in the flash to the one
 * just downloaded.
 *
 * All Connect bootloader related code relies on the Gecko Bootloader for
 * bootloading and it must be installed on the device for these plugins to work.
 * For details on the Gecko Bootloader, see UG266.
 *
 * The Unicast OTA plugins implement the OTA download operation in a unicast,
 * addressed way, so only a single client can be addressed from a server in an
 * OTA session, and downloading images to multiple devices will require the
 * server to send the image multiple times. Communication relies on standard
 * unicast data messages, which also means that the routing provided by the
 * Connect stack is availble.
 *
 * Although bootloading sleepy end devices is theoretically possible with
 * polling, it is not very effective, and it's probably simpler to reconnect
 * as a normal end device while the OTA is active.
 *
 * Unicast OTA uses a plugin configurable endpoint, which is 13 by default.
 *
 * Security can be also enabled as plugin configuration on the server, as well
 * as the interval of the messages. The client has a timeout plugin
 * configuration after which it stops the OTA session with an error.
 *
 * See UG235.06 for further details.
 *
 * @note OTA Unicast Bootloading plugins are not available in MAC mode due to
 * the lack of endpoints.
 *
 * @{
 */

/**
 * @brief OTA Unicast Bootloader return status codes.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberAfOtaUnicastBootloaderStatus
#else
typedef uint8_t EmberAfOtaUnicastBootloaderStatus;
enum
#endif
{
  /**
   *  The generic "no error" message.
   */
  EMBER_OTA_UNICAST_BOOTLOADER_STATUS_SUCCESS                        = 0x00,
  /**
   * Indicates that some parameters are invalid, or the OTA server/client is
   *  not in the expected state for the call.
   */
  EMBER_OTA_UNICAST_BOOTLOADER_STATUS_INVALID_CALL                   = 0x01,
  /**
   * Indicates that the OTA server is busy performing another OTA task.
   */
  EMBER_OTA_UNICAST_BOOTLOADER_STATUS_BUSY                           = 0x02,
  /**
   * Indicates that the application did not provided the requested data to the
   * OTA server.
   */
  EMBER_OTA_UNICAST_BOOTLOADER_STATUS_DATA_UNDERFLOW                 = 0x03,
  /**
   * Indicates that the Connect stack returned an error to the OTA server.
   */
  EMBER_OTA_UNICAST_BOOTLOADER_STATUS_STACK_ERROR                    = 0x04,
  /**
   * Indicates that the image download is timed out on the OTA client.
   */
  EMBER_OTA_UNICAST_BOOTLOADER_STATUS_TIMEOUT                        = 0x05,
  /**
   * Indicates that the image download is failed because the connection
   * between the client and server is lost.
   */
  EMBER_OTA_UNICAST_BOOTLOADER_STATUS_FAILED                         = 0x06,
  /**
   * Indicates that the application is aborted the current OTA process.
   */
  EMBER_OTA_UNICAST_BOOTLOADER_STATUS_ABORTED                        = 0x07,
  /**
   * Indicates that the client refused the image.
   */
  EMBER_OTA_UNICAST_BOOTLOADER_STATUS_REFUSED                        = 0x08,
  /**
   * Indicates that the server can not establish communication with the target
   * client.
   */
  EMBER_OTA_UNICAST_BOOTLOADER_STATUS_UNREACHABLE                    = 0x09,
};

/** @} // END addtogroup
 */

#endif // _OTA_UNICAST_BOOTLOADER_TYPES_H_
