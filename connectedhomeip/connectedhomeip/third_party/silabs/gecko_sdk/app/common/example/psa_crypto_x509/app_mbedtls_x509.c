/***************************************************************************//**
 * @file app_mbedtls_x509.c
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_mbedtls_x509.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Callback function for mbedtls_x509_crt_verify_with_profile().
 *
 * @param data Pointer to parameter.
 * @param crt Certificate in the chain.
 * @param depth Depth of current certificate.
 * @param flags Flags for current certificate.
 * @returns Returns an integer.
 ******************************************************************************/
static int verify_callback(void *data,
                           mbedtls_x509_crt *crt,
                           int depth,
                           uint32_t *flags);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// Structure for CSR
static mbedtls_x509_csr csr_ctx;

/// Structure for CSR write
static mbedtls_x509write_csr write_csr;

/// Buffer for CSR process
static uint8_t csr_buf[CSR_BUFFER_SIZE];

/// Structure for root certificate
static mbedtls_x509_crt root_cert_ctx;

/// Structure for device certificate
static mbedtls_x509_crt device_cert_ctx;

/// Structure for certificate write
static mbedtls_x509write_cert write_cert;

/// Buffer for certificate process
static uint8_t cert_buf[CERT_BUFFER_SIZE];

/// Root certificate key context
static mbedtls_pk_context root_cert_key;

/// Root certificate serial number
static const char root_cert_serial[] = "0001";

/// Root certificate DN (Distinguished Name)
static const char root_cert_dn[] = "C=US,O=Silicon Labs,CN=Root";

/// Root certificate start date
static const char root_cert_start[] = "20200101000000";

/// Root certificate end date
static const char root_cert_end[] = "21201231235959";

/// Device certificate key context
static mbedtls_pk_context device_cert_key;

/// Device certificate serial number
static const char device_cert_serial[] = "0002";

/// Device certificate DN (Distinguished Name)
static const char device_cert_dn[] = "C=US,O=Silicon Labs,CN=Device";

/// Device certificate start date
static const char device_cert_start[] = "20200101000000";

/// Device certificate end date
static const char device_cert_end[] = "20501231235959";

/// MPI structure for serial number
static mbedtls_mpi serial;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Initialize a CSR write context.
 ******************************************************************************/
psa_status_t init_csr_write_ctx(bool root, mbedtls_md_type_t hash_alg)
{
  mbedtls_x509write_csr_init(&write_csr);
  mbedtls_x509write_csr_set_md_alg(&write_csr, hash_alg);
  if (root) {
    print_error_cycle(mbedtls_x509write_csr_set_subject_name(&write_csr,
                                                             root_cert_dn));
  } else {
    print_error_cycle(mbedtls_x509write_csr_set_subject_name(&write_csr,
                                                             device_cert_dn));
  }
}

/***************************************************************************//**
 * Initialize a PK context to wrap a PSA key.
 ******************************************************************************/
psa_status_t init_pk_ctx(bool root, psa_key_id_t id)
{
  if (root) {
    mbedtls_pk_init(&root_cert_key);
    print_error_cycle(mbedtls_pk_setup_opaque(&root_cert_key, id));
  } else {
    mbedtls_pk_init(&device_cert_key);
    print_error_cycle(mbedtls_pk_setup_opaque(&device_cert_key, id));
  }
}

/***************************************************************************//**
 * Write a CSR to a PEM string.
 ******************************************************************************/
psa_status_t write_csr_pem(bool root)
{
  // Set the key for a CSR
  if (root) {
    mbedtls_x509write_csr_set_key(&write_csr, &root_cert_key);
  } else {
    mbedtls_x509write_csr_set_key(&write_csr, &device_cert_key);
  }
  // Private key used to sign the CSR when writing it, RNG is handled by PSA
  print_error_cycle(mbedtls_x509write_csr_pem(&write_csr, csr_buf,
                                              sizeof(csr_buf), NULL, NULL));
}

/***************************************************************************//**
 * Load a CSR.
 ******************************************************************************/
psa_status_t load_csr(void)
{
  mbedtls_x509_csr_init(&csr_ctx);
  // Including the NULL for PEM data
  print_error_cycle(mbedtls_x509_csr_parse(&csr_ctx, csr_buf,
                                           strlen((char *)csr_buf) + 1));
}

/***************************************************************************//**
 * Store the CSR DN (Distinguished Name).
 ******************************************************************************/
psa_status_t store_csr_dn(bool root)
{
  psa_status_t ret;

  if (root) {
    // The length of the string written (not including the NULL)
    ret = mbedtls_x509_dn_gets((char *)cert_buf, sizeof(cert_buf),
                               &csr_ctx.subject);
    // Length = add spaces between commas - NULL = size + 2 - 1 = size + 1
    if (ret == (sizeof(root_cert_dn) + 1)) {
      return(PSA_SUCCESS);
    } else {
      return(ret);
    }
  } else {
    // The length of the string written (not including the NULL)
    ret = mbedtls_x509_dn_gets((char *)csr_buf, sizeof(csr_buf),
                               &csr_ctx.subject);
    // Length = add spaces between commas - NULL = size + 2 - 1 = size + 1
    if (ret == (sizeof(device_cert_dn) + 1)) {
      return(PSA_SUCCESS);
    } else {
      return(ret);
    }
  }
}

/***************************************************************************//**
 * Store the root certificate DN (Distinguished Name).
 ******************************************************************************/
psa_status_t store_root_dn(void)
{
  // The length of the string written (not including the NULL)
  if (mbedtls_x509_dn_gets((char *)cert_buf,
                           sizeof(cert_buf),
                           &root_cert_ctx.subject)
      == (sizeof(root_cert_dn) + 1)) {
    // Length = add spaces between commas - NULL = size + 2 - 1 = size + 1
    return(PSA_SUCCESS);
  } else {
    return(PSA_ERROR_GENERIC_ERROR);
  }
}

/***************************************************************************//**
 * Initialize a certificate write context.
 ******************************************************************************/
void init_crt_write_ctx(bool root)
{
  mbedtls_x509write_crt_init(&write_cert);
  mbedtls_x509write_crt_set_issuer_key(&write_cert, &root_cert_key);
  if (root) {
    // Subject key is equal to issuer key for self signed root certificate
    mbedtls_x509write_crt_set_subject_key(&write_cert, &root_cert_key);
  } else {
    mbedtls_x509write_crt_set_subject_key(&write_cert, &device_cert_key);
  }
}

/***************************************************************************//**
 * Set the issuer name for a certificate.
 ******************************************************************************/
psa_status_t set_issuer_name(void)
{
  print_error_cycle(mbedtls_x509write_crt_set_issuer_name(&write_cert,
                                                          (char *)cert_buf));
}

/***************************************************************************//**
 * Set the subject name for a certificate.
 ******************************************************************************/
psa_status_t set_subject_name(bool root)
{
  if (root) {
    // Subject name is equal to issuer name for self signed root certificate
    print_error_cycle(mbedtls_x509write_crt_set_subject_name(&write_cert,
                                                             (char *)cert_buf));
  } else {
    print_error_cycle(mbedtls_x509write_crt_set_subject_name(&write_cert,
                                                             (char *)csr_buf));
  }
}

/***************************************************************************//**
 * Set the parameters for a certificate.
 ******************************************************************************/
psa_status_t set_parameters(bool root, mbedtls_md_type_t hash_alg)
{
  mbedtls_x509write_crt_set_version(&write_cert, CERT_VERSION);
  mbedtls_x509write_crt_set_md_alg(&write_cert, hash_alg);
  mbedtls_mpi_init(&serial);

  // Parse serial number string to MPI
  if (root) {
    print_error_cycle(mbedtls_mpi_read_string(&serial, 10, root_cert_serial));
  } else {
    print_error_cycle(mbedtls_mpi_read_string(&serial, 10, device_cert_serial));
  }
}

/***************************************************************************//**
 * Set the serial number for a certificate.
 ******************************************************************************/
psa_status_t set_serial(void)
{
  print_error_cycle(mbedtls_x509write_crt_set_serial(&write_cert, &serial));
}

/***************************************************************************//**
 * Set the validity period for a certificate.
 ******************************************************************************/
psa_status_t set_validity(bool root)
{
  if (root) {
    print_error_cycle(mbedtls_x509write_crt_set_validity(&write_cert,
                                                         root_cert_start,
                                                         root_cert_end));
  } else {
    print_error_cycle(mbedtls_x509write_crt_set_validity(&write_cert,
                                                         device_cert_start,
                                                         device_cert_end));
  }
}

#if (CERT_VERSION == MBEDTLS_X509_CRT_VERSION_3)
/***************************************************************************//**
 * Set the basic constraints extension for a certificate.
 ******************************************************************************/
psa_status_t set_basic_constraints(bool root)
{
  if (root) {
    // CA certificate
    print_error_cycle(mbedtls_x509write_crt_set_basic_constraints(&write_cert,
                                                                  1, 0));
  } else {
    // Non-CA certificate
    print_error_cycle(mbedtls_x509write_crt_set_basic_constraints(&write_cert,
                                                                  0, -1));
  }
}

/***************************************************************************//**
 * Set the key usage extension flags for a certificate.
 ******************************************************************************/
psa_status_t set_key_usage(bool root)
{
  if (root) {
    print_error_cycle(mbedtls_x509write_crt_set_key_usage(&write_cert,
                                                          ROOT_CERT_USAGE));
  } else {
    print_error_cycle(mbedtls_x509write_crt_set_key_usage(&write_cert,
                                                          DEVICE_CERT_USAGE));
  }
}

/***************************************************************************//**
 * Set the certificate type flags for a certificate.
 ******************************************************************************/
psa_status_t set_cert_type(bool root)
{
  if (root) {
    print_error_cycle(mbedtls_x509write_crt_set_ns_cert_type(&write_cert,
                                                             ROOT_CERT_TYPE));
  } else {
    print_error_cycle(mbedtls_x509write_crt_set_ns_cert_type(&write_cert,
                                                             DEVICE_CERT_TYPE));
  }
}

#if defined(MBEDTLS_SHA1_C)
/***************************************************************************//**
 * Set the subject key identifier extension for a certificate.
 ******************************************************************************/
psa_status_t set_subject_key_identifier(void)
{
  print_error_cycle(mbedtls_x509write_crt_set_subject_key_identifier(&write_cert));
}

/***************************************************************************//**
 * Set the authority key identifier extension for a certificate.
 ******************************************************************************/
psa_status_t set_authority_key_identifier(void)
{
  print_error_cycle(mbedtls_x509write_crt_set_authority_key_identifier(&write_cert));
}
#endif
#endif

/***************************************************************************//**
 * Write a certificate to a X509 PEM string.
 ******************************************************************************/
psa_status_t write_crt_pem(void)
{
  // RNG is handled by PSA
  print_error_cycle(mbedtls_x509write_crt_pem(&write_cert, cert_buf,
                                              sizeof(cert_buf), NULL, NULL));
}

/***************************************************************************//**
 * Parse the certificate in PEM format.
 ******************************************************************************/
psa_status_t parse_cert(bool root)
{
  if (root) {
    mbedtls_x509_crt_init(&root_cert_ctx);
    // Including the NULL for PEM data
    print_error_cycle(mbedtls_x509_crt_parse(&root_cert_ctx, cert_buf,
                                             strlen((char *)cert_buf) + 1));
  } else {
    mbedtls_x509_crt_init(&device_cert_ctx);
    // Including the NULL for PEM data
    print_error_cycle(mbedtls_x509_crt_parse(&device_cert_ctx, cert_buf,
                                             strlen((char *)cert_buf) + 1));
  }
}

/***************************************************************************//**
 * Verify the certificate chain against the root.
 ******************************************************************************/
psa_status_t verify_cert_chain(void)
{
  uint32_t flags;
  const mbedtls_x509_crt_profile app_x509_crt_profile =
  {
    /* Only allow SHA-2 hashes */
    MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA224)
    | MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA256)
    | MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA384)
    | MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA512),
    0xFFFFFFF, /* Any PK alg    */
    0xFFFFFFF, /* Any curve     */
    2048,
  };

  print_error_cycle(mbedtls_x509_crt_verify_with_profile(&device_cert_ctx,
                                                         &root_cert_ctx,
                                                         NULL,
                                                         &app_x509_crt_profile,
                                                         NULL,
                                                         &flags,
                                                         verify_callback,
                                                         NULL));
}

/***************************************************************************//**
 * Print the CSR (PEM format).
 ******************************************************************************/
void print_csr(void)
{
  uint32_t i = 0;

  printf("    ");
  while ((csr_buf[i] != 0) && (i < sizeof(csr_buf))) {
    printf("%c", csr_buf[i]);
    if (csr_buf[i++] == '\n') {
      printf("    ");
    }
  }
}

/***************************************************************************//**
 * Print the certificate (PEM format).
 ******************************************************************************/
void print_cert(void)
{
  uint32_t i = 0;

  printf("    ");
  while ((cert_buf[i] != 0) && (i < sizeof(cert_buf))) {
    printf("%c", cert_buf[i]);
    if (cert_buf[i++] == '\n') {
      printf("    ");
    }
  }
}

/***************************************************************************//**
 * Free the CSR and certificate write contexts.
 ******************************************************************************/
void free_create_ctx(void)
{
  mbedtls_x509write_csr_free(&write_csr);
  mbedtls_x509_csr_free(&csr_ctx);
  mbedtls_x509write_crt_free(&write_cert);
  mbedtls_mpi_free(&serial);
}

/***************************************************************************//**
 * Free the certificate and PK contexts.
 ******************************************************************************/
void free_cert_ctx(void)
{
  mbedtls_pk_free(&root_cert_key);
  mbedtls_pk_free(&device_cert_key);
  mbedtls_x509_crt_free(&root_cert_ctx);
  mbedtls_x509_crt_free(&device_cert_ctx);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Callback function for mbedtls_x509_crt_verify_with_profile().
 ******************************************************************************/
static int verify_callback(void *data,
                           mbedtls_x509_crt *crt,
                           int depth,
                           uint32_t *flags)
{
  (void) data;
  char buf[1024];
#if (PSA_CRYPTO_PRINT_CERT == 1)
  int32_t i;
  int32_t ret;
#else
  (void) depth;
#endif

  // Get information about the certificate
#if (PSA_CRYPTO_PRINT_CERT == 1)
  ret = mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "      ", crt);
  printf("  + Verify requested for (Depth %d) ... OK\n", depth);
  for (i = 0; i < ret; i++) {
    printf("%c", buf[i]);
  }
#else
  mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "      ", crt);
#endif

  // Get the verification status of a certificate
#if (PSA_CRYPTO_PRINT_CERT == 1)
  if ((*flags) != 0) {
    ret = mbedtls_x509_crt_verify_info(buf, sizeof(buf), "  ! ", *flags);
    for (i = 0; i < ret; i++) {
      printf("%c", buf[i]);
    }
  }
  if (depth == 0) {
    printf("  + Verify the certificate chain with root certificate... ");
  }
#else
  if ((*flags) != 0) {
    mbedtls_x509_crt_verify_info(buf, sizeof(buf), "  ! ", *flags);
  }
#endif
  return 0;
}
