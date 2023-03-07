/*******************************************************************************
 * @file
 * @brief Implementation of atomic operations.
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
#ifndef SL_ATOMIC_H
#define SL_ATOMIC_H

/*******************************************************************************
 * @addtogroup atomic Atomic Operations
 * @brief Atomic operations provide RAM store and read functionalities.
 * @n @section atomic_usage Atomic Operations Usage
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief         Perform an atomic load. Use when a variable must be read from
 *                RAM.
 *
 * @param dest    Variable where to copy the loaded value.
 *
 * @param source  Variable from where to load the value.
 *
 * @note          Does only support native types <= 32 bits.
 *
 * @note          Load operation on 32 bit value is atomic on ARM architecture.
 *
 * @note          Only the load operation from 'source' is guaranteed to be
 *                performed atomically. If writing to 'dest' implies a store,
 *                the load and store operations are not guaranteed to be
 *                performed atomically.
 ******************************************************************************/
#define sl_atomic_load(dest, source)     ((dest) = (source))

/*******************************************************************************
 * @brief         Perform an atomic store. Use when a value must be stored in
 *                RAM.
 *
 * @param dest    Variable where to store the value.
 *
 * @param source  Variable that contains the value to store in 'dest'.
 *
 * @note          Does only support native types <= 32 bits.
 *
 * @note          Store operation on 32 bit value is atomic on ARM architecture.
 *
 * @note          Only the store operation to 'dest' is guaranteed to be
 *                performed atomically. If reading from 'source' implies a load,
 *                the store and load operations are not guaranteed to be
 *                performed atomically.
 ******************************************************************************/
#define sl_atomic_store(dest, source)    ((dest) = (source))

/** @} (end addtogroup atomic) */

#endif /* SL_ATOMIC_H */
