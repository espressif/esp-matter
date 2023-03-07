/* $OpenBSD: evp.h,v 1.38 2014/06/24 19:31:50 miod Exp $ */
/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 *
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 *
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 *
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */

#ifndef HEADER_ENVELOPE_H
#define HEADER_ENVELOPE_H

#ifdef	__cplusplus
extern "C" {
#endif

/* Authenticated Encryption with Additional Data.
 *
 * AEAD couples confidentiality and integrity in a single primtive. AEAD
 * algorithms take a key and then can seal and open individual messages. Each
 * message has a unique, per-message nonce and, optionally, additional data
 * which is authenticated but not included in the output. */

//struct evp_aead_st;
struct evp_aead_ctx_st;
/* EVP_AEAD represents a specific AEAD algorithm. */
struct evp_aead_st {
	unsigned char key_len;
	unsigned char nonce_len;
	unsigned char overhead;
	unsigned char max_tag_len;

	int (*init)(struct evp_aead_ctx_st*, const unsigned char *key,
	    size_t key_len, size_t tag_len);
	void (*cleanup)(struct evp_aead_ctx_st*);

	int (*seal)(const struct evp_aead_ctx_st *ctx, unsigned char *out,
	    size_t *out_len, size_t max_out_len, const unsigned char *nonce,
	    size_t nonce_len, const unsigned char *in, size_t in_len,
	    const unsigned char *ad, size_t ad_len);

	int (*open)(const struct evp_aead_ctx_st *ctx, unsigned char *out,
	    size_t *out_len, size_t max_out_len, const unsigned char *nonce,
	    size_t nonce_len, const unsigned char *in, size_t in_len,
	    const unsigned char *ad, size_t ad_len);
};

typedef struct evp_aead_st EVP_AEAD;

#ifndef OPENSSL_NO_AES
/* EVP_aes_128_gcm is AES-128 in Galois Counter Mode. */
const EVP_AEAD *EVP_aead_aes_128_gcm(void);
/* EVP_aes_256_gcm is AES-256 in Galois Counter Mode. */
const EVP_AEAD *EVP_aead_aes_256_gcm(void);
#endif

#if !defined(OPENSSL_NO_CHACHA) && !defined(OPENSSL_NO_POLY1305)
/* EVP_aead_chacha20_poly1305 is ChaCha20 with a Poly1305 authenticator. */
const EVP_AEAD *EVP_aead_chacha20_poly1305(void);
#endif

/* EVP_AEAD_key_length returns the length of the keys used. */
size_t EVP_AEAD_key_length(const EVP_AEAD *aead);

/* EVP_AEAD_nonce_length returns the length of the per-message nonce. */
size_t EVP_AEAD_nonce_length(const EVP_AEAD *aead);

/* EVP_AEAD_max_overhead returns the maximum number of additional bytes added
 * by the act of sealing data with the AEAD. */
size_t EVP_AEAD_max_overhead(const EVP_AEAD *aead);

/* EVP_AEAD_max_tag_len returns the maximum tag length when using this AEAD.
 * This * is the largest value that can be passed as a tag length to
 * EVP_AEAD_CTX_init. */
size_t EVP_AEAD_max_tag_len(const EVP_AEAD *aead);

/* An EVP_AEAD_CTX represents an AEAD algorithm configured with a specific key
 * and message-independent IV. */
typedef struct evp_aead_ctx_st {
	const EVP_AEAD *aead;
	/* aead_state is an opaque pointer to the AEAD specific state. */
	void *aead_state;
} EVP_AEAD_CTX;

/* EVP_AEAD_MAX_TAG_LENGTH is the maximum tag length used by any AEAD
 * defined in this header. */
#define EVP_AEAD_MAX_TAG_LENGTH 16

/* EVP_AEAD_DEFAULT_TAG_LENGTH is a magic value that can be passed to
 * EVP_AEAD_CTX_init to indicate that the default tag length for an AEAD
 * should be used. */
#define EVP_AEAD_DEFAULT_TAG_LENGTH 0

/* EVP_AEAD_init initializes the context for the given AEAD algorithm.
 * The implementation argument may be NULL to choose the default implementation.
 * Authentication tags may be truncated by passing a tag length. A tag length
 * of zero indicates the default tag length should be used. */
int EVP_AEAD_CTX_init(EVP_AEAD_CTX *ctx, const EVP_AEAD *aead,
    const unsigned char *key, size_t key_len, size_t tag_len);

/* EVP_AEAD_CTX_cleanup frees any data allocated for this context. */
void EVP_AEAD_CTX_cleanup(EVP_AEAD_CTX *ctx);

/* EVP_AEAD_CTX_seal encrypts and authenticates the input and authenticates
 * any additional data (AD), the result being written as output. One is
 * returned on success, otherwise zero.
 *
 * This function may be called (with the same EVP_AEAD_CTX) concurrently with
 * itself or EVP_AEAD_CTX_open.
 *
 * At most max_out_len bytes are written as output and, in order to ensure
 * success, this value should be the length of the input plus the result of
 * EVP_AEAD_overhead. On successful return, out_len is set to the actual
 * number of bytes written.
 *
 * The length of the nonce is must be equal to the result of
 * EVP_AEAD_nonce_length for this AEAD.
 *
 * EVP_AEAD_CTX_seal never results in a partial output. If max_out_len is
 * insufficient, zero will be returned and out_len will be set to zero.
 *
 * If the input and output are aliased then out must be <= in. */
int EVP_AEAD_CTX_seal(const EVP_AEAD_CTX *ctx, unsigned char *out,
    size_t *out_len, size_t max_out_len, const unsigned char *nonce,
    size_t nonce_len, const unsigned char *in, size_t in_len,
    const unsigned char *ad, size_t ad_len);

/* EVP_AEAD_CTX_open authenticates the input and additional data, decrypting
 * the input and writing it as output. One is returned on success, otherwise
 * zero.
 *
 * This function may be called (with the same EVP_AEAD_CTX) concurrently with
 * itself or EVP_AEAD_CTX_seal.
 *
 * At most the number of input bytes are written as output. In order to ensure
 * success, max_out_len should be at least the same as the input length. On
 * successful return out_len is set to the actual number of bytes written.
 *
 * The length of nonce must be equal to the result of EVP_AEAD_nonce_length
 * for this AEAD.
 *
 * EVP_AEAD_CTX_open never results in a partial output. If max_out_len is
 * insufficient, zero will be returned and out_len will be set to zero.
 *
 * If the input and output are aliased then out must be <= in. */
int EVP_AEAD_CTX_open(const EVP_AEAD_CTX *ctx, unsigned char *out,
    size_t *out_len, size_t max_out_len, const unsigned char *nonce,
    size_t nonce_len, const unsigned char *in, size_t in_len,
    const unsigned char *ad, size_t ad_len);

 //xin---------------------------------
 #ifdef __UNIT_TEST__
 int testchacha();
 #endif /* __UNIT_TEST__ */



#ifdef  __cplusplus
}
#endif
#endif
