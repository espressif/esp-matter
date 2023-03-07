/******************************************************************************

 @file  ECCROMCC26XX.h

 @brief This file contains the interface to the ECC in ROM Driver.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2016-2022, Texas Instruments Incorporated
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
 *  @defgroup ECCROMCC26XX ECC ROM driver
 *  @brief ECC ROM driver implementation for a CC26XX device
 *
 *  @{
 *
 *  # Driver Include #
 *  The ECC ROM header file should be included in an application as follows:
 *  @code
 *  #include <ti/drivers/ecc/ECCROMCC26XX.h>
 *  @endcode
 *
 *  # Overview #
 *
 *  The ECCROMCC26XX driver provides reentrant access to the ROM based ECC
 *  module.  Access is given first come, first serve and subsequent calls
 *  block until all preceding callers have returned or the caller times out
 *  in which case execution returns to the caller but the operation is not
 *  performed.
 *
 *  Note: this implementation differs in byte ordering from the NIST Standard.
 *  It is not expected that keys input from a different implementation will
 *  match the output here.
 *
 * ## General Behavior #
 * For code examples, see [Use Cases](@ref ECC_USE_CASES) below.
 *
 *  ### Initialing the driver #
 *  - The application initializes the ECCROMCC26XX driver by
 *    calling ECCROMCC26XX_init().
 *  - This function may be called more than once, but it only needs to be
 *    called once per system reset.
 *
 *  @code{.c}
 *  // Initialize ECC ROM driver
 *  ECCROMCC26XX_init();
 *
 *  // Generate Public Keys
 *  status = ECCROMCC26XX_genKeys(privKey, pubKeyX, pubKeyY, &params);
 *  if (!status) {
 *      System_printf("ECCROMCC26XX did not complete successfully");
 *  }
 *  @endcode
 *
 *  ### Initializing client parameters
 * - Prior to calling either ECCROMCC26XX_genKeys() or ECCROMCC26XX_genDHKey(),
 *   parameters must be initialized with a call to ECCROMCC26XX_Params_init()
 *   and initialize an instance of ECCROMCC26XX_Params.  Generally, it is
 *   expected that each client will initialize its own instance of
 *   ECCROMCC26XX_Params, but in some cases this is not necessary. Multiple
 *   clients may share the same ECCROMCC26XX_Params instance as long as they
 *   do not rely on the status field.  This field could be overwritten by any of
 *   the clients whom share the instance.  The clients should then rely on the
 *   return value of the API function which was called.
 * - ECCROMCC26XX_Params initialize their curves to NIST P-256 defaults, set
 *   their malloc and free functions to NULL and block indefinitely when
 *   waiting for the resource.
 * - Currently only NIST P-256 parameters are included in ROM.  If a client wishes
 *   to use a different curve, that client must provide it. Curve parameters
 *   are uint32_t buffers where the first uint32_t block is a length header
 *   which specified the length of the buffer in uint32_t blocks, not including
 *   the header itself.  the maximum curve length supported in 1020 bytes or
 *   255 uint32_t blocks.  For example, the NIST P-256 length header would be 8.
 * - keyLen should be the same as the length header of Curve parameters but in
 *   in bytes.  For example, the NIST P-256 key length would be 32.
 * - workzone length is determined by the operation, curve length in uint32_t
 *   blocks and windowsize. the default value for NIST P-256 curves is only
 *   valid for those curves performing key and shared secret generation.  The
 *   workzone length for any curve using the supported operation can be
 *   calculated with the ECCROMCC26XX_WORKZONE_LEN macro.
 * - windowSize determines the speed and memory trade-off of each operation.
 *   Valid windowSize values are 2 through 5, and 3 is recommended as the best
 *   trade-off.
 *
 *  @code
 *  // Declaration of this client's ECCROMCC26XX parameters.
 *  ECCROMCC26XX_Params      params;
 *
 *  // Initialize this client's ECCROMCC26XX parameters.
 *  ECCROMCC26XX_Params_init(&params);
 *
 *  // Set the Malloc and Free functions for internal buffer allocation.
 *  params.malloc = (uint8_t *(*)(uint16_t))myMallocFunction;
 *  params.free   = (void (*)(uint8_t *))myFreeFunction;
 *
 *  // If an indefinite timeout is undesirable, alter the timeout here.
 *  // the value specified here will be a multiple of Clock.tickPeriod is
 *  // specified as.
 *  params.timeout = 1000; // wait a maximum of 1000 system time units
 *
 *  // If using non NIST P-256 curves, supply them here. Otherwise, skip this.
 *  params.curve.keyLen      = myCurveLength; // in bytes.
 *  params.curve.workzoneLen = ECCROMCC26XX_WORKZONE_LEN_IN_BYTES(myCurveLength/4, myWindowSize);
 *  params.curve.windowSize  = myWindowSize;
 *  params.curve.param_p     = myCurveParamP;
 *  params.curve.param_r     = myCurveParamR;
 *  params.curve.param_a     = myCurveParamA;
 *  params.curve.param_b     = myCurveParamB;
 *  params.curve.param_gx    = myCurveParamGx;
 *  params.curve.param_gy    = myCurveParamGy;
 *  @endcode
 *
 *  ## Use Cases @anchor ECC_USE_CASES ##
 *  ### Performing a key and shared secret generation operation #
 *  @code
 *  // Declaration of this client's ECCROMCC26XX parameters.
 *  ECCROMCC26XX_Params      params;
 *  // Declaration of local public keys
 *  uint8_t localPrivateKey[32]  = {0};
 *  uint8_t localPublicKeyX[32]  = {0};
 *  uint8_t localPublicKeyY[32]  = {0};
 *  uint8_t sharedSecretKeyX[32] = {0};
 *  uint8_t sharedSecretKeyY[32] = {0};
 *
 *  // Declaration of a remote client's public keys.
 *  // Assume they have already been generated.
 *  extern remotePublicKeyX[32];
 *  extern remotePublicKeyY[32];
 *
 *  // Fill localPrivateKey with 256 bits from a random number generator.
 *  myRandomNumberGenerator(localPrivateKey, 32);
 *
 *  // Initialize ECC ROM driver
 *  ECCROMCC26XX_init();
 *
 *  // Initialize this client's ECCROMCC26XX parameters.
 *  ECCROMCC26XX_Params_init(&params);
 *
 *  // Generate public keys
 *  ECCROMCC26XX_genKeys(localPrivateKey, localPublicKeyX,
 *                       localPublicKeyY, &params);
 *
 *  // Generate shared secret.
 *  // Note: given your local Public Keys, the remote device can generate the
 *  // same shared secret using its local private key.
 *  ECCROMCC26XX_genDHKey(localPrivateKey, remotePublicKeyX,
 *                        remotePublicKeyY, sharedSecretKeyX,
 *                        sharedSecretKeyY, &params);
 *
 *  @endcode
 *
 *  ### Supported transaction modes #
 *  - All key generation functions are blocking.
 *
 *  ## Error handling ##
 *  If an error occur during key generation, an error status will be returned and stored
 *  in the status field of the ECCROMCC26XX_Params instance passed as an argument.
 *
 *  ## Supported Functions ##
 *  | API function                   | Description                                       |
 *  |------------------------------- |---------------------------------------------------|
 *  | ECCROMCC26XX_init()            | Initializes module's synchronization resources    |
 *  | ECCROMCC26XX_Params_init()     | Initialize parameters for Key Generation          |
 *  | ECCROMCC26XX_genKeys()         | Generate Public Key X and Y Coordinates           |
 *  | ECCROMCC26XX_genDHKey()        | Generate Diffie-Hellman Shared Secret             |
 *
 *  ## Unsupported functions:
 *  Functionality that currently not supported:
 *  - ECDSA Sign and Verify.
 *  ============================================================================
 *
 *  @file  ECCROMCC26XX.h
 *
 *  @brief      ECC ROM driver implementation for a CC26XX device
 */

#ifndef ti_drivers_ECCROMCC26XX__include
#define ti_drivers_ECCROMCC26XX__include

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 *  @addtogroup ECC_STATUS
 *  ECCROMCC26XX_STATUS_* macros are command codes only defined in the
 *  ECCROMCC26XX.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/ecc/ECCROMCC26XX.h>
 *  @endcode
 *  @{
 */

/* Add ECCROMCC26XX_STATUS_* macros here */
#define ECCROMCC26XX_STATUS_SUCCESS                              0  /*!< Success Return Code      */
#define ECCROMCC26XX_STATUS_MALLOC_FAIL                         -1  /*!< Malloc Error Return Code */
#define ECCROMCC26XX_STATUS_ILLEGAL_PARAM                       -2  /*!< Illegal parameter        */
#define ECCROMCC26XX_STATUS_TIMEOUT                             -3  /*!< Semaphore Timeout        */

/* ECC status error codes from module */
#define ECCROMCC26XX_STATUS_MODULUS_EVEN                        0xDC  //!< Modulus Event
#define ECCROMCC26XX_STATUS_MODULUS_LARGER_THAN_255_WORDS       0xD2  //!< Modulus Larger than 255 words
#define ECCROMCC26XX_STATUS_MODULUS_LENGTH_ZERO                 0x08  //!< Modulus length zero
#define ECCROMCC26XX_STATUS_MODULUS_MSW_IS_ZERO	                0x30  //!< Modulus MSW is zero
#define ECCROMCC26XX_STATUS_SCALAR_TOO_LONG                     0x35  //!< Scalar too long
#define ECCROMCC26XX_STATUS_SCALAR_LENGTH_ZERO	                0x53  //!< Scalar length zero
#define ECCROMCC26XX_STATUS_ORDER_TOO_LONG                      0xC6  //!< Order too long
#define ECCROMCC26XX_STATUS_ORDER_LENGTH_ZERO	                0x6C  //!< Order length zero
#define ECCROMCC26XX_STATUS_X_COORD_TOO_LONG	                0x3C  //!< X Coordinate too long
#define ECCROMCC26XX_STATUS_X_COORD_LENGTH_ZERO	                0xC3  //!< X Coordinate length zero
#define ECCROMCC26XX_STATUS_Y_COORD_TOO_LONG	                0x65  //!< Y Coordinate too long
#define ECCROMCC26XX_STATUS_Y_COORD_LENGTH_ZERO	                0x56  //!< Y coordinate length zero
#define ECCROMCC26XX_STATUS_A_COEF_TOO_LONG                     0x5C  //!< A coefficient too long
#define ECCROMCC26XX_STATUS_A_COEF_LENGTH_ZERO	                0xC5  //!< A coefficient length zero
#define ECCROMCC26XX_STATUS_BAD_WINDOW_SIZE                     0x66  //!< Bad window size
#define ECCROMCC26XX_STATUS_SCALAR_MUL_OK                       0x99  //!< Scalar mul ok

#define ECCROMCC26XX_STATUS_ORDER_LARGER_THAN_255_WORDS	        0x28  //!< Order larger than 255 words
#define ECCROMCC26XX_STATUS_ORDER_EVEN                          0x82  //!< Order even
#define ECCROMCC26XX_STATUS_ORDER_MSW_IS_ZERO                   0x23  //!< Order MSW is zero
#define ECCROMCC26XX_STATUS_ECCROMCC26XX_STATUS_KEY_TOO_LONG    0x25  //!< Status key too long
#define ECCROMCC26XX_STATUS_ECCROMCC26XX_STATUS_KEY_LENGTH_ZERO 0x52  //!< Status key length zero
#define ECCROMCC26XX_STATUS_DIGEST_TOO_LONG                     0x27  //!< Digest too long
#define ECCROMCC26XX_STATUS_DIGEST_LENGTH_ZERO                  0x72  //!< Digest length zero
#define ECCROMCC26XX_STATUS_ECDSA_SIGN_OK                       0x32  //!< ECDSA sign OK
#define ECCROMCC26XX_STATUS_ECDSA_INVALID_SIGNATURE             0x5A  //!< ECDSA invalid signature
#define ECCROMCC26XX_STATUS_ECDSA_VALID_SIGNATURE               0xA5  //!< ECDSA valid signature
#define ECCROMCC26XX_STATUS_SIG_P1_TOO_LONG                     0x11  //!< SIG P1 too long
#define ECCROMCC26XX_STATUS_SIG_P1_LENGTH_ZERO                  0x12  //!< SIG P1 length zero
#define ECCROMCC26XX_STATUS_SIG_P2_TOO_LONG                     0x22  //!< SIG P2 too long
#define ECCROMCC26XX_STATUS_SIG_P2_LENGTH_ZERO                  0x21  //!< SIG P2 length zero

#define ECCROMCC26XX_STATUS_ECDH_KEYGEN_OK                      ECCROMCC26XX_STATUS_SCALAR_MUL_OK //!< status scalar mul OK
#define ECCROMCC26XX_STATUS_ECDH_COMMON_KEY_OK                  ECCROMCC26XX_STATUS_SCALAR_MUL_OK //!< scalar mul OK
/** @}*/

/*!
 * ECC Workzone length in bytes for NIST P-256 key and shared secret generation.
 * For use with ECC Window Size 3 only.  Used to store intermediary values in
 * ECC calculations.
 */
#define ECCROMCC26XX_NIST_P256_WORKZONE_LEN_IN_BYTES           684

/*!
 * ECC Workzone length in bytes for NIST P-256 key and shared secret generation.
 * For use with ECC Window Size 3 only.  Used to store intermediary values in
 * ECC calculations. This value needs to be adjusted while the sign and verify functions
 */
#define ECCROMCC26XX_NIST_P256_WORKZONE_SIGN_VERIFY_LEN_IN_BYTES    1100

/*!
 * ECC key length in bytes for NIST P-256 keys.
 */
#define ECCROMCC26XX_NIST_P256_KEY_LEN_IN_BYTES                32

/*!
 * @brief Compute ECC workzone length in bytes for a generic key length and window size
 *
 * len is the length of the key in uint32_t blocks. it must not exceed 255 blocks in length.
 * win is the window size, such that 1 < win < 6.  Recommended setting is 3.
 *
 * This workzone length is only valid for key and shared secret and signature generation.
 * A different workzone length would be used for verification.
 */
#define ECCROMCC26XX_WORKZONE_LEN_IN_BYTES(len, win)           (4 * ((13 * (len)) + 13 + (3 * (len) * (1 << ((win) - 2)))))

/*!
 * @brief      ECCROMCC26XX Curve Parameters
 *
 * This holds the ECC Curve information to be used. Offset 0 of each param_* buffer contains
 * the length of each parameter in uint32_t blocks, not including the first offset.
 */
typedef struct ECCROMCC26XX_CurveParams
{
    uint8_t         keyLen;         /*!<  Length in bytes of curve parameters and keys */
    uint16_t        workzoneLen;    /*!<  Length in bytes of work-zone to malloc        */
    uint8_t         windowSize;     /*!<  Window size of operation                     */
    uint32_t        *param_p;       /*!<  ECC Curve Parameter P                        */
    uint32_t        *param_r;       /*!<  ECC Curve Parameter R                        */
    uint32_t        *param_a;       /*!<  ECC Curve Parameter A                        */
    uint32_t        *param_b;       /*!<  ECC Curve Parameter B                        */
    uint32_t        *param_gx;      /*!<  ECC Curve Parameter Gx                       */
    uint32_t        *param_gy;      /*!<  ECC Curve Parameter Gy                       */
} ECCROMCC26XX_CurveParams;

/*!
 * @brief      ECCROMCC26XX Malloc Callback
 *
 *             Required to malloc temporary key buffers during operation.
 *
 * @param      len - length in bytes of buffer to malloc.
 *
 * @return     pointer to allocated buffer
 */
typedef uint8_t *(*ECCROMCC26XX_MallocCB)(uint16_t len);

 /*!
 * @brief      ECCROMCC26XX Free Callback
 *
 *             Required to free temporary key buffers during operation.
 *
 * @param      pBuf - pointer to buffer to free.
 *
 * @return     none
 */
typedef void (*ECCROMCC26XX_FreeCB)(uint8_t *pBuf);

/*!
 *  @brief    ECCROMCC26XX Parameters
 *
 *  Holds a client's parameters for performing an ECC operation.
 */
typedef struct ECCROMCC26XX_Params {
    ECCROMCC26XX_CurveParams curve;   /*!< ECC Curve Parameters   */
    ECCROMCC26XX_MallocCB    malloc;  /*!< Malloc Callback        */
    ECCROMCC26XX_FreeCB      free;    /*!< Free Callback          */
    uint32_t                 timeout; /*!< Semaphore wait timeout */
    int8_t                   status;  /*!< stored return status   */
} ECCROMCC26XX_Params;

/*!
 *  @brief  Initializes module's synchronization resources. Only needs to be
 *          called once, but safe to call multiple times.
 *
 *  @pre    This function must be called before any other ECCROMCC26XX driver APIs.
 *          Calling context: Task.
 *
 */
void ECCROMCC26XX_init(void);

/*!
 *  @brief  Function to initialize the ECCROMCC26XX_Params struct to its defaults.
 *          params should not be modified until after this function is called.
 *          Default parameters use the NIST P-256 curve, timeout is set to wait
 *          indefinitely, malloc and free are NULL and status is set to
 *          ECCROMCC26XX_STATUS_SUCCESS.  A client may call this function with
 *          the same params instance any number of times.
 *
 *  @pre    Calling context: Hwi, Swi and Task.
 *
 *
 *  @param  params  Parameter structure to initialize.
 */
void ECCROMCC26XX_Params_init(ECCROMCC26XX_Params *params);

/*!
 *  @brief  Generate Public Key X and Y Coordinates.
 *
 *  @pre    ECCROMCC26XX_init must be called prior to this and  params must be
 *          initialized with ECCROMCC26XX_Params_init().
 *          Calling context: Task. Swi or Hwi should call only when
 *          timeout is set to 0.
 *
 *  @param  privateKey      32 byte input buffer of randomly generated bits.
 *
 *  @param  publicKeyX      32 byte output buffer provided by client to store
 *                          Public Key X Coordinate.
 *
 *  @param  publicKeyY      32 byte output buffer provided by client to store
 *                          Public Key Y Coordinate.
 *
 *  @param  params          Pointer to a parameter block, if NULL operation will
 *                          fail.
 *
 *  @return status
 */
int8_t ECCROMCC26XX_genKeys(uint8_t *privateKey, uint8_t *publicKeyX,
                            uint8_t *publicKeyY, ECCROMCC26XX_Params *params);

/*!
 *  @brief  Generate Diffie-Hellman Shared Secret Key X and Y Coordinates.
 *
 *  @pre    ECCROMCC26XX_init must be called prior to this and params must be
 *          initialized with ECCROMCC26XX_Params_init().
 *          Calling context: Task. Swi or Hwi should call only when
 *          timeout is set to 0.
 *
 *  @param  privateKey      32 byte input buffer of randomly generated bits.
 *
 *  @param  publicKeyX      32 byte input buffer provided by client to store
 *                          Public Key X Coordinate.
 *
 *  @param  publicKeyY      32 byte input buffer provided by client to store
 *                          Public Key Y Coordinate.
 *
 *  @param  dHKeyX          32 byte output buffer provided by client to store
 *                          Diffie-Hellman Key X Coordinate.
 *
 *  @param  dHKeyY          32 byte output buffer provided by client to store
 *                          Diffie-Hellman Key Y Coordinate.
 *
 *  @param  params          Pointer to a parameter block, if NULL operation will
 *                          fail
 *
 *  @return status
 */
int8_t ECCROMCC26XX_genDHKey(uint8_t *privateKey, uint8_t *publicKeyX,
                             uint8_t *publicKeyY, uint8_t *dHKeyX,
                             uint8_t *dHKeyY, ECCROMCC26XX_Params *params);

/** @} End ECCROMCC26XX */

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_ECCROMCC26XX__include */
