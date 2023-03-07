/***************************************************************************//**
 * @file
 * @brief Interface definition for AES functionality.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef SILABS_AES_H
#define SILABS_AES_H

// This function loads the 16 byte key into the AES hardware accelerator.
void emLoadKeyIntoCore(const uint8_t* key);

// This function retrieves the 16 byte key from the AES hardware accelerator.
void emGetKeyFromCore(uint8_t* key);

// This function encrypts the 16 byte plaintext block with the previously-loaded
// 16 byte key using the AES hardware accelerator.
// The resulting 16 byte ciphertext is written to the block parameter,
// overwriting the plaintext.
void emStandAloneEncryptBlock(uint8_t* block);

// emAesEncrypt performs AES encryption in ECB mode on the plaintext pointed to
// by the block parameter, using the key pointed to by the key parameter, and
// places the resulting ciphertext into the 16 bytes of memory pointed to by the
// block parameter (overwriting the supplied plaintext).  Any existing key is
// destroyed.
void emAesEncrypt(uint8_t* block, const uint8_t* key);

// emAesDecrypt performs AES decryption in ECB mode on the ciphertext pointed to
// by the block parameter, using the key pointed to by the key parameter, and
// places the resulting plaintext into the 16 bytes of memory pointed to by the
// block parameter (overwriting the supplied cyphertext).  Any existing key is
// destroyed. This is only supported on EFR.
#if CORTEXM3_EFR32
void emAesDecrypt(uint8_t* block, const uint8_t* key);
#endif

#endif //__AES_H__
