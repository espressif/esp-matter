/***************************************************************************//**
 * @file
 * @brief MPU API definition.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

/***************************************************************************//**
 * @addtogroup mpu RAM execution disable
 * @brief RAM execution disable utilities can be used to disable execution from
 *        RAM and other selected memory regions.
 * @details
 *   RAM execution disable utilities are useful to protect against code injection attacks.
 *   These utilities make use of MPU to disable execution from RAM and other selected
 *   memory regions.
 *
 * @{
 ******************************************************************************/

#ifndef SL_MPU_H
#define SL_MPU_H

#include "sl_status.h"

#if (defined (__MPU_PRESENT) && (__MPU_PRESENT == 1U)) || defined(DOXYGEN)

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * Configures internal SRAM as non-executable and enable MPU.
 *
 * @note This function configures the MPU in order to make the entire RAM as
 *       non-executable (with the exception of the functions marked as ramfunc).
 ******************************************************************************/
void sl_mpu_disable_execute_from_ram(void);

/***************************************************************************//**
 * Configures an address range as non-executable and enable MPU.
 *
 * @note Configures a MPU region in order to make an address range as
 *       non-executable. The memory region must have a size of at least 32 bytes.
 *
 * @param address_begin Beginning of memory segment.
 *
 * @param address_end   End of memory segment.
 *
 * @param size          Size of memory segment.
 *
 * @return 0 if successful. Error code otherwise.
 ******************************************************************************/
sl_status_t sl_mpu_disable_execute(uint32_t address_begin,
                                   uint32_t address_end,
                                   uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* defined (__MPU_PRESENT) && (__MPU_PRESENT == 1U) */
#endif /* SL_MPU_H */

/** @} (end addtogroup mpu) */
