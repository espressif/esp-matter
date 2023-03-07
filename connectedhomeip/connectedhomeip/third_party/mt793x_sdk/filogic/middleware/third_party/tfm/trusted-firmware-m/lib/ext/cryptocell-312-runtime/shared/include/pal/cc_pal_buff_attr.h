/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



#ifndef _CC_PAL_BUFF_ATTR_H
#define _CC_PAL_BUFF_ATTR_H

/*!
@file
@brief This file contains the definitions and APIs to get inout data buffer attributes.
       This is a place holder for platform specific inout data attributes functions implementation
       The module should be updated whether data buffer is secure or non-secure,
       in order to notify the low level driver how to configure the HW accordigly.
@defgroup cc_pal_buff_attr CryptoCell PAL Data Buffer Attributes APIs
@{
@ingroup cc_pal

*/

#ifdef __cplusplus
extern "C"
{
#endif

#include "cc_pal_types.h"

/******************************************************************************
*               Buffer Information
******************************************************************************/

/*! User buffer attribute (secure / non-secure). */
#define DATA_BUFFER_IS_NONSECURE    1
#define DATA_BUFFER_IS_SECURE       0
/*! Buffer attribute (secure / non-secure) as used by arm_cmse.h . */
#define CMSE_NONSECURE              0
#define CMSE_SECURE                 1

/******************************************************************************
*               Functions
******************************************************************************/

/**
 * @brief This function purpose is to verify the buffer's attributes according to address, size, and type (in/out).
 * The function returns whether the buffer is secure or non-secure.
 * In any case of invalid memory, the function shall return an error (i.e.mixed regions of secured and non-secure memory).
 *
 * @return Zero on success.
 * @return A non-zero value in case of failure.
 */
CCError_t CC_PalDataBufferAttrGet(const unsigned char *pDataBuffer, /*!< [in] Address of data buffer. */
                                  size_t   buffSize,                /*!< [in] Buffer size in bytes. */
                                  uint8_t  buffType,                /* ! [in] Input for read / output for write */
                                  uint8_t  *pBuffNs                 /*!< [out] HNONSEC buffer attribute (0 for secure, 1 for non-secure) */
                                  );

#ifdef __cplusplus
}
#endif
/**
@}
 */
#endif


