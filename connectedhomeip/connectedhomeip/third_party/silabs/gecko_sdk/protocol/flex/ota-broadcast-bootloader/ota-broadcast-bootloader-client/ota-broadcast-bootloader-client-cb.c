/***************************************************************************//**
 * @brief Set of weakly defined callbacks for ota broadcast bootloader client.
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

#include "stack/include/ember.h"
#include "ota-broadcast-bootloader-client.h"

//------------------------------------------------------------------------------
// Weak callbacks definitions

WEAK(bool emberAfPluginOtaBootloaderClientNewIncomingImageCallback(EmberNodeId serverId,
                                                                   EmberNodeId *alternateServerId,
                                                                   uint8_t imageTag))
{
  (void)serverId;
  (void)alternateServerId;
  (void)imageTag;

  return false;
}

WEAK(void emberAfPluginOtaBootloaderClientIncomingImageSegmentCallback(EmberNodeId serverId,
                                                                       uint32_t startIndex,
                                                                       uint32_t endIndex,
                                                                       uint8_t imageTag,
                                                                       uint8_t *imageSegment))
{
  (void)serverId;
  (void)startIndex;
  (void)endIndex;
  (void)imageTag;
  (void)imageSegment;
}

WEAK(void emberAfPluginOtaBootloaderClientImageDownloadCompleteCallback(EmberAfOtaBootloaderStatus status,
                                                                        uint8_t imageTag,
                                                                        uint32_t imageSize))
{
  (void)status;
  (void)imageTag;
  (void)imageSize;
}

WEAK(void emberAfPluginOtaBootloaderClientIncomingRequestStatusCallback(EmberNodeId serverId,
                                                                        uint8_t applicationServerStatus,
                                                                        uint8_t *applicationStatus))
{
  (void)serverId;
  (void)applicationServerStatus;
  (void)applicationStatus;
}

WEAK(bool emberAfPluginOtaBootloaderClientIncomingRequestBootloadCallback(EmberNodeId serverId,
                                                                          uint8_t imageTag,
                                                                          uint32_t bootloadDelayMs,
                                                                          uint8_t *applicationStatus))
{
  (void)serverId;
  (void)imageTag;
  (void)bootloadDelayMs;
  (void)applicationStatus;

  return false;
}
