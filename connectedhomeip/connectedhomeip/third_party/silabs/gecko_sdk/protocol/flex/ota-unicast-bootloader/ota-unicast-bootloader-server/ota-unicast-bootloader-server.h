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

#ifndef _OTA_UNICAST_BOOTLOADER_SERVER_H_
#define _OTA_UNICAST_BOOTLOADER_SERVER_H_

#include "ota-unicast-bootloader/ota-unicast-bootloader-protocol.h"
#include "ota-unicast-bootloader/ota-unicast-bootloader-types.h"

/**
 * @addtogroup ota_unicast_bootloader_server
 * @brief Macros and APIs for ota-unicast-bootloader server.
 *
 * @copydetails ota_unicast_bootloader_common
 *
 * See ota-unicast-bootloader-server.h and ota-unicast-bootloader-server.c for
 * source code.
 * @{
 */

/**
 * @brief The number of consecutive stack message submission errors or stack
 * related errors such as CSMA failures after which the plugin gives up.
 */
#define EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_MAX_STACK_ERRORS         8

/**
 * @brief The number of consecutive unicast attempts after which a target is
 * declared unreachable. Legal values for this are in the [0,7] range.
 */
#define EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_MAX_UNICAST_ERRORS       4

/**
 * @brief The time in milliseconds after which the server gives up waiting for
 * a response from a client.
 */
#define EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_RESPONSE_TIMEOUT_MS    250

//------------------------------------------------------------------------------
// APIs

/** @brief  Initiate the image distribution process.
 *
 * @param[in] targetId  The node ID of the target.
 *
 * @param[in] imageSize   The image size in bytes to be distributed.
 *
 * @param[in] imageTag    A 1-byte tag that will be embedded in the
 * server-to-client over-the-air messages. The application can use the image tag
 * for versioning purposes and/or for distinguishing between different image
 * types.
 *
 * @return An ::EmberAfOtaUnicastBootloaderStatus value of:
 * - ::EMBER_OTA_UNICAST_BOOTLOADER_STATUS_BUSY if an image distribution is
 * already in progress
 * - ::EMBER_OTA_UNICAST_BOOTLOADER_STATUS_INVALID_CALL if the given target or
 * the image size is invalid
 * - ::EMBER_OTA_UNICAST_BOOTLOADER_STATUS_SUCCESS if the image distribution
 * was successfully initiated.
 */
EmberAfOtaUnicastBootloaderStatus emberAfPluginOtaUnicastBootloaderServerInitiateImageDistribution(
  EmberNodeId targetId,
  uint32_t imageSize,
  uint8_t imageTag
  );

/** @brief  Request a target
 * device to initiate the bootload of a received image at some point in the
 * future.
 *
 * @param[in] bootloadDelayMs   The delay in milliseconds after which the
 * target should perform an image bootload.
 *
 * @param[in] imageTag  A 1-byte tag that identifies the image to be bootloaded
 * at the target device.
 *
 * @param[in] targetId  The node ID of the target.
 *
 * @return An ::EmberAfOtaUnicastBootloaderStatus value of:
 * - ::EMBER_OTA_UNICAST_BOOTLOADER_STATUS_SUCCESS if the plugin successfully
 * started the process to request a target and initiate a bootload.
 * If this is the case, the corresponding callback
 * ::emberAfPluginOtaUnicastBootloaderServerRequestTargetBootloadCompleteCallback()
 * is invoked when the request process is completed.
 * - ::EMBER_OTA_UNICAST_BOOTLOADER_STATUS_INVALID_CALL if some of the passed
 * parameters are invalid.
 * - ::EMBER_OTA_UNICAST_BOOTLOADER_STATUS_BUSY if the server is currently
 * involved in another over-the-air process.
 */
EmberAfOtaUnicastBootloaderStatus emberAfPluginUnicastBootloaderServerInitiateRequestTargetBootload(
  uint32_t bootloadDelayMs,
  uint8_t imageTag,
  EmberNodeId targetId
  );

/** @brief  Abort the ongoing process, such as image distribution
 *  or bootload request.
 *
 * @return An ::EmberAfOtaUnicastBootloaderStatus value of:
 * - ::EMBER_OTA_UNICAST_BOOTLOADER_STATUS_SUCCESS if the current ongoing
 * process was successfully aborted.
 * - ::EMBER_OTA_UNICAST_BOOTLOADER_STATUS_INVALID_CALL if the server is not
 * currently involved in any process.
 */
EmberAfOtaUnicastBootloaderStatus emberAfPluginOtaUnicastBootloaderServerAbortCurrentProcess(
  void
  );

/**
 * @{
 * @name Callbacks
 */

/** @brief  A callback invoked during an image distribution process to retrieve
 * a contiguous segment of the image being distributed.
 *
 * @param[in]   startIndex    The index of the first byte the application should
 * copy into the passed array.
 *
 * @param[in]   endIndex      The index of the last byte the application should
 * copy into the passed array.
 *
 * @param[in]   imageTag    A 1-byte tag of the image for which a segment is
 * requested.
 *
 * @param[out]  imageSegment    An array of (endIndex - startIndex + 1) length
 * to which the application should copy the requested image segment.
 *
 * @return A boolean indicating whether the application successfully copied the
 * requested bytes into the passed array. If the application returns @b false,
 * the server will abort the ongoing distribution process.
 */
bool emberAfPluginOtaUnicastBootloaderServerGetImageSegmentCallback(
  uint32_t startIndex,
  uint32_t endIndex,
  uint8_t imageTag,
  uint8_t *imageSegment
  );

/** @brief  A callback invoked when the image distribution process is
 * terminated.
 *
   @param[in]   status    An ::EmberAfOtaUnicastBootloaderStatus value of:
 * - ::EMBER_OTA_UNICAST_BOOTLOADER_STATUS_SUCCESS if the target confirms
 * that the full image is received.
 * - ::EMBER_OTA_UNICAST_BOOTLOADER_STATUS_DATA_UNDERFLOW if the application
 * failed to supply the requested image segments.
 * - ::EMBER_OTA_UNICAST_BOOTLOADER_STATUS_STACK_ERROR if the server encountered
 * multiple consecutive transmission errors. The Server gives up the image
 * distribution process if
 * ::EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_MAX_STACK_ERRORS consecutive
 * transmission errors are encountered.
 * - ::EMBER_OTA_UNICAST_BOOTLOADER_STATUS_FAILED if the distribution
 * process terminated prematurely because the target can't be reached.
 * - ::EMBER_OTA_UNICAST_BOOTLOADER_STATUS_UNREACHABLE if the server can not
 * establish communication with the target client.
 * - ::EMBER_OTA_UNICAST_BOOTLOADER_STATUS_ABORTED if the application aborted
 * the current image distribution process.
 * - ::EMBER_OTA_UNICAST_BOOTLOADER_STATUS_REFUSED if the client refused the
 * image.
 */
void emberAfPluginOtaUnicastBootloaderServerImageDistributionCompleteCallback(
  EmberAfOtaUnicastBootloaderStatus status
  );

/** @brief  A callback invoked when a bootload request process has completed.
 *
 * @param[in]   status    An ::EmberAfOtaUnicastBootloaderStatus value of:
 * - ::EMBER_OTA_UNICAST_BOOTLOADER_STATUS_SUCCESS if the target has been
 * requested to perform a bootload.
 * - ::EMBER_OTA_UNICAST_BOOTLOADER_STATUS_STACK_ERROR if the server encountered
 * multiple consecutive transmission errors. The Server gives up the
 * bootload request process if
 * ::EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_MAX_STACK_ERRORS consecutive
 * transmission errors are encountered.
 * - ::EMBER_OTA_UNICAST_BOOTLOADER_STATUS_ABORTED if the application aborted
 * the current bootload request process.
 * - ::EMBER_OTA_UNICAST_BOOTLOADER_STATUS_UNREACHABLE if the server can not
 * establish communication with the target client.
 */
void emberAfPluginOtaUnicastBootloaderServerRequestTargetBootloadCompleteCallback(
  EmberAfOtaUnicastBootloaderStatus status
  );

/**
 * @}
 *
 * @}
 */

#endif // _OTA_UNICAST_BOOTLOADER_SERVER_H_
