/******************************************************************************

 @file  sha2_driverlib.h

 @brief This module ports the TI-Drivers implementation of SHA2 hashing using
        only driverlib APIs. Only SHA256 is supported.

 Group: CMCU
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2012-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

#ifndef sha2_driverlib__include
#define sha2_driverlib__include

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * CONSTANTS
 */
/* Successful status code */
#define SHA2_STATUS_SUCCESS         (0)

/* Generic error status code */
#define SHA2_STATUS_ERROR           (-1)

/* The ongoing operation was canceled */
#define SHA2_STATUS_CANCELED (-3)

/*******************************************************************************
 * FUNCTIONS
 */

/**
 * @fn         SHA2_open
 * @brief      Initializes the SHA2 peripheral.
 *
 *             This function must be called before all other SHA2 functions
 *             defined by sha2_driverlib.h.
 *
 * @return     SHA2_STATUS_SUCCESS on success, SHA2_STATUS_ERROR otherwise
 *             (for instance when the SHA2 peripheral has already been opened).
 */
int_fast16_t SHA2_open(void);

/**
 * @fn         SHA2_close
 * @brief      Closes the SHA2 peripheral.
 *
 *             This function should be called after finishing all SHA2
 *             operations.
 */
void SHA2_close(void);

/**
 * @fn         SHA2_addData
 * @brief      Hashes a segment of data to the SHA256 hash. @ref SHA2_addData()
 *             may be called an arbitrary number of times before finishing
 *             the operation with @ref SHA2_finalize().
 *
 *             This function blocks until the digest for the data segment has
 *             been computed.
 *
 * @param      data    Pointer to the location to read from.
 * @param      length  Length of the data to hash, in bytes.
 *
 * @return     SHA2_STATUS_SUCCESS on success, SHA2_STATUS_ERROR otherwise.
 */
int_fast16_t SHA2_addData(const void* data, size_t length);

/**
 * @fn         SHA2_finalize
 * @brief      Finishes the SHA256 hashing operation and writes the result to digest.
 *             This function finishes a hash operation that has been previously
 *             started by @ref SHA2_addData().
 *
 *             This function blocks until the final digest has has been computed.
 *
 * @param      digest  Pointer to the location write the digest to.
 *
 * @return     SHA2_STATUS_SUCCESS on success, SHA2_STATUS_ERROR otherwise.
 */
int_fast16_t SHA2_finalize(void *digest);

/**
 * @fn         SHA2_cancelOperation
 * @brief       Aborts an ongoing hash operation of this driver instance. The operation
 *              will terminate as though an error occurred and the status code of the
 *              operation will be SHA2_STATUS_CANCELED.
 *
 * @return     SHA2_STATUS_SUCCESS on success, SHA2_STATUS_ERROR otherwise (for instance
 *             when no operation was running).
 */
int_fast16_t SHA2_cancelOperation(void);

#ifdef __cplusplus
}
#endif

#endif /* sha2_driverlib__include */
