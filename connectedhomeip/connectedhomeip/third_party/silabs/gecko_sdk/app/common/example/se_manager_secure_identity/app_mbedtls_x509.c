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
 * Callback function for mbedtls_x509_crt_verify().
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
/// Factory certificate
static const uint8_t factory[] =
  "-----BEGIN CERTIFICATE-----\n"
  "MIICEjCCAbmgAwIBAgIIJNx7QAwynAowCgYIKoZIzj0EAwIwQjEXMBUGA1UEAwwO\n"
  "RGV2aWNlIFJvb3QgQ0ExGjAYBgNVBAoMEVNpbGljb24gTGFicyBJbmMuMQswCQYD\n"
  "VQQGEwJVUzAgFw0xODEwMTAxNzMzMDBaGA8yMTE4MDkxNjE3MzIwMFowOzEQMA4G\n"
  "A1UEAwwHRmFjdG9yeTEaMBgGA1UECgwRU2lsaWNvbiBMYWJzIEluYy4xCzAJBgNV\n"
  "BAYTAlVTMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEatHnJa9nUyTyJtuY6xgE\n"
  "msybdzjhCbmKo3qMzAt/GQ4/TKIXkCwhw1Ni6kmQzh4qrINPYWP8vnG6tPJUyzUp\n"
  "VKOBnTCBmjASBgNVHRMBAf8ECDAGAQH/AgEBMB8GA1UdIwQYMBaAFBCLCj7NdHWU\n"
  "9EyEIs2OIqSrMaVCMDQGA1UdHwQtMCswKaAnoCWGI2h0dHA6Ly9jYS5zaWxhYnMu\n"
  "Y29tL2RldmljZXJvb3QuY3JsMB0GA1UdDgQWBBRDYoRJaG86aXx20B/lHSr513PR\n"
  "FjAOBgNVHQ8BAf8EBAMCAYYwCgYIKoZIzj0EAwIDRwAwRAIgY34nvceLA1h3xYgt\n"
  "mdzguHn7yNYlJQXDp7F8iNLRTBkCIAwkPej1R90Hw2o48eNvOmJG+QeLAUdVlIGY\n"
  "07PRgSaC\n"
  "-----END CERTIFICATE-----\n";

/// Root certificate
static const uint8_t root[] =
  "-----BEGIN CERTIFICATE-----\n"
  "MIICGTCCAcCgAwIBAgIIEuaipZyqJ/kwCgYIKoZIzj0EAwIwQjEXMBUGA1UEAwwO\n"
  "RGV2aWNlIFJvb3QgQ0ExGjAYBgNVBAoMEVNpbGljb24gTGFicyBJbmMuMQswCQYD\n"
  "VQQGEwJVUzAgFw0xODEwMTAxNzMyMDBaGA8yMTE4MDkxNjE3MzIwMFowQjEXMBUG\n"
  "A1UEAwwORGV2aWNlIFJvb3QgQ0ExGjAYBgNVBAoMEVNpbGljb24gTGFicyBJbmMu\n"
  "MQswCQYDVQQGEwJVUzBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABNAp5f+cr+v9\n"
  "zxfMQMJjxLxaqdBWe4nTrCwHihHtxYZDYsSBgdzZ3VFUu0xTlP07dWsuCL99abzl\n"
  "Qyqak+tdTS2jgZ0wgZowEgYDVR0TAQH/BAgwBgEB/wIBAjAfBgNVHSMEGDAWgBQQ\n"
  "iwo+zXR1lPRMhCLNjiKkqzGlQjA0BgNVHR8ELTArMCmgJ6AlhiNodHRwOi8vY2Eu\n"
  "c2lsYWJzLmNvbS9kZXZpY2Vyb290LmNybDAdBgNVHQ4EFgQUEIsKPs10dZT0TIQi\n"
  "zY4ipKsxpUIwDgYDVR0PAQH/BAQDAgGGMAoGCCqGSM49BAMCA0cAMEQCIGlwr4G7\n"
  "IkG/9XHHk1WPthnY/yNNIzP9pThZkg2zU88ZAiBkAhsPaMKE7NOwWQIBgxy9nevX\n"
  "c7VKkqNr4UAU5zPbxg==\n"
  "-----END CERTIFICATE-----\n";

/// Certificate chain context
static mbedtls_x509_crt cert_chain;

/// Root certificate context
static mbedtls_x509_crt root_trust;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Parse the device certificate in DER format.
 ******************************************************************************/
uint32_t parse_device_cert(void)
{
  mbedtls_x509_crt_init(&cert_chain);
  print_error_cycle(mbedtls_x509_crt_parse_der(&cert_chain,
                                               (const uint8_t *)get_cert_buf_ptr(),
                                               get_cert_size(SL_SE_CERT_DEVICE_HOST)),
                    NULL);
}

/***************************************************************************//**
 * Get the public key in device certificate.
 ******************************************************************************/
int32_t get_pub_device_key(void)
{
  int32_t ret;
  mbedtls_ecp_keypair ecp_key;

  // Copy public key in device certificate to an ECP key-pair structure
  mbedtls_ecp_keypair_init(&ecp_key);
  ret = mbedtls_ecp_copy(&ecp_key.MBEDTLS_PRIVATE(Q), &mbedtls_pk_ec(cert_chain.pk)->MBEDTLS_PRIVATE(Q));
  if (ret != 0) {
    return ret;
  }

  // Save public key X to a buffer
  ret = mbedtls_mpi_write_binary(&ecp_key.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(X),
                                 (uint8_t *)get_pub_device_key_buf_ptr(),
                                 SL_SE_CERT_KEY_SIZE / 2);
  if (ret != 0) {
    return ret;
  }

  // Save public key Y to a buffer
  return mbedtls_mpi_write_binary(&ecp_key.MBEDTLS_PRIVATE(Q).MBEDTLS_PRIVATE(Y),
                                  (uint8_t *)get_pub_device_key_buf_ptr() + 32,
                                  SL_SE_CERT_KEY_SIZE / 2);
}

/***************************************************************************//**
 * Parse the batch certificate in DER format.
 ******************************************************************************/
uint32_t parse_batch_cert(void)
{
  print_error_cycle(mbedtls_x509_crt_parse_der(&cert_chain,
                                               (const uint8_t *)get_cert_buf_ptr(),
                                               get_cert_size(SL_SE_CERT_BATCH)),
                    NULL);
}

/***************************************************************************//**
 * Parse the factory certificate in PEM format.
 ******************************************************************************/
uint32_t parse_factory_cert(void)
{
  print_error_cycle(mbedtls_x509_crt_parse(&cert_chain,
                                           factory,
                                           sizeof(factory)), NULL);
}

/***************************************************************************//**
 * Parse the root certificate in PEM format.
 ******************************************************************************/
uint32_t parse_root_cert(void)
{
  mbedtls_x509_crt_init(&root_trust);
  print_error_cycle(mbedtls_x509_crt_parse(&root_trust,
                                           root,
                                           sizeof(root)), NULL);
}

/***************************************************************************//**
 * Verify the certificate chain against the root.
 ******************************************************************************/
uint32_t verify_cert_chain(void)
{
  uint32_t flags;

  print_error_cycle(mbedtls_x509_crt_verify(&cert_chain,
                                            &root_trust,
                                            NULL,
                                            NULL,
                                            &flags,
                                            verify_callback,
                                            NULL), NULL);
}

/***************************************************************************//**
 * Free the certificate contexts.
 ******************************************************************************/
void free_cert_ctx(void)
{
  mbedtls_x509_crt_free(&cert_chain);
  mbedtls_x509_crt_free(&root_trust);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Callback function for mbedtls_x509_crt_verify().
 ******************************************************************************/
static int verify_callback(void *data,
                           mbedtls_x509_crt *crt,
                           int depth,
                           uint32_t *flags)
{
  (void) data;
  char buf[1024];
#if (SE_MANAGER_PRINT_CERT == 1)
  int32_t i;
  int32_t ret;
#else
  (void) depth;
#endif

  // Get information about the certificate
#if (SE_MANAGER_PRINT_CERT == 1)
  ret = mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "      ", crt);
  printf("  + Verify requested for (Depth %d) ... OK\n", depth);
  for (i = 0; i < ret; i++) {
    printf("%c", buf[i]);
  }
#else
  mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "      ", crt);
#endif

  // Get the verification status of a certificate
#if (SE_MANAGER_PRINT_CERT == 1)
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
