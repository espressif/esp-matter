/***************************************************************************//**
 * @file
 * @brief Object Transfer Service Internal Datatypes and Definitions
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SLI_BT_OTS_DATATYPES_H
#define SLI_BT_OTS_DATATYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#define INVALID_SERVICE_HANDLE        0
#define INVALID_CONNECTION_HANDLE     0
#define INVALID_CHARACTERISTIC_HANDLE 0

#define CHECK_NULL(x)              \
  if (x == NULL) {                 \
    return SL_STATUS_NULL_POINTER; \
  }

#ifdef __cplusplus
};
#endif

#endif // SLI_BT_OTS_DATATYPES_H
