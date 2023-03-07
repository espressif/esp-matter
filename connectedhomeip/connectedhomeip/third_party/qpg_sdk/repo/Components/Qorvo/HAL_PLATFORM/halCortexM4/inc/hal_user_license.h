/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 *   Hardware Abstraction Layer for the part of HAL that needs WB.
 *
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

#ifndef _HAL_USER_LICENSE_H_
#define _HAL_USER_LICENSE_H_

#if defined(GP_COMP_VERSION)
#include "gpVersion.h"
#endif

/*****************************************************************************
 *                   License Definitions and Values
 *****************************************************************************/

#define USER_LICENSE_START_ADDRESS                  (GP_MM_FLASH_START + 0x00)

/* Magic Words */
#define USER_LICENSE_PROGRAM_LOADED_MAGIC_WORD         (0x693A5C81UL)
#define LOADED_USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD  (USER_LICENSE_PROGRAM_LOADED_MAGIC_WORD)
#define EXTENDED_USER_LICENSE_LOADED_MAGIC_WORD        (USER_LICENSE_PROGRAM_LOADED_MAGIC_WORD)

#define USER_LICENSE_SPECIAL_IMAGE_MARKER_NORMAL_MW    (0x00000000UL)
#define USER_LICENSE_SPECIAL_IMAGE_MARKER_1ST_STAGE_MW (0xC960730DUL)
#define USER_LICENSE_SPECIAL_IMAGE_MARKER_2ND_STAGE_MW (0x78454A9EUL)
#define USER_LICENSE_DEBUG_LOCK_MAGIC_WORD             (0xDBE4080FUL)
#define USER_LICENSE_BULK_ERASE_LOCK_MAGIC_WORD        (0xA2693A5CUL)

/* Bit Masks */
#if defined(GP_DIVERSITY_GPHAL_K8E)
#define USER_LICENSE_SECTOR_WRITE_LOCK_COARSE_32K_64K   BM(0)
#define USER_LICENSE_SECTOR_WRITE_LOCK_COARSE_64K_128K  BM(1)
#define USER_LICENSE_SECTOR_WRITE_LOCK_COARSE_128K_256K  BM(2)

#define USER_LICENSE_SECTOR_WRITE_LOCK_COARSE_288K_320K BM(3)
#define USER_LICENSE_SECTOR_WRITE_LOCK_COARSE_320K_384K BM(4)
#define USER_LICENSE_SECTOR_WRITE_LOCK_COARSE_384K_512K BM(5)

#define USER_LICENSE_SECTOR_WRITE_LOCK_COARSE_544K_576K BM(6)
#define USER_LICENSE_SECTOR_WRITE_LOCK_COARSE_576K_640K BM(7)
#define USER_LICENSE_SECTOR_WRITE_LOCK_COARSE_640K_768K BM(8)

#define USER_LICENSE_SECTOR_WRITE_LOCK_COARSE_800K_832K BM(9)
#define USER_LICENSE_SECTOR_WRITE_LOCK_COARSE_832K_896K BM(10)
#define USER_LICENSE_SECTOR_WRITE_LOCK_COARSE_896K_1024K BM(11)
#else /* GP_DIVERSITY_GPHAL_K8E */
#define USER_LICENSE_SECTOR_WRITE_LOCK_COARSE_32K_64K   BM(0)
#define USER_LICENSE_SECTOR_WRITE_LOCK_COARSE_64K_128K  BM(1)
#define USER_LICENSE_SECTOR_WRITE_LOCK_COARSE_160K_192K BM(2)
#define USER_LICENSE_SECTOR_WRITE_LOCK_COARSE_192K_256K BM(3)
#define USER_LICENSE_SECTOR_WRITE_LOCK_COARSE_288K_320K BM(4)
#define USER_LICENSE_SECTOR_WRITE_LOCK_COARSE_320K_384K BM(5)
#define USER_LICENSE_SECTOR_WRITE_LOCK_COARSE_416K_448K BM(6)
#define USER_LICENSE_SECTOR_WRITE_LOCK_COARSE_448K_512K BM(7)
#endif /* GP_DIVERSITY_GPHAL_K8E */

#define USER_LICENSE_PUF_USER_ENABLE_MASK               BM(0)
#define USER_LICENSE_PUF_USER_ENABLE_RANDOM_NUMBER_MASK BM(1)
#define USER_LICENSE_PUF_USER_ENABLE_PRIVATE_KEY_MASK   BM(2)
#define USER_LICENSE_PUF_USER_ENABLE_CUSTOMER_INFO_MASK BM(3)

/* Freshness Counter 2-bit mask */
#define LOADED_USER_LICENSE_FRESHNESS_COUNTER_MASK     (0x03)

/* User License Offsets */
#define USER_LICENSE_CRC_VALUE_OFFSET                  (0x10)
#define USER_LICENSE_CRC_START_ADDRESS_OFFSET          (0x14)
#define USER_LICENSE_CRC_TYPE_OFFSET                   (0x17)
#define USER_LICENSE_CRC_SIZE_OFFSET                   (0x18)
#define USER_LICENSE_VPP_OFFSET                        (0x1C)
#define USER_LICENSE_PROGRAM_LOADED_MAGIC_WORD_OFFSET  (0x20)
#define USER_LICENSE_SECTOR_FINE_READ_LOCK_OFFSET      (0x30)
#define USER_LICENSE_SECTOR_COARSE_READ_LOCK_OFFSET    (0x38)
#define USER_LICENSE_SECTOR_FINE_WRITE_LOCK_OFFSET     (0x40)
#define USER_LICENSE_SECTOR_COARSE_WRITE_LOCK_OFFSET   (0x48)
#define USER_LICENSE_SPECIAL_IMAGE_MARKER_OFFSET       (0x74)
#define USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD_OFFSET  (0x78)
#define USER_LICENSE_FRESHNESS_COUNTER_OFFSET          (0x7F)

/* Loaded User License Offsets */
#define LOADED_USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD_OFFSET    (USER_LICENSE_LOAD_COMPLETED_MAGIC_WORD_OFFSET)
#define LOADED_USER_LICENSE_FRESHNESS_COUNTER_OFFSET            (USER_LICENSE_FRESHNESS_COUNTER_OFFSET)

/* Extended User License Offsets */
#define EXTENDED_USER_LICENSE_EXTENDED_LOADED_USER_LICENSE_MAGIC_WORD_OFFSET    (0x80)
#define EXTENDED_USER_LICENSE_ROLLBACK_PROTECTION_OFFSET                        (0x84)
#define EXTENDED_USER_LICENSE_SIGNATURE_ALGORITHM_OFFSET                        (0x90)
#define EXTENDED_USER_LICENSE_CURVE_SELECTION_OFFSET                            (0x91)
#define EXTENDED_USER_LICENSE_HASH_ALGORITHM_OFFSET                             (0x92)
#define EXTENDED_USER_LICENSE_SIGNATURE_SIZE_OFFSET                             (0x93)
#define EXTENDED_USER_LICENSE_SECTION_1_START_ADDRESS_OFFSET_OFFSET             (0x98)
#define EXTENDED_USER_LICENSE_SECTION_1_SIZE_OFFSET                             (0x9C)
#define EXTENDED_USER_LICENSE_SECTION_2_START_ADDRESS_OFFSET_OFFSET             (0xA0)
#define EXTENDED_USER_LICENSE_SECTION_2_SIZE_OFFSET                             (0xA4)
#define EXTENDED_USER_LICENSE_SIGNATURE_OFFSET                                  (0xA8)


/* Values */
#define USER_LICENSE_CRC_START_ADDRESS_OOB_VALUE (0xFFFF)
#define USER_LICENSE_CRC_AVAILABLE_VALUE         (0x01)


#define EXTENDED_USER_LICENSE_SIGNATURE_ALGORITHM_ECDSA (0x01)


#define EXTENDED_USER_LICENSE_CURVE_NONE                (0x00)
#define EXTENDED_USER_LICENSE_CURVE_P256                (0x01)
#define EXTENDED_USER_LICENSE_CURVE_ED25519             (0x02)


#define EXTENDED_USER_LICENSE_HASH_ALGO_NONE            (0x00)
#define EXTENDED_USER_LICENSE_HASH_ALGO_SHA256          (0x01)

#define EXTENDED_USER_LICENSE_SECTION_NOT_IN_USE        (0xFFFFFFFF)

#define EXTENDED_USER_LICENSE_SIG_SIZE_P256             (64)

/* Sizes */
#define USER_LICENSE_TOTAL_SIZE                 (128)
#define EXTENDED_NATIVE_USER_LICENSE_TOTAL_SIZE (USER_LICENSE_TOTAL_SIZE)
#define LOADED_USER_LICENSE_TOTAL_SIZE          (USER_LICENSE_TOTAL_SIZE)
#define EXTENDED_USER_LICENSE_TOTAL_SIZE        (USER_LICENSE_TOTAL_SIZE)

/* gpVersion */
#define USER_LICENSE_SWVERSION_MEMBER_ALLOCATED_SIZE 32
#define USER_LICENSE_SWVERSION_MEMBER_PADDING_SIZE 20

/* Magic word to be included in start of extended native user license section */
#define NATIVE_USER_LICENSE_EXTENDED_MW                    0xA3243FBC

/*****************************************************************************
 *                   User License Structures
 *****************************************************************************/


/* Native User License Structure */
typedef PACKED_PRE struct {
    /* The first user license content is not part of this structure,
     * as these entries are populated by the production programmer
    BtDeviceAddress_t vendorBLEAddress;
    UInt16            reserved;
    MACAddress_t      vendorMACAdress; */
    UInt32            crcValue;
    UInt16            crcStartAdress_Msb;
    UInt8             crcStartAdress_Lsb;
    UInt8             crcType;
    UInt32            crcSize;
    UInt32            vpp;
    UInt32            programLoadedMagicWord;
    UInt32            debugLockMagicWord;
    UInt32            bulkEraseMagicWord;
    UInt32            reserved_0;
    UInt16            sectorReadLockBitmapFine_0;
    UInt16            sectorReadLockBitmapFine_1;
    UInt16            sectorReadLockBitmapFine_2;
    UInt16            sectorReadLockBitmapFine_3;
#if defined(GP_DIVERSITY_GPHAL_K8E)
    UInt16            sectorReadLockBitmapCoarse;
    UInt8             reserved_1[6];
#else
    UInt8             sectorReadLockBitmapCoarse;
    UInt8             reserved_1[7];
#endif
    UInt16            sectorWriteLockBitmapFine_0;
    UInt16            sectorWriteLockBitmapFine_1;
    UInt16            sectorWriteLockBitmapFine_2;
    UInt16            sectorWriteLockBitmapFine_3;
#if defined(GP_DIVERSITY_GPHAL_K8E)
    UInt16            sectorWriteLockBitmapCoarse;
    UInt8             reserved_2[2];
#else
    UInt8             sectorWriteLockBitmapCoarse;
    UInt8             reserved_2[3];
#endif
    UInt32            reserved_3;
#if defined(GP_COMP_VERSION)
    gpVersion_SoftwareInfo_t swVersion;
    UInt8             swVersion_padding[USER_LICENSE_SWVERSION_MEMBER_PADDING_SIZE];
#else
    UInt8             swVersion[USER_LICENSE_SWVERSION_MEMBER_ALLOCATED_SIZE];
#endif
#if   defined(GP_DIVERSITY_GPHAL_K8E)
    UInt8            disable_deviceattest_challenge_response_cmd;
    UInt8            reserved_4[3];
#else
    UInt32            reserved_4;
#endif
    UInt32            specialImageMarkerMagicWord;
    UInt8             reserved[8];
} PACKED_POST userlicense_t;


/* Extended Loaded User License Structure */
typedef PACKED_PRE struct {
    UInt32            QorvoExtendedLoadedUserLicenseMW;
    UInt32            rollbackProtection;
    UInt8             reserved_0[8];
    UInt8             signatureAlgorithm;
    UInt8             curveSelection;
    UInt8             hashAlgorithm;
    UInt8             signatureSize;
    UInt32            reserved_1;
    UInt32            section1StartOffset;
    UInt32            section1Size;
    UInt32            section2StartOffset;
    UInt32            section2Size;
#if defined(GP_DIVERSITY_K8D_SECUREBOOT_ELUL_ASYMMETRIC_KEY)
    UInt8             x25519_signature[32]; // response
    UInt8             x25519_ephemeral_public_key[32]; // Ephemeral Pub Key
#else
    UInt8             ecdsaSignature[64];
#endif
    UInt8             reserved_2[24]; // not authenticated!

} PACKED_POST extended_userlicense_t;


/* Extended Native User License Structure */
typedef PACKED_PRE struct {
    UInt32            QorvoExtendedNativeUserLicenseMW;
    UInt32            section1StartOffset;
    UInt32            section1Size;
    UInt32            reserved_1;
#if defined(GP_DIVERSITY_K8D_SECUREBOOT_ENUL_SYMMETRIC_KEY)
    UInt8             encryptedAESMMOHash[16];
    UInt8             reserved_2[96]; // not authenticated!
#elif defined(GP_DIVERSITY_K8E_SECUREBOOT_ENUL_SYMMETRIC_KEY)
    UInt8             encryptedSHA256Hash[32];
    UInt8             reserved_2[80]; // not authenticated!
#elif defined(GP_DIVERSITY_K8D_SECUREBOOT_ENUL_ASYMMETRIC_KEY)
    UInt8             signature[32]; // response
    UInt8             ephemeral_public_key[32]; // Ephemeral Pub Key
    UInt8             reserved_2[48]; // not authenticated!
#elif defined(GP_DIVERSITY_K8E_SECUREBOOT_ENUL_ECDSA_SIGNATURE)
    UInt8             ecdsaSignature[64];
    UInt8             reserved_2[48]; // not authenticated!
#else
    UInt8             reserved_2[112]; // not authenticated!
#endif
} PACKED_POST extended_nativeuserlicense_t;


/* Loaded User License Structure */
typedef PACKED_PRE struct {
    BtDeviceAddress_t vendorBLEAddress;
    UInt16            reserved_0;
    MACAddress_t      vendorMACAdress;
    UInt32            crcValue;
    UInt16            crcStartAdress_Msb;
    UInt8             crcStartAdress_Lsb;
    UInt8             crcType;
    UInt32            crcSize;
    UInt32            vpp;
    UInt32            programLoadedMagicWord;
    UInt8             reserved_1[44];
#if defined(GP_COMP_VERSION)
    gpVersion_SoftwareInfo_t swVersion;
    UInt8             swVersion_padding[USER_LICENSE_SWVERSION_MEMBER_PADDING_SIZE];
#else
    UInt8             swVersion[USER_LICENSE_SWVERSION_MEMBER_ALLOCATED_SIZE];
#endif
    UInt32            reserved_2;
    UInt32            specialImageMarkerMagicWord;
    UInt32            loadCompleteMagicWord; // not authenticated
    UInt8             reserved_3[3];
    UInt8             freshnessCounter; // not authenticated
} PACKED_POST loaded_userlicense_t;

#if defined(GP_COMP_VERSION)
/* USER_LICENSE_SWVERSION_MEMBER_PADDING_SIZE asumes a certain size for gpVersion_SoftwareInfo_t */
GP_COMPILE_TIME_VERIFY(sizeof(gpVersion_SoftwareInfo_t) + USER_LICENSE_SWVERSION_MEMBER_PADDING_SIZE == USER_LICENSE_SWVERSION_MEMBER_ALLOCATED_SIZE);
#endif

/* Vector Pointer Pointers are defined as linker symbols */
extern UInt32 _native_user_license_vpp;
extern UInt32 _loaded_user_license_vpp;

/*****************************************************************************
 *                  Functions for retrieving license information
 *****************************************************************************/
#if defined(GP_DIVERSITY_LOADED_USER_LICENSE)
const loaded_userlicense_t *hal_get_loaded_user_license(void);
#else
const userlicense_t *hal_get_user_license(void);
#endif

#endif // _HAL_USER_LICENSE_H_
