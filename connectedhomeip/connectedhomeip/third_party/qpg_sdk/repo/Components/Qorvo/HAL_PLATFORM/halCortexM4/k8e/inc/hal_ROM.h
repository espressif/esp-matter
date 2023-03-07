/*
 * Copyright (c) 2021, Qorvo Inc
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
 * $Header$
 * $Change$
 * $DateTime$
 *
 */


#ifndef _HAL_ROM_H_
#define _HAL_ROM_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/* Key size equal to block size - 128 bits */
#define AES_BLOCK_SIZE_BYTES 16

#define X25519_VERIFY_SUCCESS 1
#define X25519_VERIFY_FAIL 0

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

struct flash_info {
    /* Basic Info */
    UInt16 numSectors;
    UInt16 sectorSize;
    UInt16 pageSize;
    UInt16 unitSize;

    /* Alternative access functions */
    unsigned int (*sector_erase)(UInt32 physAddress);
    void (*chip_erase)  (UInt32 magicWord);
};

typedef struct flash_info* flash_info_t;


typedef struct
{
    UInt32 AesMmoMsgLengthBytes;
    // intermediate location where hash value is stored
    UInt8 AesMmohash[AES_BLOCK_SIZE_BYTES];
    // padded blocks
    UInt8 paddedBlock1[AES_BLOCK_SIZE_BYTES];
    UInt8 paddedBlock2[AES_BLOCK_SIZE_BYTES];
} AesMmoContext_t;



#ifdef __cplusplus
extern "C" {
#endif
/*****************************************************************************
 *                    Convenience functions
 *****************************************************************************/

/*
 * @brief Function which returns the ROM version of the device

   @return MSB == Major, LSB == Minor
 */
UInt16 hal_GetRomVersion(void);

/*****************************************************************************
 *                    ROM function prototypes
 *****************************************************************************/
/*
 * @brief sleep function
 * The 32-bit config argument encodes the following options:
 * bit 0 = CRC Enable (typically value of STANDBY_CRC_ENABLE)
 * bit 1 = MW Enable  (typically value of STANDBY_MW_ENABLE)
 * bit 2 = WB Backup Needed  (typically value of STANDBY_WB_BACKUP_NEEDED)
 * bit 3 = ISH? (typically bit 3 from GP_MM_RAM_MAGIC_WORD_START)
 * bit 4 = Is Standby mode 16 MHz? (GP_WB_READ_PMUD_STBY_MODE() == GP_WB_ENUM_STANDBY_MODE_XTAL_16MHZ_MODE)
 * bit 5 = Is BBPLL Loop closed?
 * bit 6 = Is ARM Clock speed 64 MHz?
 * bit 7 = GP_WB_READ_STANDBY_ALWAYS_USE_FAST_CLOCK_AS_SOURCE()
 * bits 8,9,10,11   = SYSRAM CRC configuration
                  - Number of bytes to CRC = pow(2,(value-1))*4
                  - 0 = No CRC
 * bits 12,13,14,15 = UCRAM CRC configuration
                 - Number of bytes to CRC = pow(2,(value-1))*4
                 - 0 = No CRC
                 - 1 = 96 kB
 * bits 16-31 = don't care
 */
NORETURN void ROM_go_to_sleep(UInt32 config);


/*
 * @brief function to return information and function pointers for flash manpulation
 */
flash_info_t ROM_flash_info(void);

/*
 * @brief function to erase flash sector
 */
unsigned int ROM_flash_sector_erase(UInt32 physAddress);

/*
 * @brief function to write to flash
 */
void ROM_flash_write(UInt32 physAddress, const UInt32* data, UInt8 num_units);


// STROBE-compatible signature verification using curve25519
// (not ed25519). This function is the public equivalent of Sign(),
// taking the long-term and ephemeral public keys instead of secret
// ones. Returns false on failure and true on success
Bool ROM_verify_x25519_signature(const uint8_t response[32],
    const uint8_t challenge[32],
    const uint8_t publicEphemeral[32], const uint8_t publicKey[32]);

// Generic point multiplication
// Calculates scalar * point.
// If include_y_in_result == 0, result_point should be an array of size 32 bytes where the resulting x coordinate will be written.
// If include_y_in_result != 0, result_point should be an array of size 64 bytes where the resulting x coordinate concatenated with y will be written.
// Returns true on success and false on invalid input (see the functions below what defines invalid input).
// If false is returned, the result is not written.
Bool ROM_P256_pointmult(uint8_t* result_point, const uint8_t point[64], const uint8_t scalar[32], Bool include_y_in_result);

// ECDH keygen
// Multiplies the scalar private_key with the curve-defined base point.
// The result should be an array of size 64 bytes where the x coordinate concatenated by the y coordinate will be written.
// Returns true on success and false if the private_key lies outside the allowed range [1..n-1], where n is the curve order.
// If false is returned, the result is not written. (At that point this function can be called again with a new randomized private_key.)
Bool ROM_P256_ecdh_keygen(uint8_t result_my_public_point[64], const uint8_t private_key[32]);


// ECDH shared secret
// Multiplies the scalar private_key with the other's public point.
// The result should be an array of size 32 bytes where the x coordinate of the result will be written (y is discarded).
// Returns true on success and false if any of the following occurs:
//  - the scalar private_key lies outside the allowed range [1..n-1], where n is the curve order
//  - a public point coordinate integer lies outside the allowed range [0..p-1], where p is the prime for the field used by the curve
//  - the public point does not lie on the curve
// If false is returned, the result is not written.
// NOTE: the boolean return value MUST be checked in order to avoid different attacks.
Bool ROM_P256_ecdh_shared_secret(uint8_t result_point_x[32], const uint8_t others_public_point[64], const uint8_t private_key[32]);



/*
 * @brief Start a new instance of AES MMO hash calculation
 *
 * The function will clear state data (intermediate hash value) before hashing new data.
 *
 * @param pCtx      Pointer to context -> make sure this context lies in SYSRAM.
*/

UInt8 ROM_aes_mmo_start(AesMmoContext_t *pCtx);

/*
 * @brief Perform AES MMO over input message, updating hash value in an internal buffer.
 *
 *  This API allows to split up the input message into chunks. gpEncryptionAesMmo_Update() can
 *  be called any number of times (or can be skipped altogether) before gpEncryptionAesMmo_Finalize(). Only
 *  restriction is that message length has to be multiple of 16 bytes.
 *
 * @param pCtx      Pointer to context -> make sure this context lies in SYSRAM.
 * @param msg       Input data buffer (either in system RAM or Flash) for which the hash value is computed
 * @param nofBlocks Number of 16-byte blocks stored in msg buffer for which the AES MMO hashing is done.
 * @return
 *          - 0 = success
 *          - 1 = error
*/

UInt8 ROM_aes_mmo_update(AesMmoContext_t *pCtx, UInt8* msg, UInt32 nofBlocks);

/*
 * @brief Finalize AES MMO hash calculation
 *
 * Find AES MMO hash for input message over message length and copy hash value to output hash buffer.
 * @param pCtx   Pointer to context -> make sure this context lies in SYSRAM.
 * @param msg    Input data buffer (either in system RAM or Flash) for which the hash value is computed
 * @param msglen Non-zero length of message (in bytes) stored in msg buffer.
 * @return
 *          - 0 = success
 *          - 1 = error
 *
 * final result is stored in pCtx->AesMmoHash buffer
*/

UInt8 ROM_aes_mmo_finalize(AesMmoContext_t *pCtx, UInt8* msg, UInt32 msglen);

typedef struct {
    UInt32 libStartAddress;
    UInt32 libSize;
    UInt32 offset_ROM_P256_pointmult;
    UInt32 offset_ROM_P256_ecdh_keygen;
    UInt32 offset_ROM_P256_ecdh_shared_secret;
} P256_cortex_ecdh_libInfo_t;


void ROM_get_P256_cortex_ecdh_lib_info(P256_cortex_ecdh_libInfo_t *info);




#ifdef __cplusplus
}
#endif

#endif // _HAL_ROM_H_

