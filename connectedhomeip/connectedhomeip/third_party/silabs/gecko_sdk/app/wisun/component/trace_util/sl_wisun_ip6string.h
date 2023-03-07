/***************************************************************************//**
 * @file
 * @brief
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

#ifndef __SL_WISUN_IP6STRING_H__
#define __SL_WISUN_IP6STRING_H__

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "ip6string.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * Print binary IPv6 address to a string.
 *
 * String must contain enough room for full address, 40 bytes exact.
 * IPv4 tunneling addresses are not covered.
 *
 * @param[in]     ip6addr IPv6 address.
 * @param[out]    p buffer to write string to.
 * @return        length of generated string excluding the terminating null
 *                character
 *****************************************************************************/
static inline uint8_t sl_wisun_ip6tos(const void *ip6addr, char *p)
{
  return (uint8_t)ip6tos(ip6addr, p);
}
/**************************************************************************//**
 * Print binary IPv6 prefix to a string.
 *
 * String buffer `p` must contain enough room for a full address and prefix
 * length, 44 bytes exact. Bits in the `prefix` buffer beyond `prefix_len`
 * bits are not shown and only the bytes containing the prefix bits are read.
 * I.e. for a 20 bit prefix 3 bytes are read, and for a 0 bit prefix 0 bytes are
 * read (thus if `prefix_len` is zero, `prefix` can be NULL). `prefix_len`
 * must be 0 to 128.
 *
 * @param[in]     prefix IPv6 prefix.
 * @param[in]     prefix_len length of `prefix` in bits.
 * @param[out]    p buffer to write string to.
 * @return  length of generated string excluding the terminating null character,
 *          or 0 for an error, such as 'prefix_len' > 128
 *****************************************************************************/
static inline uint8_t sl_wisun_ip6_prefix_tos(const void *prefix,
                                              uint8_t prefix_len,
                                              char *p)
{
  return (uint8_t)ip6_prefix_tos(prefix, (uint_fast8_t)prefix_len, p);
}

/**************************************************************************//**
 * Convert numeric IPv6 address string to a binary.
 *
 * IPv4 tunneling addresses are not covered.
 *
 * @param[in]     ip6addr IPv6 address in string format.
 * @param[in]     len Length of ipv6 string, maximum of 41.
 * @param[out]    dest buffer for address. MUST be 16 bytes. Filled with 0 on error.
 * @return boolean set to true if conversion succeed, false if it didn't
 *****************************************************************************/
static inline bool sl_wisun_stoip6(const char *ip6addr, size_t len, void *dest)
{
  return stoip6(ip6addr, len, dest);
}

/**************************************************************************//**
 * Find out numeric IPv6 address prefix length.
 *
 * @param         ip6addr  IPv6 address in string format
 * @return        prefix length or 0 if it not given
 *****************************************************************************/
static inline char sl_wisun_sipv6_prefixlength(const char *ip6addr)
{
  return sipv6_prefixlength(ip6addr);
}

/**************************************************************************//**
 * Convert numeric IPv6 address string with prefix to a binary.
 *
 * IPv4 tunneling addresses are not covered.
 *
 * @param         ip6addr IPv6 address in string format.
 * @param         dest buffer for address. MUST be 16 bytes.
 * @param         prefix_len_out length of prefix, is set to -1 if no prefix given
 *
 * @return        0 on success, negative value otherwise. prefix_len_out
 *                contains prefix length.
 *****************************************************************************/
static inline int sl_wisun_stoip6_prefix(const char *ip6addr,
                                         void *dest,
                                         int16_t *prefix_len_out)
{
  return stoip6_prefix(ip6addr, dest, (int_fast16_t*)prefix_len_out);
}

#ifdef __cplusplus
}
#endif

#endif /* __SL_WISUN_IP6STRING_H__ */
