/*
 * Copyright (c) 2013-2016, GreenPeak Technologies
 * Copyright (c) 2017-2020, Qorvo Inc
 *
 *  The file gpEncryption_aes_mmo.h contains the API definitions for AES MMO algorithm.
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifndef __GPENCRYPTION_AES_MMO_H__
#define __GPENCRYPTION_AES_MMO_H__

#include "gpEncryption.h"

/* Initialize AES MMO algorithm - called from gpEncryption_Init() */
void gpEncryptionAesMmo_Init(void);

/*
 * @brief Start a new instance of AES MMO hash calculation
 *
 * The function will clear state data (intermediate hash value) before hashing new data.
*/
void gpEncryptionAesMmo_Start(void);

/*
 * @brief Perform AES MMO over input message, updating hash value in an internal buffer.
 *
 *  This API allows to split up the input message into chunks. gpEncryptionAesMmo_Update() can
 *  be called any number of times (or can be skipped altogether) before gpEncryptionAesMmo_Finalize(). Only
 *  restriction is that message length has to be multiple of 16 bytes.
 *
 * @param msg       Input data buffer (either in system RAM or Flash) for which the hash value is computed
 * @param nofBlocks Number of 16-byte blocks stored in msg buffer for which the AES MMO hashing is done.
 * @return
 *          - gpEncryption_ResultSuccess
 *          - gpEncryption_ResultInvalidParameter
*/

gpEncryption_Result_t gpEncryptionAesMmo_Update(UInt8* msg, UInt32 nofBlocks);

/*
 * @brief Finalize AES MMO hash calculation
 *
 * Find AES MMO hash for input message over message length and copy hash value to output hash buffer.
 *
 * @param hash   Pointer to the buffer to which computed hash value is copied
 * @param msg    Input data buffer (either in system RAM or Flash) for which the hash value is computed
 * @param msglen Non-zero length of message (in bytes) stored in msg buffer.
 * @return
 *          - gpEncryption_ResultSuccess
 *          - gpEncryption_ResultInvalidParameter
*/
gpEncryption_Result_t gpEncryptionAesMmo_Finalize(UInt8 *hash, UInt8* msg, UInt32 msglen);

#endif /* __GPENCRYPTION_AES_MMO_H__ */
