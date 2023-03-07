/*
 * Copyright (c) 2014-2015 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef COMMON_FUNCTIONS_H_
#define COMMON_FUNCTIONS_H_

#include "ns_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Compare two bitstrings.
 *
 * Compare two bitstrings of specified length. The bit strings are in
 * big-endian (network) bit order.
 *
 * \param a pointer to first string
 * \param b pointer to second string
 * \param bits number of bits to compare
 *
 * \return true if the strings compare equal
 */
bool bitsequal(const uint8_t *a, const uint8_t *b, uint_fast8_t bits);

/*
 * Copy a bitstring
 *
 * Copy a bitstring of specified length. The bit string is in big-endian
 * (network) bit order. Bits beyond the bitlength at the destination are not
 * modified.
 *
 * For example, copying 4 bits sets the first 4 bits of dst[0] from src[0],
 * the lower 4 bits of dst[0] are unmodified.
 *
 * \param dst destination pointer
 * \param src source pointer
 * \param bits number of bits to copy
 *
 * \return the value of dst
 */
uint8_t *bitcopy(uint8_t *restrict dst, const uint8_t *restrict src, uint_fast8_t bits);

/*
 * Copy a bitstring and pad last byte with zeros
 *
 * Copy a bitstring of specified length. The bit string is in big-endian
 * (network) bit order. Bits beyond the bitlength in the last destination byte are
 * zeroed.
 *
 * For example, copying 4 bits sets the first 4 bits of dst[0] from src[0], and
 * the lower 4 bits of dst[0] are set to 0.
 *
 * \param dst destination pointer
 * \param src source pointer
 * \param bits number of bits to copy
 *
 * \return the value of dst
 */
uint8_t *bitcopy0(uint8_t *restrict dst, const uint8_t *restrict src, uint_fast8_t bits);

#ifdef __cplusplus
}
#endif
#endif /*__COMMON_FUNCTIONS_H_*/
