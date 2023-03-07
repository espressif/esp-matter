/******************************************************************************

 @file  SHA2CC26XX.h

 @brief This file contains the interface to the SHA2 driver.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2013-2022, Texas Instruments Incorporated
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

/** ============================================================================
 *  @file  SHA2CC26XX.h
 *
 *  @brief      SHA2 driver implementation for a CC26XX device
 *
 *  # Driver Include #
 *  The SHA2 header file should be included
 *  in an application as follows:
 *  @code
 *  #include "SHA2CC26XX.h"
 *  Add SHA2CC26XX.a to the Linker library path
 *  @endcode
 *
 *  # Overview #
 *
 * The SHA2CC26XX driver provides a stand-alone library to compute a condensed
 * representation of a message or a data file, called digest, using the SHA-256
 * algorithm, which can then be used to verify the message integrity
 *
 *  # Dependencies #
 *  This driver does not have any dependencies.
 *
 * ## General Behavior #
 * For code examples, see [Use Cases] below.
 * a) If input message is available iteratively
 *  ### Initialzing the driver #
 *  - The application initializes the SHA2CC26XX driver by
 *    calling SHA2CC26XX_initialize().
 *  - This function may be called more than once, but it only needs to be
 *    called once per system reset.
 *  ### Accumulating input iteratively #
 *  for( int i =0; i < NumRecd;i++)
 *  SHA2CC26XX_execute(&workzone,Msg,sizeof(Msg)-1)
 *  where NumRecd is the number of iterations required to receive the
 *  entire input data buffer
 *  ### Generating the hash digest
 *  SHA2CC26XX_output(&workzone,hash);
 *  where hash is the SHA256 hash digest output.
 * b) If the entire input message is available initially
 *  ### Generate hash digest
 *  SHA2CC26XX_runFullAlgorithm(&workzone,Msg, sizeof(Msg)-1,hash)
 *
*  ## Example usage in ECC sign and verify ##
 * // Given the local private key and hash, the remote device can generate 64
 *   byte signature split into two 32 byte buffers
 *  uint8_t hash[32]  = {0};
 *  uint8_t sign1[32]  = {0};
 *  uint8_t sign1[32]  = {0};
 *
 *  SHA2CC26XX_runFullAlgorithm(&workzone,Msg, sizeof(Msg)-1,hash)
 *
 *  ECCROMCC26XX_signHash(localPrivateKey,hash,32,sign1,sign2,&params)
 *
 *  // Verify signature for hashed message
 *  // Given the local public keys, hash, and generated signature the remote
 *  // device can verify the signature generated using the local private keys
 *
 *   ECCROMCC26XX_verifyHash(localPublicKeyX,localPublicKeyY,sign1,sign2,hash,
 *                           32,&params);
 *  If the input to be hashed is a file, it is recommended to use the iterative
 *  approach to compute the SHA2 hash.
 *  @endcode
 *
 *
 *  ## Supported Functions ##
 *  | API function                   | Description                                       |
 *  |------------------------------- |---------------------------------------------------|
 *  | SHA2CC26XX_initialize()        | Initializes module's resources                    |
 *  | SHA2CC26XX_execute()           | Generates input buffer for final SHA2 operation   |
    |                                | when input is available iteratively               |
 *  | SHA2CC26XX_runFullAlgorithm()  | Perform SHA2 on the the input data, to be used    |
    |                                | when entire input is available                    |
 *  | SHA2CC26XX_output()            | Perform SHA2 operation on  buffer generated by    |
    |                                | SHA2CC26XX_execute over iterative inputs          |
 *  ============================================================================
 */

#ifndef ti_drivers_SHA2CC26XX__include
#define ti_drivers_SHA2CC26XX__include

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef NULL
#define NULL 0L
#endif

#define SHA2CC26XX_STATUS_ILLEGAL_PARAM                       -1  /*!< Illegal parameter        */
/* A SHA256_memory_t variable of this type must be allocated before running any
 SHA256 functions. */
  typedef struct
  {
    uint32_t state[8];
    uint32_t textLen[2];
    uint32_t W[16];
  } SHA256_memory_t;

//*****************************************************************************
/*!
 * \brief Intializes the SHA256 engine.
 *
 * This function must be called once before all other SHA256 functions other than
 * \ref SHA256_runFullAlgorithm().
 *
 * \param pWorkZone Pointer to memory for operations, input.
 *
 * \return Status
 */
//*****************************************************************************
extern uint8_t SHA2CC26XX_initialize(SHA256_memory_t *pWorkZone);

//*****************************************************************************
/*!
* \brief Perform SHA256.
 *
 * Must call \ref SHA256_output() to receive output from this operation.
 *
 * \param pMemory Pointer to memory for operations, input.
 * \param pBufIn Pointer to input text, input.
 * \param bufLen Length of input, input.
 *
 * \return Status
 */
//*****************************************************************************
extern uint8_t SHA2CC26XX_execute(SHA256_memory_t *pMemory, uint8_t *pBufIn,
uint32_t bufLen);

//*****************************************************************************
/*!
 * \brief Complete the process by passing the modified data back.
 *
 * \param pMemory  Pointer to memory for operations, input.
 * \param pBufOut Pointer to output buffer, output. Buffer must be at least 32 bytes long.
 *
 * \return Status
 */
//*****************************************************************************
extern uint8_t SHA2CC26XX_output(SHA256_memory_t *pMemory, uint8_t *pBufOut);

//*****************************************************************************
/*!
* \brief Perform SHA256 on the the input data.
 *
 * The input and output buffer can point to the same memory.
 * This is the equivalent of calling \ref SHA256_initialize(),
 * \ref SHA256_execute() and \ref SHA256_output() sequentially.
 *
 * \param pMemory  Pointer to memory for operations, input.
 * \param pBufIn  Pointer to input buffer, input.
 * \param bufLen  Length of input.
 * \param pBufOut Pointer to output buffer, output.
 *
 * \return Status
 */
//*****************************************************************************
extern uint8_t SHA2CC26XX_runFullAlgorithm(SHA256_memory_t *pMemory, uint8_t *pBufIn,
uint32_t bufLen, uint8_t *pBufOut);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_SHA2CC26XX__include */
