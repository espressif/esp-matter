/***************************************************************************//**
 * @file app_mbedtls_x509.h
 * @brief The mbed TLS x509 functions.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef APP_MBEDTLS_X509_H
#define APP_MBEDTLS_X509_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_psa_crypto_macro.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/x509_csr.h"
#include "mbedtls/private_access.h"
#include "string.h"
#if defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
#include "sl_psa_values.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// CSR buffer size
#define CSR_BUFFER_SIZE         (1024)

/// Certificate buffer size
#define CERT_BUFFER_SIZE        (1024)

/// Certificate version
#define CERT_VERSION            MBEDTLS_X509_CRT_VERSION_3

/// Root certificate key usage
#define ROOT_CERT_USAGE         (MBEDTLS_X509_KU_DIGITAL_SIGNATURE | MBEDTLS_X509_KU_KEY_CERT_SIGN)

/// Root certificate type
#define ROOT_CERT_TYPE          (MBEDTLS_X509_NS_CERT_TYPE_OBJECT_SIGNING | MBEDTLS_X509_NS_CERT_TYPE_OBJECT_SIGNING_CA)

/// Device certificate key usage
#define DEVICE_CERT_USAGE       MBEDTLS_X509_KU_DIGITAL_SIGNATURE

/// Device certificate type
#define DEVICE_CERT_TYPE        MBEDTLS_X509_NS_CERT_TYPE_OBJECT_SIGNING

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Initialize a CSR write context.
 *
 * @param root True for root certificate, false for device certificate.
 * @param hash_alg The hash algorithm of the certificate.
 * @returns Returns an integer.
 ******************************************************************************/
psa_status_t init_csr_write_ctx(bool root, mbedtls_md_type_t hash_alg);

/***************************************************************************//**
 * Initialize a PK context to wrap a PSA key.
 *
 * @param root True for root certificate, false for device certificate.
 * @param id The identifier of the key.
 * @returns Returns an integer.
 ******************************************************************************/
psa_status_t init_pk_ctx(bool root, psa_key_id_t id);

/***************************************************************************//**
 * Write a CSR to a PEM string.
 *
 * @param root True for root certificate, false for device certificate.
 * @returns Returns an integer.
 ******************************************************************************/
psa_status_t write_csr_pem(bool root);

/***************************************************************************//**
 * Load a CSR.
 *
 * @returns Returns an integer.
 ******************************************************************************/
psa_status_t load_csr(void);

/***************************************************************************//**
 * Store the CSR DN (Distinguished Name).
 *
 * @param root True for root certificate, false for device certificate.
 * @returns Returns an integer.
 ******************************************************************************/
psa_status_t store_csr_dn(bool root);

/***************************************************************************//**
 * Store the root certificate DN (Distinguished Name).
 *
 * @returns Returns an integer.
 ******************************************************************************/
psa_status_t store_root_dn(void);

/***************************************************************************//**
 * Initialize a certificate write context.
 *
 * @param root True for root certificate, false for device certificate.
 ******************************************************************************/
void init_crt_write_ctx(bool root);

/***************************************************************************//**
 * Set the issuer name for a certificate.
 *
 * @returns Returns an integer.
 ******************************************************************************/
psa_status_t set_issuer_name(void);

/***************************************************************************//**
 * Set the subject name for a certificate.
 *
 * @param root True for root certificate, false for device certificate.
 * @returns Returns an integer.
 ******************************************************************************/
psa_status_t set_subject_name(bool root);

/***************************************************************************//**
 * Set the parameters for a certificate.
 *
 * @param root True for root certificate, false for device certificate.
 * @param hash_alg The hash algorithm of the certificate.
 * @returns Returns an integer.
 ******************************************************************************/
psa_status_t set_parameters(bool root, mbedtls_md_type_t hash_alg);

/***************************************************************************//**
 * Set the serial number for a certificate.
 *
 * @returns Returns an integer.
 ******************************************************************************/
psa_status_t set_serial(void);

/***************************************************************************//**
 * Set the validity period for a certificate.
 *
 * @param root True for root certificate, false for device certificate.
 * @returns Returns an integer.
 ******************************************************************************/
psa_status_t set_validity(bool root);

#if (CERT_VERSION == MBEDTLS_X509_CRT_VERSION_3)
/***************************************************************************//**
 * Set the basic constraints extension for a certificate.
 *
 * @param root True for root certificate, false for device certificate.
 * @returns Returns an integer.
 ******************************************************************************/
psa_status_t set_basic_constraints(bool root);

/***************************************************************************//**
 * Set the key usage extension flags for a certificate.
 *
 * @param root True for root certificate, false for device certificate.
 * @returns Returns an integer.
 ******************************************************************************/
psa_status_t set_key_usage(bool root);

/***************************************************************************//**
 * Set the certificate type flags for a certificate.
 *
 * @param root True for root certificate, false for device certificate.
 * @returns Returns an integer.
 ******************************************************************************/
psa_status_t set_cert_type(bool root);

#if defined(MBEDTLS_SHA1_C)
/***************************************************************************//**
 * Set the subject key identifier extension for a certificate.
 *
 * @returns Returns an integer.
 ******************************************************************************/
psa_status_t set_subject_key_identifier(void);

/***************************************************************************//**
 * Set the authority key identifier extension for a certificate.
 *
 * @returns Returns an integer.
 ******************************************************************************/
psa_status_t set_authority_key_identifier(void);
#endif
#endif

/***************************************************************************//**
 * Write a certificate to a X509 PEM string.
 *
 * @returns Returns an integer.
 ******************************************************************************/
psa_status_t write_crt_pem(void);

/***************************************************************************//**
 * Parse the certificate in PEM format.
 *
 * @param root True for root certificate, false for device certificate.
 * @returns Returns an integer.
 ******************************************************************************/
psa_status_t parse_cert(bool root);

/***************************************************************************//**
 * Verify the certificate chain against the root.
 *
 * @returns Returns an integer.
 ******************************************************************************/
psa_status_t verify_cert_chain(void);

/***************************************************************************//**
 * Print the CSR (PEM format).
 ******************************************************************************/
void print_csr(void);

/***************************************************************************//**
 * Print the certificate (PEM format).
 ******************************************************************************/
void print_cert(void);

/***************************************************************************//**
 * Free the CSR and certificate write contexts.
 ******************************************************************************/
void free_create_ctx(void);

/***************************************************************************//**
 * Free the certificate and PK contexts.
 ******************************************************************************/
void free_cert_ctx(void);

#endif  // APP_MBEDTLS_X509_H
