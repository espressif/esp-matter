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

#ifndef _OTA_BROADCAST_BOOTLOADER_TYPES_H_
#define _OTA_BROADCAST_BOOTLOADER_TYPES_H_

/**
 * @addtogroup ota_bootloader_common
 * @brief Set of types defined for ota-broadcast-bootloader.
 *
 * OTA bootloading plugins are usable to send firmware images Over The Air when
 * the application is running. When the firmware is downloaded to a device, a
 * bootloader can be started to replace the application in the flash to the one
 * just downloaded.
 *
 * All Connect bootloader-related code relies on the Gecko Bootloader for
 * bootloading and it must be installed on the device for these plugins to work.
 * For details on the Gecko Bootloader, see UG266.
 *
 * The Broadcast OTA plugins implement the OTA download operation in broadcast,
 * so the same image can be sent to many devices at the same time. The server
 * however requires to know the clients downloading the image, because it
 * implements error handling by querying all clients for missing segments, and
 * then the server will re-broadcast those segments.
 *
 * Communication relies on standard broadcast data messages, which means routing
 * is not available, and only clients that are in the range of the server can
 * download. However, the same device can be both client and server, i.e. after
 * downloading the image, a client can configure itself to be a server, and
 * provide the image to another part of the network.
 *
 * Sleepy end devices cannot be addressed in broadcast, but a sleepy end device
 * can reconnect as a normal end device while the OTA is active.
 *
 * Broadcast OTA uses a plugin configurable endpoint, which is 14 by default.
 *
 * Security can be also enabled as plugin configuration on the server, as well
 * as the interval of the messages. The client has a timeout plugin
 * configuration after which it stops the OTA session with an error.
 *
 * See UG235.06 for further details.
 *
 * @note OTA Broadcast Bootloading plugins are not available in MAC mode due to
 * the lack of endpoints.
 *
 * @{
 */

/**
 * @brief OTA Broadcast Bootloader return status codes.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberAfOtaBootloaderStatus
#else
typedef uint8_t EmberAfOtaBootloaderStatus;
enum
#endif
{
  EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_SUCCESS                        = 0x00,  /**< The generic "no error" message.  */
  EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_INVALID_CALL                   = 0x01,  /**< Indicates that some parameters are
                                                                                     invalid, or the OTA server/client
                                                                                     is not in the expected state for
                                                                                     the call */
  EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_BUSY                           = 0x02,  /**< Indicates that the OTA server is
                                                                                     busy performing another OTA task*/
  EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_NO_BUFFERS                     = 0x03,  /**< Indicates that the OTA server
                                                                                     couldn't allocate memory from the
                                                                                     heap (See  @ref memory_buffer for
                                                                                     details). */
  EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_DATA_UNDERFLOW                 = 0x04,  /**< Indicates that the application did
                                                                                     not provided the requested data
                                                                                     to the OTA server */
  EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_STACK_ERROR                    = 0x05,  /**< Indicates that the Connect stack
                                                                                     returned an error to the OTA
                                                                                     client/server. */
  EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_TIMEOUT                        = 0x06,  /**< Indicates that the OTA image
                                                                                     download is timed out during
                                                                                     missing segment requests. */
  EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_FAILED                         = 0x07,  /**< Indicates that the OTA image
                                                                                     download is failed during the
                                                                                     broadcast phase. */
  EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_ABORTED                        = 0x08,  /**< Indicates that the application
                                                                                     aborted the OTA process.*/
};

/**
 * @brief OTA Broadcast Bootloader target status codes, returned by
 * @ref emberAfPluginBootloaderServerGetTargetStatus().
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberAfOtaBootloaderTargetStatus
#else
typedef uint8_t EmberAfOtaBootloaderTargetStatus;
enum
#endif
{
  /**
   * The passed node ID does not appear in the current server target list of the
   * current ongoing process or there is no current ongoing process.
   */
  EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_INVALID                        = 0x00,
  /**
   * The target has not responded to any of the server's unicast messages.
   */
  EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_UNREACHABLE                    = 0x01,
  /**
   * The server is currently performing an image distribution process and the
   * target confirmed that it received the full image.
   */
  EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_DISTRIBUTION_COMPLETED         = 0x02,
  /**
   * The server is currently performing an image distribution process and the
   * target has partially received the image and distribution is continuing.
   */
  EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_DISTRIBUTION_ONGOING           = 0x03,
  /**
   * The server is currently performing an image distribution process and the
   * target reported that an error was encountered.
   */
  EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_DISTRIBUTION_FAILED            = 0x04,
  /**
   * The target has refused the current image.
   */
  EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_DISTRIBUTION_REFUSED           = 0x05,
  /**
   * The server is currently performing an image distribution process and the
   * target decided to abort the image download process. In this case, the
   * client also reports an application status.
   */
  EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_DISTRIBUTION_ABORTED           = 0x06,
  /**
   * The server is currently performing a target status request process and the
   * target has responded to the server's inquiry. In this case, the client also
   * reports an application status.
   */
  EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_STATUS_REQUEST_COMPLETED       = 0x07,
  /**
   * The server is currently performing a target status request process and the
   * target is not yet queried by the server.
   */
  EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_STATUS_REQUEST_ONGOING         = 0x08,
  /**
   * The server is currently performing a bootload request process and the target
   * has accepted to perform the requested image bootload.
   */
  EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_BOOTLOAD_REQUEST_ACCEPTED      = 0x09,
  /**
   * The server is currently performing a bootload request process and the target
   * is not yet reached by the server.
   */
  EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_BOOTLOAD_REQUEST_ONGOING       = 0x0A,
  /**
   * The server is currently performing a bootload request process and the target
   * has refused to perform the requested image bootload.
   */
  EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_BOOTLOAD_REQUEST_REFUSED       = 0x0B,
};

/**
 * @brief A value indicating that client application did not set the application
 * level target status in any of the client callbacks.
 */
#define EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_INVALID_APPLICATION_TARGET_STATUS    0xFF

/** @} // END addtogroup
 */

#endif // _OTA_BROADCAST_BOOTLOADER_TYPES_H_
