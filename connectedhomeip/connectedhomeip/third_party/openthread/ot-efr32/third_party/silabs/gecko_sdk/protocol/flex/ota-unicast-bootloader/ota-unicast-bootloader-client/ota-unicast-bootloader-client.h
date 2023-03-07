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

#ifndef _OTA_UNICAST_BOOTLOADER_CLIENT_H_
#define _OTA_UNICAST_BOOTLOADER_CLIENT_H_

#include "ota-unicast-bootloader/ota-unicast-bootloader-protocol.h"
#include "ota-unicast-bootloader/ota-unicast-bootloader-types.h"

/**
 * @addtogroup ota_unicast_bootloader_client
 * @brief APIs/callbacks for ota-unicast-bootloader clients.
 *
 * @copydetails ota_unicast_bootloader_common
 *
 * See ota-unicast-bootloader-client.h and ota-unicast-bootloader-client.c
 * for source code.
 * @{
 */

/** @brief  An API for aborting an ongoing image download process.
 *
 * @param[in]   imageTag    A 1-byte tag that identifies the image the client
 * should no longer download.
 *
 * @return   An ::EmberAfOtaUnicastBootloaderStatus value of:
 * - ::EMBER_OTA_UNICAST_BOOTLOADER_STATUS_SUCCESS   If the ongoing image
 * download process was successfully aborted.
 * - ::EMBER_OTA_UNICAST_BOOTLOADER_STATUS_INVALID_CALL   If the client was not
 * currently involved in an image download process or it was currently
 * downloading an image with a different tag.
 */
EmberAfOtaUnicastBootloaderStatus emberAfPluginOtaUnicastBootloaderClientAbortImageDownload(
  uint8_t imageTag
  );

/**
 * @{
 * @name Callbacks
 */

/** @brief  A callback invoked when the client starts receiving a new image. The
 * application can choose to start receiving the image or ignore it. If the
 * application chooses to receive the image, other images sent out by other
 * servers shall be ignored until the client completes the download.
 *
 * @param[in]  serverId  The node ID of the server that initiated the new image
 * distribution process.
 *
 * @param[in]   imageTag    A 1-byte tag that identifies the incoming image.
 *
 * @param[in]   imageSize   The size in bytes of the new image.
 *
 * @param[out]  startIndex  The index of the first byte at which the image
 * download shall be started/resumed. The client can use this argument to resume
 * a partially downloaded image. If this value is not set, it defaults to 0
 * (that is, the download starts at the beginning of the image). Note, this is
 * ignored in case the server does not support download resume.
 *
 * @return Return @b true to accept the image or @b false to ignore it.
 */
bool emberAfPluginOtaUnicastBootloaderClientNewIncomingImageCallback(
  EmberNodeId serverId,
  uint8_t imageTag,
  uint32_t imageSize,
  uint32_t *startIndex
  );

/** @brief  A callback invoked when an image segment that is part of an image
 * the application chose to download was received.
 *
 * @param[in]   serverId    The node ID of the server that initiated the
 * image distribution process.
 *
 * @param[in]   startIndex    The index of the first byte of the passed segment.
 *
 * @param[in]   endIndex   The index of the last byte of the passed segment.
 *
 * @param[in]   imageTag   A 1-byte tag of the image the passed segment belongs
 * to.
 *
 * @param[in]   imageSegment    An array containing the image segment.
 */
void emberAfPluginOtaUnicastBootloaderClientIncomingImageSegmentCallback(
  EmberNodeId serverId,
  uint32_t startIndex,
  uint32_t endIndex,
  uint8_t imageTag,
  uint8_t *imageSegment
  );

/** @brief  A callback invoked to indicate that an image download has completed.
 *
 * @param[in] status   An ::EmberAfOtaUnicastBootloaderStatus value of:
 * - ::EMBER_OTA_UNICAST_BOOTLOADER_STATUS_SUCCESS  indicating that the full
 * image corresponding to the passed tag has been received. If this is the case,
 * the client previously handed all the image segments to the application using
 * the ::emberAfPluginOtaUnicastBootloaderClientIncomingImageSegmentCallback()
 * callback.
 * - ::EMBER_OTA_UNICAST_BOOTLOADER_STATUS_FAILED  indicating that the client
 * failed to fully download the image and the download process was terminated.
 * - ::EMBER_OTA_UNICAST_BOOTLOADER_STATUS_TIMEOUT indicating that the client
 * timed out waiting for a message from the server.
 * - ::EMBER_OTA_UNICAST_BOOTLOADER_STATUS_ABORTED indicating that the
 * application aborted the ongoing image download process as result of calling
 * the API ::emberAfPluginOtaUnicastBootloaderClientAbortImageDownload().
 *
 * @param[in] imageTag   A 1-byte tag of the image this callback refers to.
 *
 * @param[in] imageSize  The total size of the downloaded image in bytes. This
 * parameter is meaningful only in case the status parameter is set to
 * ::EMBER_OTA_UNICAST_BOOTLOADER_STATUS_SUCCESS.
 */
void emberAfPluginOtaUnicastBootloaderClientImageDownloadCompleteCallback(
  EmberAfOtaUnicastBootloaderStatus status,
  uint8_t imageTag,
  uint32_t imageSize
  );

/** @brief  A callback invoked to indicate that a server has requested to
 * perform a bootload operation at a certain point in time in the future.
 *
 * @param[in] serverId   The ID of the server the request came from.
 *
 * @param[in] imageTag   A 1-byte tag of the image this callback refers to.
 *
 * @param[in] bootloadDelayMs   The delay in milliseconds after which the client
 * has been requested to perform a bootload operation.
 *
 * @return Return @b true if the application accepted the request of bootloading
 * the specified image at the requested time, @b false otherwise.
 */
bool emberAfPluginOtaUnicastBootloaderClientIncomingRequestBootloadCallback(
  EmberNodeId serverId,
  uint8_t imageTag,
  uint32_t bootloadDelayMs
  );

/**
 * @}
 *
 * @}
 */

#endif // _OTA_UNICAST_BOOTLOADER_CLIENT_H_
