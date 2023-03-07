/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * Utility functions for secure boot
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

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_SECUREBOOT

#include "gpSecureBoot.h"

#if defined(GP_DIVERSITY_LOG)
#include "gpLog.h"
#include "gpHal.h"
#endif

#if !defined(GP_SECUREBOOT_DIVERSITY_USE_AESMMO_X25519_ROM)
#include "sx_generic.h"
#include "sx_ecc_curves.h"
#include "sx_ecdsa_alg.h"
#include "sx_ecc_keygen_alg.h"
#include "sx_rng.h"
#include "cryptolib_def.h"
#include "mbedtls/sha256.h"
#endif

#include "hal_user_license.h"

#if defined(GP_COMP_EXTSTORAGE)
#include "gpExtStorage.h"
#endif

#include "gpHal.h"
#include "gpHal_kx_MSI_basic.h"
#include "gpHal_kx_regprop.h"


#if !defined(GP_SECUREBOOT_DIVERSITY_USE_AESMMO_X25519_ROM) && !defined(MBEDTLS_SHA256_ALT)
#error I want alt version
#endif

#if !defined(GP_DIVERSITY_BOOTLOADER_BUILD) || defined(GP_DIVERSITY_BOOTLOADER)
#error these functions are only to be used in a bootloader build
#endif

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#if !defined(GP_SECUREBOOT_DIVERSITY_USE_AESMMO_X25519_ROM)
/* Modify this definition for curve change */
#define CURVE          SX_ECP_DP_SECP256R1
#endif

#if defined(GP_COMP_EXTSTORAGE)
#define IMG_MAX_BLOCK_LEN   256
#endif

#define RMA_TOKEN_PRESENT_MW (0x1B4FA925LU)

#define MAX_FLASH_SIZE (GP_KX_FLASH_SIZE*1024)
#define FLASH_ADDRESS_MASK (0xFFFFF)
/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

#include "gpSecureBoot_PublicKey.c"

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
#if !defined(GP_SECUREBOOT_DIVERSITY_USE_AESMMO_X25519_ROM)
Bool gpSecureBoot_CheckRMAMode(UInt32 rmaTokenAddress, UInt32 rmaAction)
{

    UInt64 macAddress = GP_WB_READ_NVR_MAC_ADDRESSS();
#if defined(GP_DIVERSITY_LOG)
    GP_LOG_PRINTF("Checking RMA token",0);
#endif

    if (*(UInt32 *)rmaTokenAddress == RMA_TOKEN_PRESENT_MW)
    {
        uint32_t status;
        UInt8 sha256sum[ECC_MAX_KEY_SIZE];
        mbedtls_sha256_context ctx;

        mbedtls_sha256_init( &ctx );

        if( mbedtls_sha256_starts_ret( &ctx, 0 ) != 0 )
        {
            return false;
        }


        /* Hash RMA token */
        if(mbedtls_sha256_update_ret( &ctx, (void *)(&rmaAction),sizeof(UInt32)) != 0 )
        {
            return false;
        }

        /* GP_LOG_PRINTF("MAC = %2x %2x %2x %2x %2x %2x %2x %2x",0, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], mac[6], mac[7]); */
        /* Hash MAC address */
        if(mbedtls_sha256_update_ret( &ctx, (void *)(&macAddress),sizeof(UInt64)) != 0 )
        {
            return false;
        }

        /* Finish up */
        if( mbedtls_sha256_finish_ret( &ctx, sha256sum ) != 0 )
        {
            return false;
        }


        // Now that we got the hash, convert it into a silex block and verify the signature on this hash
        sx_enable_clock();

        /* Convert sha256 sum from mbedtls into block_t format to be digested by silex API */
        block_t digest_blk = block_t_convert(sha256sum, sizeof(sha256sum));

        /* Verify that signature matches the hash and public key */
        status = ecdsa_signature_verification_digest(sx_find_ecp_curve(CURVE),
                digest_blk,
                block_t_convert((void *)Upgrade_SecureBoot_PublicKey, (2 * sx_ecc_curve_bytesize(sx_find_ecp_curve(CURVE)))),
                block_t_convert((void *)(rmaTokenAddress + 4), (2 * sx_ecc_curve_bytesize(sx_find_ecp_curve(CURVE)))));

        sx_disable_clock();
#if defined(GP_DIVERSITY_LOG)
        GP_LOG_PRINTF("RMA auth = %d",0, status);
#endif
        return (status == CRYPTOLIB_SUCCESS);
    }

    return false;



}


Bool gpSecureBoot_AuthenticateImage(UInt32 startAddressImage, UInt32 licenseOffset)
{
    UInt32 startAddressLicense = startAddressImage + licenseOffset;

    /* Fetch all the extended user license information */
    UInt32 section1Offset = *(UInt32 *)(startAddressLicense+EXTENDED_USER_LICENSE_SECTION_1_START_ADDRESS_OFFSET_OFFSET);
    UInt32 section1Size = *(UInt32 *)(startAddressLicense+EXTENDED_USER_LICENSE_SECTION_1_SIZE_OFFSET);
    UInt32 section2Offset = *(UInt32 *)(startAddressLicense+EXTENDED_USER_LICENSE_SECTION_2_START_ADDRESS_OFFSET_OFFSET);
    UInt32 section2Size = *(UInt32 *)(startAddressLicense+EXTENDED_USER_LICENSE_SECTION_2_SIZE_OFFSET);
    UInt32 extendedUserLicenseMagicWord = *(UInt32 *)(startAddressLicense+EXTENDED_USER_LICENSE_EXTENDED_LOADED_USER_LICENSE_MAGIC_WORD_OFFSET);

    // check Qorvo Extended Loaded UL MW
    if(extendedUserLicenseMagicWord != EXTENDED_USER_LICENSE_LOADED_MAGIC_WORD)
    {
        /* Magic Word in EUL was not found */
        return false;
    }

    uint32_t status;
    UInt8 sha256sum[ECC_MAX_KEY_SIZE];
    mbedtls_sha256_context ctx;

    mbedtls_sha256_init( &ctx );

    if( mbedtls_sha256_starts_ret( &ctx, 0 ) != 0 )
    {
        return false;
    }

    /* See if first section needs to be hashed */
    if(section1Offset != EXTENDED_USER_LICENSE_SECTION_NOT_IN_USE)
    {
#if defined(GP_DIVERSITY_LOG)
        GP_LOG_SYSTEM_PRINTF("Hashing section 1 : [0x%lx-0x%lx]",0,startAddressImage+(section1Offset),startAddressImage+(section1Offset)+section1Size);
        HAL_WAIT_MS(100);
        GP_LOG_SYSTEM_PRINTF("Check that 0x%lx < 0x%lx",0,((startAddressImage & FLASH_ADDRESS_MASK)+section1Offset+section1Size), (UInt32) MAX_FLASH_SIZE);
        HAL_WAIT_MS(100);
#endif
        /* startAddressImage is masked, such that both ALT and NORMAL flash addresses can be used */
        /* Check that section over which to hash is not bigger than the maximum flash size */
        if(((startAddressImage & FLASH_ADDRESS_MASK)+section1Offset+section1Size) > MAX_FLASH_SIZE)
        {
            return false;
        }

        if(mbedtls_sha256_update_ret( &ctx, (void *)(startAddressImage+(section1Offset)),section1Size) != 0 )
        {
            return false;
        }
    }

    if(section2Offset != EXTENDED_USER_LICENSE_SECTION_NOT_IN_USE)
    {
#if defined(GP_DIVERSITY_LOG)
        GP_LOG_SYSTEM_PRINTF("Hashing section 2 : [0x%lx-0x%lx]",0,startAddressImage+(section2Offset),startAddressImage+(section2Offset)+section2Size);
        HAL_WAIT_MS(100);
        GP_LOG_SYSTEM_PRINTF("Check that 0x%lx < 0x%lx",0,((startAddressImage & FLASH_ADDRESS_MASK)+section2Offset+section2Size), (UInt32) MAX_FLASH_SIZE);
        HAL_WAIT_MS(100);
#endif
        /* startAddressImage is masked, such that both ALT and NORMAL flash addresses can be used */
        /* Check that section over which to hash is not bigger than the maximum flash size */
        if(((startAddressImage & FLASH_ADDRESS_MASK)+section2Offset+section2Size) > MAX_FLASH_SIZE)
        {
            return false;
        }

        if(mbedtls_sha256_update_ret( &ctx, (void *)(startAddressImage+(section2Offset)),section2Size) != 0 )
        {
            return false;
        }
    }


    /* At last, hash loaded and extended user license, masking out:
     *  * Vendor BLE Address
     *  * Vendor Greenpower Address
     *  * CRC Specification
     *  * Load Completed MW
     *  * Freshness counter
     *  * Signature
     */

#if defined(GP_DIVERSITY_LOG)
    GP_LOG_SYSTEM_PRINTF("Hash LUL Area [0x%lx,0x%lx]",0, startAddressLicense+USER_LICENSE_VPP_OFFSET,startAddressLicense+USER_LICENSE_VPP_OFFSET+USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD_OFFSET-USER_LICENSE_VPP_OFFSET);
    HAL_WAIT_MS(100);
#endif
    if(mbedtls_sha256_update_ret( &ctx, (void *)(startAddressLicense+USER_LICENSE_VPP_OFFSET),USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD_OFFSET-USER_LICENSE_VPP_OFFSET) != 0 )
    {
        return false;
    }

#if defined(GP_DIVERSITY_LOG)
    GP_LOG_SYSTEM_PRINTF("Hash EUL Area [0x%lx,0x%lx]",0, startAddressLicense+EXTENDED_USER_LICENSE_EXTENDED_LOADED_USER_LICENSE_MAGIC_WORD_OFFSET, startAddressLicense+EXTENDED_USER_LICENSE_EXTENDED_LOADED_USER_LICENSE_MAGIC_WORD_OFFSET+EXTENDED_USER_LICENSE_SIGNATURE_OFFSET-EXTENDED_USER_LICENSE_EXTENDED_LOADED_USER_LICENSE_MAGIC_WORD_OFFSET);
    HAL_WAIT_MS(100);
#endif
    /* hash extended user license upto signature offset */
    if(mbedtls_sha256_update_ret( &ctx, (void *)(startAddressLicense+EXTENDED_USER_LICENSE_EXTENDED_LOADED_USER_LICENSE_MAGIC_WORD_OFFSET),EXTENDED_USER_LICENSE_SIGNATURE_OFFSET-EXTENDED_USER_LICENSE_EXTENDED_LOADED_USER_LICENSE_MAGIC_WORD_OFFSET) != 0 )
    {
        return false;
    }

    /* Finish up */
    if( mbedtls_sha256_finish_ret( &ctx, sha256sum ) != 0 )
    {
        return false;
    }

    // Now that we got the hash, convert it into a silex block and verify the signature on this hash
    sx_enable_clock();

    /* Convert sha256 sum from mbedtls into block_t format to be digested by silex API */
    block_t digest_blk = block_t_convert(sha256sum, sizeof(sha256sum));

    /* Verify that signature matches the hash and public key */
    status = ecdsa_signature_verification_digest(sx_find_ecp_curve(CURVE),
            digest_blk,
            block_t_convert((void *)Upgrade_SecureBoot_PublicKey, (2 * sx_ecc_curve_bytesize(sx_find_ecp_curve(CURVE)))),
            block_t_convert((void *)(startAddressLicense+EXTENDED_USER_LICENSE_SIGNATURE_OFFSET), (2 * sx_ecc_curve_bytesize(sx_find_ecp_curve(CURVE)))));


    sx_disable_clock();

    return (status == CRYPTOLIB_SUCCESS);

}
#else // GP_SECUREBOOT_DIVERSITY_USE_AESMMO_X25519_ROM

#include "hal_ROM.h"

/* Context buffer for the AES-MMO hashing operation */
AesMmoContext_t aes_mmo_context __attribute__((section(".lower_ram_retain")));

/* RAM Buffer forced to SYSRAM as AES-MMO hash operation can only operate on FLASH and SYSRAM data */
UInt8 aes_mmo_value_to_hash_ram[AES_BLOCK_SIZE_BYTES]  __attribute__((section(".lower_ram_retain")));

Bool gpSecureBoot_CheckRMAMode(UInt32 rmaTokenAddress, UInt32 rmaAction)
{
    UInt64 macAddress = GP_WB_READ_NVR_MAC_ADDRESSS();
#if defined(GP_DIVERSITY_LOG)
    GP_LOG_PRINTF("Checking RMA token",0);
#endif

    if (*(UInt32 *)rmaTokenAddress == RMA_TOKEN_PRESENT_MW)
    {
        // copy 4 bytes RMA action
        memcpy(aes_mmo_value_to_hash_ram, &rmaAction, sizeof(UInt32));

        // copy 8 bytes MAC Address
        memcpy(&aes_mmo_value_to_hash_ram[4],&macAddress, sizeof(UInt64));

        if(ROM_aes_mmo_start(&aes_mmo_context) != 0)
        {
            return false;
        }

        if(ROM_aes_mmo_finalize(&aes_mmo_context, aes_mmo_value_to_hash_ram, 12) != 0)
        {
            return false;
        }


#if defined(GP_DIVERSITY_LOG)
        GP_LOG_PRINTF("RMA auth = %d",0, status);
#endif
        // challenge is concatenation of 2 AES-MMO hashes
        unsigned char challenge[32];

        memcpy(challenge,aes_mmo_context.AesMmohash,AES_BLOCK_SIZE_BYTES);
        memcpy(&challenge[AES_BLOCK_SIZE_BYTES],aes_mmo_context.AesMmohash,AES_BLOCK_SIZE_BYTES);

        const UInt8 *ENULSignature = ((UInt8 *)rmaTokenAddress) + 4;

        /* Verify challenge */
        UInt8 x25519_result = ROM_verify_x25519_signature(ENULSignature,challenge, &ENULSignature[32],Upgrade_SecureBoot_Curve25519_PublicKey);

        return x25519_result;

    }

    return false;

}
Bool gpSecureBoot_AuthenticateImage(UInt32 startAddressImage, UInt32 licenseOffset)
{

    UInt32 startAddressLicense = startAddressImage + licenseOffset;

    /* Fetch all the extended user license information */
    UInt32 section1Offset = *(UInt32 *)(startAddressLicense+EXTENDED_USER_LICENSE_SECTION_1_START_ADDRESS_OFFSET_OFFSET);
    UInt32 section1Size = *(UInt32 *)(startAddressLicense+EXTENDED_USER_LICENSE_SECTION_1_SIZE_OFFSET);
    UInt32 section2Offset = *(UInt32 *)(startAddressLicense+EXTENDED_USER_LICENSE_SECTION_2_START_ADDRESS_OFFSET_OFFSET);
    UInt32 section2Size = *(UInt32 *)(startAddressLicense+EXTENDED_USER_LICENSE_SECTION_2_SIZE_OFFSET);
    UInt32 extendedUserLicenseMagicWord = *(UInt32 *)(startAddressLicense+EXTENDED_USER_LICENSE_EXTENDED_LOADED_USER_LICENSE_MAGIC_WORD_OFFSET);

    // check Qorvo Extended Loaded UL MW
    if(extendedUserLicenseMagicWord != EXTENDED_USER_LICENSE_LOADED_MAGIC_WORD)
    {
        /* Magic Word in EUL was not found */
        return false;
    }

    if(ROM_aes_mmo_start(&aes_mmo_context) != 0)
    {
        return false;
    }

    /* See if first section needs to be hashed */
    if(section1Offset != EXTENDED_USER_LICENSE_SECTION_NOT_IN_USE)
    {
        /* startAddressImage is masked, such that both ALT and NORMAL flash addresses can be used */
        /* Check that section over which to hash is not bigger than the maximum flash size */
        if(((startAddressImage & FLASH_ADDRESS_MASK)+section1Offset+section1Size) > MAX_FLASH_SIZE)
        {
            return false;
        }


        // The ROM_aes_mmo_update function requires a multiple of 16-bytes as data to be hashed
        // The section size is divived by this 16, which rounds off downward
        // If there are any bytes left that didn't fit into a full 16-byte block, these bytes are
        // separately put in the 16-byte buffer appended with zeroes to create a 16-byte block to be fed to ROM_aes_mmo_update
        if(ROM_aes_mmo_update(&aes_mmo_context, (UInt8 *)startAddressImage+(section1Offset), section1Size/AES_BLOCK_SIZE_BYTES) != 0)
        {
            return false;
        }

        if (section1Size % AES_BLOCK_SIZE_BYTES != 0)
        {
            memset(aes_mmo_value_to_hash_ram,0x00,AES_BLOCK_SIZE_BYTES);

            // add left over bytes + zero padding
            memcpy(aes_mmo_value_to_hash_ram, (UInt8 *)startAddressImage+(section1Offset)+section1Size-(section1Size%AES_BLOCK_SIZE_BYTES),section1Size%AES_BLOCK_SIZE_BYTES);

            if(ROM_aes_mmo_update(&aes_mmo_context, aes_mmo_value_to_hash_ram, 1) != 0)
            {
                return false;
            }
        }
    }

    /* See if second section needs to be hashed */
    if(section2Offset != EXTENDED_USER_LICENSE_SECTION_NOT_IN_USE)
    {
        /* startAddressImage is masked, such that both ALT and NORMAL flash addresses can be used */
        /* Check that section over which to hash is not bigger than the maximum flash size */
        if(((startAddressImage & FLASH_ADDRESS_MASK)+section2Offset+section2Size) > MAX_FLASH_SIZE)
        {
            return false;
        }


        // The ROM_aes_mmo_update function requires a multiple of 16-bytes as data to be hashed
        // The section size is divived by this 16, which rounds off downward
        // If there are any bytes left that didn't fit into a full 16-byte block, these bytes are
        // separately put in the 16-byte buffer appended with zeroes to create a 16-byte block to be fed to ROM_aes_mmo_update
        if(ROM_aes_mmo_update(&aes_mmo_context, (UInt8 *)startAddressImage+(section2Offset), section2Size/AES_BLOCK_SIZE_BYTES) != 0)
        {
            return false;
        }
        if (section2Size % AES_BLOCK_SIZE_BYTES != 0)
        {
            memset(aes_mmo_value_to_hash_ram,0x00,AES_BLOCK_SIZE_BYTES);

            // add left over bytes + zero padding
            memcpy(aes_mmo_value_to_hash_ram, (UInt8 *)startAddressImage+(section2Offset)+section2Size-(section2Size%AES_BLOCK_SIZE_BYTES),section2Size%AES_BLOCK_SIZE_BYTES);

            if(ROM_aes_mmo_update(&aes_mmo_context, aes_mmo_value_to_hash_ram, 1) != 0)
            {
                return false;
            }
        }
    }


    /* At last, hash loaded and extended user license, masking out:
     *  * Vendor BLE Address
     *  * Vendor Greenpower Address
     *  * CRC Specification
     *  * Load Completed MW
     *  * Freshness counter
     *  * Signature
     */

    // Hash 5 blocks from LUL
    if(ROM_aes_mmo_update(&aes_mmo_context, (UInt8 *)startAddressLicense+(USER_LICENSE_VPP_OFFSET), 5) != 0)
    {
        return false;
    }

    memset(aes_mmo_value_to_hash_ram,0x00,AES_BLOCK_SIZE_BYTES);

    // copy last 12 bytes from LUL
    memcpy(aes_mmo_value_to_hash_ram,(UInt8 *)(startAddressLicense+USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD_OFFSET)-12,12);

    // copy first 4 bytes from ELUL
    memcpy(&aes_mmo_value_to_hash_ram[12],(UInt8 *)(startAddressLicense+EXTENDED_USER_LICENSE_EXTENDED_LOADED_USER_LICENSE_MAGIC_WORD_OFFSET),4);

    // hash this single 16-byte block
    if(ROM_aes_mmo_update(&aes_mmo_context, (void *)aes_mmo_value_to_hash_ram, 1) != 0)
    {
        return false;
    }

    if(ROM_aes_mmo_finalize(&aes_mmo_context, (UInt8 *)(startAddressLicense+EXTENDED_USER_LICENSE_EXTENDED_LOADED_USER_LICENSE_MAGIC_WORD_OFFSET+4), 36) != 0)
    {
        return false;
    }

    // challenge is concatenation of 2 AES-MMO hashes
    unsigned char challenge[32];

    memcpy(challenge,aes_mmo_context.AesMmohash,AES_BLOCK_SIZE_BYTES);
    memcpy(&challenge[AES_BLOCK_SIZE_BYTES],aes_mmo_context.AesMmohash,AES_BLOCK_SIZE_BYTES);

    const UInt8 *ENULSignature = (UInt8 *)(UInt8 *)(startAddressLicense+EXTENDED_USER_LICENSE_SIGNATURE_OFFSET);

    /* Verify challenge */
    UInt8 x25519_result = ROM_verify_x25519_signature(ENULSignature,challenge, &ENULSignature[32],Upgrade_SecureBoot_Curve25519_PublicKey);


    return x25519_result;



}
#endif

#if defined(GP_COMP_EXTSTORAGE)
Bool gpSecureBoot_ExtStorage_AuthenticateImage(UInt32 startAddressImage, UInt32 startAddressLicense)
{
    /* Fetch all the extended user license information */
    UInt32 section1Offset = *(UInt32 *)(startAddressLicense+EXTENDED_USER_LICENSE_SECTION_1_START_ADDRESS_OFFSET_OFFSET);
    UInt32 section1Size = *(UInt32 *)(startAddressLicense+EXTENDED_USER_LICENSE_SECTION_1_SIZE_OFFSET);
    UInt32 section2Offset = *(UInt32 *)(startAddressLicense+EXTENDED_USER_LICENSE_SECTION_2_START_ADDRESS_OFFSET_OFFSET);
    UInt32 section2Size = *(UInt32 *)(startAddressLicense+EXTENDED_USER_LICENSE_SECTION_2_SIZE_OFFSET);
    UInt32 extendedUserLicenseMagicWord = *(UInt32 *)(startAddressLicense+EXTENDED_USER_LICENSE_EXTENDED_LOADED_USER_LICENSE_MAGIC_WORD_OFFSET);

    // check Qorvo Extended Loaded UL MW
    if(extendedUserLicenseMagicWord != EXTENDED_USER_LICENSE_LOADED_MAGIC_WORD)
    {
        /* Magic Word in EUL was not found */
        return false;
    }

    UInt32 sz = 0;
    UInt16 imgBlockLen = IMG_MAX_BLOCK_LEN;
    UInt8 imgData[IMG_MAX_BLOCK_LEN];
    UInt32 imgMemAddr;

    uint32_t status;
    UInt8 sha256sum[ECC_MAX_KEY_SIZE];
    mbedtls_sha256_context ctx;

    mbedtls_sha256_init( &ctx );

    if( mbedtls_sha256_starts_ret( &ctx, 0 ) != 0 )
    {
        return false;
    }

    /* See if first section needs to be hashed */
    if(section1Offset != EXTENDED_USER_LICENSE_SECTION_NOT_IN_USE)
    {
#if defined(GP_DIVERSITY_LOG)
        GP_LOG_SYSTEM_PRINTF("Hashing ext section 1 : [0x%lx-0x%lx]",0,startAddressImage+(section1Offset),startAddressImage+(section1Offset)+section1Size);
        HAL_WAIT_MS(100);
#endif
        /* startAddressImage is masked, such that both ALT and NORMAL flash addresses can be used */
        /* Check that section over which to hash is not bigger than the maximum flash size */
        if(((startAddressImage & FLASH_ADDRESS_MASK)+section1Offset+section1Size) > MAX_FLASH_SIZE)
        {
            return false;
        }

        imgMemAddr = startAddressImage+section1Offset;
        do
        {
            imgBlockLen = (section1Size - sz > imgBlockLen) ? imgBlockLen : section1Size - sz;
            if(gpExtStorage_Success == gpExtStorage_ReadBlock(imgMemAddr, imgBlockLen, imgData))
            {
                if(mbedtls_sha256_update_ret( &ctx, imgData, imgBlockLen) == 0 )
                {
                    imgMemAddr += imgBlockLen;
                }
                else
                {
#if defined(GP_DIVERSITY_LOG)
                    GP_LOG_SYSTEM_PRINTF("ERROR hashing Ext section 1",0);
#endif
                return false;
                }
            }
            else
            {
#if defined(GP_DIVERSITY_LOG)
                GP_LOG_SYSTEM_PRINTF("Read from ext flash failed",0);
#endif
                return false;
            }
            sz += imgBlockLen;
        }while (sz < section1Size);
    }

    if(section2Offset != EXTENDED_USER_LICENSE_SECTION_NOT_IN_USE)
    {
#if defined(GP_DIVERSITY_LOG)
        GP_LOG_SYSTEM_PRINTF("Hashing ext section 2 : [0x%lx-0x%lx]",0,startAddressImage+(section2Offset),startAddressImage+(section2Offset)+section2Size);
        HAL_WAIT_MS(100);
#endif
        /* startAddressImage is masked, such that both ALT and NORMAL flash addresses can be used */
        /* Check that section over which to hash is not bigger than the maximum flash size */
        if(((startAddressImage & FLASH_ADDRESS_MASK)+section2Offset+section2Size) > MAX_FLASH_SIZE)
        {
            return false;
        }

        sz = 0;
        imgMemAddr = startAddressImage+section2Offset;
        imgBlockLen = IMG_MAX_BLOCK_LEN;
        do
        {
            imgBlockLen = (section2Size - sz > imgBlockLen) ? imgBlockLen : section2Size - sz;
            if(gpExtStorage_Success == gpExtStorage_ReadBlock(imgMemAddr, imgBlockLen, imgData))
            {
                if(mbedtls_sha256_update_ret( &ctx, imgData, imgBlockLen) == 0 )
                {
                    imgMemAddr += imgBlockLen;
                }
                else
                {
#if defined(GP_DIVERSITY_LOG)
                    GP_LOG_SYSTEM_PRINTF("ERROR hashing Ext section 2",0);
#endif
                    return false;
                }
            }
            else
            {
#if defined(GP_DIVERSITY_LOG)
                GP_LOG_SYSTEM_PRINTF("Read from ext flash failed",0);
#endif
                return false;
            }
            sz += imgBlockLen;
        }while (sz < section2Size);
    }

    /* At last, hash loaded and extended user license, masking out:
     *  * Vendor BLE Address
     *  * Vendor Greenpower Address
     *  * CRC Specification
     *  * Load Completed MW
     *  * Freshness counter
     *  * Signature
     */

#if defined(GP_DIVERSITY_LOG)
    GP_LOG_SYSTEM_PRINTF("Hash LUL Area [0x%lx,0x%lx]",0, startAddressLicense+USER_LICENSE_VPP_OFFSET,startAddressLicense+USER_LICENSE_VPP_OFFSET+USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD_OFFSET-USER_LICENSE_VPP_OFFSET);
    HAL_WAIT_MS(100);
#endif
    if(mbedtls_sha256_update_ret( &ctx, (void *)(startAddressLicense+USER_LICENSE_VPP_OFFSET),USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD_OFFSET-USER_LICENSE_VPP_OFFSET) != 0 )
    {
#if defined(GP_DIVERSITY_LOG)
        GP_LOG_SYSTEM_PRINTF("ERROR hashing Ext LUL area",0);
#endif
        return false;
    }

#if defined(GP_DIVERSITY_LOG)
    GP_LOG_SYSTEM_PRINTF("Hash EUL Area [0x%lx,0x%lx]",0, startAddressLicense+EXTENDED_USER_LICENSE_EXTENDED_LOADED_USER_LICENSE_MAGIC_WORD_OFFSET, startAddressLicense+EXTENDED_USER_LICENSE_EXTENDED_LOADED_USER_LICENSE_MAGIC_WORD_OFFSET+EXTENDED_USER_LICENSE_SIGNATURE_OFFSET-EXTENDED_USER_LICENSE_EXTENDED_LOADED_USER_LICENSE_MAGIC_WORD_OFFSET);
    HAL_WAIT_MS(100);
#endif
    /* hash extended user license upto signature offset */
    if(mbedtls_sha256_update_ret( &ctx, (void *)(startAddressLicense+EXTENDED_USER_LICENSE_EXTENDED_LOADED_USER_LICENSE_MAGIC_WORD_OFFSET),EXTENDED_USER_LICENSE_SIGNATURE_OFFSET-EXTENDED_USER_LICENSE_EXTENDED_LOADED_USER_LICENSE_MAGIC_WORD_OFFSET) != 0 )
    {
#if defined(GP_DIVERSITY_LOG)
        GP_LOG_SYSTEM_PRINTF("ERROR hashing EUL area",0);
#endif
        return false;
    }

    if( mbedtls_sha256_finish_ret( &ctx, sha256sum ) != 0 )
    {
#if defined(GP_DIVERSITY_LOG)
        GP_LOG_SYSTEM_PRINTF("ERROR finishing up",0);
#endif
        return false;
    }

    // Now that we got the hash, convert it into a silex block and verify the signature on this hash
    sx_enable_clock();

    /* Convert sha256 sum from mbedtls into block_t format to be digested by silex API */
    block_t digest_blk = block_t_convert(sha256sum, sizeof(sha256sum));

    /* Verify that signature matches the hash and public key */
    status = ecdsa_signature_verification_digest(sx_find_ecp_curve(CURVE),
            digest_blk,
            block_t_convert((void *)Upgrade_SecureBoot_PublicKey, (2 * sx_ecc_curve_bytesize(sx_find_ecp_curve(CURVE)))),
            block_t_convert((void *)(startAddressLicense+EXTENDED_USER_LICENSE_SIGNATURE_OFFSET), (2 * sx_ecc_curve_bytesize(sx_find_ecp_curve(CURVE)))));


    sx_disable_clock();

    return (status == CRYPTOLIB_SUCCESS);
}
#endif
