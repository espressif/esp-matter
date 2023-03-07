/***************************************************************************//**
 * @brief Macros, includes, and types for ota-unicast-bootloader clients.
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

#ifndef _OTA_UNICAST_BOOTLOADER_CLIENT_INTERNAL_H_
#define _OTA_UNICAST_BOOTLOADER_CLIENT_INTERNAL_H_

#include "ota-unicast-bootloader/ota-unicast-bootloader-protocol.h"
#include "ota-unicast-bootloader/ota-unicast-bootloader-common-internal.h"

//------------------------------------------------------------------------------
// Internal states

#define STATE_OTA_CLIENT_IDLE                                               0x00
#define STATE_OTA_CLIENT_IMAGE_DOWNLOAD_WAITING                             0x01

#define clientIsIdle()  (internalState == STATE_OTA_CLIENT_IDLE)
#define clientInImageDownload() \
  (internalState == STATE_OTA_CLIENT_IMAGE_DOWNLOAD_WAITING)

#endif // _OTA_UNICAST_BOOTLOADER_CLIENT_INTERNAL_H_
