// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Portions of this code are based upon code in BearSSL. License included below.

/*
 * Copyright (c) 2017 Thomas Pornin <pornin@bolet.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining 
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be 
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdlib.h>

#include "bearssl.h"

#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/tlsio.h"
#include "azure_c_shared_utility/socketio.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "azure_c_shared_utility/singlylinkedlist.h"
#include "azure_c_shared_utility/vector.h"
#include "azure_c_shared_utility/buffer_.h"
#include "azure_c_shared_utility/tlsio_bearssl.h"

static const char *const OPTION_UNDERLYING_IO_OPTIONS = "underlying_io_options";

typedef enum TLSIO_STATE_ENUM_TAG
{
    TLSIO_STATE_NOT_OPEN,
    TLSIO_STATE_OPENING_UNDERLYING_IO,
    TLSIO_STATE_IN_HANDSHAKE,
    TLSIO_STATE_OPEN,
    TLSIO_STATE_CLOSING,
    TLSIO_STATE_ERROR
} TLSIO_STATE_ENUM;

typedef struct PENDING_TLS_IO_TAG
{
    unsigned char* bytes;
    size_t size;
    ON_SEND_COMPLETE on_send_complete;
    void* callback_context;
    SINGLYLINKEDLIST_HANDLE pending_io_list;
} PENDING_TLS_IO;

typedef struct {
	int key_type;  /* BR_KEYTYPE_RSA or BR_KEYTYPE_EC */
	union {
		br_rsa_private_key rsa;
		br_ec_private_key ec;
	} key;
} private_key;

typedef struct TLS_IO_INSTANCE_TAG
{
    XIO_HANDLE socket_io;
    ON_BYTES_RECEIVED on_bytes_received;
    ON_IO_OPEN_COMPLETE on_io_open_complete;
    ON_IO_CLOSE_COMPLETE on_io_close_complete;
    ON_IO_ERROR on_io_error;
    void *on_bytes_received_context;
    void *on_io_open_complete_context;
    void *on_io_close_complete_context;
    void *on_io_error_context;
    TLSIO_STATE_ENUM tlsio_state;
    unsigned char *socket_io_read_bytes;
    size_t socket_io_read_byte_count;
    ON_SEND_COMPLETE on_send_complete;
    void *on_send_complete_callback_context;
    SINGLYLINKEDLIST_HANDLE pending_toencrypt_list;
    SINGLYLINKEDLIST_HANDLE pending_todecrypt_list;

    br_ssl_client_context sc;
    br_x509_minimal_context xc;
    br_sslio_context ioc;
    br_x509_trust_anchor *tas;
    br_x509_certificate *x509_cert;
    size_t x509_cert_len;
    private_key *x509_pk;
    size_t ta_count;
    char *trusted_certificates;
	char *x509_certificate;
    char *x509_private_key;
    unsigned char iobuf[BR_SSL_BUFSIZE_BIDI];
    char *hostname;
} TLS_IO_INSTANCE;

typedef struct {
	char *name;
	unsigned char *data;
	size_t data_len;
} pem_object;

static void indicate_error(TLS_IO_INSTANCE *tls_io_instance)
{
    if ((tls_io_instance->tlsio_state == TLSIO_STATE_NOT_OPEN) || (tls_io_instance->tlsio_state == TLSIO_STATE_ERROR))
    {
        return;
    }

    tls_io_instance->tlsio_state = TLSIO_STATE_ERROR;
    
    if (tls_io_instance->on_io_error != NULL)
    {
        tls_io_instance->on_io_error(tls_io_instance->on_io_error_context);
    }
}

static void indicate_open_complete(TLS_IO_INSTANCE *tls_io_instance, IO_OPEN_RESULT open_result)
{
    if (tls_io_instance->on_io_open_complete != NULL)
    {
        tls_io_instance->on_io_open_complete(tls_io_instance->on_io_open_complete_context, open_result);
    }
}

static int add_pending_operation(SINGLYLINKEDLIST_HANDLE list, const unsigned char *buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void *context)
{
    int result;
    PENDING_TLS_IO* pending_tls_io = (PENDING_TLS_IO*)malloc(sizeof(PENDING_TLS_IO));
    
    if (pending_tls_io == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        pending_tls_io->bytes = (unsigned char*)malloc(size);

        if (pending_tls_io->bytes == NULL)
        {
            LogError("Allocation Failure: Unable to allocate pending list.");
            free(pending_tls_io);
            result = MU_FAILURE;
        }
        else
        {
            pending_tls_io->size = size;
            pending_tls_io->on_send_complete = on_send_complete;
            pending_tls_io->callback_context = context;
            (void)memcpy(pending_tls_io->bytes, buffer, size);

            if (singlylinkedlist_add(list, pending_tls_io) == NULL)
            {
                LogError("Failure: Unable to add tls io to pending list.");
                free(pending_tls_io->bytes);
                free(pending_tls_io);
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }
        }
    }

    return result;
}

static void on_underlying_io_open_complete(void *context, IO_OPEN_RESULT open_result)
{
    if (context == NULL)
    {
        LogError("Invalid context NULL value passed");
    }
    else
    {
        TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)context;

        if (open_result != IO_OPEN_OK)
        {
            xio_close(tls_io_instance->socket_io, NULL, NULL);
            tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;
            indicate_open_complete(tls_io_instance, IO_OPEN_ERROR);
        }
        else
        {
            tls_io_instance->tlsio_state = TLSIO_STATE_IN_HANDSHAKE;
        }
    }
}

static void on_underlying_io_bytes_received(void *context, const unsigned char *buffer, size_t size)
{
    if (context != NULL)
    {
        TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)context;

        // Dump this on the queue and let dowork deal with it
        add_pending_operation(tls_io_instance->pending_todecrypt_list, buffer, size, NULL, NULL);
    }
}

static void on_underlying_io_error(void *context)
{
    if (context == NULL)
    {
        LogError("Invalid context NULL value passed");
    }
    else
    {
        TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)context;

        switch (tls_io_instance->tlsio_state)
        {
        default:
        case TLSIO_STATE_NOT_OPEN:
        case TLSIO_STATE_ERROR:
            break;

        case TLSIO_STATE_OPENING_UNDERLYING_IO:
        case TLSIO_STATE_IN_HANDSHAKE:
            xio_close(tls_io_instance->socket_io, NULL, NULL);
            tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;
            indicate_open_complete(tls_io_instance, IO_OPEN_ERROR);
            break;

        case TLSIO_STATE_OPEN:
            indicate_error(tls_io_instance);
            break;
        }
    }
}

static void on_underlying_io_close_complete_during_close(void *context)
{
    if (context == NULL)
    {
        LogError("NULL value passed in context");
    }
    else
    {
        TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)context;

        tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;

        if (tls_io_instance->on_io_close_complete != NULL)
        {
            tls_io_instance->on_io_close_complete(tls_io_instance->on_io_close_complete_context);
        }
    }
}

static void vblob_append(void *cc, const void *data, size_t len)
{
    int result = BUFFER_append_build((BUFFER_HANDLE)cc, (const unsigned char *)data, len);

    if (result != 0)
    {
        // Unfortunately the design does not allow for this error to be passed back
        LogError("Failed to reallocate pem decode buffer");
    }
}

VECTOR_HANDLE decode_pem(const void *src, size_t len)
{
    VECTOR_HANDLE pem_list;
	br_pem_decoder_context pc;
	pem_object po;
    //pem_object *pos;
	const unsigned char *buf;
    BUFFER_HANDLE bv = NULL;
	int inobj;
	int extra_nl;
    size_t i;

    pem_list = VECTOR_create(sizeof(pem_object));

	if (pem_list == NULL)
	{
		LogError("Unable to allocate vectore to decode PEM");
	}
	else
	{
		br_pem_decoder_init(&pc);
		buf = src;
		inobj = 0;
		po.name = NULL;
		po.data = NULL;
		po.data_len = 0;
		extra_nl = 1;

		while (len > 0) {
			size_t tlen;

			tlen = br_pem_decoder_push(&pc, buf, len);
			buf += tlen;
			len -= tlen;
			switch (br_pem_decoder_event(&pc)) {

			case BR_PEM_BEGIN_OBJ:
				inobj = 1;

				if (0 != mallocAndStrcpy_s(&po.name, br_pem_decoder_name(&pc)))
				{
					LogError("Unable to allocate memory for certificate name");
					break;
				}

				bv = BUFFER_new();

				if (bv == NULL)
				{
					LogError("Unable to allocate buffer to decode pem");
					len = 0;
					extra_nl = 0;
				}
				else
				{
					br_pem_decoder_setdest(&pc, vblob_append, bv);
				}
				break;

			case BR_PEM_END_OBJ:
				if (inobj)
				{
					po.data = BUFFER_u_char(bv);
					po.data_len = BUFFER_length(bv);
					free(bv);
					VECTOR_push_back(pem_list, &po, 1);
					po.name = NULL;
					po.data = NULL;
					po.data_len = 0;
					inobj = 0;
				}
				break;

			case BR_PEM_ERROR:
				LogError("ERROR: invalid PEM encoding");
				inobj = 1;
				break;
			}

			/*
			 * We add an extra newline at the end, in order to
			 * support PEM files that lack the newline on their last
			 * line (this is somewhat invalid, but PEM format is not
			 * standardised and such files do exist in the wild, so
			 * we'd better accept them).
			 */
			if (len == 0 && extra_nl) {
				extra_nl = 0;
				buf = (const unsigned char *)"\n";
				len = 1;
			}
		}

		if (inobj)
		{
			LogError("Unable to decode pem");


			for (i = 0; i < VECTOR_size(pem_list); i++)
			{
				free(((pem_object *)VECTOR_element(pem_list, i))->name);
				free(((pem_object *)VECTOR_element(pem_list, i))->data);
			}

			VECTOR_clear(pem_list);
			free(po.name);
			BUFFER_delete(bv);
			pem_list = NULL;
		}
	}

	return pem_list;
}

static void free_private_key(private_key *privkey)
{
    switch (privkey->key_type)
    {
    case BR_KEYTYPE_RSA:
        free(privkey->key.rsa.iq);
        free(privkey->key.rsa.dq);
        free(privkey->key.rsa.dp);
        free(privkey->key.rsa.q);
        free(privkey->key.rsa.p);
        break;
    case BR_KEYTYPE_EC:
        free(privkey->key.ec.x);
        break;
    default:
        LogError("Unknown private key type %d", privkey->key_type);
    }
}

static private_key *decode_key(const unsigned char *buf, size_t len)
{
	br_skey_decoder_context dc;
	int result;
	private_key *sk;
    int curve;
    uint32_t supp;

	br_skey_decoder_init(&dc);
	br_skey_decoder_push(&dc, buf, len);
	result = br_skey_decoder_last_error(&dc);

	if (result != 0) 
    {
		LogError("Error decoding private key: %d", result);
        sk = NULL;
	}
    else
    {
        switch (br_skey_decoder_key_type(&dc)) 
        {
            const br_rsa_private_key *rk;
            const br_ec_private_key *ek;

        case BR_KEYTYPE_RSA:
            rk = br_skey_decoder_get_rsa(&dc);
            if (NULL == (sk = (private_key *)malloc(sizeof *sk)))
            {
                LogError("Failed to allocate memory for RSA key structure");
            }
            else  
            {
                memset(sk, 0, sizeof(private_key));

                if (
                    NULL == (sk->key.rsa.p = (unsigned char *)malloc(rk->plen)) ||
                    NULL == (sk->key.rsa.q = (unsigned char *)malloc(rk->plen)) ||
                    NULL == (sk->key.rsa.dp = (unsigned char *)malloc(rk->plen)) ||
                    NULL == (sk->key.rsa.dq = (unsigned char *)malloc(rk->plen)) ||
                    NULL == (sk->key.rsa.iq = (unsigned char *)malloc(rk->plen))
                    )
                {
                    LogError("Failed to allocate memory for RSA key structure");
                    free_private_key(sk);
                    sk = NULL;
                }
                else
                {
                    sk->key_type = BR_KEYTYPE_RSA;
                    sk->key.rsa.n_bitlen = rk->n_bitlen;
                    sk->key.rsa.plen = rk->plen;
                    sk->key.rsa.qlen = rk->qlen;
                    sk->key.rsa.dplen = rk->dplen;
                    sk->key.rsa.dqlen = rk->dqlen;
                    sk->key.rsa.iqlen = rk->iqlen;
                    memcpy(sk->key.rsa.p, rk->p, rk->plen);
                    memcpy(sk->key.rsa.q, rk->q, rk->qlen);
                    memcpy(sk->key.rsa.dp, rk->dp, rk->dplen);
                    memcpy(sk->key.rsa.dq, rk->dq, rk->dqlen);
                    memcpy(sk->key.rsa.iq, rk->iq, rk->iqlen);
                }
            }
            break;

        case BR_KEYTYPE_EC:
            ek = br_skey_decoder_get_ec(&dc);
            if (NULL == (sk = (private_key *)malloc(sizeof *sk)))
            {
                LogError("Failed to allocate memory for EC key structure");
            }
            else  
            {
                memset(sk, 0, sizeof(private_key));

                if (NULL == (sk->key.ec.x = (unsigned char *)malloc(ek->xlen)))
                {
                    LogError("Failed to allocate memory for EC key structure");
                    free(sk);
                    sk = NULL;
                }
                else
                {
                    sk->key_type = BR_KEYTYPE_EC;
                    sk->key.ec.curve = ek->curve;
                    memcpy(sk->key.ec.x, ek->x, ek->xlen);
                    sk->key.ec.xlen = ek->xlen;
                    curve = sk->key.ec.curve;
                    supp = br_ec_get_default()->supported_curves;

                    if (curve > 31 || !((supp >> curve) & 1)) 
                    {
                        LogError("Private key curve (%d) is not supported\n", curve);
                        free_private_key(sk);
                        free(sk);
                        sk = NULL;
                    }
                }
            }
            break;

        default:
            LogError("Unknown key type: %d", br_skey_decoder_key_type(&dc));
            sk = NULL;
            break;
        }
    }

	return sk;
}

static private_key *read_private_key(const unsigned char *buf, size_t len)
{
    static const char RSA_PRIVATE_KEY[] = "RSA PRIVATE KEY";
    static const char EC_PRIVATE_KEY[] = "EC PRIVATE KEY";
    static const char PRIVATE_KEY[] = "PRIVATE KEY";
    static const int RSA_PRIVATE_KEY_LENGTH = sizeof(RSA_PRIVATE_KEY) - 1;
    static const int EC_PRIVATE_KEY_LENGTH = sizeof(EC_PRIVATE_KEY) - 1;
    static const int PRIVATE_KEY_LENGTH = sizeof(PRIVATE_KEY) - 1;

	private_key *sk = NULL;
    VECTOR_HANDLE pos;  // vector of pem_object
	pem_object *work;
    size_t u;

	pos = decode_pem(buf, len);
		
    if (pos != NULL) 
    {

        for (u = 0; u < VECTOR_size(pos); u++) 
        {
            work = (pem_object *)VECTOR_element(pos, u);

            if (0 == memcmp(work->name, RSA_PRIVATE_KEY, RSA_PRIVATE_KEY_LENGTH) ||
                0 == memcmp(work->name, EC_PRIVATE_KEY, EC_PRIVATE_KEY_LENGTH) ||
                0 == memcmp(work->name, RSA_PRIVATE_KEY, RSA_PRIVATE_KEY_LENGTH))
            {
                sk = decode_key(work->data, work->data_len);
                break;
            }
        }

        if (u >= VECTOR_size(pos))
        {
            LogError("No private key found in X.509 private key option");
            sk = NULL;
        }

        for (u = 0; u < VECTOR_size(pos); u++)
        {
            free(((pem_object *)VECTOR_element(pos, u))->name);
            free(((pem_object *)VECTOR_element(pos, u))->data);
        }

        VECTOR_destroy(pos);
    }
    else
    {
        sk = NULL;
    }

	return sk;
}

static VECTOR_HANDLE read_certificates_string(const char *buf, size_t len)
{
	VECTOR_HANDLE cert_list; //(br_x509_certificate) cert_list = VEC_INIT;
    VECTOR_HANDLE pem_list;
	size_t u;
    int result = 0;
    static const char CERTIFICATE[] = "CERTIFICATE";
    static const char X509_CERTIFICATE[] = "X509 CERTIFICATE";
    static const int CERTIFICATE_LEN = sizeof(CERTIFICATE) - 1;
    static const int X509_CERTIFICATE_LEN = sizeof(X509_CERTIFICATE) - 1;

    cert_list = VECTOR_create(sizeof(br_x509_certificate));

    if (cert_list == NULL)
    {
        LogError("Unable to allocate memory to decode pem strings");
        VECTOR_destroy(cert_list);
        cert_list = NULL;
    }
    else
    {
        pem_list = decode_pem(buf, len);
        
        if (pem_list == NULL) 
        {
            LogError("Failed to decode pem");
            VECTOR_destroy(cert_list);
            cert_list = NULL;
        }
        else
        {
            for (u = 0; u < VECTOR_size(pem_list); u++) 
            {
                if (0 == memcmp(CERTIFICATE, ((pem_object *)VECTOR_element(pem_list, u))->name, CERTIFICATE_LEN) ||
                    0 == memcmp(X509_CERTIFICATE, ((pem_object *)VECTOR_element(pem_list, u))->name, X509_CERTIFICATE_LEN))
                {
                    br_x509_certificate xc;

                    xc.data = ((pem_object *)VECTOR_element(pem_list, u))->data;
                    xc.data_len = ((pem_object *)VECTOR_element(pem_list, u))->data_len;
                    ((pem_object *)VECTOR_element(pem_list, u))->data = NULL;
                    ((pem_object *)VECTOR_element(pem_list, u))->data_len = 0;
                    free(((pem_object *)VECTOR_element(pem_list, u))->name);
                    ((pem_object *)VECTOR_element(pem_list, u))->name = NULL;

                    result = VECTOR_push_back(cert_list, &xc, 1);

                    if (result != 0)
                    {
                        LogError("Failed to add certificate to vector");
                        break;
                    }
                }
                else
                {
                    LogError("Unable to determine the certificate type");
                }
            }

            // If we enter this loop something failed
            for (; u < VECTOR_size(pem_list); u++)
            {
                free(((pem_object *)VECTOR_element(pem_list, u))->name);
                free(((pem_object *)VECTOR_element(pem_list, u))->data);
            }

            VECTOR_destroy(pem_list);

            if (0 == VECTOR_size(cert_list))
            {
                fprintf(stderr, "No certificate in string");
                result = MU_FAILURE;
            }

            if (result != 0)
            {
                for (u = 0; u < VECTOR_size(cert_list); u++)
                {
                    free(((br_x509_certificate*)VECTOR_element(cert_list, u))->data);
                }

                VECTOR_destroy(cert_list);
                cert_list = NULL;
            }
        }
    }

	return cert_list;
}

static void dn_append(void *ctx, const void *buf, size_t len)
{
	if (0 != (BUFFER_append_build((BUFFER_HANDLE)ctx, buf, len)))
    {
        // Design does not allow this error to be reported to caller
        LogError("Failed to append data");
    }
}

void free_ta_contents(br_x509_trust_anchor *ta)
{
	free(ta->dn.data);
	switch (ta->pkey.key_type) 
    {
	case BR_KEYTYPE_RSA:
		free(ta->pkey.key.rsa.n);
		free(ta->pkey.key.rsa.e);
		break;
	case BR_KEYTYPE_EC:
		free(ta->pkey.key.ec.q);
		break;
	}
}

void free_certificates(br_x509_certificate *certs, size_t num)
{
	size_t u;

	for (u = 0; u < num; u ++) {
		free(certs[u].data);
	}
}

static int certificate_to_trust_anchor(br_x509_certificate *xc, br_x509_trust_anchor *ta)
{
	br_x509_decoder_context dc;
	br_x509_pkey *pk;
    BUFFER_HANDLE vdn;
    //br_x509_trust_anchor *ta;
    int result = 0;

    vdn = BUFFER_new();

    if (NULL == vdn)
    {
        LogError("Failed to allocate memory to decode x509 certificate");
        result = MU_FAILURE;
    }
    else
    {
        memset(ta, 0, sizeof(br_x509_trust_anchor));
        br_x509_decoder_init(&dc, dn_append, vdn);
        br_x509_decoder_push(&dc, xc->data, xc->data_len);
        pk = br_x509_decoder_get_pkey(&dc);

        if (pk == NULL) 
        {
            LogError("ERROR: CA decoding failed with error %d", br_x509_decoder_last_error(&dc));
            BUFFER_delete(vdn);
        }
        else
        {
            ta->dn.data = BUFFER_u_char(vdn);
            ta->dn.len = BUFFER_length(vdn);
            free(vdn);
            ta->flags = 0;

            if (br_x509_decoder_isCA(&dc)) 
            {
                ta->flags |= BR_X509_TA_CA;
            }

            switch (pk->key_type) 
            {
            case BR_KEYTYPE_RSA:
                ta->pkey.key_type = BR_KEYTYPE_RSA;
        		ta->pkey.key.rsa.nlen = pk->key.rsa.nlen;
                ta->pkey.key.rsa.elen = pk->key.rsa.elen;

                if (NULL == (ta->pkey.key.rsa.n = (unsigned char *)malloc(ta->pkey.key.rsa.nlen)) ||
                    NULL == ( ta->pkey.key.rsa.e = (unsigned char *)malloc(ta->pkey.key.rsa.elen)))
                {
                    LogError("Unable to allocate memory");
                    free_ta_contents(ta);
                    result = MU_FAILURE;
                }
                else
                {
                    memcpy(ta->pkey.key.rsa.n, pk->key.rsa.n, ta->pkey.key.rsa.nlen);
                    memcpy(ta->pkey.key.rsa.e, pk->key.rsa.e, ta->pkey.key.rsa.elen);
                    result = 0;
                }
                break;
            case BR_KEYTYPE_EC:
                ta->pkey.key_type = BR_KEYTYPE_EC;
                ta->pkey.key.ec.curve = pk->key.ec.curve;
                ta->pkey.key.ec.qlen = pk->key.ec.qlen;

                if (NULL == (ta->pkey.key.ec.q = (unsigned char *)malloc(ta->pkey.key.ec.qlen)))
                {
                    LogError("Unable to allocate memory");
                    free_ta_contents(ta);
                    result = MU_FAILURE;
                }
                else
                {
                    memcpy(ta->pkey.key.ec.q, pk->key.ec.q, ta->pkey.key.ec.qlen);
                    result = 0;
                }
                break;
            default:
                fprintf(stderr, "ERROR: unsupported public key type in CA\n");
                free_ta_contents(ta);
                result = MU_FAILURE;
            }
        }
    }

	return result;
}

static size_t get_trusted_anchors(const char *certificates, size_t len, br_x509_trust_anchor *anchOut[])
{
    // Converts a PEM certificate in a string into the format required by BearSSL
    VECTOR_HANDLE xcs;
    br_x509_trust_anchor work;
	br_x509_trust_anchor *anchArray;
    size_t u;
    size_t v;
    size_t num;
    int result;

    xcs = read_certificates_string(certificates, len);
    num = VECTOR_size(xcs);

    if (VECTOR_size(xcs) == 0)
    {
        LogError("No certificates found in string");
    }
    else
    {
        anchArray = (br_x509_trust_anchor *)malloc(sizeof(br_x509_trust_anchor) * num);

        if (anchArray == NULL)
        {
            LogError("Memory allocation for trust anchors failed");
        }
        else
        {
            *anchOut = anchArray;

            for (u = 0; u < num; u++)
            {
                result = certificate_to_trust_anchor((br_x509_certificate *)VECTOR_element(xcs, u), &work);

                if (result != 0)
                {
                    for (v = 0; v < u; v++)
                    {
                        free_ta_contents((anchOut[u]));
                    }

                    free(anchArray);
                    *anchOut = NULL;
                    num = 0;
                    break;
                }

                anchArray[u] = work;
            }
        }
    }

    free_certificates((br_x509_certificate *)VECTOR_front(xcs), VECTOR_size(xcs));
    VECTOR_destroy(xcs);
    
	return num;
}

static int get_cert_signer_algo(br_x509_certificate *xc)
{
	br_x509_decoder_context dc;
	int result;

	br_x509_decoder_init(&dc, 0, 0);
	br_x509_decoder_push(&dc, xc->data, xc->data_len);
	result = br_x509_decoder_last_error(&dc);

	if (result != 0) 
    {
		LogError("Failed to get signer algorithm %d\n", result);
        result = 0;
	}
    else
    {
    	result = br_x509_decoder_get_signer_key_type(&dc);
    }

    return result;
}

CONCRETE_IO_HANDLE tlsio_bearssl_create(void *io_create_parameters)
{
    TLSIO_CONFIG *tls_io_config = (TLSIO_CONFIG *)io_create_parameters;
    TLS_IO_INSTANCE *result;

    if (tls_io_config == NULL)
    {
        LogError("NULL tls_io_config");
        result = NULL;
    }
    else
    {
        /* Codes_SRS_TLSIO_BEAR_OS5_TLS_99_006: [ The tlsio_bearssl_create shall return NULL if allocating memory for TLS_IO_INSTANCE failed. ]*/
        result = calloc(1, sizeof(TLS_IO_INSTANCE));
        if (result != NULL)
        {
            SOCKETIO_CONFIG socketio_config;
            const IO_INTERFACE_DESCRIPTION *underlying_io_interface;
            void *io_interface_parameters;

            if (tls_io_config->underlying_io_interface != NULL)
            {
                underlying_io_interface = tls_io_config->underlying_io_interface;
                io_interface_parameters = tls_io_config->underlying_io_parameters;
            }
            else
            {
                socketio_config.hostname = tls_io_config->hostname;
                socketio_config.port = tls_io_config->port;
                socketio_config.accepted_socket = NULL;
                underlying_io_interface = socketio_get_interface_description();
                io_interface_parameters = &socketio_config;
            }

            if (underlying_io_interface == NULL)
            {
                free(result);
                result = NULL;
                LogError("Failed getting socket IO interface description.");
            }
            else
            {
                mallocAndStrcpy_s(&result->hostname, tls_io_config->hostname);
                if (result->hostname == NULL)
                {
                    free(result);
                    result = NULL;
                }
                else
                {
                    result->socket_io = xio_create(underlying_io_interface, io_interface_parameters);
                    if (result->socket_io == NULL)
                    {
                        LogError("socket xio create failed");
                        free(result->hostname);
                        free(result);
                        result = NULL;
                    }
                    else
                    {
                        result->pending_toencrypt_list = singlylinkedlist_create();
                        result->pending_todecrypt_list = singlylinkedlist_create();
                        result->trusted_certificates = NULL;
                        result->x509_certificate = NULL;
                        result->x509_private_key = NULL;
                        result->tas = NULL;
                        result->x509_cert = NULL;
                        result->x509_pk = NULL;
                        result->tlsio_state = TLSIO_STATE_NOT_OPEN;
                    }
                }
            }
        }
    }

    return result;
}

void tlsio_bearssl_destroy(CONCRETE_IO_HANDLE tls_io)
{
    size_t i;
	LIST_ITEM_HANDLE first_pending_io;

	if (tls_io == NULL)
	{
		LogError("NULL handle");
	}
	else
    {
        TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)tls_io;

        if (tls_io_instance->socket_io_read_bytes != NULL)
        {
            free(tls_io_instance->socket_io_read_bytes);
            tls_io_instance->socket_io_read_bytes = NULL;
        }

        xio_destroy(tls_io_instance->socket_io);

        first_pending_io = singlylinkedlist_get_head_item(tls_io_instance->pending_toencrypt_list);

        while (first_pending_io != NULL)
        {
            PENDING_TLS_IO *pending_tls_io = (PENDING_TLS_IO *)singlylinkedlist_item_get_value(first_pending_io);
            if (pending_tls_io != NULL)
            {
                free(pending_tls_io->bytes);
                free(pending_tls_io);
            }

            (void)singlylinkedlist_remove(tls_io_instance->pending_toencrypt_list, first_pending_io);
            first_pending_io = singlylinkedlist_get_head_item(tls_io_instance->pending_toencrypt_list);
        }

        singlylinkedlist_destroy(tls_io_instance->pending_toencrypt_list);
        tls_io_instance->pending_toencrypt_list = NULL;

        first_pending_io = singlylinkedlist_get_head_item(tls_io_instance->pending_todecrypt_list);

        while (first_pending_io != NULL)
        {
            PENDING_TLS_IO *pending_tls_io = (PENDING_TLS_IO *)singlylinkedlist_item_get_value(first_pending_io);
            if (pending_tls_io != NULL)
            {
                free(pending_tls_io->bytes);
                free(pending_tls_io);
            }

            (void)singlylinkedlist_remove(tls_io_instance->pending_todecrypt_list, first_pending_io);
            first_pending_io = singlylinkedlist_get_head_item(tls_io_instance->pending_todecrypt_list);
        }

        singlylinkedlist_destroy(tls_io_instance->pending_todecrypt_list);
        tls_io_instance->pending_todecrypt_list = NULL;

        if (tls_io_instance->hostname != NULL)
        {
            free(tls_io_instance->hostname);
            tls_io_instance->hostname = NULL;
        }
        if (tls_io_instance->trusted_certificates != NULL)
        {
            free(tls_io_instance->trusted_certificates);
            tls_io_instance->trusted_certificates = NULL;

            if (tls_io_instance->tas != NULL)
            {
                // Free the memory if it has been previously allocated
                for (i = 0; i < tls_io_instance->ta_count; i++)
                {
		            free_ta_contents(&(tls_io_instance->tas[i]));
                }

                free(tls_io_instance->tas);
                tls_io_instance->tas = NULL;
                tls_io_instance->ta_count = 0;
            }

        }
        if (tls_io_instance->x509_certificate != NULL)
        {
            free(tls_io_instance->x509_certificate);
            tls_io_instance->x509_certificate = NULL;

            if (tls_io_instance->x509_cert != NULL)
            {
                free_certificates(tls_io_instance->x509_cert, tls_io_instance->x509_cert_len);
                free(tls_io_instance->x509_cert);
                tls_io_instance->x509_cert = NULL;
            }
        }
        if (tls_io_instance->x509_private_key != NULL)
        {
            free(tls_io_instance->x509_private_key);
            tls_io_instance->x509_private_key = NULL;

            if (tls_io_instance->x509_pk != NULL)
            {
                free_private_key(tls_io_instance->x509_pk);
                free(tls_io_instance->x509_pk);
                tls_io_instance->x509_pk = NULL;
            }
        }

        free(tls_io);
    }
}

int tlsio_bearssl_open(CONCRETE_IO_HANDLE tls_io, ON_IO_OPEN_COMPLETE on_io_open_complete, void *on_io_open_complete_context, ON_BYTES_RECEIVED on_bytes_received, void *on_bytes_received_context, ON_IO_ERROR on_io_error, void *on_io_error_context)
{
    int result = 0;
    unsigned int cert_signer_algo;                        

    if (tls_io == NULL)
    {
        LogError("NULL tls_io");
        result = MU_FAILURE;
    }
    else
    {
        TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)tls_io;

        if (tls_io_instance->tlsio_state != TLSIO_STATE_NOT_OPEN)
        {
            LogError("TLS already open");
            result = MU_FAILURE;
        }
        else
        {

            tls_io_instance->on_bytes_received = on_bytes_received;
            tls_io_instance->on_bytes_received_context = on_bytes_received_context;

            tls_io_instance->on_io_open_complete = on_io_open_complete;
            tls_io_instance->on_io_open_complete_context = on_io_open_complete_context;

            tls_io_instance->on_io_error = on_io_error;
            tls_io_instance->on_io_error_context = on_io_error_context;

            tls_io_instance->tlsio_state = TLSIO_STATE_OPENING_UNDERLYING_IO;

            if (tls_io_instance->ta_count == 0)
            {
                LogError("Trusted certificates are required but missing");
                result = MU_FAILURE;
            }
            else
            {
                if ((tls_io_instance->x509_pk == NULL && tls_io_instance->x509_cert != NULL) ||
                    (tls_io_instance->x509_pk != NULL && tls_io_instance->x509_cert == NULL))
                {
                    LogError("X.509 certificate and private key need to be both present or both absent");
                    result = MU_FAILURE;
                }
                else
                {
                    br_ssl_client_init_full(&tls_io_instance->sc, &tls_io_instance->xc, tls_io_instance->tas, tls_io_instance->ta_count);
                    br_ssl_engine_set_buffer(&tls_io_instance->sc.eng, tls_io_instance->iobuf, sizeof(tls_io_instance->iobuf), 1);
                    br_ssl_client_reset(&tls_io_instance->sc, tls_io_instance->hostname, 0);

                    if (tls_io_instance->x509_cert != NULL)
                    {
                        switch (tls_io_instance->x509_pk->key_type)
                        {
                        case BR_KEYTYPE_RSA:
                			br_ssl_client_set_single_rsa(&tls_io_instance->sc, 
                                                         tls_io_instance->x509_cert,
                                                         tls_io_instance->x509_cert_len,
                                                         &tls_io_instance->x509_pk->key.rsa,
                                                         br_rsa_pkcs1_sign_get_default());
                            break;
                        case BR_KEYTYPE_EC:
                            cert_signer_algo = get_cert_signer_algo(tls_io_instance->x509_cert);

                            if (cert_signer_algo == 0)
                            {
                                result = MU_FAILURE;
                            }
                            else
                            {
                                br_ssl_client_set_single_ec(&tls_io_instance->sc,
                                                            tls_io_instance->x509_cert,
                                                            tls_io_instance->x509_cert_len,
                                                            &tls_io_instance->x509_pk->key.ec,
                                                            BR_KEYTYPE_KEYX | BR_KEYTYPE_SIGN, 
                                                            cert_signer_algo,
                                                            br_ec_get_default(),
                                                            br_ecdsa_sign_asn1_get_default());
                            }
                            break;
                        default:
                            LogError("Unrecognized private key type");
                            tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;
                            result = MU_FAILURE;
                        }
                    }

                    if (result == 0)
                    {
                        if (xio_open(tls_io_instance->socket_io, on_underlying_io_open_complete, tls_io_instance, on_underlying_io_bytes_received, tls_io_instance, on_underlying_io_error, tls_io_instance) != 0)
                        {

                            LogError("Underlying IO open failed");
                            tls_io_instance->tlsio_state = TLSIO_STATE_NOT_OPEN;
                            result = MU_FAILURE;
                        }
                    }
                }
            }
        }
    }

    return result;
}

int tlsio_bearssl_close(CONCRETE_IO_HANDLE tls_io, ON_IO_CLOSE_COMPLETE on_io_close_complete, void *callback_context)
{
    int result = 0;

    if (tls_io == NULL)
    {
		LogError("NULL tls_io");
		result = MU_FAILURE;
    }
    else
    {
        TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)tls_io;

        if ((tls_io_instance->tlsio_state == TLSIO_STATE_NOT_OPEN) ||
            (tls_io_instance->tlsio_state == TLSIO_STATE_CLOSING))
        {
			LogError("Invalid state");
			result = MU_FAILURE;
        }
        else
        {
			tls_io_instance->on_io_close_complete = on_io_close_complete;
			tls_io_instance->on_io_close_complete_context = callback_context;
			tls_io_instance->tlsio_state = TLSIO_STATE_CLOSING;

            xio_close(tls_io_instance->socket_io, on_underlying_io_close_complete_during_close, tls_io_instance);
            result = 0;
        }
    }

    return result;
}

int tlsio_bearssl_send(CONCRETE_IO_HANDLE tls_io, const void *buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void *callback_context)
{
    int result;

    if ((tls_io == NULL) ||
        (buffer == NULL) ||
        (size == 0))
    {
        // Invalid arguments
        LogError("Invalid argument: tls_io is NULL");
        result = MU_FAILURE;
    }
    else
    {
        TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)tls_io;

        if (tls_io_instance->tlsio_state != TLSIO_STATE_OPEN && 
			tls_io_instance->tlsio_state != TLSIO_STATE_IN_HANDSHAKE && 
			tls_io_instance->tlsio_state != TLSIO_STATE_OPENING_UNDERLYING_IO)
        {
            LogError("TLS is not open");
            result = MU_FAILURE;
        }
        else
        {
            if (add_pending_operation(tls_io_instance->pending_toencrypt_list, buffer, size, on_send_complete, callback_context) != 0)
            {
                LogError("Failure: add_pending_io failed.");
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }
        }
    }

    return result;
}

void tlsio_bearssl_dowork(CONCRETE_IO_HANDLE tls_io)
{
    unsigned char *buffer;
    size_t bufferLen;
    unsigned long bearResult;
    int lasterr;

    if (tls_io != NULL)
    {
        TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)tls_io;

        if (tls_io_instance->tlsio_state == TLSIO_STATE_NOT_OPEN)
        {
            // Not open do nothing
        }
        else 
        {
            if (tls_io_instance->tlsio_state != TLSIO_STATE_OPENING_UNDERLYING_IO && tls_io_instance->tlsio_state != TLSIO_STATE_CLOSING)
            {
                bearResult = br_ssl_engine_current_state(&tls_io_instance->sc.eng);

                if (bearResult & BR_SSL_SENDREC)
                {
                    // The engine has data to send
                    buffer = br_ssl_engine_sendrec_buf(&tls_io_instance->sc.eng, &bufferLen);

                    if (xio_send(tls_io_instance->socket_io, buffer, bufferLen, NULL, NULL) != 0)
                    {
                        LogError("Error in xio_send.");
                        indicate_error(tls_io_instance);
                    }
                    else
                    {
                        br_ssl_engine_sendrec_ack(&tls_io_instance->sc.eng, bufferLen);
                    }
                }

                bearResult = br_ssl_engine_current_state(&tls_io_instance->sc.eng);

                if (bearResult & BR_SSL_RECVREC)
                {
                    // The engine can accept data from the peer if there is any
                    LIST_ITEM_HANDLE first_pending_io = singlylinkedlist_get_head_item(tls_io_instance->pending_todecrypt_list);
                    
                    if (first_pending_io != NULL)
                    {
                        PENDING_TLS_IO *pending_tls_io = (PENDING_TLS_IO*)singlylinkedlist_item_get_value(first_pending_io);

                        if (pending_tls_io != NULL)
                        {
                            buffer = br_ssl_engine_recvrec_buf(&tls_io_instance->sc.eng, &bufferLen);

                            if (bufferLen == 0)
                            {
                                LogError("Zero length buffer returned by BearSSL");
                                indicate_error(tls_io_instance);
                            }
                            else
                            {
                                if (pending_tls_io->size < bufferLen)
                                {
                                    bufferLen = pending_tls_io->size;
                                }
                                memcpy(buffer, pending_tls_io->bytes, bufferLen);
                                br_ssl_engine_recvrec_ack(&tls_io_instance->sc.eng, bufferLen);

                                if (bufferLen < pending_tls_io->size)
                                {
                                    pending_tls_io->size -= bufferLen;
                                    memcpy(pending_tls_io->bytes, pending_tls_io->bytes + bufferLen, pending_tls_io->size);
                                }
                                else
                                {
                                    free(pending_tls_io->bytes);
                                    free(pending_tls_io);
                                    (void)singlylinkedlist_remove(tls_io_instance->pending_todecrypt_list, first_pending_io);
                                }
                            }
                        }
                    }
                }

                bearResult = br_ssl_engine_current_state(&tls_io_instance->sc.eng);

                if (bearResult & BR_SSL_SENDAPP)
                {
                    if (tls_io_instance->tlsio_state == TLSIO_STATE_IN_HANDSHAKE)
                    {
                        tls_io_instance->tlsio_state = TLSIO_STATE_OPEN;
                        indicate_open_complete(tls_io_instance, IO_OPEN_OK);
                    }

                    // Engine is ready for application data - send it if we have any
                    LIST_ITEM_HANDLE first_pending_io = singlylinkedlist_get_head_item(tls_io_instance->pending_toencrypt_list);
                    
                    if (first_pending_io != NULL)
                    {
                        PENDING_TLS_IO *pending_tls_io = (PENDING_TLS_IO*)singlylinkedlist_item_get_value(first_pending_io);

                        if (pending_tls_io != NULL)
                        {
                            buffer = br_ssl_engine_sendapp_buf(&tls_io_instance->sc.eng, &bufferLen);

                            if (pending_tls_io->size < bufferLen)
                            {
                                bufferLen = pending_tls_io->size;
                            }
                            memcpy(buffer, pending_tls_io->bytes, bufferLen);
                            br_ssl_engine_sendapp_ack(&tls_io_instance->sc.eng, bufferLen);

                            if (bufferLen < pending_tls_io->size)
                            {
                                pending_tls_io->size -= bufferLen;
                                memcpy(pending_tls_io->bytes, pending_tls_io->bytes + bufferLen, pending_tls_io->size);
                            }
                            else
                            {
                                pending_tls_io->on_send_complete(pending_tls_io->callback_context, IO_SEND_OK);
                                free(pending_tls_io->bytes);
                                free(pending_tls_io);
                                (void)singlylinkedlist_remove(tls_io_instance->pending_toencrypt_list, first_pending_io);
                                br_ssl_engine_flush(&tls_io_instance->sc.eng, 0);
                            }
                        }
                    }
                    
                }

                bearResult = br_ssl_engine_current_state(&tls_io_instance->sc.eng);

                if (bearResult & BR_SSL_RECVAPP)
                {
                    // Application data is waiting to be forwarded
                    buffer = br_ssl_engine_recvapp_buf(&tls_io_instance->sc.eng, &bufferLen);
                    tls_io_instance->on_bytes_received(tls_io_instance->on_bytes_received_context, buffer, bufferLen);
                    br_ssl_engine_recvapp_ack(&tls_io_instance->sc.eng, bufferLen);
                }
            }

            if ((lasterr = br_ssl_engine_last_error(&tls_io_instance->sc.eng)) != 0)
            {
                LogError("BearSSL reported an error: %d", lasterr);
                indicate_error(tls_io_instance);
            }

            xio_dowork(tls_io_instance->socket_io);
        }
    }
}

// This function will clone an option given by name and value
static void *tlsio_bearssl_CloneOption(const char *name, const void *value)
{
    void *result = NULL;
    if (name == NULL || value == NULL)
    {
        LogError("invalid parameter detected: const char* name=%p, const void* value=%p", name, value);
        result = NULL;
    }
    else
    {
        if (strcmp(name, OPTION_UNDERLYING_IO_OPTIONS) == 0)
        {
            result = (void *)value;
        }
        else if (strcmp(name, OPTION_TRUSTED_CERT) == 0)
        {
            if (mallocAndStrcpy_s((char **)&result, value) != 0)
            {
                LogError("unable to mallocAndStrcpy_s TrustedCerts value");
                result = NULL;
            }
            else
            {
                // return as is
            }
        }
        else if (strcmp(name, SU_OPTION_X509_CERT) == 0)
        {
            if (mallocAndStrcpy_s((char**)&result, value) != 0)
            {
                LogError("unable to mallocAndStrcpy_s x509certificate value");
                result = NULL;
            }
            else
            {
                // return as is
            }
        }
        else if (strcmp(name, SU_OPTION_X509_PRIVATE_KEY) == 0)
        {
            if (mallocAndStrcpy_s((char**)&result, value) != 0)
            {
                LogError("unable to mallocAndStrcpy_s x509privatekey value");
                result = NULL;
            }
            else
            {
                // return as is
            }
        }
        else if (strcmp(name, OPTION_X509_ECC_CERT) == 0)
        {
            if (mallocAndStrcpy_s((char**)&result, value) != 0)
            {
                LogError("unable to mallocAndStrcpy_s x509EccCertificate value");
                result = NULL;
            }
            else
            {
                // return as is
            }
        }
        else if (strcmp(name, OPTION_X509_ECC_KEY) == 0)
        {
            if (mallocAndStrcpy_s((char**)&result, value) != 0)
            {
                LogError("unable to mallocAndStrcpy_s x509EccKey value");
                result = NULL;
            }
            else
            {
                // return as is
            }
        }
        else
        {
            LogError("not handled option : %s", name);
            result = NULL;
        }
    }

    return result;
}

// This function destroys an option previously created
static void tlsio_bearssl_DestroyOption(const char *name, const void *value)
{
    // Since all options for this layer are actually string copies., disposing of one is just a matter calling free
    if (name == NULL || value == NULL)
    {
        LogError("invalid parameter detected: const char* name=%p, const void* value=%p", name, value);
    }
    else
    {
        if (
            (strcmp(name, OPTION_TRUSTED_CERT) == 0) ||
            (strcmp(name, SU_OPTION_X509_CERT) == 0) ||
            (strcmp(name, SU_OPTION_X509_PRIVATE_KEY) == 0) ||
            (strcmp(name, OPTION_X509_ECC_CERT) == 0) ||
            (strcmp(name, OPTION_X509_ECC_KEY) == 0)
            )
        {
            free((void*)value);
        }
        else if (strcmp(name, OPTION_UNDERLYING_IO_OPTIONS) == 0)
        {
            OptionHandler_Destroy((OPTIONHANDLER_HANDLE)value);
        }
        else
        {
            LogError("not handled option : %s", name);
        }
    }
}

int tlsio_bearssl_setoption(CONCRETE_IO_HANDLE tls_io, const char *optionName, const void *value)
{
    int result = 0;
    size_t i;
    VECTOR_HANDLE certs;

    if (tls_io == NULL || optionName == NULL)
    {
        result = MU_FAILURE;
    }
    else
    {
        TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)tls_io;

        if (strcmp(OPTION_TRUSTED_CERT, optionName) == 0)
        {
            if (tls_io_instance->tas != NULL)
            {
                // Free the memory if it has been previously allocated
                for (i = 0; i < tls_io_instance->ta_count; i++)
                {
		            free_ta_contents(&(tls_io_instance->tas[i]));
                }

                free(tls_io_instance->tas);
                tls_io_instance->tas = NULL;
                tls_io_instance->ta_count = 0;
                free(tls_io_instance->trusted_certificates);
                tls_io_instance->trusted_certificates = NULL;
            }

            if (value != NULL)
            {
                mallocAndStrcpy_s(&tls_io_instance->trusted_certificates, (const char *)value);

                if (tls_io_instance->trusted_certificates == NULL)
                {
                    LogError("Failed to allocate memory for certificate string");
                    result = MU_FAILURE;
                }
                else
                {
                    tls_io_instance->ta_count = get_trusted_anchors((const char*)value, strlen((const char *)value) + 1, &tls_io_instance->tas);

                    if (tls_io_instance->ta_count == 0)
                    {
                        LogError("Failed to extract certificate from option value");
                        result = MU_FAILURE;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
        else if (strcmp(optionName, OPTION_UNDERLYING_IO_OPTIONS) == 0)
        {
            if (OptionHandler_FeedOptions((OPTIONHANDLER_HANDLE)value, (void*)tls_io_instance->socket_io) != OPTIONHANDLER_OK)
            {
                LogError("failed feeding options to underlying I/O instance");
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }
        }
        else if (strcmp(SU_OPTION_X509_CERT, optionName) == 0 || strcmp(OPTION_X509_ECC_CERT, optionName) == 0)
        {
            if (tls_io_instance->x509_certificate != NULL)
            {
                // Free the memory if it has been previously allocated
                free(tls_io_instance->x509_certificate);
            }

            if (mallocAndStrcpy_s(&tls_io_instance->x509_certificate, (const char *)value) != 0)
            {
                LogError("unable to mallocAndStrcpy_s on certificate");
                result = MU_FAILURE;
            }
            else
            {
                if (tls_io_instance->x509_cert != NULL)
                {
                    free_certificates(tls_io_instance->x509_cert, tls_io_instance->x509_cert_len);
                    free(tls_io_instance->x509_cert);
                    tls_io_instance->x509_cert = NULL;
                }

                certs = read_certificates_string(value, strlen(value));
                tls_io_instance->x509_cert_len = VECTOR_size(certs);
                tls_io_instance->x509_cert = VECTOR_front(certs);
                free(certs);
                result = 0;
            }
        }
        else if (strcmp(SU_OPTION_X509_PRIVATE_KEY, optionName) == 0 || strcmp(OPTION_X509_ECC_KEY, optionName) == 0)
        {
            if (tls_io_instance->x509_private_key != NULL)
            {
                // Free the memory if it has been previously allocated
                free(tls_io_instance->x509_private_key);
            }

            if (mallocAndStrcpy_s(&tls_io_instance->x509_private_key, (const char *)value) != 0)
            {
                LogError("unable to mallocAndStrcpy_s on private key");
                result = MU_FAILURE;
            }
            else
            {
                if (tls_io_instance->x509_pk != NULL)
                {
                    free_private_key(tls_io_instance->x509_pk);
                    free(tls_io_instance->x509_pk);
                    tls_io_instance->x509_pk = NULL;
                }

                tls_io_instance->x509_pk = read_private_key(value, strlen(value));
                result = 0;
            }
        }
        else if (strcmp(optionName, OPTION_SET_TLS_RENEGOTIATION) == 0)
        {
            // No need to do anything for BearSSL
            result = 0;
        }
        else
        {
            // tls_io_instance->socket_io is never NULL
            result = xio_setoption(tls_io_instance->socket_io, optionName, value);
        }
    }

    return result;
}

OPTIONHANDLER_HANDLE tlsio_bearssl_retrieveoptions(CONCRETE_IO_HANDLE handle)
{
    OPTIONHANDLER_HANDLE result = NULL;
    if (handle == NULL)
    {
        LogError("invalid parameter detected: CONCRETE_IO_HANDLE handle=%p", handle);
        result = NULL;
    }
    else
    {
        result = OptionHandler_Create(tlsio_bearssl_CloneOption, tlsio_bearssl_DestroyOption, tlsio_bearssl_setoption);
        if (result == NULL)
        {
            LogError("unable to OptionHandler_Create");
            /*return as is*/
        }
        else
        {
            // This layer cares about the certificates
            TLS_IO_INSTANCE *tls_io_instance = (TLS_IO_INSTANCE *)handle;
            OPTIONHANDLER_HANDLE underlying_io_options;

            if ((underlying_io_options = xio_retrieveoptions(tls_io_instance->socket_io)) == NULL ||
                OptionHandler_AddOption(result, OPTION_UNDERLYING_IO_OPTIONS, underlying_io_options) != OPTIONHANDLER_OK)
            {
                LogError("unable to save underlying_io options");
                OptionHandler_Destroy(underlying_io_options);
                OptionHandler_Destroy(result);
                result = NULL;
            }
            else if (tls_io_instance->trusted_certificates != NULL &&
                     OptionHandler_AddOption(result, OPTION_TRUSTED_CERT, tls_io_instance->trusted_certificates) != OPTIONHANDLER_OK)
            {
                LogError("unable to save TrustedCerts option");
                OptionHandler_Destroy(result);
                result = NULL;
            }
            else if (tls_io_instance->x509_certificate != NULL && 
                    OptionHandler_AddOption(result, SU_OPTION_X509_CERT, tls_io_instance->x509_certificate) != OPTIONHANDLER_OK)
            {
                LogError("unable to save x509certificate option");
                OptionHandler_Destroy(result);
                result = NULL;
            }
            else if (tls_io_instance->x509_private_key != NULL &&
                    OptionHandler_AddOption(result, SU_OPTION_X509_PRIVATE_KEY, tls_io_instance->x509_private_key) != OPTIONHANDLER_OK)
            {
                LogError("unable to save x509privatekey option");
                OptionHandler_Destroy(result);
                result = NULL;
            }
            else
            {
                /*all is fine, all interesting options have been saved*/
                /*return as is*/
            }
        }
    }
    return result;
}

static const IO_INTERFACE_DESCRIPTION tlsio_bearssl_interface_description =
    {
        tlsio_bearssl_retrieveoptions,
        tlsio_bearssl_create,
        tlsio_bearssl_destroy,
        tlsio_bearssl_open,
        tlsio_bearssl_close,
        tlsio_bearssl_send,
        tlsio_bearssl_dowork,
        tlsio_bearssl_setoption};

const IO_INTERFACE_DESCRIPTION *tlsio_bearssl_get_interface_description(void)
{
    return &tlsio_bearssl_interface_description;
}
