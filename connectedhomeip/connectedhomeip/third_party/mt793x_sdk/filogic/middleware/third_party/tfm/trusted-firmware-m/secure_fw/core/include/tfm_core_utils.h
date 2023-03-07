/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_CORE_UTILS_H__
#define __TFM_CORE_UTILS_H__

#include <stddef.h>
#include <stdint.h>

/**
 * \brief   Memory copy function for TF-M core
 *
 * \param[out] dest         Destination address of memory
 * \param[in]  src          Source address of memory
 * \param[in]  n            Number of bytes to copy
 *
 * \retval                  Destination address of memory
 * \note                    The function is used for copying same-sized object
 *                          only.
 */
void *tfm_core_util_memcpy(void *dest, const void *src, size_t n);

/**
 * \brief   Memory set function for TF-M core
 *
 * \param[out] s            Destination address of memory
 * \param[in]  c            Value to be written to memory
 * \param[in]  n            Number of bytes to be wirtten
 *
 * \retval                  Destination address of memory
 */
void *tfm_core_util_memset(void *s, int c, size_t n);

#endif /* __TFM_CORE_UTILS_H__ */
