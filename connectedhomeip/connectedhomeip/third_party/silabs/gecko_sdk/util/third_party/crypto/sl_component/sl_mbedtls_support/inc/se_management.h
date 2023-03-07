/***************************************************************************//**
 * @file
 * @brief Silicon Labs SE device management interface.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SE_MANAGEMENT_H
#define SE_MANAGEMENT_H

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

/***************************************************************************//**
 * \addtogroup sl_crypto
 * \{
 ******************************************************************************/

/***************************************************************************//**
 * \addtogroup sl_se_management Peripheral Instance Management: Secure Engine
 * \brief Concurrency management functions for Secure Engine mailbox access
 *
 * \{
 ******************************************************************************/

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT)

#include "sli_se_manager_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          Get ownership of the SE mailbox
 *
 * \return         0 if successful, negative on error
 */
__STATIC_INLINE int se_management_acquire(void)
{
  // Acquire SE manager lock
  return sli_se_lock_acquire() == SL_STATUS_OK ? 0 : -1;
}

/**
 * \brief          Release ownership of the SE mailbox
 *
 * \return         0 if successful, negative on error
 */
__STATIC_INLINE int se_management_release(void)
{
  // Release SE manager lock
  return sli_se_lock_release() == SL_STATUS_OK ? 0 : -1;
}

#ifdef __cplusplus
}
#endif

#endif /* SEMAILBOX_PRESENT */

/** \} (end addtogroup sl_se_management) */
/** \} (end addtogroup sl_crypto) */

/// @endcond

#endif /* SE_MANAGEMENT_H */
