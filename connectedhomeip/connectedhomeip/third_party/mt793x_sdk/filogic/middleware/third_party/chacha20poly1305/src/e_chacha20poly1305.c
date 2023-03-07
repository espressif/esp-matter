/* $OpenBSD: e_chacha20poly1305.c,v 1.7 2014/06/21 15:30:36 jsing Exp $ */
/*
 * Copyright (c) 2014, Google Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include "os_util.h"
#include "os.h"
//#include <openssl/opensslconf.h>

#if !defined(OPENSSL_NO_CHACHA) && !defined(OPENSSL_NO_POLY1305)

//#include <openssl/err.h>
//#include <openssl/evp.h>
#include <evp.h>
#include <chacha.h>
#include <poly1305.h>

//#include "evp_locl.h"

#define POLY1305_TAG_LEN 16
#define CHACHA20_NONCE_LEN 8

// As HomeKit Accessory Protocol Pairing Specification R1, change behavior
#define DEBUG_CHACHA20 0

struct aead_chacha20_poly1305_ctx {
	unsigned char key[32];
	unsigned char tag_len;
};

// As HomeKit Accessory Protocol Pairing Specification R1, change behavior
#if DEBUG_CHACHA20
char  _hbuf[1500] = {0};
char * _tohex_(char *dst, const char *src, unsigned size)
{
   int notleading = 0;
   register char *chp = dst;
      
   *dst = '\0';
   if (size != 0) do {
      if(notleading || *src != '\0') {
	if(!notleading && (*src & 0xf0) == 0) {
	  sprintf(chp, "%.1X", * (unsigned char *) src);
	  chp += 1;
	}
	else {
	  sprintf(chp, "%.2X", * (unsigned char *) src);
	  chp += 2;
	}
	notleading = 1;
      }
      ++src;
   } while (--size != 0);
   return dst;
}
#endif

int _memcmp(const void *in_a, const void *in_b, size_t len)
{
	size_t i;
	const unsigned char *a = in_a;
	const unsigned char *b = in_b;
	unsigned char x = 0;

	for (i = 0; i < len; i++)
		x |= a[i] ^ b[i];

	return x;
}

// As HomeKit Accessory Protocol Pairing Specification R1, change behavior
void _update_length(unsigned char *buf, int bufsize, size_t length )
{  
    unsigned i = 0;
   	size_t   j = length;
    
    os_memset(buf, 0, bufsize);    
    for (i = 0; i < bufsize; i++) 
    {
        buf[i] = j;
        j >>= 8;
    }	
}

static int
aead_chacha20_poly1305_init(EVP_AEAD_CTX *ctx, const unsigned char *key,
    size_t key_len, size_t tag_len)
{
        #if DEBUG_CHACHA20
        printf("Inside aead_chacha20_poly1305_init\n");
        #endif
	struct aead_chacha20_poly1305_ctx *c20_ctx;

	if (tag_len == 0)
		tag_len = POLY1305_TAG_LEN;

	if (tag_len > POLY1305_TAG_LEN) {
		//EVPerr(EVP_F_AEAD_CHACHA20_POLY1305_INIT, EVP_R_TOO_LARGE);
                #if DEBUG_CHACHA20
		printf("[AEAD_CHACHA20_POLY1305_INIT]: Error - Tag Length is Too Large!\n");
                #endif
		return 0;
	}

	/* Internal error - EVP_AEAD_CTX_init should catch this. */
	if (key_len != sizeof(c20_ctx->key))
		{
                 #if DEBUG_CHACHA20
                 printf("[AEAD_CHACHA20_POLY1305_INIT]: Error key_len!\n");
                 #endif
                 return 0;}
 
	c20_ctx = os_malloc(sizeof(struct aead_chacha20_poly1305_ctx));
	if (c20_ctx == NULL)
		{
                 #if DEBUG_CHACHA20
                 printf("[AEAD_CHACHA20_POLY1305_INIT]: Error -os_malloc!\n");
                 #endif
                 return 0;}

	os_memcpy(&c20_ctx->key[0], key, key_len);
	c20_ctx->tag_len = tag_len;
	ctx->aead_state = c20_ctx;

	return 1;
}

static void
aead_chacha20_poly1305_cleanup(EVP_AEAD_CTX *ctx)
{
	struct aead_chacha20_poly1305_ctx *c20_ctx = ctx->aead_state;

	//OPENSSL_cleanse(c20_ctx->key, sizeof(c20_ctx->key));
	os_memset(c20_ctx->key, 0, sizeof(c20_ctx->key));
	os_free(c20_ctx);
}

/*static void
poly1305_update_with_length(poly1305_state *poly1305,
    const unsigned char *data, size_t data_len)
{
	size_t j = data_len;
	unsigned char length_bytes[8];
	unsigned i;

	for (i = 0; i < sizeof(length_bytes); i++) {
		length_bytes[i] = j;
		j >>= 8;
	}

	CRYPTO_poly1305_update(poly1305, data, data_len);
	CRYPTO_poly1305_update(poly1305, length_bytes, sizeof(length_bytes));
}*/

static int
aead_chacha20_poly1305_seal(const EVP_AEAD_CTX *ctx, unsigned char *out,
    size_t *out_len, size_t max_out_len, const unsigned char *nonce,
    size_t nonce_len, const unsigned char *in, size_t in_len,
    const unsigned char *ad, size_t ad_len)
{
	const struct aead_chacha20_poly1305_ctx *c20_ctx = ctx->aead_state;
	unsigned char poly1305_key[32];
	poly1305_state poly1305;
	const uint64_t in_len_64 = in_len;

    // As HomeKit Accessory Protocol Pairing Specification R1, change behavior
    //unsigned char  cipher_buf[1500] = {0};
    unsigned char *  cipher_buf = NULL;
    unsigned char  length_bytes[8]  = {0};
    unsigned int   bufIdx           = 0;
  
	/* The underlying ChaCha implementation may not overflow the block
	 * counter into the second counter word. Therefore we disallow
	 * individual operations that work on more than 2TB at a time.
	 * in_len_64 is needed because, on 32-bit platforms, size_t is only
	 * 32-bits and this produces a warning because it's always false.
	 * Casting to uint64_t inside the conditional is not sufficient to stop
	 * the warning. */
	if (in_len_64 >= (1ULL << 32) * 64 - 64) {
		//EVPerr(EVP_F_AEAD_CHACHA20_POLY1305_SEAL, EVP_R_TOO_LARGE);
                #if DEBUG_CHACHA20
		printf("[AEAD_CHACHA20_POLY1305_SEAL]: Error - Input Length is Too Large !\n");
                #endif
		return 0;
	}

	if (max_out_len < in_len + c20_ctx->tag_len) {
		//EVPerr(EVP_F_AEAD_CHACHA20_POLY1305_SEAL,
		//    EVP_R_BUFFER_TOO_SMALL);
                #if DEBUG_CHACHA20
		printf("[AEAD_CHACHA20_POLY1305_SEAL]: Error - Received Buffer is Too Small!\n");
                #endif
		return 0;
	}

	if (nonce_len != CHACHA20_NONCE_LEN) {
		//EVPerr(EVP_F_AEAD_CHACHA20_POLY1305_SEAL, EVP_R_IV_TOO_LARGE);
                #if DEBUG_CHACHA20
		printf("[AEAD_CHACHA20_POLY1305_SEAL]: Error - Nonce Length is Too Large!\n");
                #endif
		return 0;
	}

	os_memset(poly1305_key, 0, sizeof(poly1305_key));
	CRYPTO_chacha_20(poly1305_key, poly1305_key, sizeof(poly1305_key),
	    c20_ctx->key, nonce, 0);

	CRYPTO_poly1305_init(&poly1305, poly1305_key);

    cipher_buf = (unsigned char *)os_malloc(1500* sizeof(unsigned char));
    if (cipher_buf == NULL){
        return 0;
    }
    os_memset(cipher_buf,0,1500* sizeof(unsigned char));

    // As HomeKit Accessory Protocol Pairing Specification R1, change behavior
    // Contruct AEAD Buffer - Append AAD
    if (ad && ad_len)
    {
        os_memcpy(cipher_buf, ad, ad_len);
        bufIdx += ((ad_len/16 + ((ad_len%16)?1:0))*16);
    }
	
    // Contruct AEAD Buffer - Append Ciphertext
    CRYPTO_chacha_20(out, in, in_len, c20_ctx->key, nonce, 1);
    os_memcpy(cipher_buf+bufIdx, out, in_len);
    bufIdx += ((in_len/16 + ((in_len%16)?1:0))*16);
	 	
    // Contruct AEAD Buffer - Append AAD Length
    if (ad_len)
    {
        _update_length(length_bytes, sizeof(length_bytes), ad_len);
        os_memcpy(cipher_buf+bufIdx, length_bytes, sizeof(length_bytes));	      
    }    
    bufIdx += sizeof(length_bytes);  
   
    // Contruct AEAD Buffer - Append Ciphertext Length
    _update_length(length_bytes, sizeof(length_bytes), in_len);
    os_memcpy(cipher_buf+bufIdx, length_bytes, sizeof(length_bytes));	
    bufIdx += sizeof(length_bytes);  

    #if DEBUG_CHACHA20
    printf("[ChaCha20]: Seal Before - cipher_buf(%s), len(%d)...%s-%d\n",_tohex_(_hbuf, cipher_buf, bufIdx), bufIdx, __FUNCTION__,__LINE__);	
    #endif

    CRYPTO_poly1305_update(&poly1305, cipher_buf, bufIdx);

    if (cipher_buf != NULL){
        os_free(cipher_buf);
        cipher_buf = NULL;
    }
	
    /*
	poly1305_update_with_length(&poly1305, ad, ad_len);
	CRYPTO_chacha_20(out, in, in_len, c20_ctx->key, nonce, 1);
	poly1305_update_with_length(&poly1305, out, in_len);
    */

	if (c20_ctx->tag_len != POLY1305_TAG_LEN) {
		unsigned char tag[POLY1305_TAG_LEN];
		CRYPTO_poly1305_finish(&poly1305, tag);
		os_memcpy(out + in_len, tag, c20_ctx->tag_len);
		*out_len = in_len + c20_ctx->tag_len;
		return 1;
	}

	CRYPTO_poly1305_finish(&poly1305, out + in_len);
	*out_len = in_len + POLY1305_TAG_LEN;
    #if DEBUG_CHACHA20
    printf("[ChaCha20]: Seal After - out(%s), len(%d)...%s-%d\n",_tohex_(_hbuf, out, *out_len), *out_len, __FUNCTION__,__LINE__);	
    #endif
	return 1;
}

static int
aead_chacha20_poly1305_open(const EVP_AEAD_CTX *ctx, unsigned char *out,
    size_t *out_len, size_t max_out_len, const unsigned char *nonce,
    size_t nonce_len, const unsigned char *in, size_t in_len,
    const unsigned char *ad, size_t ad_len)
{
	const struct aead_chacha20_poly1305_ctx *c20_ctx = ctx->aead_state;
	unsigned char mac[POLY1305_TAG_LEN];
	unsigned char poly1305_key[32];
	poly1305_state poly1305;
	const uint64_t in_len_64 = in_len;
	size_t plaintext_len;

    // As HomeKit Accessory Protocol Pairing Specification R1, change behavior
    unsigned char *aead_buf        = NULL;
    unsigned int   aead_buf_size   = ad_len + in_len + 48;
    unsigned int   bufIdx          = 0;
    unsigned char  length_bytes[8] = {0};

	if (in_len < c20_ctx->tag_len) {
		//EVPerr(EVP_F_AEAD_CHACHA20_POLY1305_OPEN, EVP_R_BAD_DECRYPT);
                #if DEBUG_CHACHA20
		printf("[AEAD_CHACHA20_POLY1305_OPEN]: Error - Bad Decrypt!\n");	
                #endif	
		return 0;
	}

	/* The underlying ChaCha implementation may not overflow the block
	 * counter into the second counter word. Therefore we disallow
	 * individual operations that work on more than 2TB at a time.
	 * in_len_64 is needed because, on 32-bit platforms, size_t is only
	 * 32-bits and this produces a warning because it's always false.
	 * Casting to uint64_t inside the conditional is not sufficient to stop
	 * the warning. */
	if (in_len_64 >= (1ULL << 32) * 64 - 64) {
		//EVPerr(EVP_F_AEAD_CHACHA20_POLY1305_OPEN, EVP_R_TOO_LARGE);
                #if DEBUG_CHACHA20
		printf("[AEAD_CHACHA20_POLY1305_OPEN]: Error - Input Length is Too Large!\n");	
                #endif	
		return 0;
	}

	if (nonce_len != CHACHA20_NONCE_LEN) {
		//EVPerr(EVP_F_AEAD_CHACHA20_POLY1305_OPEN, EVP_R_IV_TOO_LARGE);
                #if DEBUG_CHACHA20
		printf("[AEAD_CHACHA20_POLY1305_OPEN]: Error - Nonce is Too Large!\n");	
                #endif	
		return 0;
	}

	plaintext_len = in_len - c20_ctx->tag_len;

	if (max_out_len < plaintext_len) {
		//EVPerr(EVP_F_AEAD_CHACHA20_POLY1305_OPEN,
		//    EVP_R_BUFFER_TOO_SMALL);
                #if DEBUG_CHACHA20
		printf("[AEAD_CHACHA20_POLY1305_OPEN]: Error - Received Buffer is Too Small!\n");
                #endif		
		return 0;
	}

	os_memset(poly1305_key, 0, sizeof(poly1305_key));
	CRYPTO_chacha_20(poly1305_key, poly1305_key, sizeof(poly1305_key),
	    c20_ctx->key, nonce, 0);

	CRYPTO_poly1305_init(&poly1305, poly1305_key);

    // As HomeKit Accessory Protocol Pairing Specification R1, change behavior
	//poly1305_update_with_length(&poly1305, ad, ad_len);
	//poly1305_update_with_length(&poly1305, in, plaintext_len);

    #if DEBUG_CHACHA20	
    printf("[ChaCha20]: nonce(%s, %d),ad_len(%d)...%s-%d\n",nonce, nonce_len, ad_len, __FUNCTION__,__LINE__);
    printf("[ChaCha20]: key(%s), key len(%d)...%s-%d\n",_tohex_(_hbuf, c20_ctx->key, 32), 32, __FUNCTION__,__LINE__);
    printf("[ChaCha20]: poly1305_key(%s), len(%d)...%s-%d\n",_tohex_(_hbuf, poly1305_key, 32), 32, __FUNCTION__,__LINE__);
    printf("[ChaCha20]: in(%s), len(%d)...%s-%d\n",_tohex_(_hbuf, in, plaintext_len), plaintext_len, __FUNCTION__,__LINE__);
    #endif
	
    // Init AEAD Buffer
    aead_buf = os_malloc(aead_buf_size);
    os_memset(aead_buf, 0, aead_buf_size);

    // Contruct AEAD Buffer - Append AAD
    if (ad && ad_len)
    {
        os_memcpy(aead_buf, ad, ad_len);
        bufIdx += ((ad_len/16 + ((ad_len%16)?1:0))*16);
    }
	
    // Contruct AEAD Buffer - Append Ciphertext
    os_memcpy(aead_buf+bufIdx, in, plaintext_len);
    bufIdx += ((plaintext_len/16 + ((plaintext_len%16)?1:0))*16);

    // Contruct AEAD Buffer - Append AAD Length
    if (ad_len)
    {
        _update_length(length_bytes, sizeof(length_bytes), ad_len);
        os_memcpy(aead_buf+bufIdx, length_bytes, sizeof(length_bytes));	      
    }    
    bufIdx += sizeof(length_bytes);  
   
    // Contruct AEAD Buffer - Append Ciphertext Length
    _update_length(length_bytes, sizeof(length_bytes), plaintext_len);
    os_memcpy(aead_buf+bufIdx, length_bytes, sizeof(length_bytes));	
    bufIdx += sizeof(length_bytes);  
  
    #if DEBUG_CHACHA20
    printf("[ChaCha20]: Open Before - aead_buf(%s), len(%x)...%s-%d\n",_tohex_(_hbuf, aead_buf, bufIdx), bufIdx, __FUNCTION__,__LINE__);	
    #endif

	CRYPTO_poly1305_update(&poly1305, aead_buf, bufIdx);
	CRYPTO_poly1305_finish(&poly1305, mac);

    // Free AEAD Buffer
    if (aead_buf)
    {	
        os_free(aead_buf);
        aead_buf = NULL;
    }	

    #if DEBUG_CHACHA20
    printf("[ChaCha20]: Open After - input tag (%s), len(%d)...%s-%d\n", _tohex_(_hbuf, in + plaintext_len, c20_ctx->tag_len), c20_ctx->tag_len, __FUNCTION__,__LINE__);
    printf("[ChaCha20]: Open After - calculate tag (%s), len(%d)...%s-%d\n", _tohex_(_hbuf, mac, c20_ctx->tag_len), c20_ctx->tag_len, __FUNCTION__,__LINE__);
    #endif

	//if (CRYPTO_memcmp(mac, in + plaintext_len, c20_ctx->tag_len) != 0) 
	if (_memcmp(mac, in + plaintext_len, c20_ctx->tag_len) != 0) 
  {
		//EVPerr(EVP_F_AEAD_CHACHA20_POLY1305_OPEN, EVP_R_BAD_DECRYPT);
                #if DEBUG_CHACHA20
		printf("[AEAD_CHACHA20_POLY1305_OPEN]: Error - Bad Decrypt!\n");	
                #endif			
		return 0;
	}

	CRYPTO_chacha_20(out, in, plaintext_len, c20_ctx->key, nonce, 1);
	*out_len = plaintext_len;
	return 1;
}

static const EVP_AEAD aead_chacha20_poly1305 = {
	.key_len = 32,
	.nonce_len = CHACHA20_NONCE_LEN,
	.overhead = POLY1305_TAG_LEN,
	.max_tag_len = POLY1305_TAG_LEN,

	.init = aead_chacha20_poly1305_init,
	.cleanup = aead_chacha20_poly1305_cleanup,
	.seal = aead_chacha20_poly1305_seal,
	.open = aead_chacha20_poly1305_open,
};

const EVP_AEAD *
EVP_aead_chacha20_poly1305()
{
	return &aead_chacha20_poly1305;
}

#endif  /* !OPENSSL_NO_CHACHA && !OPENSSL_NO_POLY1305 */

