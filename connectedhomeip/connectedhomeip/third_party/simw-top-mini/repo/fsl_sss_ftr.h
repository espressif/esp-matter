/*
 *
 * Copyright 2018-2022 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SSS_APIS_INC_FSL_SSS_FTR_H_
#define SSS_APIS_INC_FSL_SSS_FTR_H_

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */

/* clang-format off */


/* # CMake Features : Start */


/** PTMW_Applet : The Secure Element Applet
 *
 * You can compile host library for different Applets listed below.
 * Please note, some of these Applets may be for NXP Internal use only.
 */

/** Compiling without any Applet Support */
#define SSS_HAVE_APPLET_NONE 0

/** A71CH (ECC) */
#define SSS_HAVE_APPLET_A71CH 0

/** A71CL (RSA) */
#define SSS_HAVE_APPLET_A71CL 0

/** Similar to A71CH */
#define SSS_HAVE_APPLET_A71CH_SIM 0

/** SE050 Type A (ECC) */
#define SSS_HAVE_APPLET_SE05X_A 0

/** SE050 Type B (RSA) */
#define SSS_HAVE_APPLET_SE05X_B 0

/** SE050 (Super set of A + B) */
#define SSS_HAVE_APPLET_SE05X_C 0

/** SE050 (Similar to A71CL) */
#define SSS_HAVE_APPLET_SE05X_L 0

/** SE051 with SPAKE Support */
#define SSS_HAVE_APPLET_SE051_H 0

/** AUTH */
#define SSS_HAVE_APPLET_AUTH 0

/** SE050E */
#define SSS_HAVE_APPLET_SE050_E 1

/** NXP Internal testing Applet */
#define SSS_HAVE_APPLET_LOOPBACK 0

#if (( 0                             \
    + SSS_HAVE_APPLET_NONE           \
    + SSS_HAVE_APPLET_A71CH          \
    + SSS_HAVE_APPLET_A71CL          \
    + SSS_HAVE_APPLET_A71CH_SIM      \
    + SSS_HAVE_APPLET_SE05X_A        \
    + SSS_HAVE_APPLET_SE05X_B        \
    + SSS_HAVE_APPLET_SE05X_C        \
    + SSS_HAVE_APPLET_SE05X_L        \
    + SSS_HAVE_APPLET_SE051_H        \
    + SSS_HAVE_APPLET_AUTH           \
    + SSS_HAVE_APPLET_SE050_E        \
    + SSS_HAVE_APPLET_LOOPBACK       \
    ) > 1)
#        error "Enable only one of 'PTMW_Applet'"
#endif


#if (( 0                             \
    + SSS_HAVE_APPLET_NONE           \
    + SSS_HAVE_APPLET_A71CH          \
    + SSS_HAVE_APPLET_A71CL          \
    + SSS_HAVE_APPLET_A71CH_SIM      \
    + SSS_HAVE_APPLET_SE05X_A        \
    + SSS_HAVE_APPLET_SE05X_B        \
    + SSS_HAVE_APPLET_SE05X_C        \
    + SSS_HAVE_APPLET_SE05X_L        \
    + SSS_HAVE_APPLET_SE051_H        \
    + SSS_HAVE_APPLET_AUTH           \
    + SSS_HAVE_APPLET_SE050_E        \
    + SSS_HAVE_APPLET_LOOPBACK       \
    ) == 0)
#        error "Enable at-least one of 'PTMW_Applet'"
#endif



/** PTMW_SE05X_Ver : SE05X Applet version.
 *
 * Selection of Applet version 03_XX enables SE050 features.
 * Selection of Applet version 06_00 enables SE051 features.
 *
 */

/** SE050 */
#define SSS_HAVE_SE05X_VER_03_XX 0

/** SE051 */
#define SSS_HAVE_SE05X_VER_06_00 0

/** SE051 */
#define SSS_HAVE_SE05X_VER_07_02 1

#if (( 0                             \
    + SSS_HAVE_SE05X_VER_03_XX       \
    + SSS_HAVE_SE05X_VER_06_00       \
    + SSS_HAVE_SE05X_VER_07_02       \
    ) > 1)
#        error "Enable only one of 'PTMW_SE05X_Ver'"
#endif


#if (( 0                             \
    + SSS_HAVE_SE05X_VER_03_XX       \
    + SSS_HAVE_SE05X_VER_06_00       \
    + SSS_HAVE_SE05X_VER_07_02       \
    ) == 0)
#        error "Enable at-least one of 'PTMW_SE05X_Ver'"
#endif



/** PTMW_HostCrypto : Counterpart Crypto on Host
 *
 * What is being used as a cryptographic library on the host.
 * As of now only OpenSSL / mbedTLS is supported
 */

/** Use mbedTLS as host crypto */
#define SSS_HAVE_HOSTCRYPTO_MBEDTLS 0

/** Use OpenSSL as host crypto */
#define SSS_HAVE_HOSTCRYPTO_OPENSSL 0

/** User Implementation of Host Crypto
 * e.g. Files at ``sss/src/user/crypto`` have low level AES/CMAC primitives.
 * The files at ``sss/src/user`` use those primitives.
 * This becomes an example for users with their own AES Implementation
 * This then becomes integration without mbedTLS/OpenSSL for SCP03 / AESKey.
 *
 * .. note:: ECKey abstraction is not implemented/available yet. */
#define SSS_HAVE_HOSTCRYPTO_USER 0

/** NO Host Crypto
 * Note, this is unsecure and only provided for experimentation
 * on platforms that do not have an mbedTLS PORT
 * Many :ref:`sssftr-control` have to be disabled to have a valid build. */
#define SSS_HAVE_HOSTCRYPTO_NONE 0

#if (( 0                             \
    + SSS_HAVE_HOSTCRYPTO_MBEDTLS    \
    + SSS_HAVE_HOSTCRYPTO_OPENSSL    \
    + SSS_HAVE_HOSTCRYPTO_USER       \
    + SSS_HAVE_HOSTCRYPTO_NONE       \
    ) > 1)
#        error "Enable only one of 'PTMW_HostCrypto'"
#endif





/** PTMW_mbedTLS_ALT : ALT Engine implementation for mbedTLS
 *
 * When set to None, mbedTLS would not use ALT Implementation to connect to / use Secure Element.
 * This needs to be set to SSS for Cloud Demos over SSS APIs
 */

/** Use SSS Layer ALT implementation */
#define SSS_HAVE_MBEDTLS_ALT_SSS 1

/** Legacy implementation */
#define SSS_HAVE_MBEDTLS_ALT_A71CH 0

/** Enable TF-M based on PSA as ALT */
#define SSS_HAVE_MBEDTLS_ALT_PSA 0

/** Not using any mbedTLS_ALT
 *
 * When this is selected, cloud demos can not work with mbedTLS */
#define SSS_HAVE_MBEDTLS_ALT_NONE 0

#if (( 0                             \
    + SSS_HAVE_MBEDTLS_ALT_SSS       \
    + SSS_HAVE_MBEDTLS_ALT_A71CH     \
    + SSS_HAVE_MBEDTLS_ALT_PSA       \
    + SSS_HAVE_MBEDTLS_ALT_NONE      \
    ) > 1)
#        error "Enable only one of 'PTMW_mbedTLS_ALT'"
#endif


#if (( 0                             \
    + SSS_HAVE_MBEDTLS_ALT_SSS       \
    + SSS_HAVE_MBEDTLS_ALT_A71CH     \
    + SSS_HAVE_MBEDTLS_ALT_PSA       \
    + SSS_HAVE_MBEDTLS_ALT_NONE      \
    ) == 0)
#        error "Enable at-least one of 'PTMW_mbedTLS_ALT'"
#endif



/** PTMW_SCP : Secure Channel Protocol
 *
 * In case we enable secure channel to Secure Element, which interface to be used.
 */

/**  */
#define SSS_HAVE_SCP_NONE 0

/** Use SSS Layer for SCP.  Used for SE050 family. */
#define SSS_HAVE_SCP_SCP03_SSS 0

/** Use Host Crypto Layer for SCP03. Legacy implementation. Used for older demos of A71CH Family. */
#define SSS_HAVE_SCP_SCP03_HOSTCRYPTO 0

#if (( 0                             \
    + SSS_HAVE_SCP_NONE              \
    + SSS_HAVE_SCP_SCP03_SSS         \
    + SSS_HAVE_SCP_SCP03_HOSTCRYPTO  \
    ) > 1)
#        error "Enable only one of 'PTMW_SCP'"
#endif


/** PTMW_FIPS : Enable or disable FIPS
 *
 * This selection mostly impacts tests, and generally not the actual Middleware
 */

/** NO FIPS */
#define SSS_HAVE_FIPS_NONE 1

/** SE050 IC FIPS */
#define SSS_HAVE_FIPS_SE050 0

/** FIPS 140-2 */
#define SSS_HAVE_FIPS_140_2 0

/** FIPS 140-3 */
#define SSS_HAVE_FIPS_140_3 0

#if (( 0                             \
    + SSS_HAVE_FIPS_NONE             \
    + SSS_HAVE_FIPS_SE050            \
    + SSS_HAVE_FIPS_140_2            \
    + SSS_HAVE_FIPS_140_3            \
    ) > 1)
#        error "Enable only one of 'PTMW_FIPS'"
#endif


#if (( 0                             \
    + SSS_HAVE_FIPS_NONE             \
    + SSS_HAVE_FIPS_SE050            \
    + SSS_HAVE_FIPS_140_2            \
    + SSS_HAVE_FIPS_140_3            \
    ) == 0)
#        error "Enable at-least one of 'PTMW_FIPS'"
#endif



/** PTMW_SBL : Enable/Disable SBL Bootable support
 *
 * This option is to enable/disable boot from SBL by switching linker address
 */

/** Not SBL bootable */
#define SSS_HAVE_SBL_NONE 1

/** SE050 based LPC55S SBL bootable */
#define SSS_HAVE_SBL_SBL_LPC55S 0

#if (( 0                             \
    + SSS_HAVE_SBL_NONE              \
    + SSS_HAVE_SBL_SBL_LPC55S        \
    ) > 1)
#        error "Enable only one of 'PTMW_SBL'"
#endif


#if (( 0                             \
    + SSS_HAVE_SBL_NONE              \
    + SSS_HAVE_SBL_SBL_LPC55S        \
    ) == 0)
#        error "Enable at-least one of 'PTMW_SBL'"
#endif



/** PTMW_SE05X_Auth : SE050 Authentication
 *
 * This settings is used by examples to connect using various options
 * to authenticate with the Applet.
 * The SE05X_Auth options can be changed for KSDK Demos and Examples.
 * To change SE05X_Auth option follow below steps.
 * Set flag ``SSS_HAVE_SCP_SCP03_SSS`` to 1 and Reset flag ``SSS_HAVE_SCP_NONE`` to 0.
 * To change SE05X_Auth option other than ``None`` and  ``PlatfSCP03``,
 * execute se05x_Delete_and_test_provision.exe in order to provision the Authentication Key.
 * To change SE05X_Auth option to ``ECKey`` or ``ECKey_PlatfSCP03``,
 * Set additional flag ``SSS_HAVE_HOSTCRYPTO_ANY`` to 1.
 */

/** Use the default session (i.e. session less) login */
#define SSS_HAVE_SE05X_AUTH_NONE 1

/** Do User Authentication with UserID */
#define SSS_HAVE_SE05X_AUTH_USERID 0

/** Use Platform SCP for connection to SE */
#define SSS_HAVE_SE05X_AUTH_PLATFSCP03 0

/** Do User Authentication with AES Key
 * Earlier this was called AppletSCP03 */
#define SSS_HAVE_SE05X_AUTH_AESKEY 0

/** Do User Authentication with EC Key
 * Earlier this was called FastSCP */
#define SSS_HAVE_SE05X_AUTH_ECKEY 0

/** UserID and PlatfSCP03 */
#define SSS_HAVE_SE05X_AUTH_USERID_PLATFSCP03 0

/** AESKey and PlatfSCP03 */
#define SSS_HAVE_SE05X_AUTH_AESKEY_PLATFSCP03 0

/** ECKey and PlatfSCP03 */
#define SSS_HAVE_SE05X_AUTH_ECKEY_PLATFSCP03 0

#if (( 0                             \
    + SSS_HAVE_SE05X_AUTH_NONE       \
    + SSS_HAVE_SE05X_AUTH_USERID     \
    + SSS_HAVE_SE05X_AUTH_PLATFSCP03 \
    + SSS_HAVE_SE05X_AUTH_AESKEY     \
    + SSS_HAVE_SE05X_AUTH_ECKEY      \
    + SSS_HAVE_SE05X_AUTH_USERID_PLATFSCP03 \
    + SSS_HAVE_SE05X_AUTH_AESKEY_PLATFSCP03 \
    + SSS_HAVE_SE05X_AUTH_ECKEY_PLATFSCP03 \
    ) > 1)
#        error "Enable only one of 'PTMW_SE05X_Auth'"
#endif


#if (( 0                             \
    + SSS_HAVE_SE05X_AUTH_NONE       \
    + SSS_HAVE_SE05X_AUTH_USERID     \
    + SSS_HAVE_SE05X_AUTH_PLATFSCP03 \
    + SSS_HAVE_SE05X_AUTH_AESKEY     \
    + SSS_HAVE_SE05X_AUTH_ECKEY      \
    + SSS_HAVE_SE05X_AUTH_USERID_PLATFSCP03 \
    + SSS_HAVE_SE05X_AUTH_AESKEY_PLATFSCP03 \
    + SSS_HAVE_SE05X_AUTH_ECKEY_PLATFSCP03 \
    ) == 0)
#        error "Enable at-least one of 'PTMW_SE05X_Auth'"
#endif



/** PTMW_A71CH_AUTH : A71CH Authentication
 *
 * This settings is used by SSS-API based examples to connect using either plain or authenticated to the A71CH.
 */

/** Plain communication, not authenticated or encrypted */
#define SSS_HAVE_A71CH_AUTH_NONE 1

/** SCP03 enabled */
#define SSS_HAVE_A71CH_AUTH_SCP03 0

#if (( 0                             \
    + SSS_HAVE_A71CH_AUTH_NONE       \
    + SSS_HAVE_A71CH_AUTH_SCP03      \
    ) > 1)
#        error "Enable only one of 'PTMW_A71CH_AUTH'"
#endif


#if (( 0                             \
    + SSS_HAVE_A71CH_AUTH_NONE       \
    + SSS_HAVE_A71CH_AUTH_SCP03      \
    ) == 0)
#        error "Enable at-least one of 'PTMW_A71CH_AUTH'"
#endif


/* ====================================================================== *
 * == Feature selection/values ========================================== *
 * ====================================================================== */


/** SE05X Secure Element : Symmetric AES */
#define SSSFTR_SE05X_AES 1

/** SE05X Secure Element : Elliptic Curve Cryptography */
#define SSSFTR_SE05X_ECC 1

/** SE05X Secure Element : RSA */
#define SSSFTR_SE05X_RSA 0

/** SE05X Secure Element : KEY operations : SET Key */
#define SSSFTR_SE05X_KEY_SET 1

/** SE05X Secure Element : KEY operations : GET Key */
#define SSSFTR_SE05X_KEY_GET 1

/** SE05X Secure Element : Authenticate via ECKey */
#define SSSFTR_SE05X_AuthECKey 1

/** SE05X Secure Element : Allow creation of user/authenticated session.
 *
 * If the intended deployment only uses Platform SCP
 * Or it is a pure session less integration, this can
 * save some code size. */
#define SSSFTR_SE05X_AuthSession 0

/** SE05X Secure Element : Allow creation/deletion of Crypto Objects
 *
 * If disabled, new Crytpo Objects are neither created and
 * old/existing Crypto Objects are not deleted.
 * It is assumed that during provisioning phase, the required
 * Crypto Objects are pre-created or they are never going to
 * be needed. */
#define SSSFTR_SE05X_CREATE_DELETE_CRYPTOOBJ 1

/** Software : Symmetric AES */
#define SSSFTR_SW_AES 1

/** Software : Elliptic Curve Cryptography */
#define SSSFTR_SW_ECC 1

/** Software : RSA */
#define SSSFTR_SW_RSA 0

/** Software : KEY operations : SET Key */
#define SSSFTR_SW_KEY_SET 1

/** Software : KEY operations : GET Key */
#define SSSFTR_SW_KEY_GET 1

/** Software : Used as a test counterpart
 *
 * e.g. Major part of the mebdTLS SSS layer is purely used for
 * testing of Secure Element implementation, and can be avoided
 * fully during many production scenarios. */
#define SSSFTR_SW_TESTCOUNTERPART 1

/* ====================================================================== *
 * == Computed Options ================================================== *
 * ====================================================================== */

/** Symmetric AES */
#define SSSFTR_AES               (SSSFTR_SE05X_AES + SSSFTR_SW_AES)
/** Elliptic Curve Cryptography */
#define SSSFTR_ECC               (SSSFTR_SE05X_ECC + SSSFTR_SW_ECC)
/** RSA */
#define SSSFTR_RSA               (SSSFTR_SE05X_RSA + SSSFTR_SW_RSA)
/** KEY operations : SET Key */
#define SSSFTR_KEY_SET           (SSSFTR_SE05X_KEY_SET + SSSFTR_SW_KEY_SET)
/** KEY operations : GET Key */
#define SSSFTR_KEY_GET           (SSSFTR_SE05X_KEY_GET + SSSFTR_SW_KEY_GET)
/** KEY operations */
#define SSSFTR_KEY               (SSSFTR_KEY_SET + SSSFTR_KEY_GET)
/** KEY operations */
#define SSSFTR_SE05X_KEY         (SSSFTR_SE05X_KEY_SET + SSSFTR_SE05X_KEY_GET)
/** KEY operations */
#define SSSFTR_SW_KEY            (SSSFTR_SW_KEY_SET + SSSFTR_SW_KEY_GET)


#define SSS_HAVE_APPLET \
 (SSS_HAVE_APPLET_A71CH | SSS_HAVE_APPLET_A71CL | SSS_HAVE_APPLET_A71CH_SIM | SSS_HAVE_APPLET_SE05X_A | SSS_HAVE_APPLET_SE05X_B | SSS_HAVE_APPLET_SE05X_C | SSS_HAVE_APPLET_SE05X_L | SSS_HAVE_APPLET_SE051_H | SSS_HAVE_APPLET_AUTH | SSS_HAVE_APPLET_SE050_E | SSS_HAVE_APPLET_LOOPBACK)

#define SSS_HAVE_APPLET_SE05X_IOT \
 (SSS_HAVE_APPLET_SE05X_A | SSS_HAVE_APPLET_SE05X_B | SSS_HAVE_APPLET_SE05X_C | SSS_HAVE_APPLET_SE051_H | SSS_HAVE_APPLET_AUTH | SSS_HAVE_APPLET_SE050_E)

#define SSS_HAVE_MBEDTLS_ALT \
 (SSS_HAVE_MBEDTLS_ALT_SSS | SSS_HAVE_MBEDTLS_ALT_A71CH | SSS_HAVE_MBEDTLS_ALT_PSA)

#define SSS_HAVE_HOSTCRYPTO_ANY \
 (SSS_HAVE_HOSTCRYPTO_MBEDTLS | SSS_HAVE_HOSTCRYPTO_OPENSSL | SSS_HAVE_HOSTCRYPTO_USER)

#define SSS_HAVE_FIPS \
 (SSS_HAVE_FIPS_SE050 | SSS_HAVE_FIPS_140_2 | SSS_HAVE_FIPS_140_3)


/* Version checks GTE - Greater Than Or Equal To */
#if SSS_HAVE_APPLET_SE05X_IOT
#    if SSS_HAVE_SE05X_VER_07_02
#        define SSS_HAVE_SE05X_VER_GTE_07_02 1
#        define SSS_HAVE_SE05X_VER_GTE_06_00 1
#        define SSS_HAVE_SE05X_VER_GTE_03_XX 1
#    endif /* SSS_HAVE_SE05X_VER_07_02 */
#    if SSS_HAVE_SE05X_VER_06_00
#        define SSS_HAVE_SE05X_VER_GTE_07_02 0
#        define SSS_HAVE_SE05X_VER_GTE_06_00 1
#        define SSS_HAVE_SE05X_VER_GTE_03_XX 1
#    endif /* SSS_HAVE_SE05X_VER_06_00 */
#    if SSS_HAVE_SE05X_VER_03_XX
#        define SSS_HAVE_SE05X_VER_GTE_07_02 0
#        define SSS_HAVE_SE05X_VER_GTE_06_00 0
#        define SSS_HAVE_SE05X_VER_GTE_03_XX 1
#    endif /* SSS_HAVE_SE05X_VER_03_XX */
#else //SSS_HAVE_APPLET_SE05X_IOT
#   define SSS_HAVE_SE05X_VER_GTE_03_XX 0
#   define SSS_HAVE_SE05X_VER_GTE_06_00 0
#   define SSS_HAVE_SE05X_VER_GTE_07_02 0
#endif // SSS_HAVE_APPLET_SE05X_IOT
/** Deprecated items. Used here for backwards compatibility. */


/* # CMake Features : END */

/* ========= Miscellaneous values : START =================== */

/* ECC Mode is available */
#define SSS_HAVE_ECC 1

/* RSA is available */
#define SSS_HAVE_RSA 0

/* TPM BARRETO_NAEHRIG Curve is enabled */
#define SSS_HAVE_TPM_BN 1

/* Edwards Curve is enabled */
#define SSS_HAVE_EC_ED 1

/* Montgomery Curve is enabled */
#define SSS_HAVE_EC_MONT 1

/* MIFARE DESFire is enabled */
#define SSS_HAVE_MIFARE_DESFIRE 1

/* PBKDF2 is enabled */
#define SSS_HAVE_PBKDF2 1

/* TLS handshake support on SE is enabled */
#define SSS_HAVE_TLS_HANDSHAKE 1

/* Import Export Key is enabled */
#define SSS_HAVE_IMPORT 1

/* With NXP NFC Reader Library */
#define SSS_HAVE_NXPNFCRDLIB 0

#define SSS_HAVE_A71XX \
    (SSS_HAVE_APPLET_A71CH | SSS_HAVE_APPLET_A71CH_SIM)

#define SSS_HAVE_SSCP  (SSS_HAVE_A71XX)

/* For backwards compatibility */
#define SSS_HAVE_TESTCOUNTERPART (SSSFTR_SW_TESTCOUNTERPART)

/* ========= Miscellaneous values : END ===================== */

/* Enable one of these
 * If none is selected, default config would be used
 */
#define SSS_PFSCP_ENABLE_SE050A1 0
#define SSS_PFSCP_ENABLE_SE050A2 0
#define SSS_PFSCP_ENABLE_SE050B1 0
#define SSS_PFSCP_ENABLE_SE050B2 0
#define SSS_PFSCP_ENABLE_SE050C1 0
#define SSS_PFSCP_ENABLE_SE050C2 0
#define SSS_PFSCP_ENABLE_SE050_DEVKIT 0
#define SSS_PFSCP_ENABLE_SE051A2 0
#define SSS_PFSCP_ENABLE_SE051C2 0
#define SSS_PFSCP_ENABLE_SE050F2 0
#define SSS_PFSCP_ENABLE_SE051C_0005A8FA 0
#define SSS_PFSCP_ENABLE_SE051A_0001A920 0
#define SSS_PFSCP_ENABLE_SE050E_0001A921 0
#define SSS_PFSCP_ENABLE_A5000_0004A736 0
#define SSS_PFSCP_ENABLE_SE050F2_0001A92A 0
#define SSS_PFSCP_ENABLE_OTHER 0

/* ========= Calculated values : START ====================== */

/* Should we expose, SSS APIs */
#define SSS_HAVE_SSS ( 0             \
    + SSS_HAVE_SSCP                  \
    + SSS_HAVE_APPLET_SE05X_IOT      \
    + SSS_HAVE_HOSTCRYPTO_OPENSSL    \
    + SSS_HAVE_HOSTCRYPTO_MBEDTLS    \
    + SSS_HAVE_HOSTCRYPTO_USER       \
    )

#if SSS_HAVE_HOSTCRYPTO_NONE
#   undef  SSSFTR_SE05X_AuthSession
#   define SSSFTR_SE05X_AuthSession 0
#endif

/* Montgomery curves is not supported in SE05X_A */
#if SSS_HAVE_APPLET_SE05X_A
#       undef SSS_HAVE_EC_MONT
#       define SSS_HAVE_EC_MONT 0
    /* ED is not supported in SE050_A */
#    if SSS_HAVE_SE05X_VER_03_XX
#       undef SSS_HAVE_EC_ED
#       define SSS_HAVE_EC_ED 0
#    endif // SSS_HAVE_SE05X_VER_03_XX
#endif // SSS_HAVE_APPLET_SE05X_A

#if SSS_HAVE_RSA
#       define SSS_HAVE_RSA_4K 1
#endif // SSS_HAVE_RSA


#if SSS_HAVE_ECC
#   define SSS_HAVE_EC_NIST_192 1
#   define SSS_HAVE_EC_NIST_224 1
#   define SSS_HAVE_EC_NIST_256 1
#   define SSS_HAVE_EC_NIST_384 1
#   define SSS_HAVE_EC_NIST_521 1
#   define SSS_HAVE_EC_BP 1
#   define SSS_HAVE_EC_NIST_K 1
#   define SSS_HAVE_ECDAA 1
#   define SSS_HAVE_EDDSA 1
#   if SSS_HAVE_APPLET_SE05X_A
#      undef SSS_HAVE_ECDAA
#      undef SSS_HAVE_EDDSA
#      define SSS_HAVE_ECDAA 0
#      define SSS_HAVE_EDDSA 0
#   endif // SSS_HAVE_APPLET_SE05X_A
#   if SSS_HAVE_APPLET_AUTH
#      undef SSS_HAVE_EC_NIST_192
#      undef SSS_HAVE_EC_NIST_224
#      undef SSS_HAVE_EC_NIST_521
#      undef SSS_HAVE_EC_BP
#      undef SSS_HAVE_EC_NIST_K
#      undef SSS_HAVE_ECDAA
#      undef SSS_HAVE_EDDSA
#      define SSS_HAVE_EC_NIST_192 0
#      define SSS_HAVE_EC_NIST_224 0
#      define SSS_HAVE_EC_NIST_521 0
#      define SSS_HAVE_EC_BP 0
#      define SSS_HAVE_EC_NIST_K 0
#      define SSS_HAVE_ECDAA 0
#      define SSS_HAVE_EDDSA 0
#   endif // SSS_HAVE_APPLET_AUTH
#endif // SSS_HAVE_ECC

#if SSS_HAVE_APPLET
#    if SSS_HAVE_APPLET_AUTH
#       define SSS_HAVE_HASH_1 0
#       define SSS_HAVE_HASH_224 0
#       define SSS_HAVE_HASH_512 0
#    else
#       define SSS_HAVE_HASH_1 1
#       define SSS_HAVE_HASH_224 1
#       define SSS_HAVE_HASH_512 1
#    endif // SSS_HAVE_APPLET_AUTH
#    if SSS_HAVE_APPLET_SE050_E
#       undef SSS_HAVE_RSA
#       define SSS_HAVE_RSA 0
#    endif //SSS_HAVE_APPLET_SE050_E
#    if SSS_HAVE_RSA
#        if SSS_HAVE_APPLET_SE051_H
#           undef SSS_HAVE_RSA_4K
#           define SSS_HAVE_RSA_4K 0
#           define SSS_HAVE_RSA_3K 0
#        else
#        define SSS_HAVE_RSA_3K 1
#        endif //SSS_HAVE_APPLET_SE051_H
#    endif //SSS_HAVE_RSA
#endif



/* ========= Calculated values : END ======================== */

/* clang-format on */

#endif /* SSS_APIS_INC_FSL_SSS_FTR_H_ */
