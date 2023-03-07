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
#include "app_se_manager_macro.h"
#include "app_se_manager_secure_identity.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/private_access.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Parse the device certificate in DER format.
 *
 * @returns Returns an integer.
 ******************************************************************************/
uint32_t parse_device_cert(void);

/***************************************************************************//**
 * Get the public key in device certificate.
 *
 * @returns Returns an integer.
 ******************************************************************************/
int32_t get_pub_device_key(void);

/***************************************************************************//**
 * Parse the batch certificate in DER format.
 *
 * @returns Returns an integer.
 ******************************************************************************/
uint32_t parse_batch_cert(void);

/***************************************************************************//**
 * Parse the factory certificate in PEM format.
 *
 * @returns Returns an integer.
 ******************************************************************************/
uint32_t parse_factory_cert(void);

/***************************************************************************//**
 * Parse the root certificate in PEM format.
 *
 * @returns Returns an integer.
 ******************************************************************************/
uint32_t parse_root_cert(void);

/***************************************************************************//**
 * Verify the certificate chain against the root.
 *
 * @returns Returns an integer.
 ******************************************************************************/
uint32_t verify_cert_chain(void);

/***************************************************************************//**
 * Free the certificate contexts.
 ******************************************************************************/
void free_cert_ctx(void);

#endif  // APP_MBEDTLS_X509_H
