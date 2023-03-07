/*
*
* Copyright 2019,2020 NXP
* SPDX-License-Identifier: Apache-2.0
*/

#ifndef FSL_SSS_SE05X_CONST_H
#define FSL_SSS_SE05X_CONST_H

#if defined(SSS_USE_FTR_FILE)
#include "fsl_sss_ftr.h"
#else
#include "fsl_sss_ftr_default.h"
#endif

#if SSS_HAVE_APPLET_SE05X_IOT

#include <se05x_ftr.h>

#define SE05X_SESSIONID_LEN (8)

/* See MAX_APDU_PAYLOAD_LENGTH in SE05x APDU Specifications.
 *
 * Using 892 so that buffer boundaries are potentially word aligned for Se050.
 * Using 1024 for Se051.
 * And expecting a failure from OnCard in case host sends a
 * larger than expected buffer.
 * Please note, depending on choice of:
 * {No Auth | UserID Auth | Applet SCP | Fast SCP }
 * and combination of either of above along with Platform SCP,
 * there is no easy way how many Exact bytes the host can
 * send to SE05x.
 */
#if SSS_HAVE_SE05X_VER_GTE_06_00
/* SE051 MAX_APDU_PAYLOAD_LENGTH 1024 */
#define SE05X_MAX_BUF_SIZE_CMD (1024)
#define SE05X_MAX_BUF_SIZE_RSP (1024)
#else
/* SE050 MAX_APDU_PAYLOAD_LENGTH 892 */
#define SE05X_MAX_BUF_SIZE_CMD (892)
#define SE05X_MAX_BUF_SIZE_RSP (892)
#endif

#define SE050_MODULE_UNIQUE_ID_LEN 18

#define SE05X_I2CM_MAX_BUF_SIZE_CMD (271)
#define SE05X_I2CM_MAX_BUF_SIZE_RSP (271)
#define SE05X_I2CM_MAX_TIMESTAMP_SIZE (12)
#define SE05X_I2CM_MAX_FRESHNESS_SIZE (16)
#define SE05X_I2CM_MAX_CHIP_ID_SIZE (18)

#define SE05X_MINIMUM_KEY_DERIVATION_OUTPUT_LEN (16)

/** How many attestation records
 *
 * Whle reading RSA Objects, modulus and public exporent get attested separately, */

#define SE05X_MAX_ATTST_DATA 2

#if SE05X_FTR_32BIT_CURVE_ID
#define START_SE05X_ID_CURVE_START (0x7E000000)
#else
#define START_SE05X_ID_CURVE_START (0)
#endif

#define CIPHER_BLOCK_SIZE 16
#define DES_BLOCK_SIZE 8
#define CIPHER_UPDATE_MAX_DATA 448
#define AEAD_UPDATE_MAX_DATA 800
#define AEAD_BLOCK_SIZE 16
#define BINARY_WRITE_MAX_LEN 500

enum Se05x_SYMM_CIPHER_MODES
{
    Se05x_SYMM_MODE_NONE = 0x00,
    Se05x_SYMM_CBC = 0x01,
    Se05x_SYMM_EBC = 0x02,
    Se05x_SYMM_CTR = 0x08, /* For AES */
};

enum Se05x_AES_PADDING
{
    Se05x_AES_PADDING_NONE = 0x00,
    Se05x_AES_PAD_NOPAD = 0x01,
    Se05x_AES_PAD_ISO9797_M1 = 0x02,
    Se05x_AES_PAD_ISO9797_M2 = 0x03,
};

enum Se05x_SHA_TYPE
{
    Se05x_SHA_1 = 0x00,
    Se05x_SHA_256 = 0x04,
    Se05x_SHA_384 = 0x05,
    Se05x_SHA_512 = 0x06,
};

enum Se05x_MAC_TYPE
{
    Se05x_CMAC = 0x0A,
};

enum Se05x_MAC_Sign_verify
{
    Se05x_MAC_Sign = 0x00,
    Se05x_MAC_Verify = 0x01,
};

enum Se05x_I2CM_RESULT_TYPE
{
    Se05x_I2CM_RESULT_SUCCESS = 0xA5,
    Se05x_I2CM_RESULT_FAILURE = 0x96 // The APDU spec defines this as 0x5A, implementation deviates!
};

#define MAX_OBJ_PCR_VALUE_SIZE 32
#define MAX_POLICY_BUFFER_SIZE 256
#define MAX_OBJ_POLICY_SIZE 55
#define MAX_OBJ_POLICY_TYPES 6
#define DEFAULT_OBJECT_POLICY_SIZE 8
#define OBJ_POLICY_HEADER_OFFSET 5
#define OBJ_POLICY_LENGTH_OFFSET 0
#define OBJ_POLICY_AUTHID_OFFSET 1
#define OBJ_POLICY_EXT_OFFSET 9
#define OBJ_POLICY_PCR_DATA_SIZE (4 + MAX_OBJ_PCR_VALUE_SIZE) /*4 bytes PCR Obj id + 32 bytes PCR value*/
#define OBJ_POLICY_AUTH_DATA_SIZE 2
#define OBJ_POLICY_OBJ_ID_SIZE 4

#define SESSION_POLICY_LENGTH_OFFSET 0
#define SESSION_POLICY_AR_HEADER_OFFSET 1
#define DEFAULT_SESSION_POLICY_SIZE 3


/*below bitmaps are set according to Se050 Applet implementation
Byte Ordering for Policy header:B1 B2 B3 B4
bits ordering
b8 b7 b6 b5 b4 b3 b2 b1
example : B1b8 : 0x80000000
*/

/* Access Rules for Object Policy*/
#define POLICY_OBJ_FORBID_ALL           0x20000000
#define POLICY_OBJ_ALLOW_SIGN           0x10000000
#define POLICY_OBJ_ALLOW_VERIFY         0x08000000
#define POLICY_OBJ_ALLOW_KA             0x04000000
#define POLICY_OBJ_ALLOW_ENC            0x02000000
#define POLICY_OBJ_ALLOW_DEC            0x01000000
#if !SSS_HAVE_SE05X_VER_GTE_07_02
#define POLICY_OBJ_ALLOW_KDF            0x00800000
#endif
#define POLICY_OBJ_ALLOW_WRAP           0x00400000
#define POLICY_OBJ_ALLOW_READ           0x00200000
#define POLICY_OBJ_ALLOW_WRITE          0x00100000
#define POLICY_OBJ_ALLOW_GEN            0x00080000
#define POLICY_OBJ_ALLOW_DELETE         0x00040000
#define POLICY_OBJ_REQUIRE_SM           0x00020000
#define POLICY_OBJ_REQUIRE_PCR_VALUE    0x00010000
#define POLICY_OBJ_ALLOW_ATTESTATION    0x00008000
#define POLICY_OBJ_ALLOW_DESFIRE_AUTHENTICATION     0x00004000
#define POLICY_OBJ_ALLOW_DESFIRE_DUMP_SESSION_KEYS  0x00002000
#define POLICY_OBJ_ALLOW_IMPORT_EXPORT              0x00001000
#if SSS_HAVE_SE05X_VER_GTE_06_00 // 4.4
#define POLICY_OBJ_FORBID_DERIVED_OUTPUT            0x00000800
#endif
#if SSS_HAVE_SE05X_VER_GTE_06_00 // 5.4
#define POLICY_OBJ_ALLOW_KDF_EXT_RANDOM     0x00000400
#endif

#if SSS_HAVE_SE05X_VER_GTE_07_02
#define POLICY_OBJ_ALLOW_TLS_KDF                    0x80000000
#define POLICY_OBJ_ALLOW_TLS_PMS                    0x40000000
#define POLICY_OBJ_ALLOW_HKDF                       0x00800000
#define POLICY_OBJ_ALLOW_DESFIRE_CHANGEKEY          0x00000200
#define POLICY_OBJ_ALLOW_DERIVED_INPUT              0x00000100
#define POLICY_OBJ_ALLOW_PBKDF                      0x00000080
#define POLICY_OBJ_ALLOW_DESFIRE_KDF                0x00000040
#define POLICY_OBJ_FORBID_EXTERNAL_IV               0x00000020
#define POLICY_OBJ_ALLOW_USAGE_AS_HMAC_PEPPER       0x00000010
#endif

#if SSS_HAVE_APPLET_SE051_H
#define POLICY_OBJ_INTERNAL_SIGN       0x00000008
#endif

/* Access Rules for Session Policy*/
#define POLICY_SESSION_MAX_APDU         0x8000
#define POLICY_SESSION_MAX_TIME         0x4000
#define POLICY_SESSION_ALLOW_REFRESH    0x2000
/**/

#endif /* SSS_HAVE_APPLET_SE05X_IOT */

#endif /* FSL_SSS_SE05X_CONST_H */
