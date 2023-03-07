/*
 * Wrapper functions for OpenSSL libcrypto
 * Copyright (c) 2004-2015, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "includes.h"

#include "common.h"
#include "wpabuf.h"
#include "dh_group5.h"
#include "sha1.h"
#include "sha256.h"
#include "sha384.h"
#include "crypto.h"
/* mbedtls */
#include "mbedtls/entropy.h"
#include "mbedtls/entropy_poll.h"
#include "mbedtls/hmac_drbg.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/dhm.h"
#include "mbedtls/gcm.h"
#include "mbedtls/ccm.h"
#include "mbedtls/cmac.h"
#include "mbedtls/md2.h"
#include "mbedtls/md4.h"
#include "mbedtls/md5.h"
#include "mbedtls/ripemd160.h"
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"
#include "mbedtls/arc4.h"
#include "mbedtls/des.h"
#include "mbedtls/aes.h"
#include "mbedtls/camellia.h"
#include "mbedtls/base64.h"
#include "mbedtls/bignum.h"
#include "mbedtls/rsa.h"
#include "mbedtls/x509.h"
#include "mbedtls/xtea.h"
#include "mbedtls/pkcs5.h"
#include "mbedtls/ecp.h"
#include "mbedtls/ecjpake.h"
#include "mbedtls/timing.h"

static mbedtls_debug = MSG_INFO; // MSG_ERROR;

#ifndef CONFIG_FIPS
int md4_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return -1;
}
#endif /* CONFIG_FIPS */


void des_encrypt(const u8 *clear, const u8 *key, u8 *cypher)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
}


#ifndef CONFIG_NO_RC4
int rc4_skip(const u8 *key, size_t keylen, size_t skip,
	     u8 *data, size_t data_len)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return 0;
}
#endif /* CONFIG_NO_RC4 */


#ifndef CONFIG_FIPS
int md5_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
    	mbedtls_md5_context ctx;
	size_t i;

    	mbedtls_md5_init( &ctx );

    	mbedtls_md5_starts( &ctx );

	for (i = 0; i < num_elem; i++) {
    		mbedtls_md5_update( &ctx, addr[i], len[i] );
	}
    	mbedtls_md5_finish( &ctx, mac );
exit:
    	mbedtls_md5_free( &ctx );

	return 0;
}
#endif /* CONFIG_FIPS */


int sha1_vector(size_t num_elem, const u8 *addr[], const size_t *len, u8 *mac)
{
    	mbedtls_sha1_context ctx;
	size_t i;

    	mbedtls_sha1_init( &ctx );

    	mbedtls_sha1_starts( &ctx );

	for (i = 0; i < num_elem; i++) {
    		mbedtls_sha1_update( &ctx, addr[i], len[i] );
	}
    	mbedtls_sha1_finish( &ctx, mac );
exit:
    	mbedtls_sha1_free( &ctx );

	return 0;
}


#ifndef NO_SHA256_WRAPPER
int sha256_vector(size_t num_elem, const u8 *addr[], const size_t *len,
		  u8 *mac)
{
    	mbedtls_sha256_context ctx;
    	int ret;
	size_t i;

    	mbedtls_sha256_init( &ctx );

    	mbedtls_sha256_starts( &ctx , 0);

	for (i = 0; i < num_elem; i++) {
    		mbedtls_sha256_update( &ctx, addr[i], len[i] );
	}
    	mbedtls_sha256_finish( &ctx, mac );
exit:
    	mbedtls_sha256_free( &ctx );

	return 0;
}
#endif /* NO_SHA256_WRAPPER */

void * aes_encrypt_init(const u8 *key, size_t len)
{
	mbedtls_aes_context *ctx;

	ctx = os_zalloc(sizeof(mbedtls_aes_context));
	if (ctx == NULL)
		return NULL;
    	mbedtls_aes_init( ctx );
        mbedtls_aes_setkey_enc( ctx, key, len );

	return ctx;
}


void aes_encrypt(void *ctx, const u8 *plain, u8 *crypt)
{
	mbedtls_aes_context *c = ctx;
	mbedtls_aes_encrypt(c, plain, crypt);
}


void aes_encrypt_deinit(void *ctx)
{
	mbedtls_aes_context *c = ctx;
	if (ctx == NULL) {
		wpa_printf(mbedtls_debug, "MbedTLS: %s NULL pointer entered", __FUNCTION__);
		return;
	}
   	mbedtls_aes_free( c );
	os_free(c);
}


void * aes_decrypt_init(const u8 *key, size_t len)
{
	mbedtls_aes_context *ctx;

	ctx = os_zalloc(sizeof(mbedtls_aes_context));
	if (ctx == NULL)
		return NULL;
    	mbedtls_aes_init( ctx );
        mbedtls_aes_setkey_dec( ctx, key, len );

	return ctx;
}


void aes_decrypt(void *ctx, const u8 *crypt, u8 *plain)
{
	mbedtls_aes_context *c = ctx;
	mbedtls_aes_decrypt(c, crypt, plain);
}


void aes_decrypt_deinit(void *ctx)
{
	mbedtls_aes_context *c = ctx;
	if (ctx == NULL) {
		wpa_printf(mbedtls_debug, "MbedTLS: %s NULL pointer entered", __FUNCTION__);
		return;
	}
   	mbedtls_aes_free( c );
	os_free(c);
}


#ifndef CONFIG_FIPS
#ifndef CONFIG_OPENSSL_INTERNAL_AES_WRAP
static const uint8_t default_iv[] = {
    0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6,
};


int aes_wrap(const u8 *kek, size_t kek_len, int n, const u8 *plain, u8 *cipher)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return -1;
}

int AES_unwrap_key(mbedtls_aes_context* aes_ctx, const uint8_t *iv, uint8_t *out,
                   const uint8_t *in, unsigned int inlen) {
  uint8_t *A, B[16], *R;
  unsigned int i, j, t;
  inlen -= 8;
  if (inlen & 0x7) return -1;
  if (inlen < 8) return -1;
  A = B;
  t = 6 * (inlen >> 3);
  memcpy(A, in, 8);
  memcpy(out, in + 8, inlen);
  for (j = 0; j < 6; j++) {
    R = out + inlen - 8;
    for (i = 0; i < inlen; i += 8, t--, R -= 8) {
      A[7] ^= (uint8_t)(t & 0xff);
      if (t > 0xff) {
        A[6] ^= (uint8_t)((t >> 8) & 0xff);
        A[5] ^= (uint8_t)((t >> 16) & 0xff);
        A[4] ^= (uint8_t)((t >> 24) & 0xff);
      }
      memcpy(B + 8, R, 8);
      mbedtls_aes_decrypt(aes_ctx, B, B);
      memcpy(R, B + 8, 8);
    }
  }
  if (!iv) iv = default_iv;
  if (memcmp(A, iv, 8)) {
    return 0;
  }
  return inlen;
}

int aes_unwrap(const u8 *kek, size_t kek_len, int n, const u8 *cipher,
	       u8 *plain)
{
	int res;
   	mbedtls_aes_context aes_ctx;
    	mbedtls_aes_init( &aes_ctx );

       	mbedtls_aes_setkey_dec( &aes_ctx, kek, kek_len<<3 );

	res = AES_unwrap_key(&aes_ctx, NULL, plain, cipher, (n + 1) * 8);

    	return res <= 0 ? -1 : 0;
}

#endif /* CONFIG_OPENSSL_INTERNAL_AES_WRAP */
#endif /* CONFIG_FIPS */


int aes_128_cbc_encrypt(const u8 *key, const u8 *iv, u8 *data, size_t data_len)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return -1;
}


int aes_128_cbc_decrypt(const u8 *key, const u8 *iv, u8 *data, size_t data_len)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);

	return -1;

}


int crypto_mod_exp(const u8 *base, size_t base_len,
		   const u8 *power, size_t power_len,
		   const u8 *modulus, size_t modulus_len,
		   u8 *result, size_t *result_len)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return -1;
}


struct crypto_cipher * crypto_cipher_init(enum crypto_cipher_alg alg,
					  const u8 *iv, const u8 *key,
					  size_t key_len)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return NULL;
}


int crypto_cipher_encrypt(struct crypto_cipher *ctx, const u8 *plain,
			  u8 *crypt, size_t len)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return -1;
}


int crypto_cipher_decrypt(struct crypto_cipher *ctx, const u8 *crypt,
			  u8 *plain, size_t len)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return -1;
}


void crypto_cipher_deinit(struct crypto_cipher *ctx)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
}


/* openssl uses 1536 bit of group 5, but mbedtls only support 2048 and 3072 bits. Don't know what do yet */
void * dh5_init(struct wpabuf **priv, struct wpabuf **publ)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return NULL;
}


void * dh5_init_fixed(const struct wpabuf *priv, const struct wpabuf *publ)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return NULL;
}


struct wpabuf * dh5_derive_shared(void *ctx, const struct wpabuf *peer_public,
				  const struct wpabuf *own_private)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return NULL;
}


void dh5_free(void *ctx)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
}


struct crypto_hash * crypto_hash_init(enum crypto_hash_alg alg, const u8 *key,
				      size_t key_len)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return NULL;
}


void crypto_hash_update(struct crypto_hash *ctx, const u8 *data, size_t len)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
}


int crypto_hash_finish(struct crypto_hash *ctx, u8 *mac, size_t *len)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return -1;
}

int hmac_vector(mbedtls_md_type_t md_type, const u8 *key, size_t key_len, size_t num_elem,
                     const u8 *addr[], const size_t *len, u8 *mac);


#ifndef CONFIG_FIPS

int hmac_md5_vector(const u8 *key, size_t key_len, size_t num_elem,
		    const u8 *addr[], const size_t *len, u8 *mac)
{
        return hmac_vector(MBEDTLS_MD_MD5, key, key_len, num_elem, addr, len, mac);
}


int hmac_md5(const u8 *key, size_t key_len, const u8 *data, size_t data_len,
	     u8 *mac)
{
	return hmac_md5_vector(key, key_len, 1, &data, &data_len, mac);
}

#endif /* CONFIG_FIPS */

static void my_error(int ret)
{
//rtos_porting
#if 0
	char buf[1024];
        mbedtls_strerror( ret, buf, 1024 );
        wpa_printf(mbedtls_debug, " mbedtls failed 0x%x - %s\n", ret, buf );
#endif
}

int my_pkcs5_pbkdf2_hmac( mbedtls_md_context_t *ctx, const unsigned char *password,
                       size_t plen, const unsigned char *salt, size_t slen,
                       unsigned int iteration_count,
                       uint32_t key_length, unsigned char *output )
{
    int ret, j;
    unsigned int i;
    unsigned char md1[MBEDTLS_MD_MAX_SIZE];
    unsigned char work[MBEDTLS_MD_MAX_SIZE];
    unsigned char md_size = mbedtls_md_get_size( ctx->md_info );
    size_t use_len;
    unsigned char *out_p = output;
    unsigned char counter[4];

    memset( counter, 0, 4 );
    counter[3] = 1;

    if( iteration_count > 0xFFFFFFFF )
        return( MBEDTLS_ERR_PKCS5_BAD_INPUT_DATA );

    while( key_length )
    {
        // U1 ends up in work
        //
        if( ( ret = mbedtls_md_hmac_starts( ctx, password, plen ) ) != 0 ) {
            return( ret );
	}

        if( ( ret = mbedtls_md_hmac_update( ctx, salt, slen ) ) != 0 ) {
            return( ret );
	}

        if( ( ret = mbedtls_md_hmac_update( ctx, counter, 4 ) ) != 0 ) {
            return( ret );
	}

        if( ( ret = mbedtls_md_hmac_finish( ctx, work ) ) != 0 ) {
            return( ret );
	}

        memcpy( md1, work, md_size );

        for( i = 1; i < iteration_count; i++ )
        {
            // U2 ends up in md1
            //
            if( ( ret = mbedtls_md_hmac_starts( ctx, password, plen ) ) != 0 ) {
                return( ret );
	    }

            if( ( ret = mbedtls_md_hmac_update( ctx, md1, md_size ) ) != 0 ) {
                return( ret );
	    }

            if( ( ret = mbedtls_md_hmac_finish( ctx, md1 ) ) != 0 ) {
                return( ret );
	    }

            // U1 xor U2
            //
            for( j = 0; j < md_size; j++ )
                work[j] ^= md1[j];
        }

        use_len = ( key_length < md_size ) ? key_length : md_size;
        memcpy( out_p, work, use_len );

        key_length -= (uint32_t) use_len;
        out_p += use_len;

        for( i = 4; i > 0; i-- )
            if( ++counter[i - 1] != 0 )
                break;
    }

    return( 0 );
}

int pbkdf2_sha1(const char *passphrase, const u8 *ssid, size_t ssid_len,
                int iterations, u8 *buf, size_t buflen)
{
	mbedtls_md_context_t ctx;
	int ret = -1;
   	const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA1);
	if (md_info == NULL) {
		wpa_printf(mbedtls_debug, "MbedTLS: %s error get info", __FUNCTION__);
		return ret;
	}
	
    	mbedtls_md_init( &ctx );
	mbedtls_md_setup(&ctx, md_info, 1);
	ret = mbedtls_pkcs5_pbkdf2_hmac(&ctx, passphrase, os_strlen(passphrase), 
		ssid, ssid_len, iterations, buflen, buf);
	if (ret != 0) {
		my_error(ret);
	}
		
    	mbedtls_md_free( &ctx );
	return ret;
}

int hmac_vector(mbedtls_md_type_t md_type, const u8 *key, size_t key_len, size_t num_elem,
                     const u8 *addr[], const size_t *len, u8 *mac)
{
	mbedtls_md_context_t ctx;
	size_t i;

   	const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(md_type);

    	mbedtls_md_init( &ctx );

	mbedtls_md_setup(&ctx, md_info, 1);
    	mbedtls_md_hmac_starts( &ctx, key, key_len );

	for (i = 0; i < num_elem; i++) {
    		mbedtls_md_hmac_update( &ctx, addr[i], len[i] );
	}
    	mbedtls_md_hmac_finish( &ctx, mac );
    	mbedtls_md_free( &ctx );

	return 0;
}

int hmac_sha1_vector(const u8 *key, size_t key_len, size_t num_elem,
                     const u8 *addr[], const size_t *len, u8 *mac)
{
        return hmac_vector(MBEDTLS_MD_SHA1, key, key_len, num_elem, addr, len, mac);
}


int hmac_sha1(const u8 *key, size_t key_len, const u8 *data, size_t data_len,
	       u8 *mac)
{
	return hmac_vector(MBEDTLS_MD_SHA1, key, key_len, 1, &data, &data_len, mac);
}


#ifdef CONFIG_SHA256

int hmac_sha256_vector(const u8 *key, size_t key_len, size_t num_elem,
		       const u8 *addr[], const size_t *len, u8 *mac)
{
        return hmac_vector(MBEDTLS_MD_SHA256, key, key_len, num_elem, addr, len, mac);
}


int hmac_sha256(const u8 *key, size_t key_len, const u8 *data,
		size_t data_len, u8 *mac)
{
	return hmac_sha256_vector(key, key_len, 1, &data, &data_len, mac);
}

#endif /* CONFIG_SHA256 */


#ifdef CONFIG_SHA384

int hmac_sha384_vector(const u8 *key, size_t key_len, size_t num_elem,
		       const u8 *addr[], const size_t *len, u8 *mac)
{
        return hmac_vector(MBEDTLS_MD_SHA384, key, key_len, num_elem, addr, len, mac);
}


int hmac_sha384(const u8 *key, size_t key_len, const u8 *data,
		size_t data_len, u8 *mac)
{
	return hmac_sha384_vector(key, key_len, 1, &data, &data_len, mac);
}

#endif /* CONFIG_SHA384 */


int crypto_get_random(void *buf, size_t len)
{
    char *personalization = "wpa_supplicant_string";
    int ret = 0;

    /*
     * Setup random number generator
     * (Note: later this might be done automatically.)
     */
    mbedtls_entropy_context entropy;    /* entropy pool for seeding PRNG */
    mbedtls_ctr_drbg_context drbg;      /* pseudo-random generator */

    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&drbg);

    /* Seed the PRNG using the entropy pool, and throw in our secret key as an
     * additional source of randomness. */
    ret = mbedtls_ctr_drbg_seed(&drbg, mbedtls_entropy_func, &entropy,
                                       (const unsigned char *) personalization, strlen( personalization ));
    if (ret != 0) {
        printf("mbedtls_ctr_drbg_init() returned -0x%04X\r\n", -ret);
        return 1;
    }

    mbedtls_ctr_drbg_random(&drbg, buf, len);
    return 0;
}


#ifdef CONFIG_OPENSSL_CMAC
int omac1_aes_vector(const u8 *key, size_t key_len, size_t num_elem,
		     const u8 *addr[], const size_t *len, u8 *mac)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return -1;
}


int omac1_aes_128_vector(const u8 *key, size_t num_elem,
			 const u8 *addr[], const size_t *len, u8 *mac)
{
	return omac1_aes_vector(key, 16, num_elem, addr, len, mac);
}


int omac1_aes_128(const u8 *key, const u8 *data, size_t data_len, u8 *mac)
{
	return omac1_aes_128_vector(key, 1, &data, &data_len, mac);
}


int omac1_aes_256(const u8 *key, const u8 *data, size_t data_len, u8 *mac)
{
	return omac1_aes_vector(key, 32, 1, &data, &data_len, mac);
}
#endif /* CONFIG_OPENSSL_CMAC */


struct crypto_bignum * crypto_bignum_init(void)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return NULL;
}


struct crypto_bignum * crypto_bignum_init_set(const u8 *buf, size_t len)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return NULL;
}


void crypto_bignum_deinit(struct crypto_bignum *n, int clear)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
}


int crypto_bignum_to_bin(const struct crypto_bignum *a,
			 u8 *buf, size_t buflen, size_t padlen)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return -1;
}


int crypto_bignum_add(const struct crypto_bignum *a,
		      const struct crypto_bignum *b,
		      struct crypto_bignum *c)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return -1;
}


int crypto_bignum_mod(const struct crypto_bignum *a,
		      const struct crypto_bignum *b,
		      struct crypto_bignum *c)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return -1;
}


int crypto_bignum_exptmod(const struct crypto_bignum *a,
			  const struct crypto_bignum *b,
			  const struct crypto_bignum *c,
			  struct crypto_bignum *d)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return -1;
}


int crypto_bignum_inverse(const struct crypto_bignum *a,
			  const struct crypto_bignum *b,
			  struct crypto_bignum *c)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return -1;
}


int crypto_bignum_sub(const struct crypto_bignum *a,
		      const struct crypto_bignum *b,
		      struct crypto_bignum *c)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return -1;
}


int crypto_bignum_div(const struct crypto_bignum *a,
		      const struct crypto_bignum *b,
		      struct crypto_bignum *c)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return -1;
}


int crypto_bignum_mulmod(const struct crypto_bignum *a,
			 const struct crypto_bignum *b,
			 const struct crypto_bignum *c,
			 struct crypto_bignum *d)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return -1;
}


int crypto_bignum_cmp(const struct crypto_bignum *a,
		      const struct crypto_bignum *b)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return -1;
}


int crypto_bignum_bits(const struct crypto_bignum *a)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return -1;
}


int crypto_bignum_is_zero(const struct crypto_bignum *a)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return -1;
}


int crypto_bignum_is_one(const struct crypto_bignum *a)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return -1;
}


int crypto_bignum_legendre(const struct crypto_bignum *a,
			   const struct crypto_bignum *p)
{
	wpa_printf(mbedtls_debug, "MbedTLS: %s  not implemented yet", __FUNCTION__);
	return -1;
}


#ifdef CONFIG_ECC
struct crypto_ec {
	EC_GROUP *group;
	BN_CTX *bnctx;
	BIGNUM *prime;
	BIGNUM *order;
	BIGNUM *a;
	BIGNUM *b;
};

struct crypto_ec * crypto_ec_init(int group)
{
	struct crypto_ec *e;
	int nid;

	/* Map from IANA registry for IKE D-H groups to OpenSSL NID */
	switch (group) {
	case 19:
		nid = NID_X9_62_prime256v1;
		break;
	case 20:
		nid = NID_secp384r1;
		break;
	case 21:
		nid = NID_secp521r1;
		break;
	case 25:
		nid = NID_X9_62_prime192v1;
		break;
	case 26:
		nid = NID_secp224r1;
		break;
#ifdef NID_brainpoolP224r1
	case 27:
		nid = NID_brainpoolP224r1;
		break;
#endif /* NID_brainpoolP224r1 */
#ifdef NID_brainpoolP256r1
	case 28:
		nid = NID_brainpoolP256r1;
		break;
#endif /* NID_brainpoolP256r1 */
#ifdef NID_brainpoolP384r1
	case 29:
		nid = NID_brainpoolP384r1;
		break;
#endif /* NID_brainpoolP384r1 */
#ifdef NID_brainpoolP512r1
	case 30:
		nid = NID_brainpoolP512r1;
		break;
#endif /* NID_brainpoolP512r1 */
	default:
		return NULL;
	}

	e = os_zalloc(sizeof(*e));
	if (e == NULL)
		return NULL;

	e->bnctx = BN_CTX_new();
	e->group = EC_GROUP_new_by_curve_name(nid);
	e->prime = BN_new();
	e->order = BN_new();
	e->a = BN_new();
	e->b = BN_new();
	if (e->group == NULL || e->bnctx == NULL || e->prime == NULL ||
	    e->order == NULL || e->a == NULL || e->b == NULL ||
	    !EC_GROUP_get_curve_GFp(e->group, e->prime, e->a, e->b, e->bnctx) ||
	    !EC_GROUP_get_order(e->group, e->order, e->bnctx)) {
		crypto_ec_deinit(e);
		e = NULL;
	}

	return e;
}


void crypto_ec_deinit(struct crypto_ec *e)
{
	if (e == NULL)
		return;
	BN_clear_free(e->b);
	BN_clear_free(e->a);
	BN_clear_free(e->order);
	BN_clear_free(e->prime);
	EC_GROUP_free(e->group);
	BN_CTX_free(e->bnctx);
	os_free(e);
}


struct crypto_ec_point * crypto_ec_point_init(struct crypto_ec *e)
{
	if (TEST_FAIL())
		return NULL;
	if (e == NULL)
		return NULL;
	return (struct crypto_ec_point *) EC_POINT_new(e->group);
}


size_t crypto_ec_prime_len(struct crypto_ec *e)
{
	return BN_num_bytes(e->prime);
}


size_t crypto_ec_prime_len_bits(struct crypto_ec *e)
{
	return BN_num_bits(e->prime);
}


const struct crypto_bignum * crypto_ec_get_prime(struct crypto_ec *e)
{
	return (const struct crypto_bignum *) e->prime;
}


const struct crypto_bignum * crypto_ec_get_order(struct crypto_ec *e)
{
	return (const struct crypto_bignum *) e->order;
}


void crypto_ec_point_deinit(struct crypto_ec_point *p, int clear)
{
	if (clear)
		EC_POINT_clear_free((EC_POINT *) p);
	else
		EC_POINT_free((EC_POINT *) p);
}


int crypto_ec_point_to_bin(struct crypto_ec *e,
			   const struct crypto_ec_point *point, u8 *x, u8 *y)
{
	BIGNUM *x_bn, *y_bn;
	int ret = -1;
	int len = BN_num_bytes(e->prime);

	if (TEST_FAIL())
		return -1;

	x_bn = BN_new();
	y_bn = BN_new();

	if (x_bn && y_bn &&
	    EC_POINT_get_affine_coordinates_GFp(e->group, (EC_POINT *) point,
						x_bn, y_bn, e->bnctx)) {
		if (x) {
			crypto_bignum_to_bin((struct crypto_bignum *) x_bn,
					     x, len, len);
		}
		if (y) {
			crypto_bignum_to_bin((struct crypto_bignum *) y_bn,
					     y, len, len);
		}
		ret = 0;
	}

	BN_clear_free(x_bn);
	BN_clear_free(y_bn);
	return ret;
}


struct crypto_ec_point * crypto_ec_point_from_bin(struct crypto_ec *e,
						  const u8 *val)
{
	BIGNUM *x, *y;
	EC_POINT *elem;
	int len = BN_num_bytes(e->prime);

	if (TEST_FAIL())
		return NULL;

	x = BN_bin2bn(val, len, NULL);
	y = BN_bin2bn(val + len, len, NULL);
	elem = EC_POINT_new(e->group);
	if (x == NULL || y == NULL || elem == NULL) {
		BN_clear_free(x);
		BN_clear_free(y);
		EC_POINT_clear_free(elem);
		return NULL;
	}

	if (!EC_POINT_set_affine_coordinates_GFp(e->group, elem, x, y,
						 e->bnctx)) {
		EC_POINT_clear_free(elem);
		elem = NULL;
	}

	BN_clear_free(x);
	BN_clear_free(y);

	return (struct crypto_ec_point *) elem;
}


int crypto_ec_point_add(struct crypto_ec *e, const struct crypto_ec_point *a,
			const struct crypto_ec_point *b,
			struct crypto_ec_point *c)
{
	if (TEST_FAIL())
		return -1;
	return EC_POINT_add(e->group, (EC_POINT *) c, (const EC_POINT *) a,
			    (const EC_POINT *) b, e->bnctx) ? 0 : -1;
}


int crypto_ec_point_mul(struct crypto_ec *e, const struct crypto_ec_point *p,
			const struct crypto_bignum *b,
			struct crypto_ec_point *res)
{
	if (TEST_FAIL())
		return -1;
	return EC_POINT_mul(e->group, (EC_POINT *) res, NULL,
			    (const EC_POINT *) p, (const BIGNUM *) b, e->bnctx)
		? 0 : -1;
}


int crypto_ec_point_invert(struct crypto_ec *e, struct crypto_ec_point *p)
{
	if (TEST_FAIL())
		return -1;
	return EC_POINT_invert(e->group, (EC_POINT *) p, e->bnctx) ? 0 : -1;
}


int crypto_ec_point_solve_y_coord(struct crypto_ec *e,
				  struct crypto_ec_point *p,
				  const struct crypto_bignum *x, int y_bit)
{
	if (TEST_FAIL())
		return -1;
	if (!EC_POINT_set_compressed_coordinates_GFp(e->group, (EC_POINT *) p,
						     (const BIGNUM *) x, y_bit,
						     e->bnctx) ||
	    !EC_POINT_is_on_curve(e->group, (EC_POINT *) p, e->bnctx))
		return -1;
	return 0;
}


struct crypto_bignum *
crypto_ec_point_compute_y_sqr(struct crypto_ec *e,
			      const struct crypto_bignum *x)
{
	BIGNUM *tmp, *tmp2, *y_sqr = NULL;

	if (TEST_FAIL())
		return NULL;

	tmp = BN_new();
	tmp2 = BN_new();

	/* y^2 = x^3 + ax + b */
	if (tmp && tmp2 &&
	    BN_mod_sqr(tmp, (const BIGNUM *) x, e->prime, e->bnctx) &&
	    BN_mod_mul(tmp, tmp, (const BIGNUM *) x, e->prime, e->bnctx) &&
	    BN_mod_mul(tmp2, e->a, (const BIGNUM *) x, e->prime, e->bnctx) &&
	    BN_mod_add_quick(tmp2, tmp2, tmp, e->prime) &&
	    BN_mod_add_quick(tmp2, tmp2, e->b, e->prime)) {
		y_sqr = tmp2;
		tmp2 = NULL;
	}

	BN_clear_free(tmp);
	BN_clear_free(tmp2);

	return (struct crypto_bignum *) y_sqr;
}


int crypto_ec_point_is_at_infinity(struct crypto_ec *e,
				   const struct crypto_ec_point *p)
{
	return EC_POINT_is_at_infinity(e->group, (const EC_POINT *) p);
}


int crypto_ec_point_is_on_curve(struct crypto_ec *e,
				const struct crypto_ec_point *p)
{
	return EC_POINT_is_on_curve(e->group, (const EC_POINT *) p,
				    e->bnctx) == 1;
}


int crypto_ec_point_cmp(const struct crypto_ec *e,
			const struct crypto_ec_point *a,
			const struct crypto_ec_point *b)
{
	return EC_POINT_cmp(e->group, (const EC_POINT *) a,
			    (const EC_POINT *) b, e->bnctx);
}

#endif /* CONFIG_ECC */

