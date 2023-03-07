/***************************************************************************//**
 * @file
 * @brief Implementation of bit operations.
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

#ifndef SL_BIT_H
#define SL_BIT_H

/***************************************************************************//**
 * @addtogroup bit Bit Manipulation
 * @brief Bitwise operations
 * @{
 ******************************************************************************/

/****************************************************************************************************//**
 *                                               SL_SET_BIT()
 *
 * @brief    Set specified bit(s) in a value.
 *
 * @param    val     Value to modify by setting specified bit(s).
 *
 * @param    mask    Mask of bits to set.
 *
 * @return   Modified value with specified bit(s) set.
 *
 * @note     'val' & 'mask' SHOULD be unsigned integers.
 *******************************************************************************************************/

#define SL_SET_BIT(val, mask)                      ((val) = ((val) | (mask)))

/****************************************************************************************************//**
 *                                               SL_CLEAR_BIT()
 *
 * @brief    Clear specified bit(s) in a value.
 *
 * @param    val     Value to modify by clearing specified bit(s).
 *
 * @param    mask    Mask of bits to clear.
 *
 * @return   Modified value with specified bit(s) clear.
 *
 * @note     'val' & 'mask' SHOULD be unsigned integers.
 *
 * @note     'mask' SHOULD be cast with the same data type than 'val'.
 *******************************************************************************************************/

#define SL_CLEAR_BIT(val, mask)  ((val) = ((val) & (~(mask))))

/****************************************************************************************************//**
 *                                               SL_IS_BIT_SET()
 *
 * @brief    Determine whether the specified bit(s) in a value are set.
 *
 * @param    val     Value to check for specified bit(s) set.
 *
 * @param    mask    Mask of bits to check if set.
 *
 * @return   true, if ALL specified bit(s) are     set in value.
 *
 *           false, if ALL specified bit(s) are NOT set in value.
 *
 * @note     'val' & 'mask' SHOULD be unsigned integers.
 *
 * @note     NULL 'mask' allowed; returns 'false' since NO mask bits specified.
 *******************************************************************************************************/

#define SL_IS_BIT_SET(val, mask)  (((((val) & (mask)) == (mask)) && ((mask) != 0u)) ? (true) : (false))

/****************************************************************************************************//**
 *                                               SL_IS_BIT_CLEAR()
 *
 * @brief    Determine whether the specified bit(s) in a value are clear.
 *
 * @param    val     Value to check for specified bit(s) clear.
 *
 * @param    mask    Mask of bits to check if clear.
 *
 * @return   true, if ALL specified bit(s) are     clear in value.
 *
 *           false, if ALL specified bit(s) are NOT clear in value.
 *
 * @note     val' & 'mask' SHOULD be unsigned integers.
 *
 * @note     NULL 'mask' allowed; returns 'false' since NO mask bits specified.
 *******************************************************************************************************/
#define SL_IS_BIT_CLEAR(val, mask)  (((((val) & (mask)) == 0u) && ((mask) != 0u)) ? (true) : (false))

/****************************************************************************************************//**
 *                                           SL_IS_ANY_BIT_SET()
 *
 * @brief    Determine whether any specified bit(s) in a value are set.
 *
 * @param    val     Value to check for specified bit(s) set.
 *
 * @param    mask    Mask of bits to check if set (see Note #2).
 *
 * @return   true, if ANY specified bit(s) are     set in value.
 *
 *           false, if ALL specified bit(s) are NOT set in value.
 *
 * @note     'val' & 'mask' SHOULD be unsigned integers.
 *
 * @note     NULL 'mask' allowed; returns 'false' since NO mask bits specified.
 *******************************************************************************************************/

#define  SL_IS_ANY_BIT_SET(val, mask)  ((((val) & (mask)) == 0u) ? (false) : (true))

/****************************************************************************************************//**
 *                                           SL_IS_ANY_BIT_CLEAR()
 *
 * @brief    Determine whether any specified bit(s) in a value are clear.
 *
 * @param    val     Value to check for specified bit(s) clear.
 *
 * @param    mask    Mask of bits to check if clear (see Note #2).
 *
 * @return   true, if ANY specified bit(s) are     clear in value.
 *
 *           false,  if ALL specified bit(s) are NOT clear in value.
 *
 * @note     'val' & 'mask' SHOULD be unsigned integers.
 *
 * @note     NULL 'mask' allowed; returns 'false' since NO mask bits specified.
 *******************************************************************************************************/

#define  SL_IS_ANY_BIT_CLEAR(val, mask)  ((((val) & (mask)) == (mask))  ? (false) : (true))

/*******************************************************************************
 ******************************   DEFINES   ************************************
 ******************************************************************************/

/** @} (end addtogroup bit) */

#endif /* SL_BIT_H */
