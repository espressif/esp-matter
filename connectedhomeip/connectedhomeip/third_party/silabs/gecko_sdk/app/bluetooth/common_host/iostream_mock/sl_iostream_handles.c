/***************************************************************************//**
 * @file
 * @brief IO Stream Handles mock implementation source file
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_iostream_handles.h"

/// Dummy definition of sl_iostream_instances_count for compatibility.
const unsigned int sl_iostream_instances_count = 0u;

/// Dummy definition of sl_iostream_instance_mock for compatibility.
sl_iostream_instance_info_t sl_iostream_instance_mock = {
  .handle = NULL,
  .name = "dummy-value",
  .type = DUMMY_VALUE,
  .periph_id = 0,
  .init = NULL,
};

/// Dummy definition of sl_iostream_instances_info for compatibility.
const sl_iostream_instance_info_t *sl_iostream_instances_info[] = {
  &sl_iostream_instance_mock,
};
