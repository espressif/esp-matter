/***************************************************************************//**
 * @file
 * @brief RAM Interrupt vector init API definition.
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
 * @addtogroup ram_interrupt_vector_init RAM interrupt vector initialization
 * @brief Simple utility to move the interrupt vector table to RAM.
 *
 * @details
 *   This utility will modify the VTOR register to point to a vector table
 *   located in RAM. The original content of the compiled interrupt vector
 *   table located in flash is copied in the RAM table.
 *
 * @{
 ******************************************************************************/

#ifndef SL_RAM_INTERRUPT_VECTOR_INIT_H
#define SL_RAM_INTERRUPT_VECTOR_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * Configures the interrupt vector pointer (VTOR) to point to a vector table
 * in RAM
 *
 * @note This function is a wrapper around CORE_InitNvicVectorTable function.
 *       The actual interrupt vector's content is copied into the RAM table
 *       defined in the source file of the function, and the VTOR pointer
 *       is set to this new table.
 ******************************************************************************/

void sl_ram_interrupt_vector_init(void);

#ifdef __cplusplus
}
#endif

#endif /* SL_RAM_INTERRUPT_VECTOR_INIT_H */

/** @} (end addtogroup ram_interrupt_vector_init) */
