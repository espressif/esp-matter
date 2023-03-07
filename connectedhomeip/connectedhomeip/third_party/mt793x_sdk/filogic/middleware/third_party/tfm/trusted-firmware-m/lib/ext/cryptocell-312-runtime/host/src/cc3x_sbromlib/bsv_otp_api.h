/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef _BSV_OTP_API_H
#define _BSV_OTP_API_H

#ifdef __cplusplus
extern "C"
{
#endif


/*! @file
@brief This file contains functions that access the OTP memory for read and write operations.
\note This implementation can be replaced by the partner, depending on memory requirements.
*/

/*!
@brief This function retrieves a 32-bit OTP memory word from a given address.

@return CC_OK on success.
@return A non-zero value from bsv_error.h on failure.
*/
CCError_t CC_BsvOTPWordRead(
    unsigned long hwBaseAddress,    /*!< [in] CryptoCell HW registers' base address. */
    uint32_t otpAddress,        /*!< [in] Word address in the OTP memory to read from. */
    uint32_t *pOtpWord      /*!< [out] The OTP memory word's contents. */
    );


/*!
@brief This function writes a 32-bit OTP memory word to a given address. Prior to writing,
    the function reads the current value in the OTP memory word, and performs bit-wise OR to generate the expected value.
        After writing, the word is read and compared to the expected value.


@return CC_OK on success.
@return A non-zero value from sbrom_bsv_error.h on failure.
*/
CCError_t CC_BsvOTPWordWrite(
    unsigned long hwBaseAddress,    /*!< [in] CryptoCell HW registers' base address. */
    uint32_t otpAddress,        /*!< [in] Word address in the OTP memory to write to. */
    uint32_t otpWord        /*!< [in] The OTP memory word's contents. */
    );


#ifdef __cplusplus
}
#endif

#endif



