// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "azure_c_shared_utility/x509_openssl.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/const_defines.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "openssl/bio.h"
#include "openssl/rsa.h"
#include "openssl/x509.h"
#include "openssl/pem.h"
#include "openssl/err.h"
#include "openssl/engine.h"

#ifdef __APPLE__
    #ifndef EVP_PKEY_id
        #define EVP_PKEY_id(evp_key) evp_key->type
    #endif // EVP_PKEY_id
#endif // __APPLE__

static void log_ERR_get_error(const char* message)
{
    char buf[128];
    AZURE_UNREFERENCED_PARAMETER(buf);
    unsigned long error;
    int i;

    if (message != NULL)
    {
        LogError("%s", message);
    }

    error = ERR_get_error();

    for (i = 0; 0 != error; i++)
    {
        LogError("  [%d] %s", i, ERR_error_string(error, buf));
        error = ERR_get_error();
    }
}

static int load_certificate_chain(SSL_CTX* ssl_ctx, const char* certificate)
{
    int result;
    BIO* bio_cert;
    X509* x509_value;

    if ((bio_cert = BIO_new_mem_buf((char*)certificate, -1)) == NULL)
    {
        log_ERR_get_error("cannot create BIO");
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_SRS_X509_OPENSSL_07_005: [ x509_openssl_add_ecc_credentials shall load the cert chain by calling PEM_read_bio_X509_AUX and SSL_CTX_use_certification. ] */
        if ((x509_value = PEM_read_bio_X509_AUX(bio_cert, NULL, NULL, NULL)) == NULL)
        {
            log_ERR_get_error("Failure PEM_read_bio_X509_AUX");
            result = MU_FAILURE;
        }
        else
        {
            if (SSL_CTX_use_certificate(ssl_ctx, x509_value) != 1)
            {
                log_ERR_get_error("Failure PEM_read_bio_X509_AUX");
                result = MU_FAILURE;
            }
            else
            {
                X509* ca_chain;

                result = 0;
                // If we could set up our certificate, now proceed to the CA
                // certificates.

                /* Codes_SRS_X509_OPENSSL_07_006: [ If successful x509_openssl_add_ecc_credentials shall to import each certificate in the cert chain. ] */
#if (OPENSSL_VERSION_NUMBER >= 0x10100000L) && (OPENSSL_VERSION_NUMBER < 0x20000000L)
                SSL_CTX_clear_extra_chain_certs(ssl_ctx);
#else
                if (ssl_ctx->extra_certs != NULL)
                {
                    sk_X509_pop_free(ssl_ctx->extra_certs, X509_free);
                    ssl_ctx->extra_certs = NULL;
                }
#endif
                while ((ca_chain = PEM_read_bio_X509(bio_cert, NULL, NULL, NULL)) != NULL)
                {
                    if (SSL_CTX_add_extra_chain_cert(ssl_ctx, ca_chain) != 1)
                    {
                        X509_free(ca_chain);
                        result = MU_FAILURE;
                        break;
                    }
                }
                if (result != 0)
                {
                    // When the while loop ends, it's usually just EOF.
                    unsigned long err_value = ERR_peek_last_error();
                    if (ERR_GET_LIB(err_value) == ERR_LIB_PEM && ERR_GET_REASON(err_value) == PEM_R_NO_START_LINE)
                    {
                        ERR_clear_error();
                        result = 0;
                    }
                    else
                    {
                        result = MU_FAILURE;
                    }
                }
            }
            X509_free(x509_value);
        }
        BIO_free(bio_cert);
    }
    /* Codes_SRS_X509_OPENSSL_07_007: [ If any failure is encountered x509_openssl_add_ecc_credentials shall return a non-zero value. ] */
    return result;
}

static int load_ecc_key(SSL_CTX* ssl_ctx, EVP_PKEY* evp_key)
{
    int result;
    /* Codes_SRS_X509_OPENSSL_07_004: [ x509_openssl_add_ecc_credentials shall import the certification using by the EVP_PKEY. ] */
    if (SSL_CTX_use_PrivateKey(ssl_ctx, evp_key) != 1)
    {
        LogError("Failed SSL_CTX_use_PrivateKey");
        result = MU_FAILURE;
    }
    else
    {
        result = 0;
    }
    /* Codes_SRS_X509_OPENSSL_07_007: [ If any failure is encountered x509_openssl_add_ecc_credentials shall return a non-zero value. ] */
    return result;
}

static int load_key_RSA(SSL_CTX* ssl_ctx, EVP_PKEY* evp_key)
{
    int result;
    /*Codes_SRS_X509_OPENSSL_02_005: [ x509_openssl_add_credentials shall use PEM_read_bio_RSAPrivateKey to read the x509 private key. ]*/
    RSA* privatekey = EVP_PKEY_get1_RSA(evp_key);
    if (privatekey == NULL)
    {
        /*Codes_SRS_X509_OPENSSL_02_009: [ Otherwise x509_openssl_add_credentials shall fail and return a non-zero number. ]*/
        log_ERR_get_error("Failure reading RSA private key");
        result = MU_FAILURE;
    }
    else
    {
        /*Codes_SRS_X509_OPENSSL_02_007: [ x509_openssl_add_credentials shall use SSL_CTX_use_RSAPrivateKey to load the private key into the SSL context. ]*/
        if (SSL_CTX_use_RSAPrivateKey(ssl_ctx, privatekey) != 1)
        {
            /*Codes_SRS_X509_OPENSSL_02_009: [ Otherwise x509_openssl_add_credentials shall fail and return a non-zero number. ]*/
            log_ERR_get_error("Failure calling SSL_CTX_use_RSAPrivateKey");
            result = MU_FAILURE;
        }
        else
        {
            /*all is fine*/
            /*Codes_SRS_X509_OPENSSL_02_008: [ If no error occurs, then x509_openssl_add_credentials shall succeed and return 0. ]*/
            result = 0;
        }
        RSA_free(privatekey);
    }
    return result;
}

int x509_openssl_add_pem_file_key(SSL_CTX* ssl_ctx, const char* x509privatekey)
{
    int result;

    BIO* bio_key = BIO_new_mem_buf((char*)x509privatekey, -1); /*taking off the const from the pointer is needed on older versions of OPENSSL*/
    if (bio_key == NULL)
    {
        log_ERR_get_error("cannot create private key BIO");
        result = MU_FAILURE;
    }
    else
    {
        // Get the Private Key type
        EVP_PKEY* evp_key = PEM_read_bio_PrivateKey(bio_key, NULL, NULL, NULL);
        if (evp_key == NULL)
        {
            log_ERR_get_error("failure creating private key evp_key");
            result = MU_FAILURE;
        }
        else
        {
            // Check the type for the EVP key
            int evp_type = EVP_PKEY_id(evp_key);
            if (evp_type == EVP_PKEY_RSA || evp_type == EVP_PKEY_RSA2)
            {
                if (load_key_RSA(ssl_ctx, evp_key) != 0)
                {
                    LogError("failure loading RSA private key cert");
                    result = MU_FAILURE;
                }
                else
                {
                    result = 0;
                }
            }
            else
            {
                if (load_ecc_key(ssl_ctx, evp_key) != 0)
                {
                    LogError("failure loading ECC private key cert");
                    result = MU_FAILURE;
                }
                else
                {
                    result = 0;
                }
            }
            EVP_PKEY_free(evp_key);
        }
        BIO_free(bio_key);
    }

    return result;
}

int x509_openssl_add_engine_key(SSL_CTX* ssl_ctx, const char* x509privatekey_id, ENGINE* engine)
{
    int result;

    // Engine functional reference is short-lived, only within this function.
    if (!ENGINE_init(engine))
    {
        log_ERR_get_error("unable to initialize ENGINE.");
        result = MU_FAILURE;
    }
    else
    {
        if (!ENGINE_set_default(engine, ENGINE_METHOD_ALL))
        {
            log_ERR_get_error("unable to configure ENGINE.");
            result = MU_FAILURE;
        }
        else
        {
            EVP_PKEY* evp_key = ENGINE_load_private_key(engine, x509privatekey_id, NULL, NULL);

            if (evp_key == NULL)
            {
                log_ERR_get_error("unable to load private key from ENGINE.");
                result = MU_FAILURE;
            }
            else
            {
                if (load_ecc_key(ssl_ctx, evp_key) != 0)
                {
                    LogError("unable to configure private key from ENGINE.");
                    result = MU_FAILURE;
                }
                else
                {
                    result = 0;
                }

                EVP_PKEY_free(evp_key);
            }
        }

        if (!ENGINE_finish(engine))  // Release functional reference.
        {
            log_ERR_get_error("unable to release ENGINE functional reference.");
            result = MU_FAILURE;
        }
    }

    return result;
}

int x509_openssl_add_credentials(
    SSL_CTX* ssl_ctx,
    const char* x509certificate,
    const char* x509privatekey,
    OPTION_OPENSSL_KEY_TYPE x509privatekeytype,
    ENGINE* engine)
{
    int result;
    if (ssl_ctx == NULL || x509certificate == NULL || x509privatekey == NULL)
    {
        /*Codes_SRS_X509_OPENSSL_02_009: [ Otherwise x509_openssl_add_credentials shall fail and return a non-zero number. ]*/
        LogError("invalid parameter detected: ssl_ctx=%p, x509certificate=%p, x509privatekey=%p", ssl_ctx, x509certificate, x509privatekey);
        result = MU_FAILURE;
    }
    else if ((x509privatekeytype == KEY_TYPE_ENGINE) && (engine == NULL))
    {
        LogError("OpenSSL Engine must be configured when KEY_TYPE_ENGINE is used.");
        result = MU_FAILURE;
    }
    else
    {
        // Configure private key.
        if (x509privatekeytype == KEY_TYPE_DEFAULT)
        {
            result = x509_openssl_add_pem_file_key(ssl_ctx, x509privatekey);
        }
        else if (x509privatekeytype == KEY_TYPE_ENGINE)
        {
            result = x509_openssl_add_engine_key(ssl_ctx, x509privatekey, engine);
        }
        else
        {
            result = 0;
        }

        if (result == 0)
        {
            if (load_certificate_chain(ssl_ctx, x509certificate) != 0)
            {
                LogError("failure loading public cert or chain");
                result = MU_FAILURE;
            }
        }
    }

    return result;
}

/*return 0 if everything was ok, any other number to signal an error*/
/*this function inserts a x509certificate+x509privatekey to a SSL_CTX (ssl context) in order to authenticate the device with the service*/
int x509_openssl_add_certificates(SSL_CTX* ssl_ctx, const char* certificates)
{
    int result;

    /*Codes_SRS_X509_OPENSSL_02_010: [ If ssl_ctx is NULL then x509_openssl_add_certificates shall fail and return a non-zero value. ]*/
    /*Codes_SRS_X509_OPENSSL_02_011: [ If certificates is NULL then x509_openssl_add_certificates shall fail and return a non-zero value. ]*/
    if ((certificates == NULL) || (ssl_ctx == NULL))
    {
        /*Codes_SRS_X509_OPENSSL_02_018: [ In case of any failure x509_openssl_add_certificates shall fail and return a non-zero value. ]*/
        LogError("invalid argument SSL_CTX* ssl_ctx=%p, const char* certificates=%s", ssl_ctx, MU_P_OR_NULL(certificates));
        result = MU_FAILURE;
    }
    else
    {
        X509_STORE* cert_store = SSL_CTX_get_cert_store(ssl_ctx);
        if (cert_store == NULL)
        {
            /*Codes_SRS_X509_OPENSSL_02_018: [ In case of any failure x509_openssl_add_certificates shall fail and return a non-zero value. ]*/
            log_ERR_get_error("failure in SSL_CTX_get_cert_store.");
            result = MU_FAILURE;
        }
        else
        {
            /*Codes_SRS_X509_OPENSSL_02_012: [ x509_openssl_add_certificates shall get the memory BIO method function by calling BIO_s_mem. ]*/
#if (OPENSSL_VERSION_NUMBER >= 0x10100000L) && (OPENSSL_VERSION_NUMBER < 0x20000000L)
            const BIO_METHOD* bio_method;
#else
            BIO_METHOD* bio_method;
#endif
            bio_method = BIO_s_mem();
            if (bio_method == NULL)
            {
                /*Codes_SRS_X509_OPENSSL_02_018: [ In case of any failure x509_openssl_add_certificates shall fail and return a non-zero value. ]*/
                log_ERR_get_error("failure in BIO_s_mem");
                result = MU_FAILURE;
            }
            else
            {
                /*Codes_SRS_X509_OPENSSL_02_013: [ x509_openssl_add_certificates shall create a new memory BIO by calling BIO_new. ]*/
                BIO* cert_memory_bio = BIO_new(bio_method);
                if (cert_memory_bio == NULL)
                {
                    /*Codes_SRS_X509_OPENSSL_02_018: [ In case of any failure x509_openssl_add_certificates shall fail and return a non-zero value. ]*/
                    log_ERR_get_error("failure in BIO_new");
                    result = MU_FAILURE;
                }
                else
                {
                    /*Codes_SRS_X509_OPENSSL_02_014: [ x509_openssl_add_certificates shall load certificates into the memory BIO by a call to BIO_puts. ]*/
                    int puts_result = BIO_puts(cert_memory_bio, certificates);
                    if ((puts_result<0) || (puts_result != (int)strlen(certificates)))
                    {
                        /*Codes_SRS_X509_OPENSSL_02_018: [ In case of any failure x509_openssl_add_certificates shall fail and return a non-zero value. ]*/
                        log_ERR_get_error("failure in BIO_puts");
                        result = MU_FAILURE;
                    }
                    else
                    {
                        X509* certificate;
                        /*Codes_SRS_X509_OPENSSL_02_015: [ x509_openssl_add_certificates shall retrieve each certificate by a call to PEM_read_bio_X509. ]*/
                        while ((certificate = PEM_read_bio_X509(cert_memory_bio, NULL, NULL, NULL)) != NULL)
                        {
                            /*Codes_SRS_X509_OPENSSL_02_016: [ x509_openssl_add_certificates shall add the certificate to the store by a call to X509_STORE_add_cert. ]*/
                            if (!X509_STORE_add_cert(cert_store, certificate))
                            {
                                /*Codes_SRS_X509_OPENSSL_02_017: [ If X509_STORE_add_cert returns with error and that error is X509_R_CERT_ALREADY_IN_HASH_TABLE then x509_openssl_add_certificates shall ignore it as the certificate is already in the store. ]*/
                                /*if the certificate is already in the store, then ERR_peek_error would return REASON == X509_R_CERT_ALREADY_IN_HASH_TABLE, so that's no a "real" error*/
                                unsigned long error = ERR_peek_error();
                                if (ERR_GET_REASON(error) == X509_R_CERT_ALREADY_IN_HASH_TABLE)
                                {
                                    /*not an error*/
                                }
                                else
                                {
                                    /*Codes_SRS_X509_OPENSSL_02_018: [ In case of any failure x509_openssl_add_certificates shall fail and return a non-zero value. ]*/
                                    log_ERR_get_error("failure in X509_STORE_add_cert");
                                    X509_free(certificate);
                                    break;
                                }

                            }
                            X509_free(certificate);
                        }

                        if (certificate == NULL)
                        {
                            /*Codes_SRS_X509_OPENSSL_02_019: [ Otherwise, x509_openssl_add_certificates shall succeed and return 0. ]*/
                            result = 0;/*all is fine*/
                        }
                        else
                        {
                            /*previous while loop terminated unfortunately*/
                            /*Codes_SRS_X509_OPENSSL_02_018: [ In case of any failure x509_openssl_add_certificates shall fail and return a non-zero value. ]*/
                            result = MU_FAILURE;
                        }
                    }
                    BIO_free(cert_memory_bio);
                }
            }
        }
    }
    return result;
}
