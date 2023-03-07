/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _CC_SEC_DEFS_H
#define _CC_SEC_DEFS_H

/*!
@file
@brief This file contains general hash definitions and types.
*/

#ifdef __cplusplus
extern "C"
{
#endif

/*! The hashblock size in words. */
#define HASH_BLOCK_SIZE_IN_WORDS                16

/*! SHA256 result size in words. */
#define HASH_RESULT_SIZE_IN_WORDS               8
/*! SHA256 result size in Bytes. */
#define HASH_RESULT_SIZE_IN_BYTES               32

/*! Defines the hash result array. */
typedef uint32_t CCHashResult_t[HASH_RESULT_SIZE_IN_WORDS];

/*! Definition for converting pointer to address. */
#define CONVERT_TO_ADDR(ptr)    (unsigned long)ptr


#ifdef __cplusplus
}
#endif

#endif



