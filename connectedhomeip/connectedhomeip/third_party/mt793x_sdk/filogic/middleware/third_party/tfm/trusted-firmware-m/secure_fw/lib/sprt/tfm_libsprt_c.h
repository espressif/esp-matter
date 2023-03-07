/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_LIBSPRT_C_H__
#define __TFM_LIBSPRT_C_H__

#include <stddef.h>

/**
 * \brief   This function moves 'n' bytes from 'src' to 'dest'.
 *
 * \param[out]  dest        Destination address
 * \param[in]   src         Source address
 * \param[in]   n           Number of bytes to be moved
 *
 * \retval      dest        Destination address
 * \note                    Memory overlap has been taken into consideration
 *                          and processed properly in the function.
 */
void *tfm_sprt_c_memmove(void *dest, const void *src, size_t n);

/**
 * \brief   This function copies 'n' bytes from 'src' to 'dest'.
 *
 * \param[out]  dest        Destination address
 * \param[in]   src         Source address
 * \param[in]   n           Number of bytes to be copied
 *
 * \retval      dest        Destination address
 * \note                    It has the same effect as tfm_sprt_c_memmove().
 */
void *tfm_sprt_c_memcpy(void *dest, const void *src, size_t n);

/**
 * \brief   Compare the first 'n' bytes of the memory areas 's1' and 's2'.
 *
 * \param[in]   s1          The address of the first memory area
 * \param[in]   s2          The address of the second memory area
 * \param[in]   n           The size(Byte) to compare
 *
 * \retval > 0              The first n bytes of s1 great than the first n
 *                          bytes of s2
 * \retval < 0              The first n bytes of s1 less than the first n
 *                          bytes of s2
 * \retval = 0              The first n bytes of s1 equal to the first n
 *                          bytes of s2
 */
int tfm_sprt_c_memcmp(const void *s1, const void *s2, size_t n);

#endif /* __TFM_LIBSPRT_C_H__ */
